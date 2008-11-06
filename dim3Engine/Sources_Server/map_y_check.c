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

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

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
	
	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

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

int pin_downward_movement_complex(obj_type *obj,int ydist,poly_pointer_type *stand_poly)
{
	int						n,cy,sz;
	bool					hits[5];
	d3pnt					spt[5],ept[5],hpt[5];
	ray_trace_contact_type	base_contact,contacts[5];
	
		// setup contact
		
	base_contact.obj.on=FALSE;
	base_contact.proj.on=FALSE;

	base_contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays
	
	sz=obj->size.x>>1;
	spt[0].x=spt[3].x=ept[0].x=ept[3].x=obj->pnt.x-sz;
	spt[1].x=spt[2].x=ept[1].x=ept[2].x=obj->pnt.x+sz;
	
	sz=obj->size.z>>1;
	spt[0].z=spt[1].z=ept[0].z=ept[1].z=obj->pnt.z-sz;
	spt[2].z=spt[3].z=ept[2].z=ept[3].z=obj->pnt.z+sz;
	
	spt[4].x=ept[4].x=obj->pnt.x;
	spt[4].z=ept[4].z=obj->pnt.z;
	
	spt[0].y=spt[1].y=spt[2].y=spt[3].y=spt[4].y=obj->pnt.y-ydist;
	ept[0].y=ept[1].y=ept[2].y=ept[3].y=ept[4].y=obj->pnt.y+ydist;
		
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
	
	if (cy==-1) return(obj->pnt.y+ydist);
	
	return(cy);
}

int pin_downward_movement_obj(obj_type *obj,int my)
{
	int				y;

		// determine if we should do easy check or complex
		// check.  Players and objects with rigid body get
		// more complex checks

	if ((!obj->player) && (!obj->rigid_body.on)) {
		y=pin_downward_movement_point(obj->pnt.x,obj->pnt.y,obj->pnt.z,my,&obj->contact.stand_poly);
	}
	else {
		y=pin_downward_movement_complex(obj,my,&obj->contact.stand_poly);
	}
	
	if (obj->contact.stand_poly.mesh_idx==-1) return(my);
	
	return(y-obj->pnt.y);
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

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

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

int pin_upward_movement_complex(obj_type *obj,int ydist,poly_pointer_type *head_poly)
{
	int						n,cy,sz;
	bool					hits[5];
	d3pnt					spt[5],ept[5],hpt[5];
	ray_trace_contact_type	base_contact,contacts[5];
	
		// setup contact
		
	base_contact.obj.on=FALSE;
	base_contact.proj.on=FALSE;

	base_contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays
	
	sz=obj->size.x>>1;
	spt[0].x=spt[3].x=ept[0].x=ept[3].x=obj->pnt.x-sz;
	spt[1].x=spt[2].x=ept[1].x=ept[2].x=obj->pnt.x+sz;
	
	sz=obj->size.z>>1;
	spt[0].z=spt[1].z=ept[0].z=ept[1].z=obj->pnt.z-sz;
	spt[2].z=spt[3].z=ept[2].z=ept[3].z=obj->pnt.z+sz;
	
	spt[4].x=ept[4].x=obj->pnt.x;
	spt[4].z=ept[4].z=obj->pnt.z;
	
	sz=obj->size.y;
	if (obj->duck.mode!=dm_stand) sz-=obj->duck.y_move;
	spt[0].y=spt[1].y=spt[2].y=spt[3].y=spt[4].y=(obj->pnt.y-sz)+ydist;
	ept[0].y=ept[1].y=ept[2].y=ept[3].y=ept[4].y=(obj->pnt.y-sz)-ydist;
		
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
	
	if (cy==-1) return((obj->pnt.y-sz)-ydist);
	
	return(cy);
}

int pin_upward_movement_obj(obj_type *obj,int my)
{
	int				y,sz;

		// determine if we should do easy check or complex
		// check.  Players and objects with rigid body get
		// more complex checks

	if ((!obj->player) && (!obj->rigid_body.on)) {
		y=pin_upward_movement_point(obj->pnt.x,obj->pnt.y,obj->pnt.z,abs(my),&obj->contact.head_poly);
	}
	else {
		y=pin_upward_movement_complex(obj,abs(my),&obj->contact.head_poly);
	}
	
	if (obj->contact.head_poly.mesh_idx==-1) return(my);

		// need to get duck value for this factor

	sz=obj->size.y;
	if (obj->duck.mode!=dm_stand) sz-=obj->duck.y_move;

	return((obj->pnt.y-sz)-y);
}

/* =======================================================

      Check if Object is being Crushed from Above
      
======================================================= */

bool map_stand_crush_object(obj_type *obj)
{
	int					fudge;
	poly_pointer_type	poly;

		// pick a crushing fudge, reduce size and
		// see if you can move up that far

	fudge=obj->size.y>>3;

	obj->size.y-=fudge;
	pin_upward_movement_complex(obj,-fudge,&poly);
	obj->size.y+=fudge;
	
	if (poly.mesh_idx==-1) return(FALSE);
	
		// if we can, go into auto duck before crushing
		// if already in duck, crush
		
	if (obj->duck.mode==dm_duck) return(TRUE);
	
	object_start_duck(obj);
	
	return(FALSE);
}

/* =======================================================

      Check if Standing Up is Possible
      
======================================================= */

bool map_stand_check_object(obj_type *obj)
{
	int					sy;
	poly_pointer_type	poly;

		// total stand up height

	sy=obj->duck.y_change;

		// possible to move up?

	pin_upward_movement_complex(obj,-sy,&poly);
	return(poly.mesh_idx!=-1);
}

