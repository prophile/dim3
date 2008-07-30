/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Collision Boxes

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Create Boxes
      
======================================================= */

void box_create_from_point(d3box *box,int x,int y,int z,int wid,int high)
{
		// box middle
		
	box->x=x;
	box->y=y;
	box->z=z;

		// box points
		
	wid=wid>>1;

	box->min_x=x-wid;
	box->max_x=x+wid;
	box->min_z=z-wid;
	box->max_z=z+wid;
	
	box->min_y=y-high;
	box->max_y=y;
}

void box_create_from_object(d3box *box,obj_type *obj)
{
	int		x,z,y,x_size,z_size,y_size;

		// box middle
		
	box->x=x=obj->pnt.x;
	box->z=z=obj->pnt.z;
	box->y=y=obj->pnt.y;

		// box points
		
	x_size=obj->size.x>>1;
	z_size=obj->size.z>>1;

	box->min_x=x-x_size;
	box->max_x=x+x_size;
	box->min_z=z-z_size;
	box->max_z=z+z_size;
	
	y_size=obj->size.y;
	if (obj->duck.mode!=dm_stand) y_size-=obj->duck.y_move;
	
	box->min_y=y-y_size;
	box->max_y=y;
}

void box_create_from_projectile(d3box *box,proj_type *proj)
{
	int		x,z,y,x_size,z_size;

		// box middle
		
	box->x=x=proj->pnt.x;
	box->z=z=proj->pnt.z;
	box->y=y=proj->pnt.y;

		// box points
		
	x_size=proj->size.x>>1;
	z_size=proj->size.z>>1;

	box->min_x=x-x_size;
	box->max_x=x+x_size;
	box->min_z=z-z_size;
	box->max_z=z+z_size;
	
	box->min_y=y-proj->size.y;
	box->max_y=y;
}

/* =======================================================

      Add Movement to Box
      
======================================================= */

void box_add_movement(d3box *box,int mx,int mz)
{
	if (mx<=0) {
		box->min_x+=mx;
	}
	else {
		box->max_x+=mx;
	}
	
	if (mz<=0) {
		box->min_z+=mz;
	}
	else {
		box->max_z+=mz;
	}
	
	box->x=(box->min_x+box->max_x)>>1;
	box->z=(box->min_z+box->max_z)>>1;
}

/* =======================================================

      Box Collisions
      
======================================================= */

bool box_collide_2D_line(d3box *box,int lx,int rx,int lz,int rz)
{
	int			lft,rgt,top,bot,
				dx,dz,x,z,lngadd,kadd;
	
		// get box
		
	lft=box->min_x;
	rgt=box->max_x;
	top=box->min_z;
	bot=box->max_z;
	
		// completely outside line
		
	if ((lx>rgt) && (rx>rgt)) return(FALSE);
	if ((lx<lft) && (rx<lft)) return(FALSE);
	if ((lz>bot) && (rz>bot)) return(FALSE);
	if ((lz<top) && (rz<top)) return(FALSE);
	
		// are either of the line points in box?
				
	if ((lx>=lft) && (lx<=rgt) && (lz>=top) && (lz<=bot)) return(TRUE);
	if ((rx>=lft) && (rx<=rgt) && (rz>=top) && (rz<=bot)) return(TRUE);
	
		// special check for horizontal lines
				
	if (lz==rz) {
		return((top<=lz) && (bot>=lz));			// z points pass through?
	}

		// special check for vertical lines
				
	if (lx==rx) {
		return((lft<=lx) && (rgt>=lx));			// x points pass through?
	}
	
		// get the slope
		
	dx=abs(rx-lx);
	dz=abs(rz-lz);
	
		// check for intersections with left and right sides
		
	if (lx>rx) {
		lngadd=lx;
		lx=rx;
		rx=lngadd;
		lngadd=lz;
		lz=rz;
		rz=lngadd;
	}
	kadd=((rz-lz)<<10)/dx;
	
	z=lz+((kadd*(lft-lx))>>10);
	if ((z>=top) && (z<=bot)) return(TRUE);
	
	z=lz+((kadd*(rgt-lx))>>10);
	if ((z>=top) && (z<=bot)) return(TRUE);
	
		// check for intersections with top and bottom sides
		
	if (lz>rz) {
		lngadd=lx;
		lx=rx;
		rx=lngadd;
		lngadd=lz;
		lz=rz;
		rz=lngadd;
	}
	kadd=((rx-lx)<<10)/dz;
	
	x=lx+((kadd*(top-lz))>>10);
	if ((x>=lft) && (x<=rgt)) return(TRUE);
		
	x=lx+((kadd*(bot-lz))>>10);
	if ((x>=lft) && (x<=rgt)) return(TRUE);

		// no collision
		
	return(FALSE);
}
