/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Node Routines

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

      Find Nodes
      
======================================================= */

int map_find_node(map_type *map,char *name)
{
	int				n,nnode;
	node_type		*node;
	
	nnode=map->nnode;
	node=map->nodes;
	
	for (n=0;n!=nnode;n++) {
		if (strcmp(node->name,name)==0) return(n);
		node++;
	}
	
	return(-1);
}

int map_find_nearest_node(map_type *map,int x,int y,int z,int user_value,float ang,float ang_sweep,int min_dist,int max_dist)
{
	int					n,i,nnode,d,dist;
	float				fang;
	node_type			*node;

	i=-1;
    dist=max_dist;
	
	nnode=map->nnode;
    
	for (n=0;n!=nnode;n++) {
		node=&map->nodes[n];
		
			// check user value
			
		if (user_value!=-1) {
			if (node->user_value!=user_value) continue;
		}
	
			// check distance
			
		d=distance_get(x,y,z,node->pnt.x,node->pnt.y,node->pnt.z);
		if (d<min_dist) continue;
		if (d>dist) continue;
		
			// check angle
			
		if (ang!=-1) {
			fang=angle_find(x,z,node->pnt.x,node->pnt.z);
			if (angle_dif(fang,ang,NULL)>ang_sweep) continue;
		}
			
		i=n;
		dist=d;
	}
	
	return(i);
}

int map_find_next_node_in_path(map_type *map,int from_idx,int to_idx)
{
	if ((from_idx==-1) || (to_idx==-1)) return(-1);
	
	return(map->nodes[from_idx].path_hint[to_idx]);
}

/* =======================================================

      Calculate Node Distance
      
======================================================= */

int map_node_to_node_distance(map_type *map,int from_idx,int to_idx)
{
	int			x,y,z,dist,idx;
	node_type	*node;

	if ((from_idx==-1) || (to_idx==-1)) return(-1);
	if (from_idx==to_idx) return(0);

		// starting position

	node=&map->nodes[from_idx];

	x=node->pnt.x;
	y=node->pnt.y;
	z=node->pnt.z;

	idx=from_idx;
	dist=0;

		// run through all the distances

	while (TRUE) {
		idx=map_find_next_node_in_path(map,idx,to_idx);
		if (idx==-1) return(0);
		if (idx==from_idx) return(0);			// circular, get out

		node=&map->nodes[idx];
		dist+=distance_get(x,y,z,node->pnt.x,node->pnt.y,node->pnt.z);

		x=node->pnt.x;
		y=node->pnt.y;
		z=node->pnt.z;

		if (idx==to_idx) break;
	}

	return(dist);
}

