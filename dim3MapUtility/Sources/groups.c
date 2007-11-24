/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Deconstruct Curved Segments into Multiple Segments

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

      Create and Dispose Single Group Segment List
      
======================================================= */

bool map_group_create_single_segment_list(map_type *map,int group_idx)
{
	int				n,seg_cnt;
	short			*seg_list;
	segment_type	*seg;
	group_type		*group;
	
	group=&map->groups[group_idx];
	
		// default setting
		
	group->seg_count=0;
	group->seg_list=NULL;
	
		// count seg list
		
	seg_cnt=0;
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if (seg->group_idx==group_idx) seg_cnt++;
		seg++;
	}
	
	if (seg_cnt==0) return(TRUE);
	
		// create seg list
		
	seg_list=(short*)valloc(seg_cnt*sizeof(short));
	if (seg_list==NULL) return(FALSE);
	
	bzero(seg_list,(seg_cnt*sizeof(short)));
	
	group->seg_count=seg_cnt;
	group->seg_list=seg_list;
	
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if (seg->group_idx==group_idx) {
			*seg_list=(short)n;
			seg_list++;
		}
		seg++;
	}

	return(TRUE);
}

/* =======================================================

      Setup Group Segment Lists
      
======================================================= */

bool map_group_create_segment_list(map_type *map)
{
	int				n;
	
	for (n=0;n!=map->ngroup;n++) {
		if (!map_group_create_single_segment_list(map,n)) return(FALSE);
	}
	
	return(TRUE);
}

void map_group_dispose_segment_list(map_type *map)
{
	int				n;
	group_type		*group;
	
	group=map->groups;
	
	for (n=0;n!=map->ngroup;n++) {
		if (group->seg_list!=NULL) free(group->seg_list);
	
		group->seg_count=0;
		group->seg_list=NULL;
		
		group++;
	}
}

/* =======================================================

      Find Center Point for Group
      
======================================================= */

void map_group_get_center(map_type *map,int group_idx,int *x,int *y,int *z)
{
	int					n,seg_cnt,mx,my,mz;
	short				*seg_list;
	segment_type		*seg;
	group_type			*group;
	
	group=&map->groups[group_idx];
	
	seg_cnt=group->seg_count;
	if (seg_cnt==0) {
		*x=*y=*z=0;
		return;
	}
	
	seg_list=group->seg_list;
	mx=my=mz=0;
	
	for (n=0;n!=seg_cnt;n++) {
		seg=&map->segments[*seg_list];

		mx+=seg->middle.x;
		my+=seg->middle.y;
		mz+=seg->middle.z;
		
		seg_list++;
	}
	
	*x=(mx/seg_cnt);
	*y=(my/seg_cnt);
	*z=(mz/seg_cnt);
}

