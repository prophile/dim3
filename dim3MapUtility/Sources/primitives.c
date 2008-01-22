/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Primitives Routines

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

      Create Primitive Unique ID
      
======================================================= */

int map_primitive_create_uid(map_type *map)
{
	int				i,uid;
	bool			hit;
	segment_type	*seg;
	
	uid=0;
	
	while (TRUE) {
	
		hit=FALSE;
		seg=map->segments;
		
		for (i=0;i!=map->nsegment;i++) {
			if (seg->primitive_uid[0]==uid) {
				hit=TRUE;
				break;
			}
			seg++;
		}
		
		if (hit!=TRUE) break;
		
		uid++;
	}
	
	return(uid);
}

/* =======================================================

      Primitive Stacks
      
======================================================= */

void map_primitive_push_uid(segment_type *seg,int primitive_uid)
{
	memmove(&seg->primitive_uid[1],&seg->primitive_uid[0],(sizeof(short)*(max_primitive_stack-1)));
	seg->primitive_uid[0]=primitive_uid;
}

void map_primitive_pop_uid(segment_type *seg)
{
	if (seg->primitive_uid[0]==-1) return;		// nothing to pop
	
	memmove(&seg->primitive_uid[0],&seg->primitive_uid[1],(sizeof(short)*(max_primitive_stack-1)));
	seg->primitive_uid[max_primitive_stack-1]=-1;
}

		
		
		
		
		
		