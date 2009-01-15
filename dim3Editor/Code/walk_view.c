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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int						top_view_x,top_view_z,txt_palette_high;

extern file_path_setup_type		file_path_setup;
extern map_type					map;

d3pnt							view_pnt;
int								walk_view_last_rn;
int								magnify_factor=magnify_size;
float							walk_view_fov,walk_view_y_angle,walk_view_x_angle;
bitmap_type						spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
								light_bitmap,sound_bitmap,particle_bitmap;

/* =======================================================

      Initialize and Shutdown Walk View
	        
======================================================= */

bool walk_view_initialize(void)
{
	char			path[1024];
	
		// interface textures
		
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","spot","png");
	bitmap_open(&spot_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","scenery","png");
	bitmap_open(&scenery_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","node","png");
	bitmap_open(&node_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","node_defined","png");
	bitmap_open(&node_defined_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","light","png");
	bitmap_open(&light_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","sound","png");
	bitmap_open(&sound_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","particle","png");
	bitmap_open(&particle_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	return(TRUE);
}

void walk_view_shutdown(void)
{
		// interface textures
		
    bitmap_close(&spot_bitmap);
    bitmap_close(&scenery_bitmap);
    bitmap_close(&node_bitmap);
    bitmap_close(&node_defined_bitmap);
    bitmap_close(&light_bitmap);
    bitmap_close(&sound_bitmap);
    bitmap_close(&particle_bitmap);
}

/* =======================================================

      Clicking
      
======================================================= */

void walk_view_click(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool rot_ok,bool dblclick)
{
		// node connection changes
		// override some scrolling keys so we
		// need to check them first
		
	if (node_link_click(view_setup,pt)) return;

       // scrolling and movement keys
        
    if (main_wind_space_down()) {
        walk_view_mouse_xy_movement(view_setup,pt,view_move_dir);
        return;
    }

    if (main_wind_option_down()) {
        walk_view_mouse_z_movement(view_setup,pt,view_move_dir);
        return;
    }

    if ((main_wind_command_down()) && (rot_ok)) {
        walk_view_mouse_turn(pt);
        return;
    }

        // click the map pieces
    
    walk_view_click_piece(view_setup,pt,view_move_dir,dblclick);
}

/* =======================================================

      Cursors
      
======================================================= */

void walk_view_cursor(bool rot_ok)
{
		// special check for nodes link changes
		
	if ((select_count()==1) && (select_has_type(node_piece))) {
	
		if (main_wind_option_down()) {
			os_set_cut_cursor();
			return;
		}
		
		if (main_wind_control_down()) {
			os_set_add_cursor();
			return;
		}
	}
	
		// normal cursors

    if (main_wind_space_down()) {
        os_set_hand_cursor();
        return;
    }
    if ((main_wind_option_down()) && ((!main_wind_control_down()) && (!main_wind_shift_down()))) {
        os_set_forward_cursor();
        return;
    }
    if ((main_wind_command_down()) && (rot_ok)) {
        os_set_rotate_cursor();
        return;
    }
    
    os_set_arrow_cursor();
}

/* =======================================================

      Walk View Keys
      
======================================================= */

void walk_view_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch)
{
	piece_key(view_setup,view_move_dir,ch);
}

