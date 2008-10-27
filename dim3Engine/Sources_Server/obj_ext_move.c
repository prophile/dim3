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

	obj->pnt.x+=xmove;
	obj->pnt.y+=ymove;
	obj->pnt.z+=zmove;

	return(FALSE);
}

inline bool object_turn_with_turn(obj_type *obj,d3pnt *mpt,float rot_y,float reduce)
{
	int			x,z;
	bool		hit;

		// get change

	x=obj->pnt.x;
	z=obj->pnt.z;
	rotate_2D_point(&x,&z,mpt->x,mpt->z,rot_y);

		// reduction in movement

	x=(int)((float)(x-obj->pnt.x)*reduce);
	z=(int)((float)(z-obj->pnt.z)*reduce);

		// move

	hit=object_move_with_move(obj,x,z);

		// rotate player

	obj->ang.y=angle_add(obj->ang.y,rot_y);

	return(hit);
}

/* =======================================================

      Objects Moving by Meshes
      
======================================================= */

bool collide_check_box_to_line(d3pnt *min,d3pnt *max,int lx,int rx,int lz,int rz)
{
	int			lft,rgt,top,bot,
				dx,dz,x,z,lngadd,kadd;
	
		// get box
		
	lft=min->x;
	rgt=max->x;
	top=min->z;
	bot=max->z;
	
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

bool object_move_with_mesh_check_object(obj_type *obj,int mesh_idx)
{
	int					n,sz;
	d3pnt				min,max;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	sz=obj->size.x>>1;
	min.x=obj->pnt.x-sz;
	max.x=obj->pnt.x+sz;

	sz=obj->size.z>>1;
	min.z=obj->pnt.z-sz;
	max.z=obj->pnt.z+sz;

	sz=obj->size.y;
	if (obj->duck.mode!=dm_stand) sz-=obj->duck.y_move;
	
	min.y=obj->pnt.y-sz;
	max.y=obj->pnt.y;

		// rough mesh check

	mesh=&map.mesh.meshes[mesh_idx];

	if ((max.x<mesh->box.min.x) || (min.x>mesh->box.max.x)) return(FALSE);
	if ((max.y<mesh->box.min.y) || (min.y>mesh->box.max.y)) return(FALSE);
	if ((max.z<mesh->box.min.z) || (min.z>mesh->box.max.z)) return(FALSE);

		// polygon checks
		// only check wall like polys for pushing

	for (n=0;n!=mesh->npoly;n++) {

		poly=&mesh->polys[n];
		if (!poly->box.wall_like) continue;
		
			// rough poly check
		
		if ((max.x<poly->box.min.x) || (min.x>poly->box.max.x)) continue;
		if ((max.y<poly->box.min.y) || (min.y>poly->box.max.y)) continue;
		if ((max.z<poly->box.min.z) || (min.z>poly->box.max.z)) continue;
		
			// check x/z (2D) wall
			
		if (collide_check_box_to_line(&min,&max,poly->line.lx,poly->line.rx,poly->line.lz,poly->line.rz)) return(TRUE);
	}

	return(FALSE);
}

void object_move_with_mesh(int mesh_idx,int xmove,int zmove)
{
	int			n,x,z,cnt;
	obj_type	*obj;

	for (n=0;n!=server.count.obj;n++) {

		obj=&server.objs[n];

			// if we are standing on mesh, move
			// with mesh

		if (obj->contact.stand_poly.mesh_idx==mesh_idx) {
			object_move_with_move(obj,xmove,zmove);
			continue;
		}

			// we might try to move multiple times to push
			// items out of meshes

		x=xmove;
		z=zmove;
		cnt=0;

		while (object_move_with_mesh_check_object(obj,mesh_idx)) {
			
			if (object_move_with_move(obj,x,z)) {
				if (obj->contact.hit_poly.mesh_idx!=mesh_idx) object_crush(obj,TRUE);
				break;
			}

			x=x>>1;
			z=z>>1;

			cnt++;
			if (cnt>2) break;
		}
	}
}

void object_rotate_with_mesh(int mesh_idx,float y)
{
	int				n,cnt;
	float			reduce;
	d3pnt			mpt;
	map_mesh_type	*mesh;
	obj_type		*obj;

		// get center point for mesh

	mesh=&map.mesh.meshes[mesh_idx];

	map_mesh_calculate_center(&map,mesh_idx,&mpt);
	mpt.x+=mesh->rot_off.x;
	mpt.y+=mesh->rot_off.y;
	mpt.z+=mesh->rot_off.z;

		// check objects interacting with mesh

	for (n=0;n!=server.count.obj;n++) {

		obj=&server.objs[n];

			// if we are standing on mesh, turn
			// with mesh

		if (obj->contact.stand_poly.mesh_idx==mesh_idx) {
			object_turn_with_turn(obj,&mpt,y,1.0f);
			continue;
		}

			// we might try to move multiple times to push
			// items out of meshes

		reduce=1.0f;
		cnt=0;

		while (object_move_with_mesh_check_object(obj,mesh_idx)) {
			
			if (object_turn_with_turn(obj,&mpt,y,reduce)) {
				if (obj->contact.hit_poly.mesh_idx!=mesh_idx) object_crush(obj,TRUE);
				break;
			}

			reduce=reduce/2.0f;

			cnt++;
			if (cnt>2) break;
		}
	}
}

/* =======================================================

      Objects Moving by Other Objects
      
======================================================= */

bool object_push_with_object(obj_type *obj,int xmove,int zmove)
{
	int				weight_dif;
	float			f,f_xmove,f_zmove;
	obj_type		*pushed_obj;
	
	if (obj->contact.obj_uid==-1) return(TRUE);
	
		// get object to push

	pushed_obj=object_find_uid(obj->contact.obj_uid);
	if (pushed_obj==NULL) return(TRUE);

	if (!pushed_obj->contact.pushable) return(TRUE);

		// compare weights

	weight_dif=obj->size.weight-pushed_obj->size.weight;
	if (weight_dif<0) return(FALSE);

		// calculate the force by weight

	if (weight_dif>2000) weight_dif=2000;
	f=0.25f+(map.settings.push_factor*weight_dif);
	
		// add in vector

	f_xmove=((float)xmove)*f;
	f_zmove=((float)zmove)*f;
	
	pushed_obj->force.vct.x=f_xmove;
	pushed_obj->force.vct.z=f_zmove;

		// movement

	object_move_with_move(pushed_obj,(int)f_xmove,(int)f_zmove);

	return(FALSE);
}

void object_move_with_standing_object(obj_type *obj,int xmove,int zmove)
{
	int			n;
	obj_type	*obj_check;

	obj_check=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if (!obj_check->remote.on) {
			if (obj_check->stand_obj_uid==obj->uid) {
				if (!obj_check->suspend) object_move_with_move(obj_check,xmove,zmove);
			}
		}

		obj_check++;
	}
}

void object_rotate_with_standing_object(obj_type *obj,float y)
{
	int				n;
	d3pnt			mpt;
	obj_type		*obj_check;

		// get center point for object

	mpt.x=obj->pnt.x;
	mpt.y=obj->pnt.y;
	mpt.z=obj->pnt.z;

		// turn any object standing on it
	
	obj_check=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if (!obj_check->remote.on) {
			if (obj_check->stand_obj_uid==obj->uid) {
				if (!obj_check->suspend) object_turn_with_turn(obj_check,&mpt,y,1.0f);
			}
		}

		obj_check++;
	}
}
