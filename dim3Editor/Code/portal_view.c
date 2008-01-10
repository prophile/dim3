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
#include "top_view.h"

extern int						cr,cx,cz,magnify_factor,txt_palette_high;
extern bool						main_wind_rot;
extern map_type					map;

extern WindowRef				mainwind;
extern CCrsrHandle				handcur,cutcur,addcur;

int								portal_view_x,portal_view_z;
bool							portal_view_active;
Rect							portal_view_box;

/* =======================================================

      Setup Portal View
      
======================================================= */

void portal_view_setup(bool active,bool full_screen)
{
    Rect			wbox;
	
	portal_view_active=active;
	
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

void portal_view_map_to_pane(int *x,int *z)
{
	int			sx,sz;
	
	sx=(*x)-(portal_view_x-1);
    *x=((sx*magnify_factor)/40)+portal_view_box.left;
	
	sz=(*z)-(portal_view_z-1);
    *z=((sz*magnify_factor)/40)+portal_view_box.top;
}

void portal_view_pane_to_map(int *x,int *z)
{
	int			sx,sz;
	
	sx=(*x)-portal_view_box.left;
	*x=((sx*40)/magnify_factor)+(portal_view_x-1);
	
	sz=(*z)-portal_view_box.top;
	*z=((sz*40)/magnify_factor)+(portal_view_z-1);
}

void portal_view_distance_pane_to_map(int *x,int *z)
{
	*x=(((*x)*40)/magnify_factor);
	*z=(((*z)*40)/magnify_factor);
}

/* =======================================================

      Draw Portal View

======================================================= */

void portal_view_draw(void)
{
	int				i,x,z,ex,ez;
	Rect			wbox,box;
	Pattern			gray;
	RGBColor		blackcolor={0x0,0x0,0x0},whitecolor={0xFFFF,0xFFFF,0xFFFF},ltltgraycolor={0xEEEE,0xEEEE,0xEEEE},
					purplecolor={0x9999,0x0,0x6666};
	
	if (!portal_view_active) return;

	ClipRect(&portal_view_box);
	
	RGBForeColor(&ltltgraycolor);
    PaintRect(&portal_view_box);
	
		// draw the portal background and outlines
		
	for (i=0;i!=map.nportal;i++) {
		portal_view_get_portal(i,&x,&z,&ex,&ez);
		SetRect(&box,x,z,(ex+1),(ez+1));
			
		RGBForeColor(&whitecolor);
		PaintRect(&box);
			
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// selected portal
		
	portal_view_get_portal(cr,&x,&z,&ex,&ez);
	SetRect(&box,x,z,(ex+1),(ez+1));

	RGBForeColor(&purplecolor);
	PenSize(3,3);
	GetQDGlobalsGray(&gray);
	PenPat(&gray);
	FrameRect(&box);
	PenNormal();
	
		// draw the portal walls
		
	for (i=0;i!=map.nportal;i++) {
		portal_view_get_portal(i,&x,&z,&ex,&ez);
		portal_view_draw_portal_walls(i);
	}
	
		// portal resize handles
		
	portal_view_get_portal(cr,&x,&z,&ex,&ez);

	RGBForeColor(&purplecolor);

	SetRect(&box,(x-5),(z-5),(x+5),(z+5));
	PaintRect(&box);
	
	SetRect(&box,(ex-5),(ez-5),(ex+5),(ez+5));
	PaintRect(&box);
		
		// reset clip
	
	GetWindowPortBounds(mainwind,&wbox);
	ClipRect(&wbox);
	
	RGBForeColor(&blackcolor);
}

/* =======================================================

      Clicking
      
======================================================= */

void portal_view_click(Point pt,bool dblclick)
{
    int			rn;
	
	if (!portal_view_active) return;
    
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
		portal_view_draw();
		info_status_line_draw();
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
	if (!portal_view_active) return;
    
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

void portal_view_key(char ch)
{
	portal_type		*portal;
	
	if (!portal_view_active) return;

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

/* =======================================================

      Portal Window Scrolling
      
======================================================= */

void portal_view_center(void)
{
	portal_view_x=(map_x_size-top_view_pane_to_map_factor(portal_view_box.right-portal_view_box.left))/2;
	portal_view_z=(map_z_size-top_view_pane_to_map_factor(portal_view_box.bottom-portal_view_box.top))/2;
}

void portal_view_reset(void)
{
    int			x_sz,z_sz;

    x_sz=portal_view_box.right-portal_view_box.left;
    z_sz=portal_view_box.bottom-portal_view_box.top;
    
    portal_view_x=cx-top_view_pane_to_map_factor(x_sz/2);
    portal_view_z=cz-top_view_pane_to_map_factor(z_sz/2);
}




