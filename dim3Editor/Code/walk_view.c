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

int								cx,cz,cy;
float							walk_view_fov,walk_view_y_angle,walk_view_x_angle;
bool							walk_view_active;
Rect							walk_view_box;
bitmap_type						spot_bitmap,scenery_bitmap,node_bitmap,
								light_bitmap,sound_bitmap,particle_bitmap;
AGLContext						ctx;

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
	GLint			attrib[]={AGL_NO_RECOVERY,AGL_RGBA,AGL_DOUBLEBUFFER,AGL_ACCELERATED,AGL_PIXEL_SIZE,24,AGL_ALPHA_SIZE,8,AGL_DEPTH_SIZE,16,AGL_NONE};
	GDHandle		gdevice;
	AGLPixelFormat	pf;

		// gl setup
		
	gdevice=GetMainDevice();

	pf=aglChoosePixelFormat(&gdevice,1,attrib);
	ctx=aglCreateContext(pf,NULL);
	aglSetCurrentContext(ctx);
	aglDestroyPixelFormat(pf);

	glEnable(GL_SMOOTH);
	glEnable(GL_DITHER);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	
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
	
		// gl shutdown
		
	aglSetCurrentContext(NULL);
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
}

/* =======================================================

      Setting up Drawing
      
======================================================= */

void walk_view_setup(bool active,bool full_screen)
{
	int				bot_y;
    Rect			wbox;
	GLint			rect[4];
	
	walk_view_active=active;
    
		// deactivated view
		
	if (!active) {
		aglSetDrawable(ctx,NULL);
		SetRect(&walk_view_box,-1,-1,-1,-1);
		return;
	}
	
		// active view
		
 	GetWindowPortBounds(mainwind,&wbox);
	bot_y=wbox.bottom;
		
	if (full_screen) {
		walk_view_box=wbox;
		walk_view_box.top+=toolbar_high;
		walk_view_box.bottom-=(txt_palette_high+info_high);
		walk_view_box.right-=piece_wid;
	}
	else {
		if (!main_wind_rot) {
			walk_view_box.top=wbox.top+(toolbar_high+2);
			walk_view_box.bottom=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)-2);
			walk_view_box.left=wbox.left+2;
			walk_view_box.right=wbox.right-(piece_wid+2);
		}
		else {
			walk_view_box.top=wbox.top+(toolbar_high+2);
			walk_view_box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
			walk_view_box.left=wbox.left+2;
			walk_view_box.right=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);
		}
	}
	
		// setup opengl
		
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(mainwind));

	rect[0]=walk_view_box.left;
	rect[1]=bot_y-walk_view_box.bottom;
	rect[2]=walk_view_box.right-walk_view_box.left;
	rect[3]=walk_view_box.bottom-walk_view_box.top;
	
	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);

	glEnable(GL_SCISSOR_TEST);
	glScissor(0,0,(walk_view_box.right-walk_view_box.left),(walk_view_box.bottom-walk_view_box.top));

	glViewport(0,0,(walk_view_box.right-walk_view_box.left),(walk_view_box.bottom-walk_view_box.top));
}

/* =======================================================

      Clicking
      
======================================================= */

void walk_view_click(Point pt,bool dblclick)
{
    int			xorg,yorg;

	if (!walk_view_active) return;
    
    xorg=walk_view_box.left;
    yorg=walk_view_box.top;

        // cmd-view scrolling
        
    if (main_wind_space_down()) {
        walk_view_mouse_xy_movement(pt);
        return;
    }
    if (main_wind_option_down()) {
        walk_view_mouse_z_movement(pt);
        return;
    }
    if (main_wind_command_down()) {
        walk_view_mouse_turn(pt);
        return;
    }
	
		// click the compass
		
	if (walk_view_compass_click(pt)) return;
    
        // click the map pieces
    
    walk_view_click_piece(xorg,yorg,pt,dblclick);
}

/* =======================================================

      Cursors
      
======================================================= */

void walk_view_cursor(void)
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
    if (main_wind_command_down()) {
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

void walk_view_key(char ch)
{
	if (walk_view_active) piece_key(cr,ch,TRUE);
}

/* =======================================================

      Reset Edit Window to Portal or Top Window
      
======================================================= */

void walk_view_top_reset(void)
{
    int			x_sz,z_sz;

    x_sz=top_view_box.right-top_view_box.left;
    z_sz=top_view_box.bottom-top_view_box.top;
    
    cx=(top_view_x+top_view_pane_to_map_factor(x_sz/2))*map_enlarge;
    cz=(top_view_z+top_view_pane_to_map_factor(z_sz/2))*map_enlarge;
}

void walk_view_portal_view_reset(void)
{
    int			x_sz,z_sz;

    x_sz=portal_view_box.right-portal_view_box.left;
    z_sz=portal_view_box.bottom-portal_view_box.top;
    
    cx=(portal_view_x+top_view_pane_to_map_factor(x_sz/2))*map_enlarge;
    cz=(portal_view_z+top_view_pane_to_map_factor(z_sz/2))*map_enlarge;
}

void walk_view_site_path_view_reset(void)
{
    int			x_sz,z_sz;

    x_sz=site_path_view_box.right-site_path_view_box.left;
    z_sz=site_path_view_box.bottom-site_path_view_box.top;
    
    cx=(site_path_view_x+top_view_pane_to_map_factor(x_sz/2))*map_enlarge;
    cz=(site_path_view_z+top_view_pane_to_map_factor(z_sz/2))*map_enlarge;
}

