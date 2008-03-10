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

      X/Y Mouse Movement
      
======================================================= */

void walk_view_mouse_xy_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,xadd,zadd,yadd;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;
    
    SetCCursor(handcur);
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;

		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
		switch (view_move_dir) {
			case vm_dir_forward:
				xadd=x;
				yadd=y;
				zadd=0;
				rotate_2D_point_center(&xadd,&zadd,-view_setup->ang.y);
				break;
			case vm_dir_side:
				xadd=-x;
				yadd=y;
				zadd=0;
				rotate_2D_point_center(&xadd,&zadd,view_setup->ang.y);
				break;
			case vm_dir_top:
				xadd=x;
				yadd=0;
				zadd=y;
				break;
			default:
				xadd=yadd=zadd=0;
				break;
		}
		
		cz+=(zadd*32);
		cx+=(xadd*32);
		cy+=(yadd*32);

		walk_view_mouse_reset_portal();
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Walk View Z Movement
      
======================================================= */

void walk_view_mouse_z_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,xadd,zadd,yadd;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;
    
    SetCCursor(forwardcur);

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
		switch (view_move_dir) {
			case vm_dir_forward:
				xadd=0;
				yadd=0;
				zadd=y;
				rotate_2D_point_center(&xadd,&zadd,-view_setup->ang.y);
				break;
			case vm_dir_side:
				xadd=0;
				yadd=0;
				zadd=-y;
				rotate_2D_point_center(&xadd,&zadd,view_setup->ang.y);
				break;
			case vm_dir_top:
				xadd=0;
				yadd=-y;
				zadd=0;
				rotate_2D_point_center(&xadd,&zadd,view_setup->ang.y);
				break;
			default:
				xadd=yadd=zadd=0;
				break;
		}
		
		cz+=(zadd*32);
		cx+=(xadd*32);
		cy+=(yadd*32);

        walk_view_mouse_reset_portal();
        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

void walk_view_scroll_wheel_z_movement(editor_3D_view_setup *view_setup,int delta,int view_move_dir)
{
	int						xadd,zadd,yadd;

	xadd=0;
	yadd=0;
	zadd=delta*10;
		
	rotate_2D_point_center(&xadd,&zadd,angle_add(view_setup->ang.y,180.0f));
	
	cz+=(zadd*32);
	cx+=(xadd*32);
	cy+=(yadd*32);
	
	walk_view_mouse_reset_portal();
	main_wind_draw();
}

/* =======================================================

      Turn Mouse Movement
      
======================================================= */

void walk_view_mouse_turn(d3pnt *pt)
{
	int						x,y;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;
    
    SetCCursor(rotatecur);

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
	
			// y turning
			
		if (labs(x)>25) {
			old_pt.x=pt->x;
			
			if (x>0) {
				walk_view_y_angle=angle_add(walk_view_y_angle,45.0f);
			}
			else {
				walk_view_y_angle=angle_add(walk_view_y_angle,-45.0f);
			}

			main_wind_draw();
			
			continue;
		}
		
			// x turning
			
		if (labs(y)>25) {
			old_pt.y=pt->y;
			
			if (y<0) {
				walk_view_x_angle-=30.0f;
				if (walk_view_x_angle<-30.0f) walk_view_x_angle=-30.0f;
			}
			else {
				walk_view_x_angle+=30.0f;
				if (walk_view_x_angle>30.0f) walk_view_x_angle=30.0f;
			}

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
	cx=((min_x+max_x)/2)+portal->x;
	cz=((min_z+max_z)/2)+portal->z;
	cy=(min_y+max_y)/2;

	main_wind_draw();
}

/* =======================================================

      Find Piece Movement for Walk View
      
======================================================= */

// supergumba -- can probably delete
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

