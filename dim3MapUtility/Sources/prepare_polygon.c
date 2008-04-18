/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Pre-Calc Polygons of Segments

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


// supergumba -- tons of this can be deleted


/* =======================================================

      Create Polygons for Ambient Wall Segments
      
======================================================= */



/* =======================================================

      Create Polygons for Ambient FC Segments
      
======================================================= */

void map_prepare_push_ambient_fc_segment_polygon(segment_type *seg)
{
	int							i,ptsz;
    ambient_fc_segment_data		*ambient_fc;
    
	ambient_fc=&seg->data.ambient_fc;
	
	ptsz=ambient_fc->ptsz;
   
    for (i=0;i!=ptsz;i++) {
	
		switch (ambient_fc->push) {
			case ap_top:
				ambient_fc->z[i]-=ambient_push_size;
				break;
			case ap_bottom:
				ambient_fc->z[i]+=ambient_push_size;
				break;
			case ap_left:
				ambient_fc->x[i]-=ambient_push_size;
				break;
			case ap_right:
				ambient_fc->x[i]+=ambient_push_size;
				break;
			case ap_up:
				ambient_fc->y[i]-=ambient_push_size;
				break;
			case ap_down:
				ambient_fc->y[i]+=ambient_push_size;
				break;
		}
    }
}

