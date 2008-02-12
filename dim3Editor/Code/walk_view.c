/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Walk View Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "top_view.h"
#include "portal_view.h"
#include "site_path_view.h"
#include "walk_view.h"

extern int						cr,top_view_x,top_view_z,
								portal_view_x,portal_view_z,site_path_view_x,site_path_view_z,
								txt_palette_high;
extern bool						main_wind_rot;
extern Rect						top_view_box,portal_view_box,site_path_view_box;

extern file_path_setup_type		file_path_setup;
extern map_type					map;

extern WindowRef				mainwind;
extern CCrsrHandle				handcur,rotatecur,forwardcur,towardcur;

int								cx,cz,cy,walk_view_last_rn;
float							walk_view_fov,walk_view_y_angle,walk_view_x_angle;
bool							walk_view_active;
Rect							walk_view_forward_box,walk_view_side_box;
bitmap_type						spot_bitmap,scenery_bitmap,node_bitmap,
								light_bitmap,sound_bitmap,particle_bitmap;

/* =======================================================

      Set FOV
      
======================================================= */

void walk_view_set_fov(float fov)
{
	walk_view_fov=fov;
	menu_set_fov_check(fov);
}

/* =======================================================

      Initialize and Shutdown Walk View
	        
======================================================= */

bool walk_view_initialize(void)
{
	char			path[1024];
	
		// interface textures
		
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","spot","png");
	bitmap_open(&spot_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","scenery","png");
	bitmap_open(&scenery_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","node","png");
	bitmap_open(&node_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","light","png");
	bitmap_open(&light_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","sound","png");
	bitmap_open(&sound_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","particle","png");
	bitmap_open(&particle_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);
	
		// setup fov
		
	walk_view_set_fov(45);

	return(TRUE);
}

void walk_view_shutdown(void)
{
		// interface textures
		
    bitmap_close(&spot_bitmap);
    bitmap_close(&scenery_bitmap);
    bitmap_close(&node_bitmap);
    bitmap_close(&light_bitmap);
    bitmap_close(&sound_bitmap);
    bitmap_close(&particle_bitmap);
}

/* =======================================================

      Setting up Drawing
      
======================================================= */

void walk_view_setup(bool active,bool full_screen)
{
    Rect			wbox;
	
	walk_view_active=active;

		// deactivated view
		
	if (!active) {
		SetRect(&walk_view_forward_box,-1,-1,-1,-1);
		SetRect(&walk_view_side_box,-1,-1,-1,-1);
		return;
	}
	
		// active view
		
 	GetWindowPortBounds(mainwind,&wbox);
	
	if (full_screen) {
		walk_view_forward_box=wbox;
		walk_view_forward_box.top+=toolbar_high;
		walk_view_forward_box.bottom-=(txt_palette_high+info_high);
		walk_view_forward_box.right-=piece_wid;
		
		SetRect(&walk_view_side_box,-1,-1,-1,-1);
	}
	else {
		if (!main_wind_rot) {
			walk_view_forward_box.top=walk_view_side_box.top=wbox.top+(toolbar_high+2);
			walk_view_forward_box.bottom=walk_view_side_box.bottom=(wbox.top+toolbar_high)+(((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2);

			walk_view_forward_box.left=wbox.left+2;
			walk_view_forward_box.right=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);

			walk_view_side_box.left=walk_view_forward_box.right+2;
			walk_view_side_box.right=wbox.right-(piece_wid+2);
		}
		else {
			walk_view_forward_box.left=walk_view_side_box.left=wbox.left+2;
			walk_view_forward_box.right=walk_view_side_box.right=(wbox.right-piece_wid)-((((wbox.right-piece_wid)-wbox.left)/2)+2);
			
			walk_view_forward_box.top=wbox.top+(toolbar_high+2);
			walk_view_forward_box.bottom=walk_view_side_box.bottom=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)-2);

			walk_view_forward_box.top=walk_view_forward_box.bottom+1;
			walk_view_forward_box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
		}
	}
}

/* =======================================================

      Walk View Portal Start
      
======================================================= */

int walk_view_find_start_portal(void)
{
	int			rn;
	
	rn=map_find_portal(&map,cx,cy,cz);
	if (rn==-1) rn=walk_view_last_rn;
	if (rn==-1) rn=cr;
	
	walk_view_last_rn=rn;
	
	return(rn);
}

/* =======================================================

      Clicking
      
======================================================= */

void walk_view_click(Rect *box,d3pnt *cpt,d3ang *ang,float fov,Point pt,int view_move_dir,bool rot_ok,bool dblclick)
{
        // cmd-view scrolling
        
    if (main_wind_space_down()) {
        walk_view_mouse_xy_movement(ang,pt,view_move_dir);
        return;
    }
    if (main_wind_option_down()) {
        walk_view_mouse_z_movement(ang,pt,view_move_dir);
        return;
    }
    if ((main_wind_command_down()) && (rot_ok)) {
        walk_view_mouse_turn(pt);
        return;
    }
    
        // click the map pieces
    
    walk_view_click_piece(box,cpt,ang,fov,pt,dblclick);
}

/* =======================================================

      Cursors
      
======================================================= */

void walk_view_cursor(bool rot_ok)
{
	if (!walk_view_active) return;

    if (main_wind_space_down()) {
        SetCCursor(handcur);
        return;
    }
    if (main_wind_option_down()) {
        SetCCursor(forwardcur);
        return;
    }
    if ((main_wind_command_down()) && (rot_ok)) {
        SetCCursor(rotatecur);
        return;
    }
	if (main_wind_control_down()) {
		SetCCursor(towardcur);
		return;
	}
    
    InitCursor();
}

/* =======================================================

      Walk View Keys
      
======================================================= */

void walk_view_key(char ch,bool on_side)
{
	piece_key(cr,ch,TRUE,on_side);
}

/* =======================================================

      Reset Edit Window to Portal or Top Window
      
======================================================= */

void walk_view_top_reset(void)
{
    int			x_sz,z_sz;

    x_sz=top_view_box.right-top_view_box.left;
    z_sz=top_view_box.bottom-top_view_box.top;
    
    cx=top_view_x+top_view_pane_to_map_factor(x_sz/2);
    cz=top_view_z+top_view_pane_to_map_factor(z_sz/2);
}

void walk_view_portal_view_reset(void)
{
    int			x_sz,z_sz;

    x_sz=portal_view_box.right-portal_view_box.left;
    z_sz=portal_view_box.bottom-portal_view_box.top;
    
    cx=portal_view_x+top_view_pane_to_map_factor(x_sz/2);
    cz=portal_view_z+top_view_pane_to_map_factor(z_sz/2);
}

void walk_view_site_path_view_reset(void)
{
    int			x_sz,z_sz;

    x_sz=site_path_view_box.right-site_path_view_box.left;
    z_sz=site_path_view_box.bottom-site_path_view_box.top;
    
    cx=site_path_view_x+top_view_pane_to_map_factor(x_sz/2);
    cz=site_path_view_z+top_view_pane_to_map_factor(z_sz/2);
}

