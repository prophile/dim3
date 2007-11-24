/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Sight Path

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

      Clear Sight Path
      
======================================================= */

void map_portal_sight_clear(map_type *map,int rn)
{
	int						n,i;
	portal_sight_list_type	*sight;
	
	sight=map->portals[rn].sight;
	
	for (n=0;n!=max_sight_list;n++) {
		sight[n].rn=nopath;
		for (i=0;i!=max_sight_link;i++) {
			sight[n].link[i]=nopath;
		}
        sight[n].root=FALSE;
	}
}

/* =======================================================

      Adjust Paths for Deletions
      
======================================================= */

void map_portal_sight_prune(map_type *map,int rn,int site_idx)
{
	int				n,i;
	portal_type		*portal;
	
	portal=&map->portals[rn];
	
		// end of path?
	
	if (site_idx==nopath) return;
	if (portal->sight[site_idx].rn==nopath) return;
	
		// clip links
	
	for (n=0;n!=max_sight_list;n++) {
		if (portal->sight[n].rn==nopath) continue;

		for (i=0;i!=max_sight_link;i++) {
			if (portal->sight[n].link[i]==site_idx) portal->sight[n].link[i]=nopath;
		}
	}

	portal->sight[site_idx].rn=nopath;
	portal->sight[site_idx].root=FALSE;
	
		// remove branches
	
	for (i=0;i!=max_sight_link;i++) {
		if (portal->sight[site_idx].link[i]!=nopath) map_portal_sight_prune(map,rn,portal->sight[site_idx].link[i]);
		portal->sight[site_idx].link[i]=nopath;
	}
}

void map_portal_sight_delete_path_to_portal(map_type *map,int rn,int del_rn)
{
	int					n,r;
	portal_type			*portal;
	
	portal=&map->portals[rn];
	
	for (n=0;n!=max_sight_list;n++) {
		r=portal->sight[n].rn;
		if (r==nopath) continue;

		if (r==del_rn) {		// prune paths to room
			map_portal_sight_prune(map,rn,n);
			continue;
		}
		
		if (r>del_rn) {			// shift for deletion
			portal->sight[n].rn--;
		}
	}
}

void map_portal_sight_delete_adjust_path(map_type *map,int rn)
{
	int					n;
	
	for (n=0;n!=map->nportal;n++) {
		map_portal_sight_delete_path_to_portal(map,n,rn);
	}
}

/* =======================================================

      Auto Generate Sight Path
      
======================================================= */

void map_portal_sight_generate_path_get_touch(map_type *map,int portal_idx,bool *touched)
{
	int				n;
	portal_type		*portal,*chk_portal;
	
	portal=&map->portals[portal_idx];
	
	for (n=0;n!=map->nportal;n++) {
		touched[n]=FALSE;
		if (portal_idx==n) continue;
		
		chk_portal=&map->portals[n];
		
		if ((chk_portal->z<=portal->ez) && (chk_portal->ez>=portal->z)) {
			if ((chk_portal->x==portal->ex) || (chk_portal->ex==portal->x)) {
				touched[n]=TRUE;
			}
		}
		
		if ((chk_portal->x<=portal->ex) && (chk_portal->ex>=portal->x)) {
			if ((chk_portal->z==portal->ez) || (chk_portal->ez==portal->z)) {
				touched[n]=TRUE;
			}
		}
	}
}

int map_portal_sight_generate_path_find_empty_sight(map_type *map,int portal_idx)
{
	int				n;
	
	for (n=0;n!=max_sight_list;n++) {
		if (map->portals[portal_idx].sight[n].rn==nopath) return(n);
	}
	
	return(-1);
}

int map_portal_sight_generate_path_find_empty_sight_link(map_type *map,int portal_idx,int sight_idx)
{
	int				n;
	
	for (n=0;n!=max_sight_list;n++) {
		if (map->portals[portal_idx].sight[sight_idx].link[n]==nopath) return(n);
	}
	
	return(-1);
}

void map_portal_sight_generate_path_trace(map_type *map,int root_portal_idx,int portal_idx,int previous_sight_idx,bool *linked,int depth,bool ignore_depth)
{
	int				n,k,t;
	bool			touched[max_portal];
	portal_type		*root_portal;
	
		// get all touching portals
		
	map_portal_sight_generate_path_get_touch(map,portal_idx,touched);
	
		// connect all touching portals
		
	t=0;
	root_portal=&map->portals[root_portal_idx];
	
	for (n=0;n!=map->nportal;n++) {
		if (!touched[n]) continue;
		if (linked[n]) continue;
		
			// add new link
			
		k=map_portal_sight_generate_path_find_empty_sight(map,root_portal_idx);
		if (k==-1) break;
		
		map->portals[root_portal_idx].sight[k].rn=n;
		map->portals[root_portal_idx].sight[k].root=FALSE;
		
		linked[n]=TRUE;
		
			// link to previous portal
			
		t=map_portal_sight_generate_path_find_empty_sight_link(map,root_portal_idx,previous_sight_idx);
		if (t==-1) {
			map->portals[root_portal_idx].sight[k].rn=nopath;
			break;
		}
		
		map->portals[root_portal_idx].sight[previous_sight_idx].link[t]=k;
		
		if ((depth!=2) || (ignore_depth)) map_portal_sight_generate_path_trace(map,root_portal_idx,n,k,linked,(depth+1),ignore_depth);
	}
}

void map_portal_sight_generate_paths(map_type *map,bool ignore_depth)
{
	int				i,root_portal_idx,portal_idx,sight_idx,
					root_sight_idx[max_portal];
	bool			linked[max_portal],touched[max_portal];
	
	for (root_portal_idx=0;root_portal_idx!=map->nportal;root_portal_idx++) {
		
			// clear sight path
		
		map_portal_sight_clear(map,root_portal_idx);
		
			// clear hit record
			
		for (i=0;i!=map->nportal;i++) {
			linked[i]=FALSE;
			root_sight_idx[i]=-1;
		}
	
			// get all touching portals
			
		map_portal_sight_generate_path_get_touch(map,root_portal_idx,touched);
		
			// find all the roots
			
		for (portal_idx=0;portal_idx!=map->nportal;portal_idx++) {
			if (!touched[portal_idx]) continue;

			sight_idx=map_portal_sight_generate_path_find_empty_sight(map,root_portal_idx);
			if (sight_idx==-1) break;
			
			map->portals[root_portal_idx].sight[sight_idx].rn=portal_idx;
			map->portals[root_portal_idx].sight[sight_idx].root=TRUE;
			
			linked[portal_idx]=TRUE;
			root_sight_idx[portal_idx]=sight_idx;
		}
		
			// trace from the roots
			
		for (portal_idx=0;portal_idx!=map->nportal;portal_idx++) {
			if (root_sight_idx[portal_idx]!=-1) map_portal_sight_generate_path_trace(map,root_portal_idx,portal_idx,root_sight_idx[portal_idx],linked,0,ignore_depth);
		}
	}
}


