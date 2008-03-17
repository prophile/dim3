/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Portal View Clicking Routines

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
#include "walk_view.h"

extern int					cr,cx,cz,vertex_mode;
extern float				walk_view_y_angle;
extern CCrsrHandle			dragcur,resizecur;
extern WindowRef			mainwind;

extern map_type				map;

/* =======================================================

      Click in Portals
      
======================================================= */

int portal_view_select_portal(d3pnt *pt)
{
	register int	i;
	int				x,z,ex,ez;
	Rect			box;
	
		// find portal clicked on
	
	for (i=0;i!=map.nportal;i++) {
		portal_view_get_portal(i,&x,&z,&ex,&ez);
		SetRect(&box,x,z,(ex+1),(ez+1));
		
		if (main_wind_click_check_box(pt,&box)) return(i);
	}
    
	return(-1);
}

/* =======================================================

      Portal Scrolling
      
======================================================= */

void portal_view_mouse_move(d3pnt *pt)
{
	int						x,y,sx,sy;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	sx=sy=-1;
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;

		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;

		memmove(&old_pt,pt,sizeof(d3pnt));
		
		cx+=(x*map_enlarge);
		cz+=(y*map_enlarge);
		
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Portal Resizing
      
======================================================= */

void portal_view_portal_resize_top_left(d3pnt *pt)
{
	int						x,z,ex,ez,sx,sz,rx,rz,tox,toz,minx,minz,
							lastx,lastz,newx,newz,itemx,itemz,cur_itemx,cur_itemz;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;
	portal_type				*portal;

	portal=&map.portals[cr];
		
	memmove(&old_pt,pt,sizeof(d3pnt));
		
	x=portal->x;
	z=portal->z;
	ex=portal->ex;
	ez=portal->ez;
	
	lastx=x;
	lastz=z;
	
	cur_itemx=cur_itemz=0;
	
	portal_get_size(cr,&minx,&minz);
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;

		rx=old_pt.x-pt->x;
		rz=old_pt.y-pt->y;
		portal_view_distance_pane_to_map(&rx,&rz);
		tox=x-rx;
		toz=z-rz;
		
		if ((ex-tox)<minx) tox=ex-minx;
		if ((ez-toz)<minz) toz=ez-minz;
	
		newx=x;
		newz=z;

		if (tox!=newx) {
			do {
				sx=newx;
				if (tox<newx) newx--;
				if (tox>newx) newx++;

				if (!portal_is_spot_ok(cr,newx,newz,ex,ez)) {
					newx=sx;
					break;
				}
			} while (tox!=newx);
		}
			
		if (toz!=newz) {
			do {
				sz=newz;
				if (toz<newz) newz--;
				if (toz>newz) newz++;

				if (!portal_is_spot_ok(cr,newx,newz,ex,ez)) {
					newz=sz;
					break;
				}
			} while (toz!=newz);
		}
		
		if ((newx!=lastx) || (newz!=lastz)) {
		
			SetCCursor(resizecur);
			
			lastx=newx;
			lastz=newz;
			
				// move items in portal
				
			itemx=(newx-x)-cur_itemx;
			itemz=(newz-z)-cur_itemz;

			portal_move_all_items(cr,itemx,0,itemz);
				
			cur_itemx+=itemx;
			cur_itemz+=itemz;
		
				// change portal size
				
			portal->x=newx;
			portal->z=newz;
			main_wind_draw();
		}

	} while (track!=kMouseTrackingMouseReleased);

	InitCursor();
}

void portal_view_portal_resize_bottom_right(d3pnt *pt)
{
	int						x,z,sx,sz,rx,rz,tox,toz,minx,minz,
							lenx,lenz,orgx,orgz,oldlenx,oldlenz;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;
	portal_type				*portal;

	portal=&map.portals[cr];
		
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	x=portal->x;
	z=portal->z;
	
	oldlenx=portal->ex-x;
	oldlenz=portal->ez-z;
	
	portal_get_size(cr,&minx,&minz);
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;

		rx=pt->x-old_pt.x;
		rz=pt->y-old_pt.y;
		portal_view_distance_pane_to_map(&rx,&rz);
		tox=oldlenx+rx;
		toz=oldlenz+rz;

		if (tox<minx) tox=minx;
		if (toz<minz) toz=minz;
		
		orgx=lenx=portal->ex-x;
		orgz=lenz=portal->ez-z;

		if (tox!=lenx) {
			do {
				sx=lenx;
				if (tox<lenx) lenx--;
				if (tox>lenx) lenx++;

				if (!portal_is_spot_ok(cr,x,z,(x+lenx),(z+lenz))) {
					lenx=sx;
					break;
				}
			} while (tox!=lenx);
		}
			
		if (toz!=lenz) {
			do {
				sz=lenz;
				if (toz<lenz) lenz--;
				if (toz>lenz) lenz++;

				if (!portal_is_spot_ok(cr,x,z,(x+lenx),(z+lenz))) {
					lenz=sz;
					break;
				}
			} while (toz!=lenz);
		}
		
		if ((lenx!=orgx) || (lenz!=orgz)) {
		
				// change portal size
				
			SetCCursor(resizecur);
			portal->ex=x+lenx;
			portal->ez=z+lenz;
			main_wind_draw();
		}

	} while (track!=kMouseTrackingMouseReleased);

	InitCursor();
}

bool portal_view_portal_resize(d3pnt *pt)
{
	int						sx,sz,ex,ez;
	Rect					box;

		// in resize handles?
		
	portal_view_get_portal(cr,&sx,&sz,&ex,&ez);
	
	SetRect(&box,(sx-5),(sz-5),(sx+5),(sz+5));
	if (main_wind_click_check_box(pt,&box)) {
		portal_view_portal_resize_top_left(pt);
		return(TRUE);
	}

	SetRect(&box,(ex-5),(ez-5),(ex+5),(ez+5));
	if (main_wind_click_check_box(pt,&box)) {
		portal_view_portal_resize_bottom_right(pt);
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Portal Snapping
      
======================================================= */

void portal_snap(int rn,d3pnt *pt)
{
	int					n,nx,nz;
	portal_type			*portal;
	
	nx=pt->x;
	nz=pt->z;
	
	for (n=0;n!=map.nportal;n++) {
		if (n==rn) continue;
		
		portal=&map.portals[n];
		
			// top-left
			
		if ((abs(portal->x-nx)<=10) && (abs(portal->z-nz<=10))) {
			pt->x=portal->x;
			pt->z=portal->z;
			return;
		}
		
			// top-right
			
		if ((abs(portal->ex-nx)<=10) && (abs(portal->z-nz<=10))) {
			pt->x=portal->ex;
			pt->z=portal->z;
			return;
		}

			// bottom-left
			
		if ((abs(portal->x-nx)<=10) && (abs(portal->ez-nz<=10))) {
			pt->x=portal->x;
			pt->z=portal->ez;
			return;
		}

			// bottom-right
			
		if ((abs(portal->ex-nx)<=10) && (abs(portal->ez-nz<=10))) {
			pt->x=portal->ex;
			pt->z=portal->ez;
			return;
		}
		
	}
}

/* =======================================================

      Portal Dragging
      
======================================================= */

void portal_view_portal_drag(d3pnt *pt)
{
	int						x,z,ex,ez,px,pz,rx,rz,orgx,orgz,lenx,lenz,sx,sz;
	d3pnt					old_pt,to_pt;
	Point					uipt;
	MouseTrackingResult		track;
	portal_type				*portal;

	if (!Button()) return;
	
	portal=&map.portals[cr];
		
	rx=pt->x;
	rz=pt->y;
	portal_view_pane_to_map(&rx,&rz);
	
	px=portal->x;
	pz=portal->z;
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;
		
		rx=pt->x-old_pt.x;
		rz=pt->y-old_pt.y;
		portal_view_distance_pane_to_map(&rx,&rz);
		to_pt.x=px+rx;
		to_pt.z=pz+rz;

		x=portal->x;
		z=portal->z;
		ex=portal->ex;
		ez=portal->ez;
		
		if (vertex_mode==vertex_mode_snap) portal_snap(cr,&to_pt);
		
		orgx=x;
		orgz=z;
		lenx=ex-x;
		lenz=ez-z;
		
		if (to_pt.x!=x) {
			do {
				sx=x;
				if (to_pt.x<x) x--;
				if (to_pt.x>x) x++;

				if (!portal_is_spot_ok(cr,x,z,(x+lenx),(z+lenz))) {
					x=sx;
					break;
				}
			} while (to_pt.x!=x);
		}
			
		if (to_pt.z!=z) {
			do {
				sz=z;
				if (to_pt.z<z) z--;
				if (to_pt.z>z) z++;

				if (!portal_is_spot_ok(cr,x,z,(x+lenx),(z+lenz))) {
					z=sz;
					break;
				}
			} while (to_pt.z!=z);
		}
		
		if ((x!=orgx) || (z!=orgz)) {
		
				// move portal
				
			SetCCursor(dragcur);
			portal->x=x;
			portal->z=z;
			portal->ex=x+lenx;
			portal->ez=z+lenz;
			main_wind_draw();
		}
		
	} while (track!=kMouseTrackingMouseReleased);

	InitCursor();
}

