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

extern int					cr,cx,cz,portal_view_x,portal_view_z,vertex_mode;
extern float				walk_view_y_angle;
extern Rect					portal_view_box;
extern CCrsrHandle			dragcur,resizecur;
extern WindowRef			mainwind;

extern map_type				map;

/* =======================================================

      Click in Portals
      
======================================================= */

int portal_view_select_portal(Point pt)
{
	register int	i;
	int				x,z,ex,ez;
	Rect			box;
	
		// find portal clicked on
	
	for (i=0;i!=map.nportal;i++) {
		portal_view_get_portal(i,&x,&z,&ex,&ez);
		SetRect(&box,x,z,(ex+1),(ez+1));
		
		if (PtInRect(pt,&box)) return(i);
	}
    
	return(-1);
}

/* =======================================================

      Portal Scrolling
      
======================================================= */

void portal_view_mouse_move(Point pt)
{
	int						x,y,sx,sy,h,v;
	Point					oldpt;
	MouseTrackingResult		track;

	h=portal_view_x;
	v=portal_view_z;
	
	oldpt=pt;
	sx=sy=-1;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		x=oldpt.h-pt.h;
		y=oldpt.v-pt.v;
		
		if ((x==sx) && (y==sy)) continue;
		
		sx=x;
		sy=y;
		
		portal_view_x=h+x;
		portal_view_z=v+y;
		
        walk_view_portal_view_reset();
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Portal Resizing
      
======================================================= */

void portal_view_portal_resize_top_left(Point pt)
{
	int						x,z,ex,ez,sx,sz,rx,rz,tox,toz,minx,minz,
							lastx,lastz,newx,newz,itemx,itemz,cur_itemx,cur_itemz;
	Point					oldpt;
	MouseTrackingResult		track;
	portal_type				*portal;

	portal=&map.portals[cr];
		
	oldpt=pt;
	
	x=portal->x;
	z=portal->z;
	ex=portal->ex;
	ez=portal->ez;
	
	lastx=x;
	lastz=z;
	
	cur_itemx=cur_itemz=0;
	
	portal_get_size(cr,&minx,&minz);
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		rx=oldpt.h-pt.h;
		rz=oldpt.v-pt.v;
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

void portal_view_portal_resize_bottom_right(Point pt)
{
	int						x,z,sx,sz,rx,rz,tox,toz,minx,minz,
							lenx,lenz,orgx,orgz,oldlenx,oldlenz;
	Point					oldpt;
	MouseTrackingResult		track;
	portal_type				*portal;

	portal=&map.portals[cr];
		
	oldpt=pt;
	
	x=portal->x;
	z=portal->z;
	
	oldlenx=portal->ex-x;
	oldlenz=portal->ez-z;
	
	portal_get_size(cr,&minx,&minz);
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		rx=pt.h-oldpt.h;
		rz=pt.v-oldpt.v;
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

bool portal_view_portal_resize(Point pt)
{
	int						x,z,ex,ez;
	Rect					box;

		// in resize handles?
		
	portal_view_get_portal(cr,&x,&z,&ex,&ez);
	
	SetRect(&box,(x-5),(z-5),(x+5),(z+5));
	if (PtInRect(pt,&box)) {
		portal_view_portal_resize_top_left(pt);
		return(TRUE);
	}

	SetRect(&box,(ex-5),(ez-5),(ex+5),(ez+5));
	if (PtInRect(pt,&box)) {
		portal_view_portal_resize_bottom_right(pt);
		return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Portal Snapping
      
======================================================= */

void portal_snap(int rn,int *x,int *z)
{
	register int		n,nx,nz;
	portal_type			*portal;
	
	nx=*x;
	nz=*z;
	
	for (n=0;n!=map.nportal;n++) {
		if (n==rn) continue;
		
		portal=&map.portals[n];
		
			// top-left
			
		if ((abs(portal->x-nx)<=10) && (abs(portal->z-nz<=10))) {
			*x=portal->x;
			*z=portal->z;
			return;
		}
		
			// top-right
			
		if ((abs(portal->ex-nx)<=10) && (abs(portal->z-nz<=10))) {
			*x=portal->ex;
			*z=portal->z;
			return;
		}

			// bottom-left
			
		if ((abs(portal->x-nx)<=10) && (abs(portal->ez-nz<=10))) {
			*x=portal->x;
			*z=portal->ez;
			return;
		}

			// bottom-right
			
		if ((abs(portal->ex-nx)<=10) && (abs(portal->ez-nz<=10))) {
			*x=portal->ex;
			*z=portal->ez;
			return;
		}
		
	}
}

/* =======================================================

      Portal Dragging
      
======================================================= */

void portal_view_portal_drag(Point pt)
{
	int						x,z,ex,ez,tox,toz,px,pz,rx,rz,orgx,orgz,lenx,lenz,sx,sz;
	Point					oldpt;
	MouseTrackingResult		track;
	portal_type				*portal;

	if (!Button()) return;
	
	portal=&map.portals[cr];
	
	rx=pt.h;
	rz=pt.v;
	portal_view_pane_to_map(&rx,&rz);
	
	px=portal->x;
	pz=portal->z;
	
	oldpt=pt;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		
		rx=pt.h-oldpt.h;
		rz=pt.v-oldpt.v;
		portal_view_distance_pane_to_map(&rx,&rz);
		tox=px+rx;
		toz=pz+rz;

		x=portal->x;
		z=portal->z;
		ex=portal->ex;
		ez=portal->ez;
		
		if (vertex_mode==vm_snap) portal_snap(cr,&tox,&toz);
		
		orgx=x;
		orgz=z;
		lenx=ex-x;
		lenz=ez-z;
		
		if (tox!=x) {
			do {
				sx=x;
				if (tox<x) x--;
				if (tox>x) x++;

				if (!portal_is_spot_ok(cr,x,z,(x+lenx),(z+lenz))) {
					x=sx;
					break;
				}
			} while (tox!=x);
		}
			
		if (toz!=z) {
			do {
				sz=z;
				if (toz<z) z--;
				if (toz>z) z++;

				if (!portal_is_spot_ok(cr,x,z,(x+lenx),(z+lenz))) {
					z=sz;
					break;
				}
			} while (toz!=z);
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

