/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map XZ Collision Checking

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

extern int				nobjinmap;

extern map_type			map;
extern server_type		server;

/* =======================================================

      Movement Against Portals/Segments
      
======================================================= */

void map_clipped_wall_y_at_spot(int x,int z,int lx,int lz,int rx,int rz,int *ty,int *by,int clip)
{
/* supergumba
	int			ys,hitdist,totdist;
	
	hitdist=distance_2D_get(x,z,lx,lz);
	totdist=distance_2D_get(rx,rz,lx,lz);
	
	ys=(*by)-(*ty);
	
	switch (clip) {
		case wc_top:
			*ty=(*ty)+((ys*hitdist)/totdist);
			break;
		case wc_bottom:
			*by=(*by)-((ys*hitdist)/totdist);
			break;
		case wc_slant:
			ys=(((ys>>1)*hitdist)/totdist);
			*ty=(*ty)+ys;
			*by=(*by)-ys;
			break;
	}
	*/
}

int move_check_xz_segments(d3box *box,int cnt,short *sptr)
{
/*
	int					i,idx,
						lx,rx,lz,rz,ty,by,clip;
	segment_type		*seg;
	wall_segment_data  *wall;
	
	for (i=0;i!=cnt;i++) {
		idx=(int)*sptr++;
		
		seg=&map.segments[idx];
		wall=&seg->data.wall;
		
			// completely out of Ys?
		
		ty=wall->ty;
		by=wall->by;
		
		if ((box->min_y>=by) || (box->max_y<=ty)) continue;
		
			// get wall points
			
		lx=wall->lx;
		rx=wall->rx;
		lz=wall->lz;
		rz=wall->rz;
		
			// further reduce y collisions for clipped walls
			
		clip=seg->clip;
		
		if (clip!=wc_none) {
			map_clipped_wall_y_at_spot(box->x,box->z,lx,lz,rx,rz,&ty,&by,clip);
			if (ty>by) continue;
			if ((box->min_y>=by) || (box->max_y<=ty)) continue;
		}
		
			// check x/z (2D) wall
			
		if (box_collide_2D_line(box,lx,rx,lz,rz)) return(idx);
	}
	*/
	return(-1);
}

int move_check_xz_portals(d3box *box)
{
	/* supergumba -- redo
	int				n,idx;
	portal_type		*portal;
	
		// find all portals box hits
		
	portal=map.portals;
	
	for (n=0;n!=map.nportal;n++) {
	
		if (!box_in_portal(box,portal)) {
			portal++;
			continue;
		}
		
		idx=move_check_xz_segments(box,portal->wall_list_hit.count,portal->wall_list_hit.list);
		if (idx!=-1) return(idx);
		
		portal++;
	}
	*/
	return(-1);
}

/* =======================================================

      Move Objects In XZ Map
      
======================================================= */

bool map_obj_move_xz(obj_type *obj,int x,int z)
{
	/* supergumba
	int			wall_seg_idx,obj_uid;
	d3box		box;
	
		// get box
		
	box_create_from_object(&box,obj);
	box_add_movement(&box,x,z);
	if (!box_in_map(&box)) return(TRUE);
	
		// wall segments

	wall_seg_idx=move_check_xz_portals(&box);
	if (wall_seg_idx!=-1) {
		obj->contact.wall_seg_idx=wall_seg_idx;
		return(TRUE);
	}

		// objects

	obj_uid=collide_find_object_for_object_move(obj,x,z);
	if (obj_uid!=-1) {
		obj->contact.obj_uid=obj_uid;
		return(TRUE);
	}

		// move object
		
	obj->pos.x+=x;
	obj->pos.z+=z;
*/
	return(FALSE);
}

bool move_obj_check_xz_map_slide_line(obj_type *obj,int x,int z,int lx,int rx,int lz,int rz)
{
	int					n,mx,mz;
	float				f,ang,rang;
	bool				cwise;
	d3vct				line_vct,obj_vct;
	
		// special check for horizontal/vertical walls

	if (lx==rx) {
		if (!map_obj_move_xz(obj,0,z)) return(FALSE);
		return(map_obj_move_xz(obj,x,0));
	}
	
	if (lz==rz) {
		if (!map_obj_move_xz(obj,x,0)) return(FALSE);
		return(map_obj_move_xz(obj,0,z));
	}
	
		// get angle between the line and the object movement
			
	obj_vct.x=(float)x;
	obj_vct.y=0.0f;
	obj_vct.z=(float)z;
	vector_normalize(&obj_vct);
	
	vector_create(&line_vct,lx,0,lz,rx,0,rz);		// perpendicular vector (swap x/z)
	
		// get the angle between them

	f=vector_dot_product(&obj_vct,&line_vct);
	f=((float)acos(f))*RAD_to_ANG;

		// get angle of wall.  If the angle between the
		// collision lines is less than 90, then head down
		// the line the opposite way

	ang=angle_find(lx,lz,rx,rz);
	if (f<90.0f) ang=angle_add(ang,180.0f);

		// change the motion to reflect the angle of the wall
		// uses facing instead of motion to check so we
		// don't get motion build up from sliding against a wall
		
	rang=obj->ang.y;
	if (obj->forward_move.reverse) rang=angle_add(rang,180.0f);

	if (angle_dif(rang,ang,&cwise)<90.0f) {
		if (!obj->forward_move.reverse) {
			obj->motion.ang.y=ang;
		}
		else {
			obj->motion.ang.y=angle_add(ang,180.0f);
		}
	}

		// reduce movement to slide against the walls

	mz=z/ws_step_factor;
	mx=x/ws_step_factor;

	for (n=0;n!=ws_step_factor;n++) {
		if (!map_obj_move_xz(obj,0,mz)) {
			map_obj_move_xz(obj,mx,0);
		}
		else {
			if (!map_obj_move_xz(obj,mx,0)) {
				map_obj_move_xz(obj,0,mz);
			}
		}
	}
	
	return(FALSE);
}

bool move_obj_check_xz_map_slide(obj_type *obj,int x,int z)
{
	/* supergumba
	int					bump_y,lx,rx,lz,rz,hit_box_idx;
	wall_segment_data	*wall;
	obj_type			*cnt_obj;
	model_draw			*draw;
	model_type			*model;

		// first attempt to move cleanly

	if (!map_obj_move_xz(obj,x,z)) return(FALSE);

		// if wall hit, then find vector for wall
		// that was hit and attempt to slide across
		// the wall
	
	if (obj->contact.wall_seg_idx!=-1) {

		wall=&map.segments[obj->contact.wall_seg_idx].data.wall;

			// don't slide on walls that are bump up candidates

		if (obj->bump.on) {
			bump_y=obj->pos.y-wall->ty;
			if ((bump_y>0) && (bump_y<=obj->bump.high)) return(TRUE);
		}
		
			// slide against the angle of the wall
			
		return(move_obj_check_xz_map_slide_line(obj,x,z,wall->lx,wall->rx,wall->lz,wall->rz));
	}

		// if object hit, find the collision lines
		// between the object and slide across that
		// line

	if (obj->contact.obj_uid==-1) return(TRUE);

	cnt_obj=object_find_uid(obj->contact.obj_uid);
	if (cnt_obj==NULL) return(FALSE);

		// no hit box collisions

	if (!cnt_obj->hit_box.on) {
		if (!collide_object_to_object_slide(obj,x,z,cnt_obj,&lx,&rx,&lz,&rz)) return(TRUE);
		return(move_obj_check_xz_map_slide_line(obj,x,z,lx,rx,lz,rz));
	}

		// hit box collisions

	hit_box_idx=obj->contact.hit_box_idx;
	if (hit_box_idx==-1) return(TRUE);
		
	draw=&cnt_obj->draw;
	if ((draw->uid==-1) || (!draw->on)) return(TRUE);
	
	model=model_find_uid(draw->uid);
	if (model==NULL) return(TRUE);
	
		// check hit boxes
		
	if (!collide_object_to_object_hit_box_slide(obj,x,z,cnt_obj,&model->hit_boxes[hit_box_idx],&lx,&rx,&lz,&rz)) return(TRUE);
	return(move_obj_check_xz_map_slide_line(obj,x,z,lx,rx,lz,rz));
	*/
	return(FALSE);
}

bool move_obj_check_bump(obj_type *obj,int x,int z,int y)
{
	d3box			box;
	
		// get object box in bumped up state
		
	obj->pos.x+=x;
	obj->pos.z+=z;
	obj->pos.y+=y;
	
	box_create_from_object(&box,obj);

	obj->pos.x-=x;
	obj->pos.z-=z;
	obj->pos.y-=y;

		// check if in map or hitting wall
		
	if (!box_in_map(&box)) return(FALSE);

	return(move_check_xz_portals(&box)==-1);
}	

/* =======================================================

      Map Spot Empty
      
======================================================= */

bool map_spot_empty_sphere(int sx,int sy,int sz,int radius,int ignore_obj_uid,int *hit_obj_uid)
{
	d3box			box;
	
	*hit_obj_uid=-1;
	
	box_create_from_point(&box,sx,sy,sz,radius,radius);
	if (!box_in_map(&box)) return(TRUE);
	
	if (move_check_xz_portals(&box)!=-1) return(TRUE);
	
	*hit_obj_uid=collide_find_object_for_sphere(sx,sy,sz,radius,ignore_obj_uid);
	return(*hit_obj_uid!=-1);
}

bool map_spot_empty_object(obj_type *obj)
{
	d3box			box;
	
	box_create_from_object(&box,obj);
	if (!box_in_map(&box)) return(TRUE);

	if (move_check_xz_portals(&box)!=-1) return(TRUE);
	
	return(collide_find_object_for_object(obj)!=-1);
}
