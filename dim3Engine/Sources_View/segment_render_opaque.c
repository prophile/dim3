/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Opaque Segment Rendering

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

#include "effects.h"
#include "consoles.h"
#include "video.h"

extern bool				hilite_on;

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern int game_time_get(void);

extern void portal_compile_gl_list_attach(int rn,int txt_unit_count);
extern void portal_compile_gl_list_dettach(void);

extern void segment_liquid_tide_draw(segment_type *seg);
extern void segment_liquid_tide_draw_bump(segment_type *seg);
extern void segment_liquid_tide_draw_lighting(segment_type *seg);

/* =======================================================

      Opaque Segments

======================================================= */

void segment_piece_draw_opaque_normal(int stencil_pass,int cnt,short *sptr)
{
	int					i,frame;
	unsigned long		txt_id;
	segment_type		*seg;
	texture_type		*texture;
	
	txt_id=-1;
	
	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];
		if (seg->render.stencil_pass!=stencil_pass) continue;
		
		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;

		glStencilFunc(GL_ALWAYS,seg->render.stencil_idx,0xFF);
		
		if (texture->bitmaps[frame].gl_id!=txt_id) {
			txt_id=texture->bitmaps[frame].gl_id;
			gl_texture_opaque_set(txt_id);
		}

		if (seg->type!=sg_liquid) {
			glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
		}
		else {
			segment_liquid_tide_draw(seg);
		}
	}
}

void segment_piece_draw_opaque_bump(int stencil_pass,int cnt,short *sptr)
{
	int					i,frame;
	unsigned long		txt_id,bump_id;
	segment_type		*seg;
	texture_type		*texture;
	
	txt_id=bump_id=-1;
	
	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];
		if (seg->render.stencil_pass!=stencil_pass) continue;

		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;

		glStencilFunc(GL_ALWAYS,seg->render.stencil_idx,0xFF);
		
		if ((texture->bitmaps[frame].gl_id!=txt_id) || (texture->bumpmaps[frame].gl_id!=bump_id)) {
			txt_id=texture->bitmaps[frame].gl_id;
			bump_id=texture->bumpmaps[frame].gl_id;
			gl_texture_opaque_bump_set(txt_id,bump_id);
		}
		
		gl_texture_opaque_bump_factor(seg->render.normal);

		if (seg->type!=sg_liquid) {
			glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
		}
		else {
			segment_liquid_tide_draw_bump(seg);
		}
	}
}

void segment_piece_draw_opaque_tesseled_lighting(int stencil_pass,int cnt,short *sptr)
{
	int					i,frame,ntrig;
	float				dark_factor;
	segment_type		*seg;
	texture_type		*texture;
	
	dark_factor=-1.0f;
	
	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];
		if (seg->render.stencil_pass!=stencil_pass) continue;
		
		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;
		
		glStencilFunc(GL_EQUAL,seg->render.stencil_idx,0xFF);
		
		if (dark_factor!=seg->dark_factor) {
			dark_factor=seg->dark_factor;
			gl_texture_tesseled_lighting_factor(dark_factor);
		}

		if (seg->type!=sg_liquid) {
		
				// tesseleated triangles lighting, if lighting
				// triangles = draw triangles, then just draw triangles,
				// otherwise finish with a total fill to catch the
				// math (T section) errors
				
			if (!seg->render.light_simple) {
				ntrig=seg->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)seg->light.trig_vertex_idx);
				if ((seg->draw.ptsz-2)!=ntrig) glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
			}
			
				// if simple render lighting, just draw the polygon
				
			else {
				glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
			}

		}
		else {
			segment_liquid_tide_draw_lighting(seg);
		}
	}
}

void segment_piece_draw_opaque_simple_normal(int cnt,short *sptr)
{
	int					i,frame;
	unsigned long		txt_id;
	float				dark_factor;
	segment_type		*seg;
	texture_type		*texture;
	
	txt_id=-1;
	dark_factor=-1.0f;
	
	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];
		
		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;
		
		if (texture->bitmaps[frame].gl_id!=txt_id) {
			txt_id=texture->bitmaps[frame].gl_id;
			gl_texture_opaque_lighting_set(txt_id);
		}
		if (seg->dark_factor!=dark_factor) {
			dark_factor=seg->dark_factor;
			gl_texture_opaque_lighting_factor(dark_factor);
		}

		if (seg->type!=sg_liquid) {
			glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
		}
		else {
			segment_liquid_tide_draw(seg);
		}
	}
}

void segment_piece_draw_opaque_simple_bump(int cnt,short *sptr)
{
	int					i,frame;
	unsigned long		txt_id,bump_id;
	segment_type		*seg;
	texture_type		*texture;
	
	txt_id=bump_id=-1;
	
	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];
		
		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;
		
		if ((texture->bitmaps[frame].gl_id!=txt_id) || (texture->bumpmaps[frame].gl_id!=bump_id)) {
			txt_id=texture->bitmaps[frame].gl_id;
			bump_id=texture->bumpmaps[frame].gl_id;
			gl_texture_opaque_bump_lighting_set(txt_id,bump_id);
		}
		
		gl_texture_opaque_bump_lighting_factor(seg->render.normal);
		
		if (seg->type!=sg_liquid) {
			glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
		}
		else {
			segment_liquid_tide_draw_bump(seg);
		}
	}
}

void segment_piece_draw_opaque_specular(int cnt,short *sptr)
{
	int					i,frame;
	unsigned long		specular_id;
	segment_type		*seg;
	texture_type		*texture;
	
	specular_id=-1;

	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];
		
		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;
			
		if (specular_id!=texture->specularmaps[frame].gl_id) {
			specular_id=texture->specularmaps[frame].gl_id;
			gl_texture_opaque_specular_set(specular_id);
		}
		
		gl_texture_opaque_specular_factor(texture->specular.factor);

		if (seg->type!=sg_liquid) {
			glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
		}
		else {
			segment_liquid_tide_draw(seg);
		}
	}
}

void segment_piece_draw_opaque_glow(int cnt,short *sptr)
{
	int					i,frame;
	unsigned long		txt_id,glow_id;
	segment_type		*seg;
	texture_type		*texture;

	txt_id=-1;
	glow_id=-1;
	
	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];
		
		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;
			
		if ((txt_id!=texture->bitmaps[frame].gl_id) || (glow_id!=texture->glowmaps[frame].gl_id)) {
			txt_id=texture->bitmaps[frame].gl_id;
			glow_id=texture->glowmaps[frame].gl_id;
			gl_texture_opaque_glow_set(txt_id,glow_id);
		}

		gl_texture_opaque_glow_color(texture->glow.current_color);

		if (seg->type!=sg_liquid) {
			glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
		}
		else {
			segment_liquid_tide_draw(seg);
		}
	}
}

/* =======================================================

      Shader Segments

======================================================= */

void segment_piece_draw_shader(int cnt,short *sptr)
{
	int					i,frame;
	segment_type		*seg;
	texture_type		*texture;
	light_spot_type		*lspot;
	
	for (i=0;i!=cnt;i++) {
		seg=&map.segments[(int)*sptr++];

		texture=&map.textures[seg->fill];
		frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;
			
		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
		
		lspot=map_portal_find_closest_light(&map.portals[seg->rn],(double)seg->middle.x,(double)seg->middle.y,(double)seg->middle.z,NULL);
		gl_shader_set_variables(texture->shader.program_obj,&seg->middle,&map.textures[seg->fill],lspot);
		
		glNormal3f(seg->render.normal[0],seg->render.normal[1],seg->render.normal[2]);
		
		if (seg->type!=sg_liquid) {
			glDrawElements(GL_POLYGON,seg->draw.ptsz,GL_UNSIGNED_INT,(GLvoid*)seg->draw.idx);
		}
		else {
			segment_liquid_tide_draw(seg);
		}
	}
}

/* =======================================================

      Opaque Segment Drawing
      
======================================================= */

int segment_render_opaque_portal(int rn,int pass_last)
{
	int							n,k,stencil_pass,
								pass_start,pass_end;
	portal_type					*portal;
	portal_segment_draw_type	*draw;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*mesh_poly;
	texture_type	*texture;
	int		frame;
	unsigned long	txt_id;
	float	dark_factor;

	portal=&map.portals[rn];
	draw=&portal->segment_draw;

		// attach compiled vertex lists

	if ((draw->opaque_specular_count!=0) || (draw->opaque_glow_count!=0)) {
		portal_compile_gl_list_attach(rn,3);
	}
	else {
		portal_compile_gl_list_attach(rn,2);
	}


// supergumba -- another hack to get meshes running
// will need to support all the stuff below

	gl_texture_opaque_lighting_start();

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		txt_id=-1;
		dark_factor=1.0f;
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			texture=&map.textures[mesh_poly->txt_idx];
			frame=(texture->animate.current_frame+mesh_poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if (texture->bitmaps[frame].gl_id!=txt_id) {
				txt_id=texture->bitmaps[frame].gl_id;
				gl_texture_opaque_lighting_set(txt_id);
			}
			if (mesh_poly->dark_factor!=dark_factor) {
				dark_factor=mesh_poly->dark_factor;
				gl_texture_opaque_lighting_factor(dark_factor);
			}

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
	
		mesh++;
	}

	gl_texture_opaque_lighting_end();

	return(pass_last);


		// need to potentially run multiple passes
		// so 8-bit stencil buffer can be used for more
		// than 256 segments in a portal

	pass_start=portal->opaque_stencil_pass_start;
	pass_end=portal->opaque_stencil_pass_end;

	for (stencil_pass=pass_start;stencil_pass<=pass_end;stencil_pass++) {
		
			// clear buffer when passes change

		if (stencil_pass!=pass_last) {
			glClear(GL_STENCIL_BUFFER_BIT);
			pass_last=stencil_pass;
		}
		
			// normal textures

		if (draw->opaque_normal_count!=0) {

			gl_texture_opaque_start();

			glDisable(GL_BLEND);
			
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);

			glEnable(GL_STENCIL_TEST);					// stencil for lighting pass
			glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

			segment_piece_draw_opaque_normal(stencil_pass,draw->opaque_normal_count,draw->opaque_normal_list);

			glDisable(GL_STENCIL_TEST);

			gl_texture_opaque_end();
		}
	
			// bump mapped textures

		if (draw->opaque_bump_count!=0) {
		
			gl_texture_opaque_bump_start();

			glDisable(GL_BLEND);
			
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);

			glEnable(GL_STENCIL_TEST);					// stencil for lighting pass
			glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

			segment_piece_draw_opaque_bump(stencil_pass,draw->opaque_bump_count,draw->opaque_bump_list);

			glDisable(GL_STENCIL_TEST);

			gl_texture_opaque_bump_end();
		}

			// opaque tesseled lighting

		if ((draw->opaque_light_count!=0) && (!hilite_on)) {

			gl_texture_tesseled_lighting_start();

			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO,GL_SRC_COLOR);

			glDisable(GL_ALPHA_TEST);
			glDisable(GL_DEPTH_TEST);
					
			glEnable(GL_STENCIL_TEST);				// use stencil for lighting pass
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
			
			segment_piece_draw_opaque_tesseled_lighting(stencil_pass,draw->opaque_light_count,draw->opaque_light_list);

			glDisable(GL_STENCIL_TEST);

			gl_texture_tesseled_lighting_end();

		}
	}
	
		// simple normal textures
		// these are textures that are drawn pre-lighted and not with tesseled lighting

	if (draw->opaque_simple_normal_count!=0) {
		gl_texture_opaque_lighting_start();

		glDisable(GL_BLEND);
		
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);

		segment_piece_draw_opaque_simple_normal(draw->opaque_simple_normal_count,draw->opaque_simple_normal_list);

		gl_texture_opaque_lighting_end();
	}

		// specular mapped textures
		
	if (draw->opaque_specular_count!=0) {

		gl_texture_opaque_specular_start();

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);
		
		glDisable(GL_ALPHA_TEST);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);
		glDepthMask(GL_FALSE);
					
		segment_piece_draw_opaque_specular(draw->opaque_specular_count,draw->opaque_specular_list);

		gl_texture_opaque_specular_end();
	}
	
		// glow mapped textures
		
	if (draw->opaque_glow_count!=0) {

		gl_texture_opaque_glow_start();
		
		glDisable(GL_BLEND);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);
		glDepthMask(GL_FALSE);
					
		segment_piece_draw_opaque_glow(draw->opaque_glow_count,draw->opaque_glow_list);

		gl_texture_opaque_glow_end();
	}

		// shaders

	if (draw->shader_count!=0) {
		gl_shader_start();
		gl_texture_shader_start();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		
		segment_piece_draw_shader(draw->shader_count,draw->shader_list);
	
		gl_texture_shader_end();
		gl_shader_end();
	}

	return(pass_last);
}

void segment_render_opaque(int portal_cnt,int *portal_list)
{
	int							i,pass_last;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
		
		// run through portals
		// we want to go from closest to furthest to
		// catch as much z-buffer eliminates as possible

	pass_last=0;
		
	for (i=(portal_cnt-1);i>=0;i--) {
		pass_last=segment_render_opaque_portal(portal_list[i],pass_last);
	}

		// dettach any attached lists

	portal_compile_gl_list_dettach();
}
