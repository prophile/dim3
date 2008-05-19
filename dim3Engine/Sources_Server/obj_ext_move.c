/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object External Movement

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
#include "models.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;

extern bool collide_object_to_map(obj_type *obj,int *xadd,int *yadd,int *zadd);

/* =======================================================

      Objects External Movement
      
======================================================= */

inline bool object_move_with_move(obj_type *obj,int xmove,int zmove)
{
	int			ymove;

	if (obj->lock.x) xmove=0;
	if (obj->lock.z) zmove=0;
	ymove=0;
	
	if (collide_object_to_map(obj,&xmove,&ymove,&zmove)) return(TRUE);

	obj->pos.x+=xmove;
	obj->pos.y+=ymove;
	obj->pos.z+=zmove;

	return(FALSE);
}

inline void object_turn_with_turn(obj_type *obj,d3pnt *mpt,float rot_y)
{
	int			x,z;

		// get change

	x=obj->pos.x;
	z=obj->pos.z;
	rotate_2D_point(&x,&z,mpt->x,mpt->z,rot_y);

		// move

	object_move_with_move(obj,(x-obj->pos.x),(z-obj->pos.z));

		// rotate player

	obj->ang.y=angle_add(obj->ang.y,rot_y);
}

/* =======================================================

      Objects Moving by Meshes
      
======================================================= */

// supergumba -- this all needs work
bool object_move_with_wall_segment_check_wall(d3box *box,int seg_idx,int xmove,int zmove)
{
/*
	int					lx,rx,lz,rz;
	wall_segment_data	*wall;
	
	wall=&map.segments[seg_idx].data.wall;
		
		// completely out of Ys?
		
	if ((box->min_y>=wall->by) || (box->max_y<=wall->ty)) return(FALSE);
		
		// get wall points plus movement
			
	lx=wall->lx+xmove;
	rx=wall->rx+xmove;
	lz=wall->lz+zmove;
	rz=wall->rz+zmove;
		
		// check x/z (2D) wall
			
	return(box_collide_2D_line(box,lx,rx,lz,rz));
	*/
	return(FALSE);
}

void object_move_with_wall_segment(int seg_idx,int xmove,int zmove)
{
/* supergumba -- work on this
	int			i;
	d3box		box;
	obj_type	*obj;

	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {

		if (!obj->remote.on) {

				// has wall moved into object?

			box_create_from_object(&box,obj);

			if (object_move_with_wall_segment_check_wall(&box,seg_idx,xmove,zmove)) {
				if (object_move_with_move(obj,xmove,zmove)) {
					object_crush(obj,TRUE);					// if move fails, then auto-crush
				}
			}
		}

		obj++;
	}
	*/
}

void object_move_with_standing_mesh(int portal_idx,int mesh_idx,int xmove,int zmove)
{
	int			n;
	obj_type	*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		if ((obj->contact.stand_poly.portal_idx==portal_idx) && (obj->contact.stand_poly.mesh_idx==mesh_idx)) object_move_with_move(obj,xmove,zmove);
		obj++;
	}
}

void object_rotate_with_standing_mesh(int portal_idx,int mesh_idx,float y)
{
	int				n;
	d3pnt			mpt;
	map_mesh_type	*mesh;
	obj_type		*obj;

		// get center point for mesh

	mesh=&map.portals[portal_idx].mesh.meshes[mesh_idx];

	map_portal_mesh_calculate_center(&map,portal_idx,mesh_idx,&mpt);
	mpt.x+=mesh->rot_off.x;
	mpt.y+=mesh->rot_off.y;
	mpt.z+=mesh->rot_off.z;

		// turn anything standing on it
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		if ((obj->contact.stand_poly.portal_idx==portal_idx) && (obj->contact.stand_poly.mesh_idx==mesh_idx)) object_turn_with_turn(obj,&mpt,y);
		obj++;
	}
}

/* =======================================================

      Objects Moving by Other Objects
      
======================================================= */

bool object_push_with_object(obj_type *obj,float xmove,float zmove)
{
	int				weight_dif;
	float			f;
	obj_type		*pushed_obj;
	
	if (obj->contact.obj_uid==-1) return(TRUE);
	
		// get object to push

	pushed_obj=object_find_uid(obj->contact.obj_uid);
	if (pushed_obj==NULL) return(TRUE);

	if (!pushed_obj->contact.pushable) return(TRUE);

		// compare weights

	weight_dif=obj->size.weight-pushed_obj->size.weight;
	if (weight_dif<0) return(TRUE);

		// reduce movement for weight

	if (weight_dif>500) weight_dif=500;
	
	f=1.0f+(((float)weight_dif)/1000.0f);
	
		// add in vector

	xmove*=f;
	zmove*=f;
	
	pushed_obj->force.vct.x=xmove;
	pushed_obj->force.vct.z=zmove;

		// movement

	return(object_move_with_move(pushed_obj,(int)xmove,(int)zmove));
}


void object_move_with_standing_object(obj_type *obj,int xmove,int zmove)
{
	int			i;
	obj_type	*obj_check;
	
	obj_check=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		if (!obj_check->remote.on) {
			if (obj_check->stand_obj_uid==obj->uid) {
				if (!obj_check->suspend) object_move_with_move(obj_check,xmove,zmove);
			}
			obj_check++;
		}
	}
}

