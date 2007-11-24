/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Segment Sorting Routines

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

extern map_type				map;

/* =======================================================

      Sort Segments
      
======================================================= */

void segments_sort(void)
{
    int				t,q,k,n,ncopy;
    segment_type	*seg,*segcopy;
    
    segcopy=(segment_type*)valloc(sizeof(segment_type)*map.nsegment);
    if (segcopy==NULL) {
        InitCursor();
        StandardAlert(0,"\pNot Enough Memory",NULL,NULL,NULL);
        return;
    }
	
    progress_wind_start("Sorting Segments...",map.nsegment);
    
        // sort by texture then by room then by type
    
    ncopy=0;
    
    for ((t=0);(t!=max_map_texture);t++) {
        for ((q=0);(q!=map.nportal);q++) {
            for ((k=sg_wall);(k<=sg_ambient_fc);k++) {
            
                seg=map.segments;
                for ((n=0);(n!=map.nsegment);n++) {
                    if ((seg->fill==t) && (seg->rn==q) && (seg->type==k)) {
                        memmove(&segcopy[ncopy],seg,sizeof(segment_type));
                        ncopy++;
                        
                        progress_wind_update(ncopy);
                    }
                    seg++;
                }
            
            }
        }
    }
    
    memmove(map.segments,segcopy,(sizeof(segment_type)*map.nsegment));
    free(segcopy);
    
    progress_wind_close();
}

