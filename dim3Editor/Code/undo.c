/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Undo

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

#include "common_view.h"
#include "interface.h"

#define undo_type_segment_move		0

extern map_type						map;

int									undo_type;
//segment_type						*undo_segments=NULL;

/* =======================================================

      Map Undo
      
======================================================= */

void undo_clear(void)
{
/*
	DisableMenuItem(GetMenuRef(app_menu_edit),1);
	
	if (undo_segments!=NULL) {
		free(undo_segments);
		undo_segments=NULL;
	}
	*/
}

void undo_set_segment_move(void)
{
/* supergumba
	int				n,k,primitive_uid,
					sel_count,type,index,cnt;
	segment_type	*seg;
	
		// count segments
		
	cnt=0;
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);

		switch (type) {
			case segment_piece:
				cnt++;
				break;
			case primitive_piece:
				cnt+=primitive_count_segments(map.segments[index].primitive_uid[0]);
				break;
		}
	}
	
	if (cnt==0) return;
	
		// create segment undos
		
	if (undo_segments!=NULL) free(undo_segments);
	
	undo_segments=(segment_type*)valloc(cnt*sizeof(segment_type));
	if (undo_segments==NULL) return;
	
		// move in the segments
		
	seg=undo_segments;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);

		switch (type) {
			case segment_piece:
				memmove(seg,&map.segments[index],sizeof(segment_type));
				seg++;
				break;
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];
				for (k=0;k!=map.nsegment;k++) {
					if (map.segments[k].primitive_uid[0]==primitive_uid) {
						memmove(seg,&map.segments[k],sizeof(segment_type));
						seg++;
					}
				}
				break;
		}
	}

	undo_type=undo_type_segment_move;

	EnableMenuItem(GetMenuRef(app_menu_edit),1);
	*/
}

void undo_get_segment_move(void)
{
/* supergumba
	int				n,k,primitive_uid,
					sel_count,type,index;
	segment_type	*seg;
	
		// move in the segments
		
	seg=undo_segments;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);

		switch (type) {
			case segment_piece:
				memmove(&map.segments[index],seg,sizeof(segment_type));
				seg++;
				break;
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];
				for (k=0;k!=map.nsegment;k++) {
					if (map.segments[k].primitive_uid[0]==primitive_uid) {
						memmove(&map.segments[k],seg,sizeof(segment_type));
						seg++;
					}
				}
				break;
		}
	}
	*/
}

void undo_run(void)
{
/*
	switch (undo_type) {
	
		case undo_type_segment_move:
			undo_get_segment_move();
			main_wind_draw();
			break;
			
	}
	
	undo_clear();
	*/
}

