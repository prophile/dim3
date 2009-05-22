/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Opaque Segment Rendering

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#include "lights.h"
#include "effects.h"
#include "consoles.h"
#include "video.h"

int						render_current_multitexture_mode;

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern int game_time_get(void);
extern bool fog_solid_on(void);
extern void view_compile_gl_list_attach(void);
extern void view_compile_gl_list_uv_layer_attach(int uv_idx);
extern void view_compile_gl_list_enable_color(void);
extern void view_compile_gl_list_disable_color(void);
extern void view_compile_gl_list_dettach(void);
		
/* =======================================================

      MultiTexture Blending
      
======================================================= */

void render_opaque_mesh_multitexture_blend(int multitexture_mode,int uv_idx)
{
	int			mode;
	
		// find new mode
		
	if (uv_idx==0) {
		mode=-1;
	}
	else {
		mode=multitexture_mode;
	}
	
		// need a switch?
		
	if (mode==render_current_multitexture_mode) return;
	
		// change mode
	
	switch (mode) {
	
		case -1:
			glDisable(GL_BLEND);
			break;
			
		case multitexture_mode_replace:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR,GL_ZERO);
			break;
			
		case multitexture_mode_multiply:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR,GL_ZERO);
			break;
			
	}
	
	render_current_multitexture_mode=mode;
}

void render_opaque_mesh_multitexture_blend_start()
{
	glDisable(GL_BLEND);
	render_current_multitexture_mode=-1;
}

/* =======================================================

      Opaque Map Shaders
      
======================================================= */

void render_opaque_mesh_simple(int uv_idx)
{
	int							n,k;
	bool						enable;
	GLuint						gl_id;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	texture_type				*texture;
	
		// setup drawing

	enable=FALSE;

	gl_texture_opaque_start();
	
		// run through the meshes

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		
			// skip meshes without proper uv layer
			
		if (uv_idx>=mesh->nuv) continue;

			// skip meshes with no opaques and all non-shaders
			// unless debug is on

		if ((!mesh->render.has_opaque) || ((!dim3_debug) && (!mesh->render.has_no_shader))) continue;
		
			// run through the polys
			
		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent or shader polys

			if ((poly->render.transparent_on) || ((!dim3_debug) && (poly->render.shader_on))) {
				poly++;
				continue;
			}

				// time to enable color array?

			if (!enable) {
				enable=TRUE;
				view_compile_gl_list_enable_color();
			}

				// get texture

			texture=&map.textures[poly->uv[uv_idx].txt_idx];

			if (!gl_back_render_get_texture(poly->camera,&gl_id)) {
				gl_id=texture->frames[poly->render.frame].bitmap.gl_id;
			}

			gl_texture_opaque_set(gl_id);

				// draw polygon

			render_opaque_mesh_multitexture_blend(texture->multitexture_mode,uv_idx);

			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);

			poly++;
		}
	}

		// was color array enabled?

	if (enable) view_compile_gl_list_disable_color();

		// end drawing

	gl_texture_opaque_end();
}

void render_opaque_mesh_shader(int uv_idx)
{
	int							n,k;
	GLuint						gl_id;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	texture_type				*texture;
	view_glsl_light_list_type	light_list;

		// setup drawing

	gl_shader_draw_start();
	
		// run through the meshes

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];

			// skip meshes without proper uv layer
			
		if (uv_idx>=mesh->nuv) continue;

			// skip meshes with no shaders or opaques

		if ((!mesh->render.has_opaque) || (!mesh->render.has_shader)) continue;

			// run through the polys
			
		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent or non-shader polys

			if ((poly->render.transparent_on) || (!poly->render.shader_on)) {
				poly++;
				continue;
			}

				// build lights

			gl_lights_build_from_poly(poly,&light_list);
			
				// setup shader

			texture=&map.textures[poly->uv[uv_idx].txt_idx];

			if (!mesh->flag.hilite) {
				gl_shader_draw_execute(texture,poly->uv[uv_idx].txt_idx,poly->render.frame,poly->dark_factor,1.0f,&light_list);
			}
			else {
				gl_shader_draw_hilite_execute(texture,poly->uv[uv_idx].txt_idx,poly->render.frame,poly->dark_factor,1.0f,&poly->box.mid,NULL);
			}

				// fix texture if any back rendering

			if (gl_back_render_get_texture(poly->camera,&gl_id)) {
				gl_shader_texture_override(gl_id);
			}

				// draw polygon

			render_opaque_mesh_multitexture_blend(texture->multitexture_mode,uv_idx);

			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
			
			poly++;
		}
	}

		// end drawing

	gl_shader_draw_end();
}

void render_opaque_mesh_glow(void)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;
	
		// setup drawing

	gl_texture_glow_start();
	
		// run through the meshes

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];

			// skip meshes with no glows or opaques

		if ((!mesh->render.has_opaque) || (!mesh->render.has_glow)) continue;

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent or non-glow polys

			if ((poly->render.transparent_on) || (!poly->render.glow_on)) {
				poly++;
				continue;
			}

				// get texture

			texture=&map.textures[poly->uv[0].txt_idx];

				// draw glow

			gl_texture_glow_set(texture->frames[poly->render.frame].bitmap.gl_id,texture->frames[poly->render.frame].glowmap.gl_id,texture->glow.current_color);
			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);

			poly++;
		}
	}

		// end drawing

	gl_texture_glow_end();
}

/* =======================================================

      Opaque Map Rendering
      
======================================================= */

void render_map_mesh_opaque(void)
{
	int					uv_idx;
	
		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// attach map complied open gl list

	view_compile_gl_list_attach();
	
		// common setup
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
		
		// render polygons

	glDepthMask(GL_TRUE);

	render_opaque_mesh_multitexture_blend_start();
	
	for (uv_idx=0;uv_idx!=max_mesh_poly_uv_layer;uv_idx++) {
		view_compile_gl_list_uv_layer_attach(uv_idx);
		render_opaque_mesh_simple(uv_idx);
		if (!dim3_debug) render_opaque_mesh_shader(uv_idx);
	}
	
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);

	render_opaque_mesh_glow();

	glDepthMask(GL_TRUE);

		// dettach any attached lists

	view_compile_gl_list_dettach();
}

