/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Site Path View Clicking Routines

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
#include "portal_view.h"
#include "site_path_view.h"
#include "walk_view.h"

extern int					cr,cx,cz,site_path_view_x,site_path_view_z,vertex_mode;
extern float				walk_view_y_angle;
extern Rect					site_path_view_box;
extern CCrsrHandle			dragcur,resizecur;
extern WindowRef			mainwind;

extern map_type				map;

/* =======================================================

      Click in Portals
      
======================================================= */

int site_path_view_select_portal(Point pt)
{
	register int	i;
	int				x,z,ex,ez;
	Rect			box;
	
		// find portal clicked on
	
	for (i=0;i!=map.nportal;i++) {
		site_path_view_get_portal(i,&x,&z,&ex,&ez);
		SetRect(&box,x,z,(ex+1),(ez+1));
		
		if (PtInRect(pt,&box)) return(i);
	}
    
	return(-1);
}

/* =======================================================

      Portal Scrolling
      
======================================================= */

void site_path_view_mouse_move(Point pt)
{
	int						x,y,sx,sy,h,v;
	Point					oldpt;
	MouseTrackingResult		track;

	h=site_path_view_x;
	v=site_path_view_z;
	
	oldpt=pt;
	sx=sy=-1;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		x=oldpt.h-pt.h;
		y=oldpt.v-pt.v;
		
		if ((x==sx) && (y==sy)) continue;
		
		sx=x;
		sy=y;
		
		site_path_view_x=h+x;
		site_path_view_z=v+y;
		
        walk_view_site_path_view_reset();
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Sight Path Utilities
      
======================================================= */

int site_path_view_find_open_sight(void)
{
	register int	n;
	portal_type		*portal;
	
	portal=&map.portals[cr];
	
	for (n=0;n!=max_sight_list;n++) {
		if (portal->sight[n].rn==nopath) return(n);
	}
	
	return(-1);
}

int site_path_view_find_site_for_portal(int rn)
{
	register int	n;
	portal_type		*portal;
	
	portal=&map.portals[cr];
	
	for (n=0;n!=max_sight_list;n++) {
		if (portal->sight[n].rn==rn) return(n);
	}
	
	return(-1);
}

bool site_path_view_path_duplicate(int rn)
{
	register int	n;
	int				r;
	portal_type		*portal;
	
	if (rn==cr) return(TRUE);
	
	portal=&map.portals[cr];
	
	for (n=0;n!=max_sight_list;n++) {
		r=portal->sight[n].rn;
		if ((r!=nopath) && (r==rn)) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Cut Sight Path
      
======================================================= */

bool site_path_view_cut_sight(Point pt)
{
	int				rn,site_idx;
	
		// find sight clicked on
	
	rn=site_path_view_select_portal(pt);
	if (rn==-1) return(FALSE);
	
	site_idx=site_path_view_find_site_for_portal(rn);
	if (site_idx==-1) return(FALSE);
	
        // cut path
	
	map_portal_sight_prune(&map,cr,site_idx);
	
    main_wind_draw();
    return(TRUE);
}

/* =======================================================

      Drag Sight Path
      
======================================================= */

void site_path_view_draw_drag_sight(int rn,int sx,int sy,int tx,int ty)
{
	Rect		box;
	Pattern		gray;
	RGBColor	blackcolor={0x0,0x0,0x0},greencolor={0x0,0xFFFF,0x0};
	
	ClipRect(&site_path_view_box);

		// dragged point
		
	RGBForeColor(&greencolor);
	
	SetRect(&box,(tx-6),(ty-6),(tx+6),(ty+6));
	PaintRect(&box);
	
		// if not dropping on portal, then dot line
		
	if (rn==-1) {
		GetQDGlobalsGray(&gray);
		PenPat(&gray);
	}
	
		// line
		
	PenSize(2,2);
	MoveTo(sx,sy);
	LineTo(tx,ty);
	
	PenNormal();
	
	RGBForeColor(&blackcolor);
}

int site_path_view_sight_drag(int start_rn,int sx,int sy)
{
	int						x,y,old_x,old_y,rn;
	Point					pt;
	MouseTrackingResult		track;

	ClipRect(&site_path_view_box);
	
	old_x=old_y=-1;
	
	site_path_view_draw(start_rn);
	site_path_view_draw_drag_sight(start_rn,sx,sy,sx,sy);
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
	
		rn=site_path_view_select_portal(pt);
		
			// is room already in sight path
			
		if (rn!=-1) {
			if (site_path_view_path_duplicate(rn)) rn=-1;
		}

		x=pt.h;
		y=pt.v;
		
		if ((x==old_x) && (y==old_y)) continue;
		
		site_path_view_draw(rn);
		site_path_view_draw_drag_sight(rn,sx,sy,x,y);

		old_x=x;
		old_y=y;
		
	} while (track!=kMouseTrackingMouseReleased);

	site_path_view_draw(-1);

	return(rn);	
}

/* =======================================================

      Click in Sight Path
      
======================================================= */

bool site_path_view_click_sight(Point pt)
{
	register int	n;
	int				t,i,k,start_rn,rn;
	Rect			box;
	portal_type		*portal;
	
	portal=&map.portals[cr];
	
		// click in portal
		
	start_rn=site_path_view_select_portal(pt);
	
	if (start_rn!=cr) {
		if (site_path_view_find_site_for_portal(start_rn)==-1) return(FALSE);		// this portal not in site path
	}
		
		// run the drag
		
	site_path_view_get_site_path_box(start_rn,&box);
	rn=site_path_view_sight_drag(start_rn,((box.left+box.right)/2),((box.top+box.bottom)/2));

		// path dropping
		
	if (rn!=-1) {
		n=site_path_view_find_open_sight();
		if (n==-1) {
			SysBeep(2);
			return(TRUE);
        }
		
		for (t=0;t!=max_sight_link;t++) {
			portal->sight[n].link[t]=nopath;
		}
            
            // new root path
            
		if (start_rn==cr) {
            portal->sight[n].root=TRUE;
            portal->sight[n].rn=rn;
            main_wind_draw();
            return(TRUE);
        }
        
            // new link in list
		
		k=site_path_view_find_site_for_portal(start_rn);
		
        for (i=0;i!=max_sight_link;i++) {
            if (portal->sight[k].link[i]==nopath) {
                portal->sight[k].link[i]=n;
				portal->sight[n].root=FALSE;
				portal->sight[n].rn=rn;
				main_wind_draw();
                return(TRUE);
            }
        }
		
		SysBeep(2);				// ran out of sight links
	}

	return(TRUE);
}

