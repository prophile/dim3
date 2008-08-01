/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Node Link Routines

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

#include "interface.h"
#include "common_view.h"

extern map_type			map;

int						link_dist[max_node][max_node_link];

/* =======================================================

      Node Links Lookup
      
======================================================= */

int node_link_find_node_by_point(editor_3D_view_setup *view_setup,d3pnt *click_pt)
{
	int				n,px[4],pz[4],ty,by,fz,hit_z,node_idx;
	node_type		*node;
	
	node_idx=-1;
	hit_z=100000;

	for (n=0;n!=map.nnode;n++) {
	
		node=&map.nodes[n];
		walk_view_sprite_select_size(&view_setup->cpt,&node->pnt,px,pz,&ty,&by);
		
		if (walk_view_cube_click_index(view_setup,click_pt,px,pz,ty,by,&fz)) {
			if (fz<hit_z) {
				hit_z=fz;
				node_idx=n;
			}
		}
	}
	
	return(node_idx);
}

int node_link_is_node_link_selected(void)
{
	int			type,main_idx,sub_idx;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&main_idx,&sub_idx);
	if (type!=node_piece) return(-1);
	
	return(main_idx);
}

/* =======================================================

      Node Links Add/Delete
      
======================================================= */

int node_link_get_free_link(int node_idx)
{
	int			n;
	
	for (n=0;n!=max_node_link;n++) {
		if (map.nodes[node_idx].link[n]==-1) return(n);
	}
	
	return(-1);
}

bool node_link_has_link(int node_idx,int link_node_idx)
{
	int			n;
	
	for (n=0;n!=max_node_link;n++) {
		if (map.nodes[node_idx].link[n]==link_node_idx) return(TRUE);
	}
	
	return(FALSE);
}

bool node_link_click(editor_3D_view_setup *view_setup,d3pnt *pt)
{
	int			n,node_idx,next_idx,k1,k2;
	d3pnt		click_pt;
	
		// is a node selected?
		
	node_idx=node_link_is_node_link_selected();
	if (node_idx==-1) return(FALSE);
	
		// are we clicking on a new node?
		
	click_pt.x=pt->x-view_setup->box.left;
	click_pt.y=pt->y-view_setup->box.top;
		
	next_idx=node_link_find_node_by_point(view_setup,&click_pt);
    if ((next_idx==node_idx) || (next_idx==-1)) return(FALSE);
	
		// delete link with option key
		
	if (main_wind_option_down()) {
			
		for (n=0;n!=max_node_link;n++) {
			if (map.nodes[node_idx].link[n]==next_idx) map.nodes[node_idx].link[n]=-1;
			if (map.nodes[next_idx].link[n]==node_idx) map.nodes[next_idx].link[n]=-1;
		}
		
		main_wind_draw();
		
		return(TRUE);
	}
	
		// add link with control
		
	if (main_wind_control_down()) {
		
		k1=node_link_get_free_link(node_idx);
		k2=node_link_get_free_link(next_idx);
		
		if ((k1==-1) || (k2==-1)) {
			StandardAlert(kAlertCautionAlert,"\pCan not connect nodes","\pYou've reached the maximum number of connected nodes for this node.",NULL,NULL);
			return(TRUE);
		}
		
		if (!node_link_has_link(node_idx,next_idx)) map.nodes[node_idx].link[k1]=next_idx;
		if (!node_link_has_link(next_idx,node_idx)) map.nodes[next_idx].link[k2]=node_idx;
		
		main_wind_draw();
		
		return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Trace Down Node Paths
      
======================================================= */

int node_path_node_in_path(int from_node,int to_node,unsigned char *node_hits,int curdist,int windist)
{
	int				i,k,d,dist,nxtdist;

	dist=-1;
    nxtdist=windist;
    
    node_hits[from_node]=1;
	
	for (i=0;i!=max_node_link;i++) {
	
            // node doubling back?
            
		k=map.nodes[from_node].link[i];
		if (k==-1) continue;
        if (node_hits[k]!=0) continue;
        
         d=link_dist[from_node][i]+curdist;
        
            // is this distance already greater than a known winning distance?

        if ((windist!=-1) && (d>windist)) {
            dist=-1;
            break;
        }

            // are we at destination?
            
		if (k==to_node) {
            dist=d;
            break;
        }
        
        d=node_path_node_in_path(k,to_node,node_hits,d,nxtdist);
        if (d==-1) continue;
        
		if ((dist!=-1) && (d>dist)) continue;
                
        dist=d;
        nxtdist=dist;
	}
    
	node_hits[from_node]=0;

	return(dist);		
}

int node_path_find_next_node_to_node(int from_node,int to_node)
{
	int				i,k,link,n,d,dist;
	unsigned char	node_hits[max_node];
   
	link=-1;
	dist=-1;
    
        // block off nodes as we trace down paths
    
    for (n=0;n!=map.nnode;n++) {
        node_hits[n]=0;
    }
    node_hits[from_node]=1;
		
        // trace through links
        
	for (i=0;i!=max_node_link;i++) {
	
		k=map.nodes[from_node].link[i];
		if (k==-1) continue;
		
		if (k==to_node) return(k);
        
		d=node_path_node_in_path(k,to_node,node_hits,0,dist);
        if (d==-1) continue;
        
		if ((link!=-1) && (d>dist)) continue;
        
        link=k;
        dist=d;
	}
    
    return(link);
}

/* =======================================================

      Rebuild Node Paths
      
======================================================= */

void node_path_rebuild(void)
{
    int				i,x,z,k,n;
	node_type		*node,*to_node;
    
        // find distances between all node links
        
	node=map.nodes;
		
    for (i=0;i!=map.nnode;i++) {
    
        x=node->pnt.x;
        z=node->pnt.z;
		
		to_node=map.nodes;
    
        for (k=0;k!=max_node_link;k++) {
            n=node->link[k];
            if (n==-1) {
                link_dist[i][k]=0;
            }
            else {
            	link_dist[i][k]=distance_2D_get(x,z,to_node->pnt.x,to_node->pnt.z);
            }
			
			to_node++;
        }
        
            // clear out quick paths

        for (k=0;k!=map.nnode;k++) {
            node->path_hint[k]=-1;
        }
        
        node++;
    }
    
        // find the best path between all nodes
		
	node=map.nodes;
        
    for (i=0;i!=map.nnode;i++) {
        
        for (k=0;k!=map.nnode;k++) {
            if (k!=i) node->path_hint[k]=node_path_find_next_node_to_node(i,k);
        }
		
		node++;
    }
}

