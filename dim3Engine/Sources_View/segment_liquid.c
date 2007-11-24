/****************************** File *********************************

Module: dim3 Engine
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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

extern map_type				map;
extern server_type			server;
extern view_type			view;

/* =======================================================

      Draw Liquid Tide
      
======================================================= */

void segment_liquid_tide_draw(segment_type *seg)
{
	int						x,z,lx,rx,tz,bz,top_row,bot_row,
							vl_idx[4],xsz,zsz,lx_add,tz_add,vl_idx_start;
	liquid_segment_data		*liq;
	
	liq=&seg->data.liquid;
	
		// get texture vertexes sizes

	xsz=liq->vl_x_sz;
	zsz=liq->vl_z_sz;
	
	vl_idx_start=liq->vl_idx_start;
	
		// run through tide segments
		
	tz=liq->top;
	tz_add=liq->wavesize-(tz%liq->wavesize);
	top_row=vl_idx_start;
	
	for (z=0;z!=zsz;z++) {
		bz=tz+tz_add;
		tz_add=liq->wavesize;
		if (bz>=liq->bot) bz=liq->bot;
		
		lx=liq->lft;
		lx_add=liq->wavesize-(lx%liq->wavesize);
		bot_row=top_row+(xsz+1);
		
		for (x=0;x!=xsz;x++) {
			rx=lx+lx_add;
			lx_add=liq->wavesize;
			if (rx>=liq->rgt) rx=liq->rgt;
				
				// pull polygon vertexes out of liquid vertex list
			
			vl_idx[0]=top_row+x;
			vl_idx[1]=top_row+(x+1);
			vl_idx[2]=bot_row+(x+1);
			vl_idx[3]=bot_row+x;
			
				// draw polygon

			glDrawElements(GL_POLYGON,4,GL_UNSIGNED_INT,(GLvoid*)vl_idx);
			
			lx=rx;
		}
	
		tz=bz;
		top_row=bot_row;
	}
}

/* =======================================================

      Draw Liquid Tide Bump
      
======================================================= */

void segment_liquid_tide_draw_bump(segment_type *seg)
{
	int						x,z,lx,rx,tz,bz,top_row,bot_row;
	int						vl_idx[4],xsz,zsz,lx_add,tz_add,vl_idx_start;
	liquid_segment_data		*liq;
	
	liq=&seg->data.liquid;
	
		// get texture vertexes sizes

	xsz=liq->vl_x_sz;
	zsz=liq->vl_z_sz;
	
	vl_idx_start=liq->vl_idx_start;
	
		// run through tide segments
		
	tz=liq->top;
	tz_add=liq->wavesize-(tz%liq->wavesize);
	top_row=vl_idx_start;
	
	for (z=0;z!=zsz;z++) {
		bz=tz+tz_add;
		tz_add=liq->wavesize;
		if (bz>=liq->bot) bz=liq->bot;
		
		lx=liq->lft;
		lx_add=liq->wavesize-(lx%liq->wavesize);
		bot_row=top_row+(xsz+1);
		
		for (x=0;x!=xsz;x++) {
			rx=lx+lx_add;
			lx_add=liq->wavesize;
			if (rx>=liq->rgt) rx=liq->rgt;
				
				// pull polygon vertexes out of liquid vertex list
			
			vl_idx[0]=top_row+x;
			vl_idx[1]=top_row+(x+1);
			vl_idx[2]=bot_row+(x+1);
			vl_idx[3]=bot_row+x;
			
				// draw polygon
				
			glDrawElements(GL_POLYGON,4,GL_UNSIGNED_INT,(GLvoid*)vl_idx);
			
			lx=rx;
		}
	
		tz=bz;
		top_row=bot_row;
	}
}

/* =======================================================

      Draw Liquid Tide Lighting
      
======================================================= */

void segment_liquid_tide_draw_lighting(segment_type *seg,bool light)
{
	int						x,z,lx,rx,tz,bz,top_row,bot_row,
							vl_idx[4],xsz,zsz,lx_add,tz_add,vl_idx_start;
	liquid_segment_data		*liq;
	
	liq=&seg->data.liquid;
	
		// get texture vertexes sizes

	xsz=liq->vl_x_sz;
	zsz=liq->vl_z_sz;
	
	vl_idx_start=liq->vl_idx_start;
	
		// run through tide segments
		
	tz=liq->top;
	tz_add=liq->wavesize-(tz%liq->wavesize);
	top_row=vl_idx_start;
	
	for (z=0;z!=zsz;z++) {
		bz=tz+tz_add;
		tz_add=liq->wavesize;
		if (bz>=liq->bot) bz=liq->bot;
		
		lx=liq->lft;
		lx_add=liq->wavesize-(lx%liq->wavesize);
		bot_row=top_row+(xsz+1);
		
		for (x=0;x!=xsz;x++) {
			rx=lx+lx_add;
			lx_add=liq->wavesize;
			if (rx>=liq->rgt) rx=liq->rgt;
				
				// pull polygon vertexes out of liquid vertex list
			
			vl_idx[0]=top_row+x;
			vl_idx[1]=top_row+(x+1);
			vl_idx[2]=bot_row+(x+1);
			vl_idx[3]=bot_row+x;
			
				// draw polygon
				
			glDrawElements(GL_POLYGON,4,GL_UNSIGNED_INT,(GLvoid*)vl_idx);
			
			lx=rx;
		}
	
		tz=bz;
		top_row=bot_row;
	}
}

