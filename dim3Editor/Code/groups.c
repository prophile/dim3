/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Group Routines

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

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

extern map_type					map;

extern ControlRef				group_combo;

/* =======================================================

      Add\Delete Group
      
======================================================= */

void group_add(void)
{
	group_type			*grp;
	
	if (map.ngroup<max_group) {
	
		grp=&map.groups[map.ngroup];
		grp->name[0]=0x0;
		if (dialog_group_settings_run(grp)) {
			map.ngroup++;
		}
		
	}
	
	main_wind_tool_fill_group_combo();
}

void group_delete(void)
{
	int					n,group_idx;
	segment_type		*seg;
	
		// get current group
		
	group_idx=GetControl32BitValue(group_combo);
	
	group_idx-=3;
	if (group_idx<0) return;
	
		// delete group from segments
		
	seg=map.segments;

	for (n=0;n!=map.nsegment;n++) {
		if (seg->group_idx!=-1) {
			if (seg->group_idx==group_idx) {
				seg->group_idx=-1;
			}
			else {
				if (seg->group_idx>group_idx) seg->group_idx--;
			}
		}
		seg++;
	}

		// remove group

	for (n=group_idx;n<map.ngroup;n++) {
		map.groups[n]=map.groups[n+1];
	}
	map.ngroup--;
	
	main_wind_tool_fill_group_combo();
}

void group_clear(void)
{
	int					n,group_idx;
	segment_type		*seg;
	
		// get current group
		
	group_idx=GetControl32BitValue(group_combo);
	
	group_idx-=3;
	if (group_idx<0) return;
	
		// clear group from segments

	seg=map.segments;

	for (n=0;n!=map.nsegment;n++) {
		if (seg->group_idx!=-1) {
			if (seg->group_idx==group_idx) {
				seg->group_idx=-1;
			}
		}
		seg++;
	}
	
	main_wind_tool_fill_group_combo();
}

/* =======================================================

      Group Counts
      
======================================================= */

int group_count(int group_idx)
{
	int					n,cnt;
	segment_type		*seg;
	
	cnt=0;
	seg=map.segments;

	for (n=0;n!=map.nsegment;n++) {
		if (seg->group_idx==group_idx) cnt++;
		seg++;
	}
	
	return(cnt);
}

/* =======================================================

      Group Sets
      
======================================================= */

void group_set(int group_idx)
{
/* supergumba
	int				n,k,sel_count,type,index,primitive_uid;
	segment_type	*seg;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
			
			case segment_piece:
				map.segments[index].group_idx=group_idx;
				break;
				
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];
		
				seg=map.segments;
				
				for (k=0;k!=map.nsegment;k++) {
					if (seg->primitive_uid[0]==primitive_uid) seg->group_idx=group_idx;
					seg++;
				}
				break;
		}
	}
	
	main_wind_tool_fill_group_combo();
	*/
}

