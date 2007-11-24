/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Sight Path Routines

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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

extern map_type			map;

/* =======================================================

      Mark Portals in Sight Path
      
======================================================= */

void edit_view_sight_path_trace(portal_sight_list_type *sight,int idx)
{
	int			n,t,rn;
	
		// is portal already in path?

	rn=sight[idx].rn;
    if (map.portals[rn].in_path) return;

		// put in path and follow links

	map.portals[rn].in_path=TRUE;

	for (n=0;n!=max_sight_link;n++) {
		t=(int)sight[idx].link[n];
		if (t!=nopath) edit_view_sight_path_trace(sight,t);
	}
}

void edit_view_sight_path_mark(int rn)
{
	int							n;
	portal_sight_list_type		*sight;
	
        // clear all portals from path
        
    for (n=0;n!=map.nportal;n++) {
        map.portals[n].in_path=FALSE;
    }
					
		// start with current room
	
    map.portals[rn].in_path=TRUE;

		// add in portals sighted with linked room list

	sight=map.portals[rn].sight;
	
	for (n=0;n!=max_sight_list;n++) {
        if (sight[n].root) edit_view_sight_path_trace(sight,n);
	}
}


