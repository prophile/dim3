/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Selection Routines

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

#include "common_view.h"

extern map_type				map;

int							nselect_item,nselect_duplicate_item;
select_item_type			select_items[select_max_item],select_duplicate_items[select_max_item];

/* =======================================================

      Selection Utilities
      
======================================================= */

void select_clear(void)
{
	nselect_item=0;
}

void select_add(int type,int portal_idx,int main_idx,int sub_idx)
{
	if (nselect_item==select_max_item) return;
		
		// add to selection list
	
	select_items[nselect_item].type=type;
	select_items[nselect_item].portal_idx=portal_idx;
	select_items[nselect_item].main_idx=main_idx;
	select_items[nselect_item].sub_idx=sub_idx;
	nselect_item++;
}

int select_count(void)
{
	return(nselect_item);
}

void select_get(int sel_idx,int *type,int *portal_idx,int *main_idx,int *sub_idx)
{
	*type=select_items[sel_idx].type;
	*portal_idx=select_items[sel_idx].portal_idx;
	*main_idx=select_items[sel_idx].main_idx;
	*sub_idx=select_items[sel_idx].sub_idx;
}

int select_find(int type,int portal_idx,int main_idx,int sub_idx)
{
	int					n;
	select_item_type	*select_item;
	
	select_item=select_items;
	
	for (n=0;n!=nselect_item;n++) {
		if (main_idx==-1) {
			if (select_item->type==type) return(n);
		}
		else {
			if ((select_item->type==type) && (select_item->portal_idx==portal_idx) && (select_item->main_idx==main_idx) && (select_item->sub_idx==sub_idx)) return(n);
		}
		select_item++;
	}
	
	return(-1);
}

bool select_check(int type,int portal_idx,int main_idx,int sub_idx)
{
	return(select_find(type,portal_idx,main_idx,sub_idx)!=-1);
}

bool select_check_segment(int index)
{
/* supergumba
	int					n,primitive_uid;
	select_item_type	*select_item;
	
	select_item=select_items;
	
	for (n=0;n!=nselect_item;n++) {
	
		switch (select_item->type) {
		
			case segment_piece:
				if (select_item->index==index) return(TRUE);
				break;
			
			case primitive_piece:
				primitive_uid=map.segments[select_item->index].primitive_uid[0];
				if (map.segments[index].primitive_uid[0]==primitive_uid) return(TRUE);
				break;
		}
		
		select_item++;
	}
	*/
	return(FALSE);
}	

int select_check_primitive_find_index(int primitive_uid)
{
/* supergumba
	int					n;
	select_item_type	*select_item;
	
	select_item=select_items;
	
	for (n=0;n!=nselect_item;n++) {
	
		switch (select_item->type) {
		
			case segment_piece:
			case primitive_piece:
				if (map.segments[select_item->index].primitive_uid[0]==primitive_uid) return(select_item->index);
				break;
				
		}
		
		select_item++;
	}
*/	
	return(-1);
}	

bool select_has_type(int type)
{
	return(select_find(type,-1,-1,-1)!=-1);
}

void select_flip(int type,int portal_idx,int main_idx,int sub_idx)
{
	int				n,sel_idx;
	
	sel_idx=select_find(type,portal_idx,main_idx,sub_idx);
		
	if (sel_idx==-1) {
		select_add(type,portal_idx,main_idx,sub_idx);
		return;
	}
	
	for (n=sel_idx;n<nselect_item;n++) {
		memmove(&select_items[n],&select_items[n+1],sizeof(select_item_type));
	}
	
	nselect_item--;
}

/* =======================================================

      Sorting Selection
      
======================================================= */

void select_sort(void)
{
	int						n;
	bool					switch_item;
	select_item_type		*temp_item;
	
	switch_item=TRUE;
	
	while (switch_item) {
		
		switch_item=FALSE;
		
		for (n=0;n<(nselect_item-1);n++) {
			if (select_items[n].portal_idx==select_items[n+1].portal_idx) {
				if (select_items[n].main_idx<select_items[n+1].main_idx) {
					memmove(&temp_item,&select_items[n],sizeof(select_item_type));
					memmove(&select_items[n],&select_items[n+1],sizeof(select_item_type));
					memmove(&select_items[n+1],&temp_item,sizeof(select_item_type));
					switch_item=TRUE;
				}
			}
		}
	}
}

/* =======================================================

      Duplicate Selection
      
======================================================= */

void select_duplicate_clear(void)
{
	nselect_duplicate_item=0;
}

void select_duplicate_add(int type,int portal_idx,int main_idx,int sub_idx)
{
	if (nselect_duplicate_item==select_max_item) return;
	
	select_duplicate_items[nselect_duplicate_item].type=type;
	select_duplicate_items[nselect_duplicate_item].portal_idx=portal_idx;
	select_duplicate_items[nselect_duplicate_item].main_idx=main_idx;
	select_duplicate_items[nselect_duplicate_item].sub_idx=sub_idx;
	nselect_duplicate_item++;
}

void select_duplicate_copy(void)
{
	memmove(select_items,select_duplicate_items,(sizeof(select_item_type)*nselect_duplicate_item));
	nselect_item=nselect_duplicate_item;
}

/* =======================================================

      Get Selection Extent
      
======================================================= */

void select_get_extent(d3pnt *min,d3pnt *max)
{
	int				n,sel_count,
					type,portal_idx,main_idx,sub_idx;
	d3pnt			t_min,t_max;
	
	min->x=min->z=min->y=map_max_size;
	max->x=max->z=max->y=-map_max_size;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
		
		switch (type) {
		
			case mesh_piece:
				map_portal_mesh_calculate_extent(&map,portal_idx,main_idx,&t_min,&t_max);
				break;
				
			case liquid_piece:
				t_min.x=map.portals[portal_idx].liquid.liquids[main_idx].lft;
				t_max.x=map.portals[portal_idx].liquid.liquids[main_idx].rgt;
				t_min.z=map.portals[portal_idx].liquid.liquids[main_idx].top;
				t_max.z=map.portals[portal_idx].liquid.liquids[main_idx].bot;
				t_min.y=t_max.y=map.portals[portal_idx].liquid.liquids[main_idx].y;
				break;
				
			case node_piece:
				t_min.x=t_max.x=map.nodes[main_idx].pos.x;
				t_min.y=t_max.y=map.nodes[main_idx].pos.y;
				t_min.z=t_max.z=map.nodes[main_idx].pos.z;
				break;
				
			case spot_piece:
				t_min.x=t_max.x=map.spots[main_idx].pos.x;
				t_min.y=t_max.y=map.spots[main_idx].pos.y;
				t_min.z=t_max.z=map.spots[main_idx].pos.z;
				break;
				
			case scenery_piece:
				t_min.x=t_max.x=map.sceneries[main_idx].pos.x;
				t_min.y=t_max.y=map.sceneries[main_idx].pos.y;
				t_min.z=t_max.z=map.sceneries[main_idx].pos.z;
				break;
				
			case light_piece:
				t_min.x=t_max.x=map.lights[main_idx].pos.x;
				t_min.y=t_max.y=map.lights[main_idx].pos.y;
				t_min.z=t_max.z=map.lights[main_idx].pos.z;
				break;
				
			case sound_piece:
				t_min.x=t_max.x=map.sounds[main_idx].pos.x;
				t_min.y=t_max.y=map.sounds[main_idx].pos.y;
				t_min.z=t_max.z=map.sounds[main_idx].pos.z;
				break;
				
			case particle_piece:
				t_min.x=t_max.x=map.particles[main_idx].pos.x;
				t_min.y=t_max.y=map.particles[main_idx].pos.y;
				t_min.z=t_max.z=map.particles[main_idx].pos.z;
				break;
				
		}
			
		if (t_min.x<(min->x)) min->x=t_min.x;
		if (t_min.y<(min->y)) min->y=t_min.y;
		if (t_min.z<(min->z)) min->z=t_min.z;
		if (t_max.x>(max->x)) max->x=t_max.x;
		if (t_max.y>(max->y)) max->y=t_max.y;
		if (t_max.z>(max->z)) max->z=t_max.z;
	}
}
