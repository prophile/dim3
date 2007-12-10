/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Portal Routines

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

extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);
extern bool portal_outside_fog(d3pos *pos,portal_type *portal);

extern map_type					map;

/* =======================================================

      Portal Sight Path
      
======================================================= */

void view_portal_trace_sight_path(d3pos *pos,portal_sight_list_type *sight,int k)
{
	int					i,t,dist;
    portal_type			*portal;
	
        // is portal already in path?
        
	portal=&map.portals[sight[k].rn];
    if (portal->in_path) return;

        // portal bounding box in view?
		
	portal->in_path=boundbox_inview(portal->x,portal->z,portal->ex,portal->ez,portal->ty,portal->by);
	
		// outside of solid fog view?
		
	if (portal->in_path) portal->in_path=!portal_outside_fog(pos,portal);
	
        // trace path from portal

	for (i=0;i!=max_sight_link;i++) {
		t=(int)sight[k].link[i];
		if (t!=nopath) {
			view_portal_trace_sight_path(pos,sight,t);
		}
	}
}

void view_portal_create_sight_path(d3pos *pos)
{
	int							i,k;
    portal_type					*portal;
	portal_sight_list_type		*sight;
	
        // clear out site path

    portal=map.portals;
    for (i=0;i!=map.nportal;i++) {
        portal->in_path=FALSE;
        portal++;
    }
    
		// start with current portal
	
    portal=&map.portals[pos->rn];
    portal->in_path=TRUE;

		// add in portals sighted with linked portal list

	sight=portal->sight;

	for (k=0;k!=max_sight_list;k++) {
        if (sight->root) {
			view_portal_trace_sight_path(pos,portal->sight,k);
		}
		sight++;
	}
}

