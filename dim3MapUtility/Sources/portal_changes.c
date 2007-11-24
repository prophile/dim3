/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Changes That Force Rebuilds

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

      Catch Segment List Changes
      
======================================================= */

void map_portal_clear_segment_list_changes(map_type *map)
{
	int				i;
	portal_type		*portal;
	
	portal=map->portals;
	
	for (i=0;i!=map->nportal;i++) {
		portal->reset_segment_list=FALSE;
		portal++;
	}
}

void map_portal_set_segment_list_changes(map_type *map,int rn)
{
	map->portals[rn].reset_segment_list=TRUE;
}

/* =======================================================

      Run Segment List Rebuilds
      
======================================================= */

void map_portal_rebuild_segment_list_changes(map_type *map)
{
	int				i;
	portal_type		*portal;
	
	portal=map->portals;
	
	for (i=0;i!=map->nportal;i++) {
		if (portal->reset_segment_list) map_portal_build_single_segment_list(map,i);
		portal++;
	}
}



