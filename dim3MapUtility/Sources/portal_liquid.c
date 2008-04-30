/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Liquids

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

      Add Liquid to Portals
      
======================================================= */

int map_portal_liquid_add(map_type *map,int portal_idx)
{
	int					liquid_idx;
	portal_liquid_type	*portal_liquid;
	map_liquid_type		*nptr;
	
	portal_liquid=&map->portals[portal_idx].liquid;
	
	liquid_idx=portal_liquid->nliquid;

		// create new memory

	if (portal_liquid->nliquid==0) {
		portal_liquid->liquids=(map_liquid_type*)valloc(sizeof(map_liquid_type));
		if (portal_liquid->liquids==NULL) return(-1);
	}
	else {
		nptr=(map_liquid_type*)valloc((portal_liquid->nliquid+1)*sizeof(map_liquid_type));
		if (nptr==NULL) return(-1);

		memmove(nptr,portal_liquid->liquids,(portal_liquid->nliquid*sizeof(map_liquid_type)));
		free(portal_liquid->liquids);

		portal_liquid->liquids=nptr;
	}
	
	portal_liquid->nliquid++;

	return(liquid_idx);
}

bool map_portal_liquid_delete(map_type *map,int portal_idx,int liquid_idx)
{
	int					sz;
	portal_liquid_type	*portal_liquid;
	map_liquid_type		*nptr;
	
	portal_liquid=&map->portals[portal_idx].liquid;

	if (portal_liquid->nliquid<=1) {
		portal_liquid->nliquid=0;
		free(portal_liquid->liquids);
		return(TRUE);
	}

	nptr=(map_liquid_type*)valloc((portal_liquid->nliquid-1)*sizeof(map_liquid_type));
	if (nptr==NULL) return(FALSE);

	if (liquid_idx>0) {
		sz=(liquid_idx+1)*sizeof(map_liquid_type);
		memmove(nptr,portal_liquid->liquids,sz);
	}

	sz=(portal_liquid->nliquid-liquid_idx)*sizeof(map_liquid_type);
	if (sz>0) memmove(&nptr[liquid_idx],&portal_liquid->liquids[liquid_idx+1],sz);

	free(portal_liquid->liquids);

	portal_liquid->liquids=nptr;
	portal_liquid->nliquid--;

	return(TRUE);
}

/* =======================================================

      Duplicate Liquid
      
======================================================= */

int map_portal_liquid_duplicate(map_type *map,int portal_idx,int liquid_idx)
{
	int					new_liquid_idx;
	map_liquid_type		*liq,*liq2;
	
	new_liquid_idx=map_portal_liquid_add(map,portal_idx);
	if (new_liquid_idx==-1) return(-1);
	
	liq=&map->portals[portal_idx].liquid.liquids[liquid_idx];
	liq2=&map->portals[portal_idx].liquid.liquids[new_liquid_idx];
	memmove(liq2,liq,sizeof(map_liquid_type));

	return(new_liquid_idx);
}

/* =======================================================

      Liquid Info
      
======================================================= */

void map_portal_liquid_calculate_center(map_type *map,int portal_idx,int liquid_idx,d3pnt *pt)
{
	map_liquid_type			*liq;
	
	liq=&map->portals[portal_idx].liquid.liquids[liquid_idx];

	pt->x=(liq->lft+liq->rgt)>>1;
	pt->y=liq->y;
	pt->z=(liq->top+liq->bot)>>1;
}

