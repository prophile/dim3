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

int node_link_find_node_by_point(Point pt)
{
/* supergumba -- need to work on connecting nodes
    int			i;
    Rect		box;

	for (i=0;i!=map.nnode;i++) {
		top_view_make_rect_by_pos(&map.nodes[i].pos,5,&box);
		if (PtInRect(pt,&box)) return(i);
	}
	*/
    return(-1);
}

int node_link_is_node_link_selected(void)
{
	int			type,portal_idx,main_idx,sub_idx;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&portal_idx,&main_idx,&sub_idx);
	if (type!=node_piece) return(-1);
	
	return(main_idx);
}

/* =======================================================

      Node Links Add/Delete
      
======================================================= */

int node_link_get_free_link(int k)
{
	int			i;
	
	for (i=0;i!=max_node_link;i++) {
		if (map.nodes[k].link[i]==-1) return(i);
	}
	
	return(-1);
}

bool node_link_has_link(int k,int lnk)
{
	int			i;
	
	for (i=0;i!=max_node_link;i++) {
		if (map.nodes[k].link[i]==lnk) return(TRUE);
	}
	
	return(FALSE);
}

void node_link_add(Point pt)
{
	int			cur_index,new_index,k1,k2;

    cur_index=node_link_is_node_link_selected();
	if (cur_index==-1) return;
    
    new_index=node_link_find_node_by_point(pt);
    if ((new_index==cur_index) || (new_index==-1)) return;

    k1=node_link_get_free_link(cur_index);
    k2=node_link_get_free_link(new_index);
    
    if ((k1==-1) || (k2==-1)) return;
    
	if (!node_link_has_link(cur_index,new_index)) map.nodes[cur_index].link[k1]=new_index;
	if (!node_link_has_link(new_index,cur_index)) map.nodes[new_index].link[k2]=cur_index;
    
	main_wind_draw();
}

void node_link_cut(Point pt)
{
    int			cur_index,new_index,k;
    
    cur_index=node_link_is_node_link_selected();
	if (cur_index==-1) return;
    
    new_index=node_link_find_node_by_point(pt);
    if ((new_index==cur_index) || (new_index==-1)) return;

    for (k=0;k!=max_node_link;k++) {
        if (map.nodes[cur_index].link[k]==new_index) map.nodes[cur_index].link[k]=-1;
        if (map.nodes[new_index].link[k]==cur_index) map.nodes[new_index].link[k]=-1;
    }
    
	main_wind_draw();
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
    
        x=node->pos.x;
        z=node->pos.z;
		
		to_node=map.nodes;
    
        for (k=0;k!=max_node_link;k++) {
            n=node->link[k];
            if (n==-1) {
                link_dist[i][k]=0;
            }
            else {
            	link_dist[i][k]=distance_2D_get(x,z,to_node->pos.x,to_node->pos.z);
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

