/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Segment Liquid Tide and Breakup Routines

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

      Liquid Vertex Lists
      
======================================================= */

int map_liquid_count_vertexes(segment_type *seg)
{
	int						x,z,x_add,z_add,vl_cnt,
							vl_x_sz,vl_z_sz;
	bool					x_break,z_break;
	liquid_segment_data		*liq;

	liq=&seg->data.liquid;
	
		// count vertexes
		
	vl_cnt=0;
	
	z=liq->top;
	z_add=liq->wavesize-(z%liq->wavesize);
	z_break=FALSE;
	vl_z_sz=0;
	
	while (TRUE) {
		x=liq->lft;
		x_add=liq->wavesize-(x%liq->wavesize);
		x_break=FALSE;
		vl_x_sz=0;
		
		while (TRUE) {
			vl_cnt++;
			vl_x_sz++;
			
			if (x_break) break;
			
			x+=x_add;
			x_add=liq->wavesize;
			
			if (x>=liq->rgt) {
				x=liq->rgt;
				x_break=TRUE;
			}
		}
		
		vl_z_sz++;
		
		if (z_break) break;
		
		z+=z_add;
		z_add=liq->wavesize;
		
		if (z>=liq->bot) {
			z=liq->bot;
			z_break=TRUE;
		}
	}
	
	liq->vl_x_sz=vl_x_sz-1;
	liq->vl_z_sz=vl_z_sz-1;
	
	return(vl_cnt);
}

int map_liquid_add_vertex_list(portal_vertex_list_type *vl,int vl_cnt,segment_type *seg)
{
	int						x,z,y,x_add,z_add;
	bool					x_break,z_break;
	liquid_segment_data		*liq;
	
	liq=&seg->data.liquid;
	
	y=liq->y;
	
		// liquids have vertexes all together
		
	liq->vl_idx_start=vl_cnt;
	vl+=vl_cnt;

		// create vertexes from tide segments
		
	z=liq->top;
	z_add=liq->wavesize-(z%liq->wavesize);
	z_break=FALSE;
	
	while (TRUE) {
		x=liq->lft;
		x_add=liq->wavesize-(x%liq->wavesize);
		x_break=FALSE;
		
		while (TRUE) {
			vl->x=(float)x;
			vl->z=(float)z;
			vl->y=(float)y;

			vl->gx=seg->x_txtoff+((seg->x_txtfact*(float)(x-liq->lft))/(float)(liq->rgt-liq->lft));
			vl->gy=seg->y_txtoff+((seg->y_txtfact*(float)(z-liq->top))/(float)(liq->bot-liq->top));
			
			vl++;
			vl_cnt++;
			
			if (x_break) break;
			
			x+=x_add;
			x_add=liq->wavesize;
			
			if (x>=liq->rgt) {
				x=liq->rgt;
				x_break=TRUE;
			}
		}
		
		if (z_break) break;
		
		z+=z_add;
		z_add=liq->wavesize;
		
		if (z>=liq->bot) {
			z=liq->bot;
			z_break=TRUE;
		}
	}
	
	return(vl_cnt);
}

/* =======================================================

      Update Vertex List UVs
      
======================================================= */

void map_liquid_vertex_update_uvs(portal_type *portal,segment_type *seg)
{
	int							x,z,x_add,z_add;
	bool						x_break,z_break;
	liquid_segment_data			*liq;
	portal_vertex_list_type		*vl;
	
	liq=&seg->data.liquid;
	vl=portal->vertexes.vertex_list+liq->vl_idx_start;

		// reset the uvs
		
	z=liq->top;
	z_add=liq->wavesize-(z%liq->wavesize);
	z_break=FALSE;
	
	while (TRUE) {
		x=liq->lft;
		x_add=liq->wavesize-(x%liq->wavesize);
		x_break=FALSE;
		
		while (TRUE) {
			vl->x=(float)x;
			vl->z=(float)z;

			vl->gx=seg->x_txtoff+((seg->x_txtfact*(float)(x-liq->lft))/(float)(liq->rgt-liq->lft));
			vl->gy=seg->y_txtoff+((seg->y_txtfact*(float)(z-liq->top))/(float)(liq->bot-liq->top));
			
			vl++;
			
			if (x_break) break;
			
			x+=x_add;
			x_add=liq->wavesize;
			
			if (x>=liq->rgt) {
				x=liq->rgt;
				x_break=TRUE;
			}
		}
		
		if (z_break) break;
		
		z+=z_add;
		z_add=liq->wavesize;
		
		if (z>=liq->bot) {
			z=liq->bot;
			z_break=TRUE;
		}
	}
}

/* =======================================================

      Change Tide Y's in Portal Vertex Lists
      
======================================================= */

void map_liquid_vertex_segment_tide(int tick,segment_type *seg,portal_vertex_list_type *vl)
{
	int						x,z,
							xsz,zsz,y,k,liquid_wave_size,liquid_tide_rate,liquid_tide_size;
	float					f_break,f_time,sn,
							f_liquid_wave_size,f_liquid_tide_size,f_liquid_tide_rate;
	liquid_segment_data		*liq;
	
	liq=&seg->data.liquid;

		// no size or rate means no tides
		
	liquid_tide_size=liq->tidesize;
	liquid_tide_rate=liq->tiderate;
	if ((liquid_tide_size<=0) || (liquid_tide_rate<=0)) return;
	
		// starting vl and size
		
	xsz=liq->vl_x_sz+1;
	zsz=liq->vl_z_sz+1;
	
	vl+=liq->vl_idx_start;
	
		// pre tide setup
		
	y=liq->y;
	
	liquid_wave_size=liq->wavesize<<2;
	f_liquid_wave_size=(float)liquid_wave_size;
	
	f_liquid_tide_size=(float)liquid_tide_size;
	f_liquid_tide_rate=(float)liquid_tide_rate;
	
		// alter the tide
	
	for (z=0;z!=zsz;z++) {
		for (x=0;x!=xsz;x++) {
			
				// get tide breaking into 0...1
				
			if (liq->tidedirection==ld_vertical) {
				k=(int)vl->z;
			}
			else {
				k=(int)vl->x;
			}
				
			f_break=(float)(k%liquid_wave_size);
			f_break=f_break/f_liquid_wave_size;
			
				// get rate into 0..1
				
			f_time=(float)(tick%liquid_tide_rate);
			f_time=f_time/f_liquid_tide_rate;
				
				// create tide Y
		   
			sn=(float)sin((TRIG_PI*2.0f)*(f_break+f_time));
			vl->y=y-(f_liquid_tide_size*sn);

			vl++;
		}
	}
}

void map_portal_liquid_vertex_set_tide(map_type *map,int tick,portal_type *portal)
{
	int						i,cnt,idx;
	short					*sptr;
	portal_vertex_list_type	*vl;
	
	vl=portal->vertexes.vertex_list;
	
	cnt=portal->liquid_list_hit.count;
	sptr=portal->liquid_list_hit.list;
	
	for (i=0;i!=cnt;i++) {
		idx=(int)*sptr++;
		map_liquid_vertex_segment_tide(tick,&map->segments[idx],vl);
	}
}

