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


// supergumba -- we need to fix all this


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

      Map Spot Empty
      
======================================================= */

bool map_spot_empty_sphere(int sx,int sy,int sz,int radius,int ignore_obj_uid,int *hit_obj_uid)
{
	d3box			box;
	
	*hit_obj_uid=-1;
	
	box_create_from_point(&box,sx,sy,sz,radius,radius);
	
	if (move_check_xz_portals(&box)!=-1) return(TRUE);
	
	*hit_obj_uid=collide_find_object_for_sphere(sx,sy,sz,radius,ignore_obj_uid);
	return(*hit_obj_uid!=-1);
}

bool map_spot_empty_object(obj_type *obj)
{
	d3box			box;
	
	box_create_from_object(&box,obj);

	if (move_check_xz_portals(&box)!=-1) return(TRUE);
	
	return(collide_find_object_for_object(obj)!=-1);
}
