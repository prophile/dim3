/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Segment Lists

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


// supergumba -- can probably delete most of this

/* =======================================================

      Create/Destroy Single Portal Segment List
      
======================================================= */

bool map_portal_create_single_segment_list_memory(portal_segment_list_type *list)
{
	int				sz;

	sz=list->max_count*sizeof(short);

	list->list=(short*)valloc(sz);
	if (list->list==NULL) return(FALSE);

	bzero(list->list,sz);

	return(TRUE);
}

bool map_portal_create_single_segment_list(map_type *map,int rn)
{
	int					n;
	portal_type			*portal;
	segment_type		*seg;
	
	portal=&map->portals[rn];
	
		// find the count for various lists

	portal->wall_list_hit.max_count=0;
	portal->fc_list_hit.max_count=0;
	portal->liquid_list_hit.max_count=0;
	portal->segment_list_all.max_count=0;
	portal->segment_list_draw.max_count=0;
	portal->segment_list_texture_shift.max_count=0;

	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {

		if (seg->rn!=rn) {
			seg++;
			continue;
		}
		
		switch (seg->type) {
			case sg_wall:
				portal->wall_list_hit.max_count++;
				break;
			case sg_floor:
			case sg_ceiling:
				portal->fc_list_hit.max_count++;
				break;
			case sg_liquid:
				portal->liquid_list_hit.max_count++;
				break;
		}

		portal->segment_list_all.max_count++;
		portal->segment_list_draw.max_count++;
		portal->segment_list_texture_shift.max_count++;

		seg++;
	}

		// memory for lists
		
	if (!map_portal_create_single_segment_list_memory(&portal->wall_list_hit)) return(FALSE);
	if (!map_portal_create_single_segment_list_memory(&portal->fc_list_hit)) return(FALSE);
	if (!map_portal_create_single_segment_list_memory(&portal->liquid_list_hit)) return(FALSE);
	if (!map_portal_create_single_segment_list_memory(&portal->segment_list_all)) return(FALSE);
	if (!map_portal_create_single_segment_list_memory(&portal->segment_list_draw)) return(FALSE);
	if (!map_portal_create_single_segment_list_memory(&portal->segment_list_texture_shift)) return(FALSE);
	
	return(TRUE);
}

void map_portal_dispose_single_segment_list(map_type *map,int rn)
{
	portal_type				*portal;

	portal=&map->portals[rn];
	
	free(portal->wall_list_hit.list);
	free(portal->fc_list_hit.list);
	free(portal->liquid_list_hit.list);
	free(portal->segment_list_draw.list);
	free(portal->segment_list_texture_shift.list);
}

/* =======================================================

      Create/Destroy Single Portal Segment Draw
      
======================================================= */

bool map_portal_create_single_segment_draw(map_type *map,int rn)
{
	int							n,sz,cnt;
	portal_type					*portal;
	segment_type				*seg;
	portal_segment_draw_type	*draw;
	
	portal=&map->portals[rn];
	draw=&portal->segment_draw;

		// count max segments in portal

	cnt=0;
	seg=map->segments;

	for (n=0;n!=map->nsegment;n++) {
		if (seg->rn==rn) cnt++;
		seg++;
	}

	if (cnt==0) cnt=1;

		// memory for segment draw
		
	sz=cnt*sizeof(short);
		
	draw->opaque_normal_list=(short*)valloc(sz);
	if (draw->opaque_normal_list==NULL) return(FALSE);
	
	draw->opaque_bump_list=(short*)valloc(sz);
	if (draw->opaque_bump_list==NULL) return(FALSE);

	draw->opaque_light_list=(short*)valloc(sz);
	if (draw->opaque_light_list==NULL) return(FALSE);

	draw->opaque_simple_normal_list=(short*)valloc(sz);
	if (draw->opaque_simple_normal_list==NULL) return(FALSE);
	
	draw->opaque_specular_list=(short*)valloc(sz);
	if (draw->opaque_specular_list==NULL) return(FALSE);
	
	draw->opaque_glow_list=(short*)valloc(sz);
	if (draw->opaque_glow_list==NULL) return(FALSE);

	draw->transparent_list=(short*)valloc(sz);
	if (draw->transparent_list==NULL) return(FALSE);

	draw->transparent_sort_list=(short*)valloc(sz);
	if (draw->transparent_sort_list==NULL) return(FALSE);

	draw->shader_list=(short*)valloc(sz);
	if (draw->shader_list==NULL) return(FALSE);

	bzero(draw->opaque_normal_list,sz);
	bzero(draw->opaque_bump_list,sz);
	bzero(draw->opaque_light_list,sz);
	bzero(draw->opaque_simple_normal_list,sz);
	bzero(draw->opaque_specular_list,sz);
	bzero(draw->opaque_glow_list,sz);
	bzero(draw->transparent_list,sz);
	bzero(draw->transparent_sort_list,sz);
	bzero(draw->shader_list,sz);

		// special list for z sorting

	sz=cnt*sizeof(float);

	draw->transparent_sort_z_list=(float*)valloc(sz);
	if (draw->transparent_sort_z_list==NULL) return(FALSE);

	bzero(draw->transparent_sort_z_list,sz);
	
	return(TRUE);
}

void map_portal_dispose_single_segment_draw(map_type *map,int rn)
{
	portal_segment_draw_type	*draw;
	
	draw=&map->portals[rn].segment_draw;
	
	free(draw->opaque_normal_list);
	free(draw->opaque_bump_list);
	free(draw->opaque_light_list);
	free(draw->opaque_simple_normal_list);
	free(draw->opaque_specular_list);
	free(draw->opaque_glow_list);
	free(draw->transparent_list);
	free(draw->transparent_sort_list);
	free(draw->shader_list);
	free(draw->transparent_sort_z_list);
}

/* =======================================================

      Build Single Portals Segment List/Draw
      
======================================================= */

void map_portal_build_single_segment_list(map_type *map,int rn)
{
	int							n,t,idx,wall_cnt,fc_cnt,liquid_cnt,
								all_cnt,draw_cnt,shift_cnt;
	short						*wall_ptr,*fc_ptr,*liquid_ptr,
								*all_ptr,*draw_ptr,*shift_ptr;
	portal_type					*portal;
	segment_type				*seg;
		
	portal=&map->portals[rn];
	
		// create the segment lists
		
	wall_cnt=0;
	fc_cnt=0;
	liquid_cnt=0;
	all_cnt=0;
	shift_cnt=0;
	
	wall_ptr=portal->wall_list_hit.list;
	fc_ptr=portal->fc_list_hit.list;
	liquid_ptr=portal->liquid_list_hit.list;
	all_ptr=portal->segment_list_all.list;
	shift_ptr=portal->segment_list_texture_shift.list;
	
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
	
		if ((seg->rn!=rn) || (!seg->on)) {
			seg++;
			continue;
		}
		
			// part of wall, floor, ceiling hit list?
			
		if (!seg->pass_through) {

			switch (seg->type) {
				case sg_wall:
					*wall_ptr++=n;
					wall_cnt++;
					break;
				case sg_floor:
				case sg_ceiling:
					*fc_ptr++=n;
					fc_cnt++;
					break;
				case sg_liquid:
					*liquid_ptr++=n;
					liquid_cnt++;
					break;
			}

		}

			// all segments

		*all_ptr++=n;
		all_cnt++;
		
			// part of shift list?
			
		if (seg->shiftable) {
			*shift_ptr++=n;
			shift_cnt++;
		}

		seg++;
	}
	
	portal->wall_list_hit.count=wall_cnt;
	portal->fc_list_hit.count=fc_cnt;
	portal->liquid_list_hit.count=liquid_cnt;
	portal->segment_list_all.count=all_cnt;
	portal->segment_list_texture_shift.count=shift_cnt;
	
		// sort the draw list by texture
		
	draw_cnt=0;
	draw_ptr=portal->segment_list_draw.list;
	
	for (t=0;t!=max_map_texture;t++) {
		if (map->textures[t].bitmaps[0].data==NULL) continue;
		
		all_ptr=portal->segment_list_all.list;
		
		for (n=0;n!=all_cnt;n++) {
			idx=*all_ptr++;
			seg=&map->segments[idx];
		
			if ((seg->rn==rn) && (seg->on) && (seg->fill==t)) {
				*draw_ptr++=idx;
				draw_cnt++;
			}
			
		}
	}
	
	portal->segment_list_draw.count=draw_cnt;
}

/* =======================================================

      Create/Destroy Portal Segment List
      
======================================================= */

bool map_portal_create_segment_lists(map_type *map)
{
	int			i;
	
	for (i=0;i!=map->nportal;i++) {
		if (!map_portal_create_single_segment_list(map,i)) return(FALSE);
		map_portal_build_single_segment_list(map,i);

		if (!map_portal_create_single_segment_draw(map,i)) return(FALSE);
	}
	
	return(TRUE);
}

void map_portal_dispose_segment_lists(map_type *map)
{
	int			i;
	
	for (i=0;i!=map->nportal;i++) {
		map_portal_dispose_single_segment_list(map,i);
		map_portal_dispose_single_segment_draw(map,i);
	}
}

