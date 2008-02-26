/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Site Path View Routines

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
#include "dialog.h"
#include "common_view.h"
#include "portal_view.h"
#include "site_path_view.h"

extern int						cr,cx,cz,magnify_factor,txt_palette_high;
extern map_type					map;

extern WindowRef				mainwind;
extern CCrsrHandle				handcur,cutcur,addcur;

int								site_path_view_drag_hilite_rn;
bool							site_path_view_drag_on;
d3pnt							site_path_view_drag_start_pt,site_path_view_drag_end_pt;

/* =======================================================

      Site Path Map to Pane Transformations

======================================================= */

int site_path_view_pane_to_map_factor(int k)
{
	return((k*magnify_size)/magnify_factor);
}

void site_path_view_map_to_pane(int *x,int *z)
{
	int			sx,sz,view_x,view_z;
	Rect		box;
	
	main_wind_setup_full_screen_box(&box);
	
    view_x=cx-site_path_view_pane_to_map_factor((box.right-box.left)>>1);
	view_z=cz-site_path_view_pane_to_map_factor((box.bottom-box.top)>>1);
	
	sx=(*x)-(view_x-1);
    *x=((sx*magnify_factor)/magnify_size)+box.left;
	
	sz=(*z)-(view_z-1);
    *z=((sz*magnify_factor)/magnify_size)+box.top;
}

void site_path_view_pane_to_map(int *x,int *z)
{
	int			sx,sz,view_x,view_z;
	Rect		box;
	
	main_wind_setup_full_screen_box(&box);
	
    view_x=cx-site_path_view_pane_to_map_factor((box.right-box.left)>>1);
	view_z=cz-site_path_view_pane_to_map_factor((box.bottom-box.top)>>1);

	sx=(*x)-box.left;
	*x=((sx*magnify_size)/magnify_factor)+(view_x-1);
	
	sz=(*z)-box.top;
	*z=((sz*magnify_size)/magnify_factor)+(view_z-1);
}

void site_path_view_distance_pane_to_map(int *x,int *z)
{
	*x=(((*x)*magnify_size)/magnify_factor);
	*z=(((*z)*magnify_size)/magnify_factor);
}

/* =======================================================

      Draw Site Path View

======================================================= */

void site_path_view_draw(void)
{
	Rect			box;
	
		// setup viewport
		
	main_wind_setup_full_screen_box(&box);
	main_wind_set_viewport(&box,0.75f);
	
		// draw portals
	
	site_path_view_draw_portals();

		// portal sight path
		
	site_path_view_draw_portal_sight_path(cr);

		// draw cursor position
		
	site_path_view_draw_position();
}

/* =======================================================

      Clicking
      
======================================================= */

void site_path_view_click(d3pnt *pt,bool dblclick)
{
    int			rn;
    
        // cmd-portal scrolling

    if (main_wind_space_down()) {
        site_path_view_mouse_move(pt);
        return;
    }

		// double clicking selects portal
		
	if (dblclick) {
	
		rn=site_path_view_select_portal(pt);
		if (cr!=rn) {
			cr=rn;
			main_wind_draw();
		}
		
		return;
	}

        // path cutting
        
    if (main_wind_option_down()) {
		site_path_view_cut_sight(pt);
		return;
    }
    
        // check paths first
        
    if (site_path_view_click_sight(pt)) return;
}

/* =======================================================

      Cursors
      
======================================================= */

void site_path_view_cursor(void)
{
    if (main_wind_space_down()) {
        SetCCursor(handcur);
        return;
    }

    if (main_wind_option_down()) {
        SetCCursor(cutcur);
        return;
    }

    InitCursor();
}

/* =======================================================

      Key Presses
      
======================================================= */

void site_path_view_key(char ch)
{
}




