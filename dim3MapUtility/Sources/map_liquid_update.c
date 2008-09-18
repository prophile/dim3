/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Liquid Updates

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern float map_get_texture_reduce_coord(float f);
extern float map_get_texture_round_coord(float f);
extern void map_get_texture_uv_get_scale(map_type *map,int txt_idx,float *txt_scale_x,float *txt_scale_y);

/* =======================================================

      Move Liquid
      
======================================================= */

void map_liquid_move(map_type *map,int liquid_idx,int x,int y,int z)
{
	map_liquid_type			*liq;
	
	liq=&map->liquid.liquids[liquid_idx];
	
	liq->lft+=x;
	liq->rgt+=x;
	
	liq->top+=z;
	liq->bot+=z;
	
	liq->y+=y;
}

/* =======================================================

      Reset Liquid UV
      
======================================================= */

void map_liquid_reset_uv(map_type *map,int liquid_idx)
{
	float					ltxtx,rtxtx,ltxtz,rtxtz,
							txt_scale_x,txt_scale_y;
	map_liquid_type			*liq;

	liq=&map->liquid.liquids[liquid_idx];
	
		// get scale

	map_get_texture_uv_get_scale(map,liq->txt_idx,&txt_scale_x,&txt_scale_y);
	
	ltxtx=((float)liq->lft)*txt_scale_x;
	rtxtx=((float)liq->rgt)*txt_scale_x;

	ltxtz=((float)liq->top)*txt_scale_y;
	rtxtz=((float)liq->bot)*txt_scale_y;

	liq->x_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx));
	liq->x_txtfact=map_get_texture_round_coord(rtxtx-ltxtx);
	
	liq->y_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtz));
	liq->y_txtfact=map_get_texture_round_coord(rtxtz-ltxtz);
	
		// deal with offset locks
		
	if (map->textures[liq->txt_idx].scale.lock_offset) {
		liq->x_txtoff=0.0f;
		liq->y_txtoff=0.0f;
	}
}

