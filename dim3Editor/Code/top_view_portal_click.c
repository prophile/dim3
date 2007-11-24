/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Top View Portal Click Routines

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
#include "portal_view.h"
#include "top_view.h"
#include "walk_view.h"

extern CCrsrHandle			dragcur,resizecur;

extern int					cr,cx,cz,cy,top_view_x,top_view_z;
extern float				walk_view_y_angle;

extern map_type				map;

/* =======================================================

      Find Portal For Point
      
======================================================= */

int top_view_portal_find(Point pt)
{
	int			n;
	
		// check selected room first
		
    if (portal_point_in_box(cr,pt)) return(cr);
	
		// portals
	
	for (n=0;n!=map.nportal;n++) {
		if (portal_point_in_box(n,pt)) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Select Portal
	  
======================================================= */

bool top_view_portal_select(Point pt)
{
	int				t;
	
	t=top_view_portal_find(pt);
    if (t==-1) return(FALSE);
	
	if (cr!=t) {
		cr=t;
        select_clear();
        
 		main_wind_draw();
		texture_palette_reset();
        
		menu_fix_enable();
		main_wind_tool_fix_enable();
    }
    
	return(TRUE);
}

/* =======================================================

      Mouse Scrolling
      
======================================================= */

void top_view_portal_hand_scroll(Point pt)
{
	int						x,y,sx,sy,h,v;
	Point					oldpt;
	MouseTrackingResult		track;

	h=top_view_x;
	v=top_view_z;
	
	oldpt=pt;
	sx=sy=-1;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		x=oldpt.h-pt.h;
		y=oldpt.v-pt.v;
		
		if ((x==sx) && (y==sy)) continue;
		
		sx=x;
		sy=y;
		
		top_view_x=h+x;
		top_view_z=v+y;
		
        walk_view_top_reset();
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Click in Position
      
======================================================= */

bool top_view_click_position(Point pt)
{
    int			x,z;
    Rect		box;
    
	x=cx/map_enlarge;
	z=cz/map_enlarge;
	top_view_map_to_pane(&x,&z);
    
    SetRect(&box,(x-5),(z-5),(x+5),(z+5));
    if (!PtInRect(pt,&box)) return(FALSE);
    
	walk_view_y_angle=angle_add(walk_view_y_angle,45);
    
    main_wind_draw();
    
    return(TRUE);
}


