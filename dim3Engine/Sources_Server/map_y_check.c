/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Y Collision Checking

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

      Check Floor/Ceiling for Point
      
======================================================= */

inline int fc_in_point(fc_segment_data *fc,int x,int y,int z,int ydist)
{
	if (x<fc->min_x) return(-1);
	if (x>fc->max_x) return(-1);
	if (z<fc->min_z) return(-1);
	if (z>fc->max_z) return(-1);
	if (y<(fc->min_y-ydist)) return(-1);
	if (y>(fc->max_y+ydist)) return(-1);
	
	if (fc->flat) {
		if (!polygon_2D_point_inside(fc->ptsz,fc->x,fc->z,x,z)) return(-1);
		return(fc->y[0]);
	}
	
	return(polygon_find_y(fc->ptsz,fc->x,fc->y,fc->z,x,z));
}

int find_poly_for_upward_point(int rn,int x,int y,int z,int ydist,int *mesh_idx,int *poly_idx)
{
	/* supergumba

	int			i,cnt,idx,ty,by,sy;
	short		*sptr;
	
	*hit=-1;

	ty=y-ydist;
	by=y+ydist;
	
	cnt=map.portals[rn].fc_list_hit.count;
	sptr=map.portals[rn].fc_list_hit.list;

	for (i=0;i!=cnt;i++) {
		idx=(int)*sptr++;

		sy=fc_in_point(&map.segments[idx].data.fc,x,y,z,ydist);
		if (sy==-1) continue;

			// when comparing floor/ceiling points in a upward
			// direction, we want to find the lowest
			// point within the slope distance
			
		if ((sy<by) && (sy>ty)) {
			*hit=idx;
			ty=sy;
		}
	}

	if (*hit==-1) return(-1);
	
	return(ty);
	*/
	return(y);
}


/*
	ray_trace_contact_type	contact[15];

		// get collision points

	collide_object_ray_trace_points(obj,*xadd,*zadd,px,py,pz);

		// run rays on all points
		// find the one that moves the leasts
		// as the most suitable hit point

	idx=-1;
	dist=0;

	for (n=0;n!=15;n++) {
			
			// setup ray trace

	
			// run trace

		if (ray_trace_map_by_point(&spt,&ept,&hpt[n],&contact[n])) {
			d=collide_point_distance(&spt,&hpt[n]);
			if ((d<=dist) || (idx==-1)) {
				dist=d;
				idx=n;
			}
		}
	}
*/

int find_poly_for_downward_point(int x,int y,int z,int ydist,poly_pointer_type *poly)
{
	int						i,cnt,idx,ty,by,sy;
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y-ydist;
	spt.z=z;

	ept.x=x;
	ept.y=y+ydist;
	ept.z=z;

	contact.obj_on=FALSE;
	contact.proj_on=FALSE;		// supergumba -- rewrite ALL of this

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		memmove(poly,&contact.poly,sizeof(poly_pointer_type));
		return(hpt.y);
	}

	poly->portal_idx=-1;

	return(y+ydist);
}

/* =======================================================

      Find Next Nearest Floor\Ceiling Point
      
======================================================= */

int fc_nearest_y(int rn,int x,int y,int z,int ydist,bool ignore_higher)
{
	int					i,cnt,idx,ty,sy,fy;
	short				*sptr;
	fc_segment_data		*fc;
	
	fy=-1;
	
	cnt=map.portals[rn].fc_list_hit.count;
	sptr=map.portals[rn].fc_list_hit.list;
	
	for (i=0;i!=cnt;i++) {
		idx=(int)*sptr++;
		
		fc=&map.segments[idx].data.fc;
		
		if ((x<fc->min_x) || (x>fc->max_x) || (z<fc->min_z) || (z>fc->max_z)) continue;
		
		if (fc->flat) {
			sy=fc->y[0];
		}
		else {
			sy=polygon_find_y(fc->ptsz,fc->x,fc->y,fc->z,x,z);
			if (sy==-1) continue;
		}

		if ((ignore_higher) && (sy<y)) continue;

		ty=abs(sy-y);
		if (ty<ydist) {
			ydist=ty;
			fy=sy;
		}
	}
	
	return(fy);
}

/* =======================================================

      Pin Downward Box Movements
      
======================================================= */

int pin_downward_movement_box(d3box *box,int ydist,poly_pointer_type *stand_poly)
{
	int				i,rn,x,y,z,cy,fy,
					portal_idx,mesh_idx,poly_idx,hit_portal_idx,hit_mesh_idx,hit_poly_idx,
					px[5],pz[5];
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
		
		rn=map_find_portal_hint(&map,box->hint_rn,x,y,z);
		if (rn==-1) continue;
		
			// check floor\ceiling
			
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
	int				i,rn,x,y,z,cy,fy,
					mesh_idx,poly_idx,hit_mesh_idx,hit_poly_idx,
					px[5],pz[5];
	
	y=box->min_y;
	
	px[0]=px[3]=box->min_x;
	px[1]=px[2]=box->max_x;
	
	pz[0]=pz[1]=box->min_z;
	pz[2]=pz[3]=box->max_z;
	
	px[4]=box->x;
	pz[4]=box->z;
	
		// find the lowest point
		
	hit_mesh_idx=-1;
	cy=-1;
	
	for (i=0;i!=5;i++) {
		x=px[i];
		z=pz[i];
		
		rn=map_find_portal_hint(&map,box->hint_rn,x,y,z);
		if (rn==-1) continue;
		
			// check floor\ceiling
			
		fy=find_poly_for_upward_point(rn,x,y,z,ydist,&mesh_idx,&poly_idx);
		if (mesh_idx!=-1) {
			if (hit_mesh_idx==-1) {
				hit_mesh_idx=mesh_idx;
				hit_poly_idx=poly_idx;
				cy=fy;
			}
			else {
				if (fy>cy) {
					hit_mesh_idx=mesh_idx;
					hit_poly_idx=poly_idx;
					cy=fy;
				}
			}
		}
	}
	
		// any hits?
		
	if (mesh_idx!=-1) {
		head_poly->portal_idx=rn;
		head_poly->mesh_idx=hit_mesh_idx;
		head_poly->poly_idx=hit_poly_idx;
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

      Pin Projectile Movements
      
======================================================= */

int pin_downward_movement_proj(proj_type *proj,int my)
{
	/* supergumba
	int			sy,seg_idx;
	
	sy=find_poly_for_downward_point(proj->pos.rn,proj->pos.x,proj->pos.y,proj->pos.z,my,&seg_idx);
	if ((sy==-1) || (seg_idx==-1)) return(my);

	switch (map.segments[seg_idx].type) {
		case sg_floor:
			proj->contact.floor_seg_idx=seg_idx;
			break;
		case sg_ceiling:
			proj->contact.ceiling_seg_idx=seg_idx;
			break;
	}

	return(sy-proj->pos.y);
	*/
	return(0);
}

int pin_upward_movement_proj(proj_type *proj,int my)
{
	/* supergumba
	int			y,sy,seg_idx;
	
	y=proj->pos.y-proj->size.y;
	
	sy=find_poly_for_upward_point(proj->pos.rn,proj->pos.x,y,proj->pos.z,abs(my),&seg_idx);
	if ((sy==-1) || (seg_idx==-1)) return(my);

	switch (map.segments[seg_idx].type) {
		case sg_floor:
			proj->contact.floor_seg_idx=seg_idx;
			break;
		case sg_ceiling:
			proj->contact.ceiling_seg_idx=seg_idx;
			break;
	}

	return(sy-proj->pos.y);
	*/
	return(0);
}

/* =======================================================

      Pin Y Movement against Map
      
======================================================= */

int pin_downward_movement_point(int rn,int x,int y,int z,int my,int *seg_idx)
{
	/* supergumba -- fix
	int			sy;

	sy=find_poly_for_downward_point(rn,x,y,z,my,seg_idx);
	if (sy==-1) return(my);

	return(sy-y);
	*/
	return(0);
}

int pin_upward_movement_point(int rn,int x,int y,int z,int my,int *seg_idx)
{
	/* supergumba -- fix
	int			sy;

	sy=find_poly_for_upward_point(rn,x,y,z,abs(my),seg_idx);
	if (sy==-1) return(my);

	return(sy-y);
	*/
	return(0);
}

/* =======================================================

      Check if Floor/Ceiling Segments Crush Object
      
======================================================= */

bool map_crush_floor_ceiling(fc_segment_data *fc,d3box *box)
{
	if (fc->min_x>=box->max_x) return(FALSE);
	if (fc->max_x<=box->min_x) return(FALSE);
	if (fc->min_z>=box->max_z) return(FALSE);
	if (fc->max_z<=box->min_z) return(FALSE);
	if (fc->min_y>=box->max_y) return(FALSE);
	return(!(fc->max_y<=box->min_y));
}

bool map_crush_object(obj_type *obj)
{
	int				i,rn,idx,cnt;
	short			*sptr;
	d3box			box;
	portal_type		*portal;
	segment_type	*seg;
	
	box_create_from_object(&box,obj);

	rn=box.hint_rn;
	portal=&map.portals[rn];

		// check floors/ceilings

	cnt=portal->fc_list_hit.count;
	sptr=portal->fc_list_hit.list;

	for (i=0;i!=cnt;i++) {
		idx=(int)*sptr++;
		seg=&map.segments[idx];
		if (seg->moveable) {
			if (map_crush_floor_ceiling(&seg->data.fc,&box)) return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Check if Standing Up is Possible
      
======================================================= */

bool map_stand_check_object(obj_type *obj)
{
	int				i,rn,idx,cnt,sz;
	short			*sptr;
	d3box			box;
	portal_type		*portal;
	
	box_create_from_object(&box,obj);
	
		// reduce hit to only check top of head
		
	sz=(obj->size.y>>2)*3;
	
	box.y-=sz;
	box.max_y-=sz;
		
		// get check portal

	rn=box.hint_rn;
	portal=&map.portals[rn];

		// check floors\ceilings

	cnt=portal->fc_list_hit.count;
	sptr=portal->fc_list_hit.list;

	for (i=0;i!=cnt;i++) {
		idx=(int)*sptr++;
		if (map_crush_floor_ceiling(&map.segments[idx].data.fc,&box)) return(TRUE);
	}

	return(FALSE);
}

