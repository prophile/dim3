/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Movement Routines

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
#include "portal_view.h"
#include "top_view.h"
#include "walk_view.h"

extern int						cr,cx,cz,cy;
extern float					walk_view_y_angle,walk_view_x_angle;

extern CCrsrHandle				handcur,forwardcur,rotatecur;

extern map_type					map;

/* =======================================================

      Change Portal for Mouse Movement
      
======================================================= */

void walk_view_mouse_reset_portal(void)
{
    register int		i;
	int					x,z,ex,ez;
    
    for (i=0;i!=map.nportal;i++) {
        x=map.portals[i].x*map_enlarge;
        z=map.portals[i].z*map_enlarge;
        ex=map.portals[i].ex*map_enlarge;
        ez=map.portals[i].ez*map_enlarge;
        
        if ((cx>=x) && (cx<=ex) && (cz>=z) && (cz<=ez)) {
            cr=i;
            return;
        }
    }
}

/* =======================================================

      Find Movement for Walk View
      
======================================================= */

void walk_view_get_forward_movement(int x,int y,int *xadd,int *zadd,int *yadd,bool on_side)
{
	if (!on_side) {
		*xadd=0;
		*zadd=y;
		*yadd=0;
	}
	else {
		*xadd=-y;
		*zadd=0;
		*yadd=0;
	}
	
	rotate_2D_point_center(xadd,zadd,walk_view_y_angle);
}

void walk_view_get_move_movement(int x,int y,int *xadd,int *zadd,int *yadd,bool on_side)
{
	if (!on_side) {
		*xadd=x;
		*zadd=0;
		*yadd=y;
	}
	else {
		*xadd=0;
		*zadd=x;
		*yadd=y;
	}
	
	rotate_2D_point_center(xadd,zadd,walk_view_y_angle);
}

/* =======================================================

      X/Y Mouse Movement
      
======================================================= */

void walk_view_mouse_xy_movement(Point pt,bool on_side)
{
	int						x,y,xadd,zadd,yadd;
	Point					oldpt;
	MouseTrackingResult		track;
    
    SetCCursor(handcur);
	
	oldpt=pt;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		
		if ((pt.h==oldpt.h) && (pt.v==oldpt.v)) continue;

		x=oldpt.h-pt.h;
		y=oldpt.v-pt.v;
		oldpt=pt;
	
		walk_view_get_move_movement(x,y,&xadd,&zadd,&yadd,on_side);
		
		cz+=(zadd*32);
		cx+=(xadd*32);
		cy+=(yadd*32);

		walk_view_mouse_reset_portal();
        top_view_reset();
		portal_view_reset();
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Walk View Z Movement
      
======================================================= */

void walk_view_mouse_z_movement(Point pt,bool on_side)
{
	int						x,y,xadd,zadd,yadd;
	Point					oldpt;
	MouseTrackingResult		track;
    
    SetCCursor(forwardcur);

	oldpt=pt;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		if ((pt.h==oldpt.h) && (pt.v==oldpt.v)) continue;
		
		x=oldpt.h-pt.h;
		y=oldpt.v-pt.v;
		oldpt=pt;
	
		walk_view_get_forward_movement(x,y,&xadd,&zadd,&yadd,on_side);
		
		cz+=(zadd*32);
		cx+=(xadd*32);
		cy+=(yadd*32);

        walk_view_mouse_reset_portal();
        top_view_reset();
		portal_view_reset();
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

void walk_view_scroll_wheel_z_movement(int delta,bool on_side)
{
	int						xadd,zadd,yadd;

	walk_view_get_forward_movement(0,-(delta*10),&xadd,&zadd,&yadd,on_side);
	
	cz+=(zadd*32);
	cx+=(xadd*32);
	cy+=(yadd*32);
	
	walk_view_mouse_reset_portal();
	top_view_reset();
	portal_view_reset();
	main_wind_draw();
}

/* =======================================================

      Turn Mouse Movement
      
======================================================= */

void walk_view_mouse_turn(Point pt)
{
	int						x,y;
	Point					oldpt;
	MouseTrackingResult		track;
    
    SetCCursor(rotatecur);

	oldpt=pt;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		
		x=oldpt.h-pt.h;
		y=oldpt.v-pt.v;
	
			// y turning
			
		if (labs(x)>25) {
			oldpt=pt;
			
			if (x>0) {
				walk_view_y_angle=angle_add(walk_view_y_angle,-45);
			}
			else {
				walk_view_y_angle=angle_add(walk_view_y_angle,45);
			}

			top_view_reset();
			portal_view_reset();
			main_wind_draw();
			
			continue;
		}
		
			// x turning
			
		if (labs(y)>25) {
			oldpt=pt;
			
			if (y<0) {
				walk_view_x_angle-=30;
				if (walk_view_x_angle<-30) walk_view_x_angle=-30;
			}
			else {
				walk_view_x_angle+=30;
				if (walk_view_x_angle>30) walk_view_x_angle=30;
			}

			top_view_reset();
			portal_view_reset();
			main_wind_draw();
			
			continue;
		}
			
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Walk View Go To Portal Top/Bottom/Selection
      
======================================================= */

void walk_view_portal_go_to_top(void)
{
	register int			n,ty;
	register segment_type	*seg;
	
	ty=10000;
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
		if (seg->rn==cr) {
			switch (seg->type) {
				case sg_wall:
					if (seg->data.wall.ty<ty) ty=seg->data.wall.ty;
					break;
				case sg_floor:
				case sg_ceiling:
					if (seg->data.fc.y[0]<ty) ty=seg->data.fc.y[0];
					break;
			}
		}
		seg++;
	}
	
	if (ty==10000) return;
	
	cy=ty*map_enlarge;
	main_wind_draw();
}

void walk_view_portal_go_to_bottom(void)
{
	register int			n,by;
	register segment_type	*seg;
	
	by=0;
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
		if (seg->rn==cr) {
			switch (seg->type) {
				case sg_wall:
					if (seg->data.wall.by>by) by=seg->data.wall.by;
					break;
				case sg_floor:
				case sg_ceiling:
					if (seg->data.fc.y[0]>by) by=seg->data.fc.y[0];
					break;
			}
		}
		seg++;
	}
	
	if (by==0) return;
	
	cy=by*map_enlarge;
	main_wind_draw();
}

void walk_view_portal_go_to_selection(void)
{
	int				min_x,min_z,min_y,max_x,max_z,max_y;
	portal_type		*portal;
	
	select_get_extent(&min_x,&min_z,&min_y,&max_x,&max_z,&max_y);
	
	portal=&map.portals[cr];
	cx=(((min_x+max_x)/2)+portal->x)*map_enlarge;
	cz=(((min_z+max_z)/2)+portal->z)*map_enlarge;
	cy=((min_y+max_y)/2)*map_enlarge;

	main_wind_draw();
}

/* =======================================================

      Find Piece Movement for Walk View
      
======================================================= */

void walk_view_get_piece_movement(bool cmdkey,int x,int y,int *xadd,int *zadd,int *yadd)
{
	*xadd=*zadd=*yadd=0;
	
	if (!cmdkey) {
		*xadd=x;
		*zadd=0;
		*yadd=y;
	}
	else {
		*xadd=0;
		*zadd=y;
		*yadd=0;
	}
	
	rotate_2D_point_center(xadd,zadd,walk_view_y_angle);
}

