/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Handle Drawing Routines

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
#include "walk_view.h"

extern int					cr,cx,cz,cy,vertex_mode;
extern CCrsrHandle			dragcur;

extern map_type				map;

/* =======================================================

      View Draws Handles
      
======================================================= */

void walk_view_draw_sel_handles(int ptsz,int *x,int *y,int *z)
{
	int			t;

	glDisable(GL_DEPTH_TEST);
	
    glPointSize(10);
 	glColor4f(1,0.8,0,1);
    
	for ((t=0);(t<ptsz);t++) {
		glBegin(GL_POINTS);
        glVertex3i((x[t]-cx),(y[t]-cy),(cz-z[t]));
		glEnd();
		glPassThrough((float)t);				// for feedback selection
    }
    
    glPointSize(1);
	
	glEnable(GL_DEPTH_TEST);
}

void walk_view_draw_segment_handles(void)
{
/* supergumba
	int				t,rn,type,index,primitive_uid,
					xadd,zadd,ptsz,x[8],z[8],y[8],lx,rx,lz,rz,ty,by;
	segment_type	*seg;
    
    if (select_count()!=1) return;
	
	select_get(0,&type,&index);
	if ((type!=segment_piece) && (type!=primitive_piece)) return;

    seg=&map.segments[index];
	
	rn=seg->rn;
	xadd=map.portals[rn].x*map_enlarge;
	zadd=map.portals[rn].z*map_enlarge;
	
		// primitives
		
	if (type==primitive_piece) {
		primitive_uid=seg->primitive_uid[0];
	
		ptsz=6;
		primitive_get_3D_handles(primitive_uid,x,z,y);
		
		for (t=0;t!=ptsz;t++) {
			x[t]=(x[t]*map_enlarge)+xadd;
			z[t]=(z[t]*map_enlarge)+zadd;
			y[t]=y[t]*map_enlarge;
		}
		
		walk_view_draw_sel_handles(ptsz,x,y,z);
		return;
	}
		
		// regular segments
    
    ptsz=0;
		
    switch (seg->type) {
    
        case sg_wall:
            lx=(seg->data.wall.lx*map_enlarge)+xadd;
            rx=(seg->data.wall.rx*map_enlarge)+xadd;
            lz=(seg->data.wall.lz*map_enlarge)+zadd;
            rz=(seg->data.wall.rz*map_enlarge)+zadd;
            ty=seg->data.wall.ty*map_enlarge;
            by=(seg->data.wall.by+1)*map_enlarge;

            ptsz=4;
            x[0]=x[2]=(lx+rx)/2;
            z[0]=z[2]=(lz+rz)/2;
            y[0]=ty;
            y[2]=by;
            x[1]=rx;
            x[3]=lx;
            z[1]=rz;
            z[3]=lz;
            y[1]=y[3]=(ty+by)/2;
            break;
            
        case sg_floor:
        case sg_ceiling:
            ptsz=seg->data.fc.ptsz;
            
            for ((t=0);(t<ptsz);t++) {
                x[t]=(seg->data.fc.x[t]*map_enlarge)+xadd;
                z[t]=(seg->data.fc.z[t]*map_enlarge)+zadd;
                y[t]=seg->data.fc.y[t]*map_enlarge;
            }
            break;
            
        case sg_liquid:
            lx=(seg->data.liquid.lft*map_enlarge)+xadd;
            rx=(seg->data.liquid.rgt*map_enlarge)+xadd;
            lz=(seg->data.liquid.top*map_enlarge)+zadd;
            rz=(seg->data.liquid.bot*map_enlarge)+zadd;

            x[3]=lx;
            x[1]=rx;
            x[0]=x[2]=(lx+rx)/2;
            z[0]=lz;
            z[2]=rz;
            z[1]=z[3]=(lz+rz)/2;
            y[0]=y[1]=y[2]=y[3]=(seg->data.liquid.y+1)*map_enlarge;
            break;
					
        case sg_ambient_wall:
            lx=(seg->data.ambient_wall.lx*map_enlarge)+xadd;
            rx=(seg->data.ambient_wall.rx*map_enlarge)+xadd;
            lz=(seg->data.ambient_wall.lz*map_enlarge)+zadd;
            rz=(seg->data.ambient_wall.rz*map_enlarge)+zadd;
            ty=seg->data.ambient_wall.ty*map_enlarge;
            by=(seg->data.ambient_wall.by+1)*map_enlarge;

            ptsz=4;
            x[0]=x[2]=(lx+rx)/2;
            z[0]=z[2]=(lz+rz)/2;
            y[0]=ty;
            y[2]=by;
            x[1]=rx;
            x[3]=lx;
            z[1]=rz;
            z[3]=lz;
            y[1]=y[3]=(ty+by)/2;
            break;
			
        case sg_ambient_fc:
            ptsz=seg->data.ambient_fc.ptsz;
            
            for ((t=0);(t<ptsz);t++) {
                x[t]=(seg->data.ambient_fc.x[t]*map_enlarge)+xadd;
                z[t]=(seg->data.ambient_fc.z[t]*map_enlarge)+zadd;
                y[t]=seg->data.ambient_fc.y[t]*map_enlarge;
            }
            break;
            
    }
    
    walk_view_draw_sel_handles(ptsz,x,y,z);
	*/
}

