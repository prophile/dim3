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

#include "consoles.h"
#include "video.h"

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

/* =======================================================

      Liquid Rendering Portal
      
======================================================= */

void liquid_render_portal(int rn)
{
	int							n,nliquid,y,frame;
	unsigned long				txt_id;
	float						alpha;
	portal_type					*portal;
	map_liquid_type				*liq;
	texture_type				*texture;

	portal=&map.portals[rn];

		// if no liquids then skip

	nliquid=portal->liquid.nliquid;
	if (nliquid==0) return;

		// setup drawing

	gl_texture_transparent_start();
						
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	txt_id=-1;
	alpha=0.0f;

		// run through liquids

	liq=portal->liquid.liquids;

	for (n=0;n!=nliquid;n++) {

		texture=&map.textures[liq->txt_idx];
		frame=texture->animate.current_frame&max_texture_frame_mask;

			// draw the transparent texture

		if ((texture->bitmaps[frame].gl_id!=txt_id) || (liq->alpha!=alpha)) {
		
			txt_id=texture->bitmaps[frame].gl_id;
			alpha=liq->alpha;
			gl_texture_transparent_set(txt_id,alpha);
			
			if (texture->additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}

		y=liq->y-view.camera.pos.y;

		glBegin(GL_QUADS);

		glTexCoord2f(liq->x_txtoff,liq->y_txtoff);
		glVertex3i(liq->lft-view.camera.pos.x,y,view.camera.pos.z-liq->top);

		glTexCoord2f(liq->x_txtoff+liq->x_txtfact,liq->y_txtoff);
		glVertex3i(liq->rgt-view.camera.pos.x,y,view.camera.pos.z-liq->top);

		glTexCoord2f(liq->x_txtoff+liq->x_txtfact,liq->y_txtoff+liq->y_txtfact);
		glVertex3i(liq->rgt-view.camera.pos.x,y,view.camera.pos.z-liq->bot);

		glTexCoord2f(liq->x_txtoff,liq->y_txtoff+liq->y_txtfact);
		glVertex3i(liq->lft-view.camera.pos.x,y,view.camera.pos.z-liq->bot);

		glEnd();

		liq++;
	}

	gl_texture_transparent_end();
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render(int portal_cnt,int *portal_list)
{
	int					i;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
	
		// run through portals
		// we want to go from furthest away to closest
		// for the transparency sorting
		
	for (i=0;i<portal_cnt;i++) {
		liquid_render_portal(portal_list[i]);
	}
}

