/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Collisions

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

#define collide_obj_ray_count			35

#include "scripts.h"
#include "objects.h"
#include "projectiles.h"
#include "models.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Determine if Contact Poly is Wall-Like
      
======================================================= */

bool collide_contact_is_wall_hit(poly_pointer_type *hit_poly)
{
	map_mesh_poly_type		*mesh_poly;

	if (hit_poly->mesh_idx==-1) return(FALSE);

	mesh_poly=&map.mesh.meshes[hit_poly->mesh_idx].polys[hit_poly->poly_idx];
	return(mesh_poly->box.wall_like);
}

/* =======================================================

      Check for Object-Map Collisions
      
======================================================= */

int collide_point_distance(d3pnt *pt_1,d3pnt *pt_2)
{
	double		dx,dz,dy;
	
	dx=(double)(pt_1->x-pt_2->x);
	dy=(double)(pt_1->y-pt_2->y);
	dz=(double)(pt_1->z-pt_2->z);
	
	return((int)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

bool collide_object_box_to_map(obj_type *obj,d3pnt *pt,d3pnt *box_sz,int *xadd,int *yadd,int *zadd)
{
	int						n,k,mx,mz,y,idx,d,dist,hx[7],hz[7],vert_y[5];
	float					move_ang,ang;
	double					radius,rad;
	bool					bump,hits[collide_obj_ray_count];
	d3pnt					obj_pnt,mov,
							spt[collide_obj_ray_count],ept[collide_obj_ray_count],hpt[collide_obj_ray_count];
	ray_trace_contact_type	base_contact,contacts[collide_obj_ray_count];
	map_mesh_poly_type		*poly;
	model_type				*mdl;

		// get movements out of pointers
		// and movement angle

	mov.x=*xadd;
	mov.y=*yadd;
	mov.z=*zadd;

	move_ang=angle_find(0,0,mov.x,mov.z);

		// find the object center

	memmove(&obj_pnt,&obj->pnt,sizeof(d3pnt));

	mdl=model_find_uid(obj->draw.uid);
	if (mdl!=NULL) {
		obj_pnt.x+=mdl->center.x;
		obj_pnt.y+=mdl->center.y;
		obj_pnt.z+=mdl->center.z;
	}

		// get movement radius

	radius=(double)(obj->size.radius+map_enlarge);

		// vertical race trace positions

	vert_y[0]=obj_pnt.y-obj->size.y;
	vert_y[2]=obj_pnt.y-(obj->size.y>>1);
	vert_y[4]=obj_pnt.y-1;

	vert_y[1]=(vert_y[0]+vert_y[2])>>1;
	vert_y[3]=(vert_y[2]+vert_y[4])>>1;

		// create the ray trace points

	for (n=0;n!=7;n++) {
		ang=(move_ang-90.0f)+(float)(n*30);
		if (ang<0) ang=360.0f+ang;
		if (ang>=360) ang=ang-360.0f;

		rad=(double)(ang*ANG_to_RAD);

		mx=(int)(radius*sin(rad));
		mz=-(int)(radius*cos(rad));

		hx[n]=mx+mov.x;
		hz[n]=mz+mov.z;
	}

		// create the rays that come from
		// center of object

	idx=0;

	for (n=0;n!=7;n++) {

		for (k=0;k!=5;k++) {

			y=vert_y[k];

			spt[idx].x=obj_pnt.x;
			spt[idx].y=y;
			spt[idx].z=obj_pnt.z;
			ept[idx].x=obj_pnt.x+hx[n];
			ept[idx].y=y+mov.y;
			ept[idx].z=obj_pnt.z+hz[n];

			idx++;
		}
	}

		// set the collisions and run the
		// ray tracing

	base_contact.obj.on=TRUE;
	base_contact.obj.ignore_uid=obj->uid;

	base_contact.proj.on=FALSE;
	base_contact.proj.ignore_uid=-1;

	base_contact.hit_mode=poly_ray_trace_hit_mode_wall_only;
	base_contact.origin=poly_ray_trace_origin_object;

		// run ray trace

	ray_trace_map_by_point_array(collide_obj_ray_count,spt,ept,hpt,hits,&base_contact,contacts);

		// find the one that moves the leasts
		// as the most suitable hit point

	idx=-1;
	dist=0;

	for (n=0;n!=collide_obj_ray_count;n++) {
			
		if (hits[n]) {
			d=collide_point_distance(&spt[n],&hpt[n]);
			if ((d<=dist) || (idx==-1)) {
				dist=d;
				idx=n;
			}
		}
	}

		// no hits?

	if (idx==-1) return(FALSE);
	
		// determine if hit poly is bump-capable
		// we ignore object radius calculations
		// for bump-capable segments as it pushes
		// the object around on stairs
		
	if (contacts[idx].poly.mesh_idx!=-1) {
	
		if (obj->bump.on) {
			poly=&map.mesh.meshes[contacts[idx].poly.mesh_idx].polys[contacts[idx].poly.poly_idx];
			bump=((obj->pnt.y-poly->box.min.y)<=obj->bump.high);
		}
		else {
			bump=FALSE;
		}
		
	}
	
		// make sure distance stays within
		// object radius

	if (!bump) {
		mx=hpt[idx].x-spt[idx].x;
		mz=hpt[idx].z-spt[idx].z;

		d=(int)(sqrt((double)(mx*mx)+(double)(mz*mz)));

		angle_get_movement(move_ang,(obj->size.radius-d),&mx,&mz);
		*xadd=(*xadd)-mx;
		*zadd=(*zadd)-mz;
	}
	else {
		*xadd=hpt[idx].x-spt[idx].x;
		*zadd=hpt[idx].z-spt[idx].z;
	}
	
		// setup the hits

	if (contacts[idx].poly.mesh_idx!=-1) {
		memmove(&obj->contact.hit_poly,&contacts[idx].poly,sizeof(poly_pointer_type));
	}

	if (contacts[idx].obj.uid!=-1) {
		obj->contact.obj_uid=contacts[idx].obj.uid;
		obj->contact.hit_face=contacts[idx].obj.hit_face;
	}

	if (contacts[idx].proj.uid!=-1) {
		obj->contact.proj_uid=contacts[idx].proj.uid;
		obj->contact.hit_face=contacts[idx].proj.hit_face;
	}

	return(TRUE);
}

bool collide_object_to_map(obj_type *obj,int *xadd,int *yadd,int *zadd)
{
	int					n,nhit_box;
	bool				hit_box_hit;
	d3pnt				pt,box_sz;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;

		// are we using hit box hits?

	hit_box_hit=FALSE;

	if (obj->hit_box.on) {

		draw=&obj->draw;
		if ((draw->uid!=-1) && (!draw->on)) {
	
			model=model_find_uid(draw->uid);
			if (model!=NULL) {
				hit_box_hit=TRUE;
			}
		}
	}

		// regular collisions

	if (!hit_box_hit) {
		box_sz.x=obj->size.x;
		box_sz.z=obj->size.z;

		box_sz.y=obj->size.y;
		if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;

		return(collide_object_box_to_map(obj,&obj->pnt,&box_sz,xadd,yadd,zadd));
	}

		// hit box collisions

	else {

		hit_box=model->hit_boxes;
		nhit_box=model->nhit_box;
		
		for (n=0;n!=nhit_box;n++) {
			pt.x=obj->pnt.x+hit_box->box.offset.x;
			pt.y=obj->pnt.y+hit_box->box.offset.y;
			pt.z=obj->pnt.z+hit_box->box.offset.z;

			box_sz.x=hit_box->box.size.x;
			box_sz.z=hit_box->box.size.z;

			box_sz.y=hit_box->box.size.y;
			if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;
			
			if (collide_object_box_to_map(obj,&pt,&box_sz,xadd,yadd,zadd)) {
				obj->hit_box.obj_hit_box_idx=n;
				return(TRUE);
			}

			hit_box++;
		}
	}

	return(FALSE);
}

/* =======================================================

      Check for Project-Map Collisions
      
======================================================= */

bool collide_projectile_to_map(proj_type *proj,int xadd,int yadd,int zadd)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;
	proj_setup_type			*proj_setup;

		// setup ray trace

	spt.x=proj->pnt.x;
	spt.y=proj->pnt.y;
	spt.z=proj->pnt.z;

	ept.x=spt.x+xadd;
	ept.y=spt.y+yadd;
	ept.z=spt.z+zadd;

	contact.obj.on=TRUE;
	if (proj->parent_grace>0) {
		contact.obj.ignore_uid=proj->obj_uid;
	}
	else {
		contact.obj.ignore_uid=-1;
	}

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	contact.proj.on=proj_setup->collision;
	contact.proj.ignore_uid=proj->uid;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_projectile;

		// run trace

	if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		proj->pnt.x=ept.x;
		proj->pnt.y=ept.y;
		proj->pnt.z=ept.z;
		return(FALSE);
	}

		// move to hit point

	proj->pnt.x=hpt.x;
	proj->pnt.y=hpt.y;
	proj->pnt.z=hpt.z;

		// setup hits

	proj->contact.obj_uid=contact.obj.uid;
	proj->contact.proj_uid=contact.proj.uid;
	memmove(&proj->contact.hit_poly,&contact.poly,sizeof(poly_pointer_type));

	return(TRUE);
}

