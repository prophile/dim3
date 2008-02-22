/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Updating Texture Shifted Segments

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Update Segments with Texture Shifting
      
======================================================= */

void map_portal_segment_update_shift_single(float f_tick,segment_type *seg)
{
	int				k;
	
	k=(int)(f_tick*seg->x_shift);
	seg->x_txtoff=(float)((1000-k)%1000)/1000.0f;
		
	k=(int)(f_tick*seg->y_shift);
	seg->y_txtoff=(float)((1000-k)%1000)/1000.0f;
}

void map_portal_segment_update_uvs(portal_type *portal,segment_type *seg)
{
	int							n,ptsz;
	portal_vertex_list_type		*vl;
	
	ptsz=seg->draw.ptsz;
	
	for (n=0;n!=ptsz;n++) {
		vl=portal->vertexes.vertex_list+seg->draw.idx[n];
		vl->gx=seg->draw.gx[n];
		vl->gy=seg->draw.gy[n];
	}
}

void map_portal_segment_update_shift(map_type *map,int tick,int rn)
{
	int							i,idx,cnt;
	short						*sptr;
	float						f_tick;
	portal_type					*portal;
	segment_type				*seg;
	
	portal=&map->portals[rn];
	
	f_tick=(float)tick;
	
	cnt=portal->segment_list_texture_shift.count;
	sptr=portal->segment_list_texture_shift.list;
	
	for (i=0;i!=cnt;i++) {
		idx=(int)*sptr++;
		seg=&map->segments[idx];
		
		switch (seg->type) {
		
			case sg_wall:
				map_portal_segment_update_shift_single(f_tick,seg);
				map_prepare_create_wall_segment_uv(seg);
				map_portal_segment_update_uvs(portal,seg);
				break;
				
			case sg_floor:
			case sg_ceiling:
				map_portal_segment_update_shift_single(f_tick,seg);
				map_prepare_create_fc_segment_uv(seg);
				map_portal_segment_update_uvs(portal,seg);
				break;

			case sg_ambient_wall:
				map_portal_segment_update_shift_single(f_tick,seg);
				map_prepare_create_ambient_wall_segment_uv(seg);
				map_portal_segment_update_uvs(portal,seg);
				break;
				
			case sg_ambient_fc:
				map_portal_segment_update_shift_single(f_tick,seg);
				map_prepare_create_ambient_fc_segment_uv(seg);
				map_portal_segment_update_uvs(portal,seg);
				break;
		}
	}
}

