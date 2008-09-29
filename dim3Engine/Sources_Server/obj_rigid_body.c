/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Rigid Body

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

#include "scripts.h"
#include "objects.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Rigid Body Utilities
      
======================================================= */

int object_rigid_body_get_point_y(obj_type *obj,int x_off,int z_off,int y)
{
	int			x,z;
	
	rotate_2D_point_center(&x_off,&z_off,obj->ang.y);

	x=obj->pnt.x+x_off;
	z=obj->pnt.z+z_off;
	
	return(find_poly_nearest_stand(x,y,z,obj->rigid_body.max_drop_y,FALSE));
}

void object_rigid_body_offset_reset_y(obj_type *obj)
{
	float			f;

	if (obj->rigid_body.draw_offset_y==0) return;

	f=(float)obj->rigid_body.draw_offset_y;
	obj->rigid_body.draw_offset_y=(int)(f*obj->rigid_body.reset_factor_y);
}

void object_rigid_body_angle_reset_x(obj_type *obj)
{
	obj->ang.x*=obj->rigid_body.reset_factor_x;
	if (fabsf(obj->ang.x)<1.0f) obj->ang.x=0;
}

void object_rigid_body_angle_reset_z(obj_type *obj)
{
	obj->ang.z*=obj->rigid_body.reset_factor_z;
	if (fabsf(obj->ang.z)<1.0f) obj->ang.z=0;
}

/* =======================================================

      Objects Reseting Angles for Rigid Bodies
      
======================================================= */

void object_rigid_body_reset_angle(obj_type *obj)
{
	int					xsz,zsz,y,ky,off_y,ry,
						fy[4];
	float				x_ang,x_neg_ang,x_pos_ang,
						z_ang,z_neg_ang,z_pos_ang;
	poly_pointer_type	stand_poly;

		// is rigid body on?

	if (!obj->rigid_body.on) return;

		// if in air, reset angles

	if (obj->air_mode!=am_ground) {
		object_rigid_body_offset_reset_y(obj);
		object_rigid_body_angle_reset_x(obj);
		object_rigid_body_angle_reset_z(obj);
		return;
	}

		// move middle of object to closest ground

	y=obj->pnt.y;

	if (obj->stand_obj_uid!=-1) {
		object_rigid_body_offset_reset_y(obj);			// no movement if standing on object
	}
	else {

		ky=pin_downward_movement_point(obj->pnt.x,y,obj->pnt.z,obj->rigid_body.max_drop_y,&stand_poly);
		if (stand_poly.mesh_idx!=-1) {
			ky+=y;

			if (ky>y) {

					// reset draw offset (with smoothing)

				off_y=ky-y;
				ry=obj->rigid_body.draw_offset_y;

				obj->rigid_body.draw_offset_y=ry+(int)((float)(off_y-ry)*obj->rigid_body.smooth_factor_y);

					// set y for angle changes

				y=ky;
			}
		}
	}

		// get rigid y points around size

	xsz=obj->size.x>>1;
	zsz=obj->size.z>>1;

	fy[0]=object_rigid_body_get_point_y(obj,-xsz,-zsz,y);
	fy[1]=object_rigid_body_get_point_y(obj,xsz,-zsz,y);
	fy[2]=object_rigid_body_get_point_y(obj,xsz,zsz,y);
	fy[3]=object_rigid_body_get_point_y(obj,-xsz,zsz,y);
	
		// find angles
	
	x_neg_ang=x_pos_ang=0.0f;
	
	if ((fy[0]!=-1) && (fy[3]!=-1)) x_neg_ang=angle_find(0,0,obj->size.z,abs(fy[0]-fy[3]))-90.0f;
	if ((fy[1]!=-1) && (fy[2]!=-1)) x_pos_ang=angle_find(0,0,obj->size.z,abs(fy[1]-fy[2]))-90.0f;

	z_neg_ang=z_pos_ang=0.0f;

	if ((fy[3]!=-1) && (fy[2]!=-1)) z_neg_ang=angle_find(0,0,obj->size.x,abs(fy[3]-fy[2]))-90.0f;
	if ((fy[0]!=-1) && (fy[1]!=-1)) z_pos_ang=angle_find(0,0,obj->size.x,abs(fy[0]-fy[1]))-90.0f;
	
	x_ang=x_neg_ang;
	if (x_pos_ang>x_ang) x_ang=x_pos_ang;
	if (x_ang>obj->rigid_body.max_ang_x) x_ang=obj->rigid_body.max_ang_x;
	if ((y<fy[0]) || (y<fy[1])) x_ang=-x_ang;

	z_ang=z_neg_ang;
	if (z_pos_ang>z_ang) z_ang=z_pos_ang;
	if (z_ang>obj->rigid_body.max_ang_z) z_ang=obj->rigid_body.max_ang_z;
	if ((y<fy[0]) || (y<fy[3])) z_ang=-z_ang;
	
		// set angle (smooth out drastic angle changes)
		
	if (fabsf(obj->ang.x-x_ang)<1.0f) {
		obj->ang.x=x_ang;
	}
	else {
		obj->ang.x=obj->ang.x+((x_ang-obj->ang.x)*obj->rigid_body.smooth_factor_x);
	}

	z_ang=-z_ang;
	
	if (fabsf(obj->ang.z-z_ang)<1.0f) {
		obj->ang.z=z_ang;
	}
	else {
		obj->ang.z=obj->ang.z+((z_ang-obj->ang.z)*obj->rigid_body.smooth_factor_x);
	}
}

