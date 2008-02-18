/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Portal View Routines

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

extern int						cr,cx,cz,magnify_factor,txt_palette_high;
extern bool						main_wind_rot;
extern map_type					map;

extern WindowRef				mainwind;
extern CCrsrHandle				handcur,cutcur,addcur;

Rect							portal_view_box;

/* =======================================================

      Setup Portal View
      
======================================================= */

void portal_view_setup(bool active,bool full_screen)
{
    Rect			wbox;
	
		// deactived view
		
	if (!active) {
		SetRect(&portal_view_box,-1,-1,-1,-1);
		return;
	}
	
		// active view
    
 	GetWindowPortBounds(mainwind,&wbox);
	
	if (full_screen) {
		portal_view_box=wbox;
		portal_view_box.top+=toolbar_high;
		portal_view_box.bottom-=(txt_palette_high+info_high);
		portal_view_box.right-=piece_wid;
	}
	else {
		if (!main_wind_rot) {
			portal_view_box.top=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)+2);
			portal_view_box.bottom=wbox.bottom-2;
			portal_view_box.left=wbox.left+2;
			portal_view_box.right=wbox.right-(piece_wid+2);
		}
		else {
			portal_view_box.top=wbox.top+(toolbar_high+2);
			portal_view_box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
			portal_view_box.left=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);
			portal_view_box.right=wbox.right-(piece_wid+2);
		}
	}
}

/* =======================================================

      Portal Map to Pane Transformations

======================================================= */

int portal_view_pane_to_map_factor(int k)
{
	return((k*magnify_size)/magnify_factor);
}

void portal_view_map_to_pane(int *x,int *z)
{
	int			sx,sz,view_x,view_z;
	
    view_x=cx-portal_view_pane_to_map_factor((portal_view_box.right-portal_view_box.left)>>1);
    view_z=cz-portal_view_pane_to_map_factor((portal_view_box.bottom-portal_view_box.top)>>1);

	sx=(*x)-(view_x-1);
    *x=((sx*magnify_factor)/magnify_size)+portal_view_box.left;
	
	sz=(*z)-(view_z-1);
    *z=((sz*magnify_factor)/magnify_size)+portal_view_box.top;
}

void portal_view_pane_to_map(int *x,int *z)
{
	int			sx,sz,view_x,view_z;
	
    view_x=cx-portal_view_pane_to_map_factor((portal_view_box.right-portal_view_box.left)>>1);
    view_z=cz-portal_view_pane_to_map_factor((portal_view_box.bottom-portal_view_box.top)>>1);
	
	sx=(*x)-portal_view_box.left;
	*x=((sx*magnify_size)/magnify_factor)+(view_x-1);
	
	sz=(*z)-portal_view_box.top;
	*z=((sz*magnify_size)/magnify_factor)+(view_z-1);
}

void portal_view_distance_pane_to_map(int *x,int *z)
{
	*x=(((*x)*magnify_size)/magnify_factor);
	*z=(((*z)*magnify_size)/magnify_factor);
}

/* =======================================================

      Draw Portal View

======================================================= */

void portal_view_draw(void)
{
		// setup viewport
		
	main_wind_set_viewport(&portal_view_box,0.75f);
	
		// draw portal pieces
		
	portal_view_draw_portals();
	portal_view_draw_selection();
	portal_view_draw_position();
}

/* =======================================================

      Clicking
      
======================================================= */

void portal_view_click(d3pnt *pt,bool dblclick)
{
    int			rn;
    
        // cmd-portal scrolling

    if (main_wind_space_down()) {
        portal_view_mouse_move(pt);
        return;
    }

        // portal resizing?
        
	if (portal_view_portal_resize(pt)) return;
    
		// select portal
		
	rn=portal_view_select_portal(pt);
	if (rn==-1) return;
	
	if (cr!=rn) {
		cr=rn;
		main_wind_draw();
	}
	
		// double click brings up info
		
	if (dblclick) {
		dialog_portal_settings_run(&map.portals[cr]);
		return;
	}
	
		// if single click, then we are dragging
		
	portal_view_portal_drag(pt);
}

/* =======================================================

      Cursors
      
======================================================= */

void portal_view_cursor(void)
{
    if (main_wind_space_down()) {
        SetCCursor(handcur);
        return;
    }

    InitCursor();
}

/* =======================================================

      Key Presses
      
======================================================= */

void portal_view_key(char ch)
{
	portal_type		*portal;
	
    if (cr==-1) return;
	
	portal=&map.portals[cr];
    
	switch (ch) {
		case 0x1C:
			if (portal->x==0) break;
			if (!portal_is_spot_ok(cr,(portal->x-1),portal->z,(portal->ex-1),portal->ez)) break;
			portal->x--;
			portal->ex--;
			main_wind_draw();
			break;
		case 0x1D:
			if (portal->ex==map_x_size) break;
			if (!portal_is_spot_ok(cr,(portal->x+1),portal->z,(portal->ex+1),portal->ez)) break;
			portal->x++;
			portal->ex++;
			main_wind_draw();
			break;
		case 0x1E:
			if (portal->z==0) break;
			if (!portal_is_spot_ok(cr,portal->x,(portal->z-1),portal->ex,(portal->ez-1))) break;
			portal->z--;
			portal->ez--;
			main_wind_draw();
			break;
		case 0x1F:
			if (portal->ez==map_z_size) break;
			if (!portal_is_spot_ok(cr,portal->x,(portal->z+1),portal->ex,(portal->ez+1))) break;
			portal->z++;
			portal->ez++;
			main_wind_draw();
			break;
	}
}

