/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Liquid Updates

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

/* =======================================================

      Move Liquid To Different Portal
      
======================================================= */

int map_portal_liquid_switch_portal(map_type *map,int portal_idx,int liquid_idx,int new_portal_idx)
{
	int						new_liquid_idx;
	map_liquid_type			*org_liq,*new_liq;
	
		// create new liquid
		
	new_liquid_idx=map_portal_liquid_add(map,new_portal_idx);
	if (new_liquid_idx==-1) return(-1);
	
		// switch liquids
		
	org_liq=&map->portals[portal_idx].liquid.liquids[liquid_idx];
	new_liq=&map->portals[new_portal_idx].liquid.liquids[new_liquid_idx];
	
	memmove(new_liq,org_liq,sizeof(map_liquid_type));
	
		// delete original liquid
		
	map_portal_liquid_delete(map,portal_idx,liquid_idx);
	
	return(new_liquid_idx);
}

/* =======================================================

      Move Liquid
      
======================================================= */

void map_portal_liquid_move(map_type *map,int portal_idx,int liquid_idx,int x,int y,int z)
{
	map_liquid_type			*liq;
	
	liq=&map->portals[portal_idx].liquid.liquids[liquid_idx];
	
	liq->lft+=x;
	liq->rgt+=x;
	
	liq->top+=z;
	liq->bot+=z;
	
	liq->y+=y;
}
