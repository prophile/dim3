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

void select_add(int type,int index)
{
	int					n,primitive_uid;
	select_item_type	*select_item;
	
	if (nselect_item==select_max_item) return;
	
		// special check for primitives already selected by another segment
		// if so, just switch selected segment with new one
		
	if (type==primitive_piece) {
	
		primitive_uid=map.segments[index].primitive_uid[0];
		select_item=select_items;
		
		for (n=0;n!=nselect_item;n++) {
			if (select_item->type==primitive_piece) {
				if (map.segments[select_item->index].primitive_uid[0]==primitive_uid) {
					select_item->index=index;
					return;
				}
			}
			select_item++;
		}
	}
		
		// add to selection list
	
	select_items[nselect_item].type=type;
	select_items[nselect_item].index=index;
	nselect_item++;
}

int select_count(void)
{
	return(nselect_item);
}

void select_get(int sel_idx,int *type,int *index)
{
	*type=select_items[sel_idx].type;
	*index=select_items[sel_idx].index;
}

int select_find(int type,int index)
{
	int					n;
	select_item_type	*select_item;
	
	select_item=select_items;
	
	for (n=0;n!=nselect_item;n++) {
		if (index==-1) {
			if (select_item->type==type) return(n);
		}
		else {
			if ((select_item->type==type) && (select_item->index==index)) return(n);
		}
		select_item++;
	}
	
	return(-1);
}

bool select_check(int type,int index)
{
	return(select_find(type,index)!=-1);
}

bool select_check_segment(int index)
{
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
	
	return(FALSE);
}	

int select_check_primitive_find_index(int primitive_uid)
{
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
	
	return(-1);
}	

bool select_has_type(int type)
{
	return(select_find(type,-1)!=-1);
}

void select_flip(int type,int index)
{
	int				n,sel_idx;
	
	sel_idx=select_find(type,index);
		
	if (sel_idx==-1) {
		select_add(type,index);
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
			if (select_items[n].index<select_items[n+1].index) {
				memmove(&temp_item,&select_items[n],sizeof(select_item_type));
				memmove(&select_items[n],&select_items[n+1],sizeof(select_item_type));
				memmove(&select_items[n+1],&temp_item,sizeof(select_item_type));
				switch_item=TRUE;
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

void select_duplicate_add(int type,int index)
{
	if (nselect_duplicate_item==select_max_item) return;
	
	select_duplicate_items[nselect_duplicate_item].type=type;
	select_duplicate_items[nselect_duplicate_item].index=index;
	nselect_duplicate_item++;
}

void select_duplicate_copy(void)
{
	memmove(select_items,select_duplicate_items,(sizeof(select_item_type)*nselect_duplicate_item));
	nselect_item=nselect_duplicate_item;
}

