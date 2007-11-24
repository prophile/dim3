/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Light Ray Tracing

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"

extern map_type				map;
extern server_type			server;

extern float ray_trace_polygon(d3pnt *spt,d3vct *vct,d3pnt *hpt,int ptsz,int *x,int *y,int *z);

/* =======================================================

      Light Trace Portals
      
======================================================= */

bool light_trace_portal(int rn,d3pnt *spt,d3pnt *ept,d3vct *vct)
{
	int					n,idx,cnt;
	short				*sptr;
	float				t;
	d3pnt				pt;
	segment_type		*seg;
	wall_segment_data	*wall;
	fc_segment_data		*fc;
	portal_type			*portal;
	
	portal=&map.portals[rn];
	pt.x=pt.y=pt.z=0;
	
		// check the wall segments

	sptr=portal->wall_list_hit.list;
	cnt=portal->wall_list_hit.count;
	
	for (n=0;n!=cnt;n++) {
		idx=(int)*sptr++;

			// skip transparents

		seg=&map.segments[idx];
		if (map.textures[seg->fill].bitmaps[0].alpha_mode!=alpha_mode_none) continue;

		wall=&seg->data.wall;
			
			// rough bounds check

		if (wall->lx<wall->rx) {
			if ((spt->x<wall->lx) && (ept->x<wall->lx)) continue;
			if ((spt->x>wall->rx) && (ept->x>wall->rx)) continue;
		}
		else {
			if ((spt->x>wall->lx) && (ept->x>wall->lx)) continue;
			if ((spt->x<wall->rx) && (ept->x<wall->rx)) continue;
		}
		
		if (wall->lz<wall->rz) {
			if ((spt->z<wall->lz) && (ept->z<wall->lz)) continue;
			if ((spt->z>wall->rz) && (ept->z>wall->rz)) continue;
		}
		else {
			if ((spt->z>wall->lz) && (ept->z>wall->lz)) continue;
			if ((spt->z<wall->rz) && (ept->z<wall->rz)) continue;
		}
		
		if ((spt->y<wall->ty) && (ept->y<wall->ty)) continue;
		if ((spt->y>wall->by) && (ept->y>wall->by)) continue;

			// ray trace
			
		t=ray_trace_polygon(spt,vct,&pt,wall->ptsz,wall->x,wall->y,wall->z);
		if (t!=-1.0f) return(TRUE);
	}
	
		// check the floor\ceiling segments

	sptr=portal->fc_list_hit.list;
	cnt=portal->fc_list_hit.count;
	
	for (n=0;n!=cnt;n++) {
		idx=(int)*sptr++;

			// skip transparents

		seg=&map.segments[idx];
		if (map.textures[seg->fill].bitmaps[0].alpha_mode!=alpha_mode_none) continue;

		fc=&seg->data.fc;
			
			// rough bounds check

		if ((spt->y<fc->min_y) && (ept->y<fc->min_y)) continue;
		if ((spt->y>fc->max_y) && (ept->y>fc->max_y)) continue;
		if ((spt->x<fc->min_x) && (ept->x<fc->min_x)) continue;
		if ((spt->x>fc->max_x) && (ept->x>fc->max_x)) continue;
		if ((spt->z<fc->min_z) && (ept->z<fc->min_z)) continue;
		if ((spt->z>fc->max_z) && (ept->z>fc->max_z)) continue;

			// ray trace
			
		t=ray_trace_polygon(spt,vct,&pt,fc->ptsz,fc->x,fc->y,fc->z);
		if (t!=-1.0f) return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Light Trace MainLine
      
======================================================= */

bool light_trace_map(d3pnt *spt,d3pnt *ept)
{
	int					n;
	d3vct				vct;
	portal_type			*portal;
	
		// get vector
		
	vct.x=(float)(ept->x-spt->x);
	vct.y=(float)(ept->y-spt->y);
	vct.z=(float)(ept->z-spt->z);
	
		// move start point ahead a bit
		
	spt->x+=(int)(vct.x*0.01);
	spt->y+=(int)(vct.y*0.01);
	spt->z+=(int)(vct.z*0.01);
	
		// check against all portals

	for (n=0;n!=map.nportal;n++) {
		portal=&map.portals[n];
		
		if ((spt->x<portal->x) && (ept->x<portal->x)) continue;
		if ((spt->x>portal->ex) && (ept->x>portal->ex)) continue;
		if ((spt->z<portal->z) && (ept->z<portal->z)) continue;
		if ((spt->z>portal->ez) && (ept->z>portal->ez)) continue;
		if ((spt->y<portal->ty) && (ept->y<portal->ty)) continue;
		if ((spt->y>portal->by) && (ept->y>portal->by)) continue;
		
		if (light_trace_portal(n,spt,ept,&vct)) return(TRUE);
	}
	
	return(FALSE);
}


