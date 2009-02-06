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

typedef struct			{
							int					win_link_idx,win_dist,
												link_dist[max_node_link];
						} node_scan_type;

node_scan_type			node_scan[max_node];

/* =======================================================

      Node Links Lookup
      
======================================================= */

int node_link_find_node_by_point(editor_3D_view_setup *view_setup,d3pnt *click_pt)
{
	int				n,px[8],py[8],pz[8],fz,hit_z,node_idx;
	node_type		*node;
	
	node_idx=-1;
	hit_z=100000;

	for (n=0;n!=map.nnode;n++) {
	
		node=&map.nodes[n];
		walk_view_sprite_select_size(&node->pnt,px,py,pz);
		
		if (walk_view_cube_click_index(view_setup,click_pt,px,py,pz,&fz)) {
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
	int			n,node_idx,next_idx,k1,k2,sz;
	d3pnt		click_pt;
	
		// is a node selected?
		
	node_idx=node_link_is_node_link_selected();
	if (node_idx==-1) return(FALSE);
	
		// are we clicking on a new node?
		
	click_pt.x=pt->x-view_setup->box.lx;
	click_pt.y=pt->y-view_setup->box.ty;
		
	next_idx=node_link_find_node_by_point(view_setup,&click_pt);
    if ((next_idx==node_idx) || (next_idx==-1)) return(FALSE);
	
		// delete link with option key
		
	if (main_wind_option_down()) {
			
		for (n=0;n!=max_node_link;n++) {
		
			if (map.nodes[node_idx].link[n]==next_idx) {
				sz=(max_node_link-n)-1;
				if (sz>0) memmove(&map.nodes[node_idx].link[n],&map.nodes[node_idx].link[n+1],(sz*sizeof(short)));
				map.nodes[node_idx].link[max_node_link-1]=-1;
			}
			
			if (map.nodes[next_idx].link[n]==node_idx) {
				sz=(max_node_link-n)-1;
				if (sz>0) memmove(&map.nodes[next_idx].link[n],&map.nodes[next_idx].link[n+1],(sz*sizeof(short)));
				map.nodes[next_idx].link[max_node_link-1]=-1;
			}
		}
		
		main_wind_tool_fill_node_combo();
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
		
		main_wind_tool_fill_node_combo();
		main_wind_draw();
		
		return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Rebuild Node Paths
      
======================================================= */

void node_path_trace_all(int org_node_idx,int org_link_idx,int node_idx,int cur_dist,unsigned char *node_hit,int level)
{
	int				n,nxt_node_idx,dist;
	node_type		*node;
	
		// if at the top, node = original node
		// and clear the hit list and win distance
		// lists.
		
	if (level==40) return;
	
	if (level==0) {
		node_idx=org_node_idx;
		cur_dist=0;
		
		memset(node_hit,0x0,map.nnode);
		
		for (n=0;n!=map.nnode;n++) {
			node_scan[n].win_link_idx=-1;
		}
	}
	
	node_hit[node_idx]=0x1;
		
		// trace all paths from this node
		
	node=&map.nodes[node_idx];
		
	for (n=0;n!=max_node_link;n++) {
	
			// if we are at the top, set the original
			// link index for distance writes
			
		if (level==0) org_link_idx=n;
		
			// get next node
			
		nxt_node_idx=node->link[n];
		if (nxt_node_idx==-1) break;
		
			// already hit?
			
		if (node_hit[nxt_node_idx]!=0x0) continue;
		
			// see if this node's distance is
			// already greater than the current winning distance
			// or is the winning distance
			
		dist=cur_dist+node_scan[node_idx].link_dist[n];
		
		if ((node_scan[nxt_node_idx].win_link_idx!=-1) && (dist>node_scan[nxt_node_idx].win_dist)) continue;
		
		if ((node_scan[nxt_node_idx].win_link_idx==-1) || (dist<node_scan[nxt_node_idx].win_dist)) {
			node_scan[nxt_node_idx].win_link_idx=org_link_idx;
			node_scan[nxt_node_idx].win_dist=dist;
		}
		
			// continue down the path
		
		node_path_trace_all(org_node_idx,org_link_idx,nxt_node_idx,dist,node_hit,(level+1));
	}
	
		// backing up, free this node for additional searches
		
	node_hit[node_idx]=0x0;
	
		// if we are at the first level, then
		// we can set all the path wins as hints
		
	if (level!=0) return;
	
	for (n=0;n!=map.nnode;n++) {
		if (n!=node_idx) {
			node->path_hint[n]=node->link[node_scan[n].win_link_idx];
		}
	}
}

void node_path_rebuild(void)
{
    int				i,x,z,k,n;
	unsigned char	node_hit[max_node];
	node_type		*node,*to_node;
	
        // precalc the distance between each
		// directly linked node
        
	node=map.nodes;
		
    for (n=0;n!=map.nnode;n++) {
    
        x=node->pnt.x;
        z=node->pnt.z;
		
			// get distance to each directly linked node
			
        for (i=0;i!=max_node_link;i++) {
            k=node->link[i];
            if ((k==-1) || (k==n)) {
                node_scan[n].link_dist[i]=0;
            }
            else {
				to_node=&map.nodes[k];
            	node_scan[n].link_dist[i]=distance_2D_get(x,z,to_node->pnt.x,to_node->pnt.z);
            }
        }
        
            // clear out path hints

        for (k=0;k!=map.nnode;k++) {
            node->path_hint[k]=-1;
        }
        
        node++;
    }
    
        // find the best path between all nodes
	
    for (i=0;i!=map.nnode;i++) {
		node_path_trace_all(i,0,0,0,node_hit,0);
    }
}

