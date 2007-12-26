/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Auto-Generation (and Import) Utility Routines

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
	#include "dim3maputility.h"
#endif

/* =======================================================

      Import Map Clear
      
======================================================= */

void map_auto_generate_clear(map_type *map)
{
	map->nportal=0;
	map->nlight=0;
	map->nsound=0;
	map->nspot=0;
	map->nnode=0;
	map->nsegment=0;
	map->ngroup=0;
	map->nmovement=0;
}

/* =======================================================

      Fix Segments UVs
      
======================================================= */

void map_auto_generate_fix_segments_uv(map_type *map)
{
	register int		i;
	segment_type		*seg;
	
	seg=map->segments;
	
	for (i=0;i!=map->nsegment;i++) {
		map_segment_reset_texture_uvs(map,seg);
		seg++;
	}
}

/* =======================================================

      Create Simple Lights
      
======================================================= */

void map_auto_generate_add_simple_lights(map_type *map)
{
	int					n,x,z,y,intensity;
	double				dx,dz;
	portal_type			*portal;
	map_light_type		*lit;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {

		if (map->nlight>=max_map_light) break;
	
			// find light position
			
		map_portal_calculate_center(map,n,&x,&y,&z);
		
			// get intensity
			
		dx=(portal->ex-portal->x);
		dz=(portal->ez-portal->z);
		intensity=(int)sqrt((dx*dx)+(dz*dz));
		
		intensity/=2;								// radius, so use half
		if (intensity<100) intensity=100;
		
			// create the light
	
		lit=&map->lights[map->nlight];
		map->nlight++;
		
		lit->pos.rn=n;
		lit->pos.x=x;
		lit->pos.z=z;
		lit->pos.y=y;
		lit->type=lt_normal;
		lit->col.r=lit->col.g=lit->col.b=1.0f;
		lit->intensity=intensity;
		lit->on=TRUE;

		portal++;
	}
}

/* =======================================================

      Add Player Start
      
======================================================= */

void map_auto_generate_add_player_spot(map_type *map)
{
	register int			n;
	register segment_type	*seg;
	int						x,y,z,seg_idx;
	spot_type				*spot;
	
	if (map->nportal==0) return;
	
		// find first floor in first portal
		
	seg_idx=-1;
	
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if ((seg->rn==0) && (seg->type==sg_floor)) {
			seg_idx=n;
			break;
		}
		seg++;
	}
	
		// spot place

	if (seg_idx!=-1) {
		map_segment_calculate_center(map,seg_idx,&x,&y,&z);
	}
	else {
		map_portal_calculate_center(map,0,&x,&y,&z);
	}
	
	if (x<20) x=20;
	if (z<20) z=20;			// sanity check for getting caught in walls
	
	y--;
	
		// add spot
		
	spot=&map->spots[0];
	map->nspot=1;
	
	spot->pos.rn=0;
	spot->pos.x=x;
	spot->pos.z=z;
	spot->pos.y=y;
	spot->ang.y=135;
	strcpy(spot->name,"Start");
	strcpy(spot->type,"Player");
	strcpy(spot->script,"Player");
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
}

