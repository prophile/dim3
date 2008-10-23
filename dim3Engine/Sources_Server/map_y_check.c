/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Y Collision Checking

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
 /Users/bbarnes/dim3Code/dim3Engine/Sources_Server/map_y_check.c:46: warning: 'n' may be used uninitialized in this function

*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif


// supergumba -- move to physics
#define object_box_check_point_division		100
#define object_box_check_radius_simple		500				// this radius or below we use simple checks

#include "scripts.h"
#include "objects.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Find Next Nearest Stand Poly Point
      
======================================================= */

int find_poly_nearest_stand(int x,int y,int z,int ydist,bool ignore_higher)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y;
	if (!ignore_higher) spt.y-=ydist;
	spt.z=z;

	ept.x=x;
	ept.y=y+ydist;
	ept.z=z;

	contact.obj_on=FALSE;
	contact.proj_on=FALSE;

	contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) return(hpt.y);

	return(-1);
}

/* =======================================================

      Pin Downward Movements
      
======================================================= */

int pin_downward_movement_point(int x,int y,int z,int ydist,poly_pointer_type *stand_poly)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y-ydist;
	spt.z=z;

	ept.x=x;
	ept.y=y+ydist;
	ept.z=z;
	
	contact.obj_on=FALSE;
	contact.proj_on=FALSE;

	contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		stand_poly->mesh_idx=contact.poly.mesh_idx;
		stand_poly->poly_idx=contact.poly.poly_idx;
		return(hpt.y);
	}

	stand_poly->mesh_idx=-1;

	return(y+ydist);
}

int pin_downward_movement_box(d3box *box,int ydist,poly_pointer_type *stand_poly)
{
	int						n,cy;
	bool					hits[5];
	d3pnt					spt[5],ept[5],hpt[5];
	ray_trace_contact_type	base_contact,contacts[5];
	
		// setup contact
		
	base_contact.obj_on=FALSE;
	base_contact.proj_on=FALSE;

	base_contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays
	
	spt[0].x=spt[3].x=ept[0].x=ept[3].x=box->min_x;
	spt[1].x=spt[2].x=ept[1].x=ept[2].x=box->max_x;
	
	spt[0].z=spt[1].z=ept[0].z=ept[1].z=box->min_z;
	spt[2].z=spt[3].z=ept[2].z=ept[3].z=box->max_z;
	
	spt[4].x=ept[4].x=box->x;
	spt[4].z=ept[4].z=box->z;
	
	spt[0].y=spt[1].y=spt[2].y=spt[3].y=spt[4].y=box->max_y-ydist;
	ept[0].y=ept[1].y=ept[2].y=ept[3].y=ept[4].y=box->max_y+ydist;
		
		// run the rays
		
	ray_trace_map_by_point_array(5,spt,ept,hpt,hits,&base_contact,contacts);
	
		// find the highest point
		
	stand_poly->mesh_idx=-1;
	cy=-1;
	
	for (n=0;n!=5;n++) {
		
			// check poly collisions
			
		if (hits[n]) {
		
			if (stand_poly->mesh_idx==-1) {
				stand_poly->mesh_idx=contacts[n].poly.mesh_idx;
				stand_poly->poly_idx=contacts[n].poly.poly_idx;
				cy=hpt[n].y;
			}
			else {
				if (hpt[n].y<cy) {
					stand_poly->mesh_idx=contacts[n].poly.mesh_idx;
					stand_poly->poly_idx=contacts[n].poly.poly_idx;
					cy=hpt[n].y;
				}
			}
			
		}
	}
	
	if (cy==-1) return(box->max_y+ydist);
	
	return(cy);
}

int pin_downward_movement_obj(obj_type *obj,int my)
{
	int				y;
	bool			simple;
	d3box			box;
	map_mesh_type	*mesh;
	
		// determine if we should do easy check or complex
		// check.  Players and objects with rigid body get
		// more complex checks

	simple=(!obj->player) && (!obj->rigid_body.on);
	
		// if simple, we can check to see if it's currently
		// on a mesh and if that mesh is not moveable.  If so,
		// don't recheck
		
	if (simple) {
	
		if (obj->contact.stand_poly.mesh_idx!=-1) {
		
			mesh=&map.mesh.meshes[obj->contact.stand_poly.mesh_idx];
			if (!mesh->flag.moveable) return;
		
		// supergumba -- move this around to falling part
		}
	
	}
		
		// otherwise, recheck the movement
		
	box_create_from_object(&box,obj);

	if (simple) {
		y=pin_downward_movement_point(box.x,box.max_y,box.z,my,&obj->contact.stand_poly);
		fprintf(stdout,"%d: simple\n",obj->uid);
	}
	else {
		y=pin_downward_movement_box(&box,my,&obj->contact.stand_poly);
		fprintf(stdout,"%d: complex\n",obj->uid);
	}
	
	if (obj->contact.stand_poly.mesh_idx==-1) return(my);
	
	return(y-box.max_y);
}

/* =======================================================

      Pin Upward Movements
      
======================================================= */

int pin_upward_movement_point(int x,int y,int z,int ydist,poly_pointer_type *head_poly)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y+ydist;
	spt.z=z;

	ept.x=x;
	ept.y=y-ydist;
	ept.z=z;

	contact.obj_on=FALSE;
	contact.proj_on=FALSE;

	contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		head_poly->mesh_idx=contact.poly.mesh_idx;
		head_poly->poly_idx=contact.poly.poly_idx;
		return(hpt.y);
	}

	head_poly->mesh_idx=-1;

	return(y-ydist);
}

int pin_upward_movement_box(d3box *box,int ydist,poly_pointer_type *head_poly)
{
	int						n,cy;
	bool					hits[5];
	d3pnt					spt[5],ept[5],hpt[5];
	ray_trace_contact_type	base_contact,contacts[5];
	
		// setup contact
		
	base_contact.obj_on=FALSE;
	base_contact.proj_on=FALSE;

	base_contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays
	
	spt[0].x=spt[3].x=ept[0].x=ept[3].x=box->min_x;
	spt[1].x=spt[2].x=ept[1].x=ept[2].x=box->max_x;
	
	spt[0].z=spt[1].z=ept[0].z=ept[1].z=box->min_z;
	spt[2].z=spt[3].z=ept[2].z=ept[3].z=box->max_z;
	
	spt[4].x=ept[4].x=box->x;
	spt[4].z=ept[4].z=box->z;
	
	spt[0].y=spt[1].y=spt[2].y=spt[3].y=spt[4].y=box->min_y+ydist;
	ept[0].y=ept[1].y=ept[2].y=ept[3].y=ept[4].y=box->min_y-ydist;
		
		// run the rays
		
	ray_trace_map_by_point_array(5,spt,ept,hpt,hits,&base_contact,contacts);
	
		// find the lowest point
		
	head_poly->mesh_idx=-1;
	cy=-1;
	
	for (n=0;n!=5;n++) {
		
			// check poly collisions
			
		if (hits[n]) {
		
			if (head_poly->mesh_idx==-1) {
				head_poly->mesh_idx=contacts[n].poly.mesh_idx;
				head_poly->poly_idx=contacts[n].poly.poly_idx;
				cy=hpt[n].y;
			}
			else {
				if (hpt[n].y>cy) {
					head_poly->mesh_idx=contacts[n].poly.mesh_idx;
					head_poly->poly_idx=contacts[n].poly.poly_idx;
					cy=hpt[n].y;
				}
			}
			
		}
	}
	
	if (cy==-1) return(box->min_y-ydist);
	
	return(cy);
}

int pin_upward_movement_obj(obj_type *obj,int my)
{
	int				y;
	d3box			box;

		// choose either simple one point checks or more
		// complex checks

	box_create_from_object(&box,obj);

	if (obj->size.radius<=object_box_check_radius_simple) {
		y=pin_upward_movement_point(box.x,box.min_y,box.z,abs(my),&obj->contact.head_poly);
	}
	else {
		y=pin_upward_movement_box(&box,abs(my),&obj->contact.head_poly);
	}
	
	if (obj->contact.head_poly.mesh_idx==-1) return(my);

	return(box.min_y-y);
}

/* =======================================================

      Check if Object is being Crushed
      
======================================================= */

bool map_crush_object(obj_type *obj)
{
	/* supergumba -- need to work on this
	int					sy,fudge;
	d3box				box;
	poly_pointer_type	poly;

		// crushing fudge (only check middle 1/2 of object)

	fudge=obj->size.y>>2;

		// get movement box

	box_create_from_object(&box,obj);
	box.min_y=obj->pos.y-fudge;

	sy=obj->size.y-(fudge<<1);

		// possible to move up?

	pin_upward_movement_box(&box,-sy,&poly);
	return(poly.portal_idx!=-1);
*/
	return(FALSE);
}

/* =======================================================

      Check if Standing Up is Possible
      
======================================================= */

bool map_stand_check_object(obj_type *obj)
{
	int					sy;
	d3box				box;
	poly_pointer_type	poly;

		// total stand up height

	sy=obj->duck.y_change;

		// get movement box

	box_create_from_object(&box,obj);
	box.min_y=(obj->pnt.y-obj->size.y)+sy;

		// possible to move up?

	pin_upward_movement_box(&box,-sy,&poly);
	return(poly.mesh_idx!=-1);
}

