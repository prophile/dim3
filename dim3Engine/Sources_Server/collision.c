/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Collisions

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

#include "objects.h"
#include "models.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

// supergumba -- will need to check which of these to delete

/* =======================================================

      Check for Object-Object Collisions
      
======================================================= */

bool collide_set_object_hit_box_for_object_hit(obj_type *obj,int x,int z,obj_type *check_obj)
{
	int					n,nhit_box;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;

		// no hit boxes hit

	obj->hit_box.obj_hit_box_idx=-1;
	
		// check model
		
	draw=&check_obj->draw;
	if ((draw->uid==-1) || (!draw->on)) return(FALSE);
	
	model=model_find_uid(draw->uid);
	if (model==NULL) return(FALSE);
	
		// check hit boxes
		// unlike projectiles, we can just take the
		// first hit off the first hitbox
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
		if (collide_object_to_object_hit_box(obj,x,z,check_obj,hit_box)) {
			obj->hit_box.obj_hit_box_idx=n;
			return(TRUE);
		}
		hit_box++;
	}

	return(FALSE);
}

int collide_find_object_for_object_move(obj_type *obj,int x,int z)
{
	int				n,ignore_obj_uid;
	obj_type	 	*check_obj;
	
	ignore_obj_uid=obj->uid;
	
	check_obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
	
		if ((check_obj->hidden) || (!check_obj->contact.object_on) || (check_obj->pickup.on) || (check_obj->uid==ignore_obj_uid)) {
			check_obj++;
			continue;
		}

			// object hit?
			
		if (!collide_object_to_object(obj,x,z,check_obj,TRUE,FALSE)) {
			check_obj++;
			continue;
		}
		
			// which hit box?
		
		if (!check_obj->hit_box.on) return(check_obj->uid);
		if (collide_set_object_hit_box_for_object_hit(obj,x,z,check_obj)) return(check_obj->uid);
		
		check_obj++;
	}
	
	return(-1);
}

int collide_find_object_for_object(obj_type *obj)
{
	return(collide_find_object_for_object_move(obj,0,0));
}

int collide_find_object_for_standing_object(obj_type *obj)
{
	int			n,uid,y,ty,ydist,ignore_obj_uid;
	obj_type	*stand_obj;
	
	y=obj->pnt.y;
	ydist=floor_slop;
	uid=-1;
	
	ignore_obj_uid=obj->uid;
	
	stand_obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
	
		if ((stand_obj->uid==ignore_obj_uid) || (stand_obj->hidden) || (!stand_obj->contact.object_on) || (stand_obj->pickup.on)) {
			stand_obj++;
			continue;
		}
		
		ty=abs(y-(stand_obj->pnt.y-stand_obj->size.y));
		if (ty>ydist) {
			stand_obj++;
			continue;
		}

		if (collide_object_to_object(obj,0,0,stand_obj,FALSE,FALSE)) {
			uid=stand_obj->uid;
			ydist=ty;
		}
		
		stand_obj++;
	}
	
	return(uid);	
}

/* =======================================================

      Object-Projectile Collision
      
======================================================= */

bool collide_set_object_hit_box_for_projectile_hit(proj_type *proj,obj_type *obj)
{
	int					n,nhit_box,
						dist,d,x,y,z;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;
	
		// tag no hit box hits
		
	obj->hit_box.proj_hit_box_idx=-1;
	
		// find hit box middle closest to damage box middle
	
	dist=-1;
	
		// check model
		
	draw=&obj->draw;
	if ((draw->uid==-1) || (!draw->on)) return(FALSE);
	
	model=model_find_uid(draw->uid);
	if (model==NULL) return(FALSE);
	
		// check hit boxes
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
	
			// in this hit box?
			
		if (!collide_projectile_to_object_hit_box(proj,obj,hit_box)) {
			hit_box++;
			continue;
		}
		
			// get the distance
	
		x=obj->pnt.x+draw->offset.x+hit_box->box.offset.x;
		y=obj->pnt.y+draw->offset.y+hit_box->box.offset.y;
		z=obj->pnt.z+draw->offset.z+hit_box->box.offset.z;
	
		d=distance_get(proj->pnt.x,proj->pnt.y,proj->pnt.z,x,y,z);
				
		if ((d<dist) || (dist==-1)) {
			dist=d;
			obj->hit_box.proj_hit_box_idx=n;
		}
		
		hit_box++;
	}

	return(obj->hit_box.proj_hit_box_idx!=-1);
}

int collide_find_object_for_projectile_hit(proj_type *proj,int ignore_obj_uid)
{
	int			n;
	obj_type	*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if ((obj->hidden) || (!obj->contact.projectile_on) || (obj->pickup.on) || (obj->uid==ignore_obj_uid)) {
			obj++;
			continue;
		}
		
			// projectile hit?
			
		if (!collide_projectile_to_object(proj,obj)) {
			obj++;
			continue;
		}
		
			// which hit box?
		
		if (!obj->hit_box.on) return(obj->uid);
		if (collide_set_object_hit_box_for_projectile_hit(proj,obj)) return(obj->uid);
		
		obj++;
	}
	
	return(-1);
}

int collide_find_object_for_projectile(proj_type *proj,int ignore_obj_uid)
{
	int			n;
	obj_type	*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if ((obj->hidden) || (!obj->contact.projectile_on) || (obj->pickup.on) || (obj->uid==ignore_obj_uid)) {
			obj++;
			continue;
		}
		
			// projectile hit?
			
		if (collide_projectile_to_object(proj,obj))  return(obj->uid);
		
		obj++;
	}
	
	return(-1);
}

/* =======================================================

      Projectile-Projectile Collision
      
======================================================= */

int collide_find_projectile_for_projectile(proj_type *proj)
{
	int				n,ignore_proj_uid;
	proj_type		*check_proj;
	
	ignore_proj_uid=proj->uid;
	
	check_proj=server.projs;
	
	for (n=0;n!=server.count.proj;n++) {

		if (check_proj->uid==ignore_proj_uid) {
			proj++;
			continue;
		}
		
			// projectile hit?
			
		if (collide_projectile_to_projectile(proj,check_proj))  return(check_proj->uid);
		
		proj++;
	}
	
	return(-1);
}

/* =======================================================

      Object-Sphere Collision
      
======================================================= */

bool collide_set_object_hit_box_for_sphere_hit(int sx,int sy,int sz,int radius,obj_type *obj)
{
	int					n,nhit_box,
						dist,d,x,y,z;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;
	
		// tag no hit box hits
		
	obj->hit_box.obj_hit_box_idx=-1;
	
		// find hit box middle closest to damage box middle
	
	dist=-1;
	
		// check model
		
	draw=&obj->draw;
	if ((draw->uid==-1) || (!draw->on)) return(FALSE);
	
	model=model_find_uid(draw->uid);
	if (model==NULL) return(FALSE);
	
		// check hit boxes
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
	
			// in this hit box?
			
		if (!collide_sphere_to_object_hit_box(sx,sy,sz,radius,obj,hit_box)) {
			hit_box++;
			continue;
		}
		
			// get the distance
	
		x=obj->pnt.x+draw->offset.x+hit_box->box.offset.x;
		y=obj->pnt.y+draw->offset.y+hit_box->box.offset.y;
		z=obj->pnt.z+draw->offset.z+hit_box->box.offset.z;
	
		d=distance_get(sx,sy,sz,x,y,z);
				
		if ((d<dist) || (dist==-1)) {
			dist=d;
			obj->hit_box.obj_hit_box_idx=n;
		}
		
		hit_box++;
	}

	return(obj->hit_box.obj_hit_box_idx!=-1);
}

int collide_find_object_for_sphere_hit(int sx,int sy,int sz,int radius,int ignore_obj_uid)
{
	int			n;
	obj_type	*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if ((obj->hidden) || (!obj->contact.object_on) || (obj->pickup.on) || (obj->uid==ignore_obj_uid)) {
			obj++;
			continue;
		}
		
			// sphere hit?
			
		if (!collide_sphere_to_object(sx,sy,sz,radius,obj)) {
			obj++;
			continue;
		}
		
			// which hit box?
		
		if (!obj->hit_box.on) return(obj->uid);
		if (collide_set_object_hit_box_for_sphere_hit(sx,sy,sz,radius,obj)) return(obj->uid);
		
		obj++;
	}
	
	return(-1);
}

int collide_find_object_for_sphere(int sx,int sy,int sz,int radius,int ignore_obj_uid)
{
	int			n;
	obj_type	*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if ((obj->hidden) || (!obj->contact.object_on) || (obj->pickup.on) || (obj->uid==ignore_obj_uid)) {
			obj++;
			continue;
		}
		
			// sphere hit?
			
		if (collide_sphere_to_object(sx,sy,sz,radius,obj))  return(obj->uid);
		
		obj++;
	}
	
	return(-1);
}

/* =======================================================

      Push Objects
      
======================================================= */

void collide_push_objects(int sx,int sy,int sz,int radius,int force)
{
	int			n,yhit;
	d3ang		ang;
	obj_type	*obj;
	
		// check objects
		
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if ((obj->hidden) || (obj->suspend) || (!obj->contact.object_on)) {
			obj++;
			continue;
		}
		
			// check for collision
			
		if (!collide_sphere_to_object(sx,sy,sz,radius,obj)) {
			obj++;
			continue;
		}
		
			// add push
				
		yhit=obj->pnt.y-(obj->size.y/2);
		
		ang.x=angle_find(sy,sz,yhit,obj->pnt.z);
		ang.y=angle_find(sx,sz,obj->pnt.x,obj->pnt.z);
		ang.z=0;
		
		object_push(obj,&ang,force,FALSE);
		
		obj++;
	}
}


