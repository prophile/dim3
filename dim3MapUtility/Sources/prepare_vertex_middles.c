/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Pre-calc Segment Vertexes and Middles

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

      Create Segment Middles
      
======================================================= */

void map_prepare_calculate_segment_vertex_middle(segment_type *seg)
{
	int							i,ptsz,mx,mz,my;
    wall_segment_data			*wall;
    fc_segment_data				*fc;
	liquid_segment_data			*liq;
	ambient_wall_segment_data	*ambient_wall;
	ambient_fc_segment_data		*ambient_fc;
    
    switch (seg->type) {
    
            // walls
        
    	case sg_wall:
            wall=&seg->data.wall;

			seg->middle.x=(wall->lx+wall->rx)>>1;
			seg->middle.z=(wall->lz+wall->rz)>>1;
			seg->middle.y=(wall->ty+wall->by)>>1;
            return;
			
            // floors and ceilings

        case sg_floor:
        case sg_ceiling:
		case sg_poly:
            fc=&seg->data.fc;
			
			mx=mz=my=0;
       
            ptsz=fc->ptsz;
            for ((i=0);(i!=ptsz);i++) {
                mx+=fc->x[i];
                mz+=fc->z[i];
                my+=fc->y[i];
            }
    
            seg->middle.x=mx/ptsz;
            seg->middle.z=mz/ptsz;
            seg->middle.y=my/ptsz;
            return;
			
			// liquids
			
		case sg_liquid:
			liq=&seg->data.liquid;
			
			seg->middle.x=(liq->lft+liq->rgt)>>1;
			seg->middle.z=(liq->top+liq->bot)>>1;
			seg->middle.y=liq->y;
			return;
			
			// ambient walls
			
		case sg_ambient_wall:
			ambient_wall=&seg->data.ambient_wall;
			
			seg->middle.x=(ambient_wall->lx+ambient_wall->rx)>>1;
			seg->middle.z=(ambient_wall->lz+ambient_wall->rz)>>1;
			seg->middle.y=(ambient_wall->ty+ambient_wall->by)>>1;
			return;
			
			// ambient fcs
			
        case sg_ambient_fc:
            ambient_fc=&seg->data.ambient_fc;
			
			mx=mz=my=0;
        
            ptsz=ambient_fc->ptsz;
            for ((i=0);(i!=ptsz);i++) {
                mx+=ambient_fc->x[i];
                mz+=ambient_fc->z[i];
                my+=ambient_fc->y[i];
            }
    
            seg->middle.x=mx/ptsz;
            seg->middle.z=mz/ptsz;
            seg->middle.y=my/ptsz;
            return;
   }
}

void map_prepare_create_segment_middles(map_type *map)
{
   int					i;
    segment_type		*seg;
    
    seg=map->segments;
    
    for (i=0;i!=map->nsegment;i++) {
        map_prepare_calculate_segment_vertex_middle(seg);
		seg++;
    }
}

