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
#include "lights.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);
extern void view_next_vertex_object(void);
extern void view_bind_current_vertex_object(void);
extern void view_unbind_current_vertex_object(void);

/* =======================================================

      Setup Liquid Rendering
      
======================================================= */

inline int liquid_render_liquid_get_max_vertex(map_liquid_type *liq)
{
	int				x_sz,z_sz;

	x_sz=((liq->rgt-liq->lft)/liq->tide.split)+4;		// possible extra edges on side because of griding
	z_sz=((liq->bot-liq->top)/liq->tide.split)+4;

	return(x_sz*z_sz);
}

void liquid_render_liquid_create_vertex(int tick,map_liquid_type *liq)
{
	int				x,y,z,x_add,z_add,x_sz,z_sz,
					v_cnt,v_sz,sz,tide_split,tide_split_half,
					tide_high,tide_rate;
	float			fy,fgx,fgy,normal[3],
					f_break,f_time,sn,
					f_tide_split_half,f_tide_high;
	bool			x_break,z_break;
	float			*vertex_ptr,*vl,*uv,*cl;
	
	y=liq->y;
	fy=(float)(y-view.camera.pnt.y);

		// get total vertex count

	v_sz=liquid_render_liquid_get_max_vertex(liq);

		// get next VBO to use

	view_next_vertex_object();

		// map VBO to memory

	view_bind_current_vertex_object();

	sz=(v_sz*(3+2+3))*sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);

	vl=vertex_ptr;
	uv=vertex_ptr+(v_sz*3);
	cl=vertex_ptr+(v_sz*(3+2));

		// reduce lighting to liquid space

	map_calculate_light_reduce_liquid(liq);

		// setup tiding

	tide_high=liq->tide.high;
	tide_rate=liq->tide.rate;

	if ((tide_high<=0) || (tide_rate<=0)) {

	}
	else {
		tide_split_half=liq->tide.split<<2;
		f_tide_split_half=(float)tide_split_half;
		
		f_tide_high=(float)tide_high;

		f_time=(float)(tick%tide_rate);		// get rate between 0..1
		f_time=f_time/(float)tide_rate;
	}

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

				// color

			if (setup.ray_trace_lighting) {
				map_calculate_ray_trace_light_color_normal((double)x,(double)y,(double)z,cl,normal);
			}
			else {
				map_calculate_light_color_normal((double)x,(double)y,(double)z,cl,normal);
			}
			cl+=3;

				// setup tide Y

			if (liq->tide.direction==ld_vertical) {
				f_break=(float)(z%tide_split_half);
			}
			else {
				f_break=(float)(x%tide_split_half);
			}
				
			f_break=f_break/f_tide_split_half;
		   
			sn=(float)sin((TRIG_PI*2.0f)*(f_break+f_time));

				// vertex and uvs

			*vl++=(float)(x-view.camera.pnt.x);
			*vl++=fy-(f_tide_high*sn);
			*vl++=(float)(view.camera.pnt.z-z);

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

		// unmap VBO

	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

	view_unbind_current_vertex_object();
}

int liquid_render_liquid_create_quads(map_liquid_type *liq)
{
	int				x,z,x_sz,z_sz,v_sz,quad_cnt,
					tz,bz,tz_add,top_row,bot_row,
					lx,rx,lx_add,tide_split;
	int				*d_idx;
		
		// drawing the quads only draws to the edges

	x_sz=liq->draw.x_sz-1;
	if (x_sz<=0) return(0);

	z_sz=liq->draw.z_sz-1;
	if (z_sz<=0) return(0);

	tide_split=liq->tide.split;

		// create the draw indexes

	v_sz=liquid_render_liquid_get_max_vertex(liq);

	quad_cnt=0;
	d_idx=map.liquid_vertexes.index_ptr;
		
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

void liquid_render_liquid_start_array(map_liquid_type *liq)
{
	int				v_sz;

		// use last compiled buffer

	view_bind_current_vertex_object();

		// vertexes

	v_sz=liquid_render_liquid_get_max_vertex(liq);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

		// uv array
		
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((v_sz*3)*sizeof(float)));
	
		// color array
		
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)((v_sz*(3+2))*sizeof(float)));
}

void liquid_render_liquid_end_array(void)
{
	glDisableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_current_vertex_object();
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render_liquid(int tick,map_liquid_type *liq)
{
	int						quad_cnt,frame;
	float					col[3],normal[3];
	d3pnt					mid;
	texture_type			*texture;

		// liquid in view?

	if (!boundbox_inview(liq->lft,liq->top,liq->rgt,liq->bot,liq->y,liq->y)) return;

		// setup liquid for drawing

	liquid_render_liquid_create_vertex(tick,liq);
	quad_cnt=liquid_render_liquid_create_quads(liq);
	if (quad_cnt==0) return;
	
	liquid_render_liquid_start_array(liq);

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

		map_calculate_light_reduce_liquid(liq);
		map_calculate_light_color_normal((double)mid.x,(double)mid.y,(double)mid.z,col,normal);

		glNormal3f(normal[0],normal[1],normal[2]);
	}
	else {
		gl_texture_transparent_start();
		gl_texture_transparent_set(texture->bitmaps[frame].gl_id,liq->alpha);
	}

		// draw the quads

	glDrawElements(GL_QUADS,(quad_cnt*4),GL_UNSIGNED_INT,(GLvoid*)map.liquid_vertexes.index_ptr);

		// end texture

	if (texture->shader.on) {
		gl_texture_shader_end();
		gl_shader_program_end();
	}
	else {
		gl_texture_transparent_end();
	}

	liquid_render_liquid_end_array();
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render(int tick)
{
	int					n;
	map_liquid_type		*liq;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
	
		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
		// draw liquids
		
		// supergumba -- need to sort, obscure, etc
		
	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {
		liquid_render_liquid(tick,liq);
		liq++;
	}
}

