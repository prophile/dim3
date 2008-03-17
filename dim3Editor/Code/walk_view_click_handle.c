/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Handle Clicking Routines

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
#include "walk_view.h"

extern int					cr,cx,cz,cy,vertex_mode;
extern bool					dp_auto_texture;
extern CCrsrHandle			dragcur;

extern map_type				map;

/* =======================================================

      Move Similiar Handles
      
======================================================= */

// supergumba -- delete?
void walk_view_move_similiar_handle(int rn,int x,int z,int y,int xadd,int zadd,int yadd)
{
    int				i,k,ptsz,nx,nz,nrn;
    segment_type	*seg;
    fc_segment_data	*fc;
    
    x+=map.portals[rn].x;
    z+=map.portals[rn].z;
    
    seg=map.segments;
    
    for ((i=0);(i!=map.nsegment);i++) {
	
		if (seg->primitive_uid[0]!=-1) {
			seg++;
			continue;
		}
    
        if ((seg->type==sg_floor) || (seg->type==sg_ceiling)) {
        	fc=&seg->data.fc;
            
            nrn=seg->rn;
            
            ptsz=fc->ptsz;
            for ((k=0);(k!=ptsz);k++) {
                nx=fc->x[k]+map.portals[nrn].x;
                nz=fc->z[k]+map.portals[nrn].z;
                
                if ((nx==x) && (nz==z) && (fc->y[k]==y)) {
                    fc->x[k]-=xadd;
                    fc->z[k]-=zadd;
                    fc->y[k]-=yadd;
                }
            }
        }
    
        seg++;
    }
}

