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

      Liquid Memory
      
======================================================= */

bool liquid_create_memory(void)
{
	int				n,k,x_sz,z_sz,v_sz;
	portal_type		*portal;
	map_liquid_type	*liq;

	portal=map.portals;

	for (n=0;n!=map.nportal;n++) {

		liq=portal->liquid.liquids;

		for (k=0;k!=portal->liquid.nliquid;k++) {

			x_sz=((liq->rgt-liq->lft)/liq->tide.split)+2;
			z_sz=((liq->bot-liq->top)/liq->tide.split)+2;

			v_sz=x_sz*z_sz;

				// vertex list

			liq->draw.vl_list=(float*)valloc((v_sz*3)*sizeof(float));
			if (liq->draw.vl_list==NULL) return(FALSE);

				// uv list

			liq->draw.uv_list=(float*)valloc((v_sz*2)*sizeof(float));
			if (liq->draw.uv_list==NULL) return(FALSE);

				// element index list

			liq->draw.idx_list=(int*)valloc((v_sz*4)*sizeof(int));
			if (liq->draw.idx_list==NULL) return(FALSE);

			liq++;
		}

		portal++;
	}

	return(TRUE);
}

void liquid_free_memory(void)
{
	int				n,k;
	portal_type		*portal;
	map_liquid_type	*liq;

	portal=map.portals;

	for (n=0;n!=map.nportal;n++) {

		liq=portal->liquid.liquids;

		for (k=0;k!=portal->liquid.nliquid;k++) {
			free(liq->draw.vl_list);
			free(liq->draw.uv_list);
			free(liq->draw.idx_list);
			liq++;
		}

		portal++;
	}
}

/* =======================================================

      Liquid Rendering Liquid
      
======================================================= */

void liquid_render_portal_liquid_create_vertex(map_liquid_type *liq)
{
	int						x,z,x_add,z_add,x_sz,z_sz,
							v_cnt,tide_split;
	float					fy,fgx,fgy;
	bool					x_break,z_break;
	float					*vl,*uv;
	
	fy=(float)(liq->y-view.camera.pos.y);

	vl=liq->draw.vl_list;
	uv=liq->draw.uv_list;

		// create vertexes from tide splits

	tide_split=liq->tide.split;

	fgx=(float)(liq->rgt-liq->lft);
	fgy=(float)(liq->bot-liq->top);
		
	z=liq->top;
	z_add=tide_split-(z%tide_split);
	z_break=FALSE;

	v_cnt=0;

	z_sz=0;
	
	while (TRUE) {

		x=liq->lft;
		x_add=tide_split-(x%tide_split);
		x_break=FALSE;

		x_sz=0;
		
		while (TRUE) {
			*vl++=(float)(x-view.camera.pos.x);
			*vl++=fy;
			*vl++=(float)(view.camera.pos.z-z);

			*uv++=liq->x_txtoff+((liq->x_txtfact*(float)(x-liq->lft))/fgx);
			*uv++=liq->y_txtoff+((liq->y_txtfact*(float)(z-liq->top))/fgy);

			v_cnt++;
			
			x_sz++;

			if (x_break) break;
			
			x+=x_add;
			x_add=tide_split;
			
			if (x>=liq->rgt) {
				x=liq->rgt;
				x_break=TRUE;
			}
		}

		z_sz++;
		
		if (z_break) break;
		
		z+=z_add;
		z_add=tide_split;
		
		if (z>=liq->bot) {
			z=liq->bot;
			z_break=TRUE;
		}
	}

		// setup split sizes

	liq->draw.v_cnt=v_cnt;

	liq->draw.x_sz=x_sz;
	liq->draw.z_sz=z_sz;
}

void liquid_render_portal_liquid_alter_tide(int tick,map_liquid_type *liq)
{
	int						x,z,k,x_sz,z_sz,
							tide_split_half,tide_high,tide_rate;
	float					fy,f_break,f_time,sn,
							f_tide_split_half,f_tide_high;
	float					*vl;
	
		// any tide differences?

	tide_high=liq->tide.high;
	tide_rate=liq->tide.rate;

	if ((tide_high<=0) || (tide_rate<=0)) return;

		// setup tide changes

	x_sz=liq->draw.x_sz;
	z_sz=liq->draw.z_sz;

	fy=(float)(liq->y-view.camera.pos.y);

	tide_split_half=liq->tide.split<<2;
	f_tide_split_half=(float)tide_split_half;
	
	f_tide_high=(float)tide_high;

	f_time=(float)(tick%tide_rate);		// get rate between 0..1
	f_time=f_time/(float)tide_rate;

		// alter Ys

	vl=liq->draw.vl_list;

	for (z=0;z!=z_sz;z++) {
		for (x=0;x!=x_sz;x++) {
			
				// get tide breaking into 0...1
				
			if (liq->tide.direction==ld_vertical) {
				k=((int)*(vl+2))-view.camera.pos.z;
			}
			else {
				k=((int)*vl)+view.camera.pos.x;
			}
				
			f_break=(float)(k%tide_split_half);
			f_break=f_break/f_tide_split_half;
				
				// create tide Y
		   
			sn=(float)sin((TRIG_PI*2.0f)*(f_break+f_time));
			*(vl+1)=fy-(f_tide_high*sn);

			vl+=3;
		}
	}
}

void liquid_render_portal_liquid(int tick,map_liquid_type *liq)
{
	int						x,z,x_sz,z_sz,quad_cnt,v_sz,
							tz,bz,tz_add,top_row,bot_row,
							lx,rx,lx_add,tide_split;
	int						*d_idx;
	
		// create vertexes and UVs

	liquid_render_portal_liquid_create_vertex(liq);

		// alter Ys for tide

	liquid_render_portal_liquid_alter_tide(tick,liq);

		// drawing the quads only draws to the edges

	x_sz=liq->draw.x_sz-1;
	if (x_sz<=0) return;

	z_sz=liq->draw.z_sz-1;
	if (z_sz<=0) return;

	tide_split=liq->tide.split;

		// create the draw indexes

	quad_cnt=0;
	d_idx=liq->draw.idx_list;
		
	tz=liq->top;
	tz_add=tide_split-(tz%tide_split);
	top_row=0;
	
	for (z=0;z!=z_sz;z++) {
		bz=tz+tz_add;
		tz_add=tide_split;
		if (bz>=liq->bot) bz=liq->bot;
		
		lx=liq->lft;
		lx_add=tide_split-(lx%tide_split);
		bot_row=top_row+(x_sz+1);
		
		for (x=0;x!=x_sz;x++) {
			rx=lx+lx_add;
			lx_add=tide_split;
			if (rx>=liq->rgt) rx=liq->rgt;
			
			*d_idx++=top_row+x;
			*d_idx++=top_row+(x+1);
			*d_idx++=bot_row+(x+1);
			*d_idx++=bot_row+x;

			quad_cnt++;

			lx=rx;
		}
	
		tz=bz;
		top_row=bot_row;
	}

		// vertex array

	v_sz=(liq->draw.v_cnt*4)*sizeof(float);

#ifdef D3_OS_MAC
	glVertexArrayRangeAPPLE(v_sz,liq->draw.vl_list);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,liq->draw.vl_list);

		// uv array
		
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,liq->draw.uv_list);

		// draw the quads

	glDrawElements(GL_QUADS,(quad_cnt*4),GL_UNSIGNED_INT,(GLvoid*)liq->draw.idx_list);

		// release the lists

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

#ifdef D3_OS_MAC
	glDisableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif
}

/* =======================================================

      Liquid Rendering Portal
      
======================================================= */

void liquid_render_portal(int tick,int rn)
{
	int							n,nliquid,frame;
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

		liquid_render_portal_liquid(tick,liq);

		/*
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
*/
		liq++;
	}

	gl_texture_transparent_end();
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render(int tick,int portal_cnt,int *portal_list)
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
		liquid_render_portal(tick,portal_list[i]);
	}
}

