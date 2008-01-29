/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Top View Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

extern int						cr,cx,cz,cy,txt_palette_high;
extern bool						dp_node,dp_y_hide,main_wind_rot;

extern WindowRef				mainwind;
extern CCrsrHandle				handcur,cutcur,addcur;

int								top_view_x,top_view_z,magnify_factor=magnify_size;
bool							top_view_active;
Rect							top_view_box;

/* =======================================================

      Magnify
      
======================================================= */

void top_view_map_to_pane(int *x,int *z)
{
	int			sx,sz;
	
	sx=(*x)-(top_view_x-1);
    *x=((sx*magnify_factor)/magnify_size)+top_view_box.left;
	
	sz=(*z)-(top_view_z-1);
    *z=((sz*magnify_factor)/magnify_size)+top_view_box.top;
}

void top_view_pane_to_map(int *x,int *z)
{
	int			sx,sz;
	
	sx=(*x)-top_view_box.left;
	*x=((sx*magnify_size)/magnify_factor)+(top_view_x-1);
	
	sz=(*z)-top_view_box.top;
	*z=((sz*magnify_size)/magnify_factor)+(top_view_z-1);
}

int top_view_pane_to_map_factor(int k)
{
	return((k*magnify_size)/magnify_factor);
}

void top_view_distance_pane_to_map(int *x,int *z)
{
	*x=((*x)*magnify_size)/magnify_factor;
	*z=((*z)*magnify_size)/magnify_factor;
}

void top_view_drag_pane_to_map(int x,int z,int *rx,int *rz)
{
	*rx=((x+(top_view_x-1))*magnify_size)/magnify_factor;
	*rz=((z+(top_view_z-1))*magnify_size)/magnify_factor;
}

/* =======================================================

      Setup Top View
      
======================================================= */

void top_view_setup(bool active,bool full_screen)
{
    Rect			wbox;
	
	top_view_active=active;
    
		// deactivated view
		
	if (!active) {
		SetRect(&top_view_box,-1,-1,-1,-1);
		return;
	}
	
		// active view
		
 	GetWindowPortBounds(mainwind,&wbox);
	
	if (full_screen) {
		top_view_box=wbox;
		top_view_box.top+=toolbar_high;
		top_view_box.bottom-=(txt_palette_high+info_high);
		top_view_box.right-=piece_wid;
	}
	else {
		if (!main_wind_rot) {
			top_view_box.top=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)+2);
			top_view_box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
			top_view_box.left=wbox.left+2;
			top_view_box.right=wbox.right-(piece_wid+2);
		}
		else {
			top_view_box.top=wbox.top+(toolbar_high+2);
			top_view_box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
			top_view_box.left=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);
			top_view_box.right=wbox.right-(piece_wid+2);
		}
	}
}

/* =======================================================

      Top View Y Hiding
      
======================================================= */

bool top_view_hide_y(fc_segment_data *fc)
{
	int				i,ptsz;
	
		// is Y hiding on?
		
	if (!dp_y_hide) return(FALSE);
	
		// Y hiding
		
	ptsz=fc->ptsz;
	
	for (i=0;i<ptsz;i++) {
		if (labs(fc->y[i]-cy)<=10) return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Draw Top View

======================================================= */

void top_view_draw(void)
{
		// setup viewport
		
	main_wind_set_viewport(&top_view_box,0.75f);

		// draw top down view
		
	top_view_portal_draw();
	top_view_piece_selection_draw();
	top_view_portal_selection_draw();
	top_view_portal_position_draw();
}

/* =======================================================

      Clicking
      
======================================================= */

void top_view_click(Point pt,bool dblclick)
{	
	int				type,portal_idx,main_idx,sub_idx;
	
	if (!top_view_active) return;
    
        // space scrolling

    if (main_wind_space_down()) {
        top_view_portal_hand_scroll(pt);
        return;
    }
    
        // node paths
		
	if (dp_node) {
        
		if (main_wind_option_down()) {
			node_link_cut(pt);
			return;
        }
        
		if (main_wind_control_down()) {
			node_link_add(pt);
			return;
		}
	}
	
        // view rotation
        
    if (top_view_click_position(pt)) return;
    
        // select portal
        
    top_view_portal_select(pt);
	
		// click segment handles
		
	if (top_view_segment_handle_drag(pt)) {
		main_wind_draw();			// supergumba -- combine?
		texture_palette_reset();
		menu_fix_enable();
		main_wind_tool_fix_enable();
		return;
	}
	
		// click pieces
		
	if (top_view_piece_click(pt,&type,&portal_idx,&main_idx,&sub_idx)) {
	
		if (!main_wind_shift_down()) {
			if (!select_check(type,portal_idx,main_idx,sub_idx)) {			// keep selection if selecting an already selected piece
				select_clear();	
				select_add(type,portal_idx,main_idx,sub_idx);
			}
		}
		else {
			select_flip(type,portal_idx,main_idx,sub_idx);
		}
		
		main_wind_draw();
		texture_palette_reset();
		menu_fix_enable();
		main_wind_tool_fix_enable();
		
		if (dblclick) {
			top_view_piece_double_click(type,index);
		}
		else {
			top_view_piece_drag(pt);
		}

		main_wind_draw();
		texture_palette_reset();
		menu_fix_enable();
		main_wind_tool_fix_enable();
	}
}

/* =======================================================

      Cursors
      
======================================================= */

void top_view_cursor(void)
{
	if (!top_view_active) return;
	
    if (main_wind_space_down()) {
        SetCCursor(handcur);
        return;
    }
    
	if (dp_node) {
		if (main_wind_option_down()) {
			SetCCursor(cutcur);
			return;
		}
    
		if (main_wind_control_down()) {
			SetCCursor(addcur);
			return;
		}
    }
	
    InitCursor();
}

/* =======================================================

      Key Presses
      
======================================================= */

void top_view_key(char ch)
{
	piece_key(cr,ch,FALSE,FALSE);
}

/* =======================================================

      Top View Scrolling
      
======================================================= */

void top_view_center(void)
{
	top_view_x=(map_x_size-top_view_pane_to_map_factor(top_view_box.right-top_view_box.left))/2;
	top_view_z=(map_z_size-top_view_pane_to_map_factor(top_view_box.bottom-top_view_box.top))/2;
}

void top_view_reset(void)
{
    int			x_sz,z_sz;

    x_sz=top_view_box.right-top_view_box.left;
    z_sz=top_view_box.bottom-top_view_box.top;
    
    top_view_x=cx-top_view_pane_to_map_factor(x_sz/2);
    top_view_z=cz-top_view_pane_to_map_factor(z_sz/2);
}



