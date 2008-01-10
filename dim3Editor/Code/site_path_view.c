/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Site Path View Routines

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
#include "dialog.h"
#include "common_view.h"
#include "portal_view.h"
#include "site_path_view.h"
#include "top_view.h"

extern int						cr,cx,cz,magnify_factor,txt_palette_high;
extern bool						main_wind_rot;
extern map_type					map;

extern WindowRef				mainwind;
extern CCrsrHandle				handcur,cutcur,addcur;

int								site_path_view_x,site_path_view_z;
bool							site_path_view_active;
Rect							site_path_view_box;

/* =======================================================

      Setup Site Path View
      
======================================================= */

void site_path_view_setup(bool active,bool full_screen)
{
    Rect			wbox;
	
	site_path_view_active=active;
	
		// deactived view
		
	if (!active) {
		SetRect(&site_path_view_box,-1,-1,-1,-1);
		return;
	}
	
		// active view
    
 	GetWindowPortBounds(mainwind,&wbox);
	
	if (full_screen) {
		site_path_view_box=wbox;
		site_path_view_box.top+=toolbar_high;
		site_path_view_box.bottom-=(txt_palette_high+info_high);
		site_path_view_box.right-=piece_wid;
	}
	else {
		if (!main_wind_rot) {
			site_path_view_box.top=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)+2);
			site_path_view_box.bottom=wbox.bottom-2;
			site_path_view_box.left=wbox.left+2;
			site_path_view_box.right=wbox.right-(piece_wid+2);
		}
		else {
			site_path_view_box.top=wbox.top+(toolbar_high+2);
			site_path_view_box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
			site_path_view_box.left=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);
			site_path_view_box.right=wbox.right-2;
		}
	}
}

/* =======================================================

      Site Path Map to Pane Transformations

======================================================= */

void site_path_view_map_to_pane(int *x,int *z)
{
	int			sx,sz;
	
	sx=(*x)-(site_path_view_x-1);
    *x=((sx*magnify_factor)/40)+site_path_view_box.left;
	
	sz=(*z)-(site_path_view_z-1);
    *z=((sz*magnify_factor)/40)+site_path_view_box.top;
}

void site_path_view_pane_to_map(int *x,int *z)
{
	int			sx,sz;
	
	sx=(*x)-site_path_view_box.left;
	*x=((sx*40)/magnify_factor)+(site_path_view_x-1);
	
	sz=(*z)-site_path_view_box.top;
	*z=((sz*40)/magnify_factor)+(site_path_view_z-1);
}

void site_path_view_distance_pane_to_map(int *x,int *z)
{
	*x=(((*x)*40)/magnify_factor);
	*z=(((*z)*40)/magnify_factor);
}

/* =======================================================

      Draw Site Path View

======================================================= */

void site_path_view_draw(int drag_hilite_rn)
{
	int				i,x,z,ex,ez;
	Rect			wbox,box;
	RGBColor		blackcolor={0x0,0x0,0x0},ltltgraycolor={0xEEEE,0xEEEE,0xEEEE},
					ltgraycolor={0x9999,0x9999,0x9999},graycolor={0x7FFF,0x7FFF,0x7FFF},dkgraycolor={0x1FFF,0x1FFF,0x1FFF};
	
	if (!site_path_view_active) return;

	ClipRect(&site_path_view_box);
	
	RGBForeColor(&ltltgraycolor);
    PaintRect(&site_path_view_box);
	
		// draw the site path background and outlines
		
	for (i=0;i!=map.nportal;i++) {
		site_path_view_get_portal(i,&x,&z,&ex,&ez);
		SetRect(&box,x,z,(ex+1),(ez+1));
			
		if (i==cr) {
			RGBForeColor(&ltgraycolor);
		}
		else {
			if ((site_path_portal_in_path(cr,i)) || (drag_hilite_rn==i)) {
				RGBForeColor(&graycolor);
			}
			else {
				RGBForeColor(&dkgraycolor);
			}
		}
		PaintRect(&box);
			
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// draw the portal walls
		
	for (i=0;i!=map.nportal;i++) {
		site_path_view_get_portal(i,&x,&z,&ex,&ez);
		site_path_view_draw_portal_walls(i);
	}
	
		// portal sight path
		
	site_path_view_draw_portal_sight_path(cr);
		
		// reset clip
	
	GetWindowPortBounds(mainwind,&wbox);
	ClipRect(&wbox);
	
	RGBForeColor(&blackcolor);
}

/* =======================================================

      Clicking
      
======================================================= */

void site_path_view_click(Point pt,bool dblclick)
{
    int			rn;
	
	if (!site_path_view_active) return;
    
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
			site_path_view_draw(-1);
			info_status_line_draw();
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
	if (!site_path_view_active) return;
    
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
/*
	portal_type		*portal;
	
	if (!site_path_view_active) return;
*/
}

/* =======================================================

      Site Path Window Scrolling
      
======================================================= */

void site_path_view_center(void)
{
	site_path_view_x=(map_x_size-top_view_pane_to_map_factor(site_path_view_box.right-site_path_view_box.left))/2;
	site_path_view_z=(map_z_size-top_view_pane_to_map_factor(site_path_view_box.bottom-site_path_view_box.top))/2;
}

void site_path_view_reset(void)
{
    int			x_sz,z_sz;

    x_sz=site_path_view_box.right-site_path_view_box.left;
    z_sz=site_path_view_box.bottom-site_path_view_box.top;
    
    site_path_view_x=cx-top_view_pane_to_map_factor(x_sz/2);
    site_path_view_z=cz-top_view_pane_to_map_factor(z_sz/2);
}




