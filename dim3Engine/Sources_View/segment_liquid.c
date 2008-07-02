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
extern setup_type			setup;

extern void light_trace_calculate_light_color(portal_type *portal,float x,float y,float z,float *cf);

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

			x_sz=((liq->rgt-liq->lft)/liq->tide.split)+4;		// possible extra edges on side because of griding
			z_sz=((liq->bot-liq->top)/liq->tide.split)+4;

			v_sz=x_sz*z_sz;

				// vertex list

			liq->draw.vl_list=(float*)valloc((v_sz*3)*sizeof(float));
			if (liq->draw.vl_list==NULL) return(FALSE);

				// uv list

			liq->draw.uv_list=(float*)valloc((v_sz*2)*sizeof(float));
			if (liq->draw.uv_list==NULL) return(FALSE);
			
				// color list

			liq->draw.cl_list=(float*)valloc((v_sz*3)*sizeof(float));
			if (liq->draw.cl_list==NULL) return(FALSE);

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
			free(liq->draw.cl_list);
			free(liq->draw.idx_list);
			liq++;
		}

		portal++;
	}
}

/* =======================================================

      Setup Liquid Rendering
      
======================================================= */

void liquid_render_portal_liquid_create_vertex(portal_type *portal,map_liquid_type *liq)
{
	int						x,y,z,x_add,z_add,x_sz,z_sz,
							v_cnt,tide_split;
	float					fy,fgx,fgy;
	bool					x_break,z_break;
	float					*vl,*uv,*cl;
	
	y=liq->y;
	fy=(float)(y-view.camera.pos.y);

	vl=liq->draw.vl_list;
	uv=liq->draw.uv_list;
	cl=liq->draw.cl_list;

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
			if (setup.ray_trace_lighting) {
				light_trace_calculate_light_color(portal,(float)x,(float)y,(float)z,cl);
			}
			else {
				map_portal_calculate_light_color(portal,(double)x,(double)y,(double)z,cl);
			}
			cl+=3;

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

int liquid_render_portal_liquid_create_quads(map_liquid_type *liq)
{
	int						x,z,x_sz,z_sz,quad_cnt,
							tz,bz,tz_add,top_row,bot_row,
							lx,rx,lx_add,tide_split;
	int						*d_idx;
		
		// drawing the quads only draws to the edges

	x_sz=liq->draw.x_sz-1;
	if (x_sz<=0) return(0);

	z_sz=liq->draw.z_sz-1;
	if (z_sz<=0) return(0);

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

	return(quad_cnt);
}

void liquid_render_portal_liquid_start_array(map_liquid_type *liq)
{
	int				v_sz;

	v_sz=((liq->draw.v_cnt*4)*3)*sizeof(float);

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
	
		// color array
		
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,liq->draw.cl_list);
}

void liquid_render_portal_liquid_end_array(void)
{
	glDisableClientState(GL_COLOR_ARRAY);

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

void liquid_render_portal_liquid(int tick,portal_type *portal,map_liquid_type *liq)
{
	int						quad_cnt,frame;
	float					normal[3];
	d3pnt					mid;
	texture_type			*texture;

		// setup liquid for drawing

	liquid_render_portal_liquid_create_vertex(portal,liq);
	liquid_render_portal_liquid_alter_tide(tick,liq);
	quad_cnt=liquid_render_portal_liquid_create_quads(liq);
	if (quad_cnt==0) return;
	
	liquid_render_portal_liquid_start_array(liq);

		// setup texture

	texture=&map.textures[liq->txt_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;
			
	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

		// start shader or regular texture

	if (texture->shader.on) {
		gl_shader_program_start();
		gl_texture_shader_start();

		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
		
		mid.x=(liq->lft+liq->rgt)>>1;
		mid.y=liq->y;
		mid.z=(liq->top+liq->bot)>>1;

		gl_shader_set_variables(texture->shader.program_obj,&mid,texture);

		map_portal_calculate_light_normal(portal,(double)mid.x,(double)mid.y,(double)mid.z,normal);
		glNormal3f(normal[0],normal[1],normal[2]);
	}
	else {
		gl_texture_transparent_start();
		gl_texture_transparent_set(texture->bitmaps[frame].gl_id,liq->alpha);
	}

		// draw the quads

	glDrawElements(GL_QUADS,(quad_cnt*4),GL_UNSIGNED_INT,(GLvoid*)liq->draw.idx_list);

		// end texture

	if (texture->shader.on) {
		gl_texture_shader_end();
		gl_shader_program_end();
	}
	else {
		gl_texture_transparent_end();
	}

	liquid_render_portal_liquid_end_array();
}

void liquid_render_portal(int tick,int rn)
{
	int							n,nliquid;
	portal_type					*portal;
	map_liquid_type				*liq;
	
	portal=&map.portals[rn];

		// if no liquids then skip

	nliquid=portal->liquid.nliquid;
	if (nliquid==0) return;

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

		// run through liquids

	liq=portal->liquid.liquids;

	for (n=0;n!=nliquid;n++) {
		liquid_render_portal_liquid(tick,portal,liq);
		liq++;
	}
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

