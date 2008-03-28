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
	int					n,k,group_idx;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;
	
		// get current group
		
	group_idx=GetControl32BitValue(group_combo);
	
	group_idx-=3;
	if (group_idx<0) return;
	
		// clear group from meshes and liquids
		
	portal=map.portals;

	for (n=0;n!=map.nportal;n++) {
	
		mesh=portal->mesh.meshes;
		
		for (k=0;k!=portal->mesh.nmesh;k++) {
			if (mesh->group_idx==group_idx) mesh->group_idx=-1;
			mesh++;
		}
		
		liq=portal->liquid.liquids;
		
		for (k=0;k!=portal->liquid.nliquid;k++) {
			if (liq->group_idx==group_idx) liq->group_idx=-1;
			liq++;
		}
		
		portal++;
	}
	
	main_wind_tool_fill_group_combo();
}

/* =======================================================

      Group Counts
      
======================================================= */

int group_count(int group_idx)
{
	int					n,k,cnt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;
	
	cnt=0;
	
	portal=map.portals;

	for (n=0;n!=map.nportal;n++) {
	
		mesh=portal->mesh.meshes;
		
		for (k=0;k!=portal->mesh.nmesh;k++) {
			if (mesh->group_idx==group_idx) cnt++;
			mesh++;
		}
		
		liq=portal->liquid.liquids;
		
		for (k=0;k!=portal->liquid.nliquid;k++) {
			if (liq->group_idx==group_idx) cnt++;
			liq++;
		}
		
		portal++;
	}
	
	return(cnt);
}

/* =======================================================

      Group Sets
      
======================================================= */

void group_set(int group_idx)
{
	int				n,sel_count,type,portal_idx,main_idx,sub_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
		
		switch (type) {
		
			case mesh_piece:
				map.portals[portal_idx].mesh.meshes[main_idx].group_idx=group_idx;
				break;
				
			case liquid_piece:
				map.portals[portal_idx].liquid.liquids[main_idx].group_idx=group_idx;
				break;
				
		}
	}
	
	main_wind_tool_fill_group_combo();
}

