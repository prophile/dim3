/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Selection Movement Routines

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
 
*********************************************************************/

#include "common_view.h"

extern int					vertex_mode;

extern map_type				map;

/* =======================================================

      Get Selection 2D Extent
      
======================================================= */

void select_get_extent(int *min_x,int *min_z,int *min_y,int *max_x,int *max_z,int *max_y)
{
	int				n,sel_count,
					type,index,t_min_x,t_min_z,t_max_x,t_max_z,t_min_y,t_max_y;
	
	*min_x=*min_z=*min_y=1000000;
	*max_x=*max_z=*max_y=0;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		
		piece_get_extent(type,index,&t_min_x,&t_min_z,&t_max_x,&t_max_z,&t_min_y,&t_max_y);
		
		if (t_min_x<(*min_x)) *min_x=t_min_x;
		if (t_min_z<(*min_z)) *min_z=t_min_z;
		if (t_min_y<(*min_y)) *min_y=t_min_y;
		if (t_max_x>(*max_x)) *max_x=t_max_x;
		if (t_max_z>(*max_z)) *max_z=t_max_z;
		if (t_max_y>(*max_y)) *max_y=t_max_y;
	}
}

/* =======================================================

      Selection Moving
      
======================================================= */

void select_move(int rn,int xadd,int zadd,int yadd)
{
	int				n,sel_count,
					type,index,sel_min_x,sel_min_z,sel_max_x,sel_max_z,sel_min_y,sel_max_y;
	
		// can we move inside portal?
		
	select_get_extent(&sel_min_x,&sel_min_z,&sel_min_y,&sel_max_x,&sel_max_z,&sel_max_y);
	
	sel_min_x-=xadd;
	sel_max_x-=xadd;
	sel_min_z-=zadd;
	sel_max_z-=zadd;
	
	if (sel_min_x<0) xadd=0;
	if (sel_max_x>(map.portals[rn].ex-map.portals[rn].x)) xadd=0;
	
	if (sel_min_z<0) zadd=0;
	if (sel_max_z>(map.portals[rn].ez-map.portals[rn].z)) zadd=0;
	
	if ((xadd==0) && (zadd==0) && (yadd==0)) return;
	
		// move selection
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);

		switch (type) {
		
			case segment_piece:
				if (vertex_mode==vm_lock) vertexes_move_similiar_to_segment(&map.segments[index],xadd,yadd,zadd);
				segment_move(&map.segments[index],xadd,zadd,yadd);
				break;
				
			case primitive_piece:
				primitive_move(map.segments[index].primitive_uid[0],xadd,zadd,yadd);
				break;
				
			case node_piece:
				map.nodes[index].pos.x-=xadd;
				map.nodes[index].pos.z-=zadd;
				map.nodes[index].pos.y-=yadd;
				break;
				
			case spot_piece:
				map.spots[index].pos.x-=xadd;
				map.spots[index].pos.z-=zadd;
				map.spots[index].pos.y-=yadd;
				break;
				
			case scenery_piece:
				map.sceneries[index].pos.x-=xadd;
				map.sceneries[index].pos.z-=zadd;
				map.sceneries[index].pos.y-=yadd;
				break;
				
			case light_piece:
				map.lights[index].pos.x-=xadd;
				map.lights[index].pos.z-=zadd;
				map.lights[index].pos.y-=yadd;
				break;
				
			case sound_piece:
				map.sounds[index].pos.x-=xadd;
				map.sounds[index].pos.z-=zadd;
				map.sounds[index].pos.y-=yadd;
				break;
				
			case particle_piece:
				map.particles[index].pos.x-=xadd;
				map.particles[index].pos.z-=zadd;
				map.particles[index].pos.y-=yadd;
				break;
				
		}
	}
}

