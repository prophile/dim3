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

#include "scripts.h"
#include "objects.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Check Floor/Ceiling for Point
      
======================================================= */

int find_poly_for_upward_point(int x,int y,int z,int ydist,poly_pointer_type *poly)
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

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		memmove(poly,&contact.poly,sizeof(poly_pointer_type));
		return(hpt.y);
	}

	poly->portal_idx=-1;

	return(y-ydist);
}

int find_poly_for_downward_point(int x,int y,int z,int ydist,poly_pointer_type *poly)
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

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		memmove(poly,&contact.poly,sizeof(poly_pointer_type));
		return(hpt.y);
	}

	poly->portal_idx=-1;

	return(y+ydist);
}

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

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) return(hpt.y);

	return(-1);
}

/* =======================================================

      Pin Downward Box Movements
      
======================================================= */

// supergumba -- testing

/*
int pin_downward_movement_box(d3box *box,int ydist,poly_pointer_type *stand_poly)
{
	int					x,y,z,cy,fy,
						xdiv,zdiv,xsz,zsz;
	poly_pointer_type	poly;

		// get check division

	xsz=(box->max_x-box->min_x)/object_box_check_point_division;
	if (xsz<=0) xsz=1;

	zsz=(box->max_z-box->min_z)/object_box_check_point_division;
	if (zsz<=0) zsz=1;

		// find the highest point
		
	stand_poly->portal_idx=-1;
	cy=-1;

	y=box->max_y;
	
	for (zdiv=0;zdiv<=zsz;zdiv++) {

		if (zdiv==zsz) {
			z=box->max_z;
		}
		else {
			z=box->min_z+(zdiv*object_box_check_point_division);
		}

		for (xdiv=0;xdiv<=xsz;xdiv++) {

			if (xdiv==xsz) {
				x=box->max_x;
			}
			else {
				x=box->min_x+(xdiv*object_box_check_point_division);
			}
		
				// check poly collisions
				
			fy=find_poly_for_downward_point(x,y,z,ydist,&poly);
			if (poly.portal_idx!=-1) {
				if (stand_poly->portal_idx==-1) {
					memmove(stand_poly,&poly,sizeof(poly_pointer_type));
					cy=fy;
				}
				else {
					if (fy<cy) {
						memmove(stand_poly,&poly,sizeof(poly_pointer_type));
						cy=fy;
					}
				}
			}
		}
	}
	
	return(cy);
}
*/

int pin_downward_movement_box(d3box *box,int ydist,poly_pointer_type *stand_poly)
{
	int					i,x,y,z,cy,fy,px[5],pz[5];
	poly_pointer_type	poly;
	
	y=box->max_y;
	
	px[0]=px[3]=box->min_x;
	px[1]=px[2]=box->max_x;
	
	pz[0]=pz[1]=box->min_z;
	pz[2]=pz[3]=box->max_z;
	
	px[4]=box->x;
	pz[4]=box->z;
	
		// find the highest point
		
	stand_poly->portal_idx=-1;
	cy=-1;
	
	for (i=0;i!=5;i++) {
		x=px[i];
		z=pz[i];
		
			// check poly collisions
			
		fy=find_poly_for_downward_point(x,y,z,ydist,&poly);
		if (poly.portal_idx!=-1) {
			if (stand_poly->portal_idx==-1) {
				memmove(stand_poly,&poly,sizeof(poly_pointer_type));
				cy=fy;
			}
			else {
				if (fy<cy) {
					memmove(stand_poly,&poly,sizeof(poly_pointer_type));
					cy=fy;
				}
			}
		}
	}
	
	return(cy);
}

/* =======================================================

      Pin Upward Box Movements
      
======================================================= */

int pin_upward_movement_box(d3box *box,int ydist,poly_pointer_type *head_poly)
{
	int					i,x,y,z,cy,fy,px[5],pz[5];
	poly_pointer_type	poly;
	
	y=box->min_y;
	
	px[0]=px[3]=box->min_x;
	px[1]=px[2]=box->max_x;
	
	pz[0]=pz[1]=box->min_z;
	pz[2]=pz[3]=box->max_z;
	
	px[4]=box->x;
	pz[4]=box->z;
	
		// find the lowest point
		
	head_poly->portal_idx=-1;
	cy=-1;
	
	for (i=0;i!=5;i++) {
		x=px[i];
		z=pz[i];
		
			// check poly collisions
			
		fy=find_poly_for_upward_point(x,y,z,ydist,&poly);
		if (poly.portal_idx!=-1) {
			if (head_poly->portal_idx==-1) {
				memmove(head_poly,&poly,sizeof(poly_pointer_type));
				cy=fy;
			}
			else {
				if (fy>cy) {
					memmove(head_poly,&poly,sizeof(poly_pointer_type));
					cy=fy;
				}
			}
		}
	}

	return(cy);
}

/* =======================================================

      Pin Object Movements
      
======================================================= */

int pin_downward_movement_obj(obj_type *obj,int my)
{
	int				y;
	d3box			box;

	box_create_from_object(&box,obj);
	y=pin_downward_movement_box(&box,my,&obj->contact.stand_poly);

	if (y==-1) return(my);
	
	return(y-box.max_y);
}

int pin_upward_movement_obj(obj_type *obj,int my)
{
	int				y;
	d3box			box;

	box_create_from_object(&box,obj);
	y=pin_upward_movement_box(&box,abs(my),&obj->contact.head_poly);

	if (y==-1) return(my);

	return(box.min_y-y);
}

/* =======================================================

      Pin Y Movement against Map
      
======================================================= */

int pin_downward_movement_point(int x,int y,int z,int my,bool *hit)
{
	int						sy;
	poly_pointer_type		poly;

	sy=find_poly_for_downward_point(x,y,z,my,&poly);
	if (poly.portal_idx==-1) {
		*hit=FALSE;
		return(0);
	}

	*hit=TRUE;
	return(sy-y);
}

int pin_upward_movement_point(int x,int y,int z,int my,bool *hit)
{
	int						sy;
	poly_pointer_type		poly;

	sy=find_poly_for_upward_point(x,y,z,my,&poly);
	if (poly.portal_idx==-1) {
		*hit=FALSE;
		return(0);
	}

	*hit=TRUE;
	return(sy-y);
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
	box.min_y=(obj->pos.y-obj->size.y)+sy;

		// possible to move up?

	pin_upward_movement_box(&box,-sy,&poly);
	return(poly.portal_idx!=-1);
}

