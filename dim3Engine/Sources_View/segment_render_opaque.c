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
extern bool fog_solid_on(void);
extern void portal_compile_gl_list_attach(int rn);
extern void portal_compile_gl_list_dettach(void);

/* =======================================================

      Opaque Map Normal Textures
      
======================================================= */

void render_opaque_portal_normal(int mesh_cnt,int *mesh_list,int stencil_pass)
{
	int					n,k,frame;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// setup drawing

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

	gl_texture_opaque_start();
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// in stencil pass?

			if (poly->draw.stencil_pass!=stencil_pass) {
				poly++;
				continue;
			}

				// get texture

			texture=&map.textures[poly->txt_idx];
			if (texture->shader.on) {
				poly++;
				continue;
			}
			
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// setup texture

			gl_texture_opaque_set(texture->bitmaps[frame].gl_id);
			glStencilFunc(GL_ALWAYS,poly->draw.stencil_idx,0xFF);

				// draw polygon

			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			
			poly++;
		}
	}

		// end drawing

	gl_texture_opaque_end();
}

/* =======================================================

      Opaque Map Bumping
      
======================================================= */

void render_opaque_portal_bump(int mesh_cnt,int *mesh_list,int stencil_pass,bool is_simple_lighting)
{
	int					n,k,frame,ntrig;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// need to use normal map in color array

	glColorPointer(3,GL_FLOAT,0,map.vertexes.pnormal);

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO,GL_SRC_COLOR);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	gl_texture_opaque_tesseled_bump_start();
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

			// if hilite is on or this mesh is
			// hilited, then we need to clear the stencil
			// here as there will be no lighting pass

		if ((mesh->flag.hilite) || (hilite_on)) {
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
		}
		else {
			glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		}

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// in stencil pass?

			if (poly->draw.stencil_pass!=stencil_pass) {
				poly++;
				continue;
			}

				// get texture

			texture=&map.textures[poly->txt_idx];
			if (texture->shader.on) {
				poly++;
				continue;
			}

			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// bump texture set?

			if (texture->bumpmaps[frame].gl_id==-1) {
				poly++;
				continue;
			}
			
				// blend in the bump

			gl_texture_opaque_tesseled_bump_set(texture->bumpmaps[frame].gl_id);
			glStencilFunc(GL_EQUAL,poly->draw.stencil_idx,0xFF);

			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			}

			poly++;
		}
	}

		// end drawing

	gl_texture_opaque_tesseled_bump_end();

		// restore original color array

	glColorPointer(3,GL_FLOAT,0,map.vertexes.pcolor);
}

/* =======================================================

      Opaque Map Lighting
      
======================================================= */

void render_opaque_portal_lighting(int mesh_cnt,int *mesh_list,int stencil_pass,bool is_simple_lighting)
{
	int					n,k,frame,ntrig;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO,GL_SRC_COLOR);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	gl_texture_tesseled_lighting_start();

		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

			// skip hilited polygons

		if (mesh->flag.hilite) continue;

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// in stencil pass?

			if (poly->draw.stencil_pass!=stencil_pass) {
				poly++;
				continue;
			}

				// get texture

			texture=&map.textures[poly->txt_idx];
			if (texture->shader.on) {
				poly++;
				continue;
			}

			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;
			
				// if no specular pass, then clear stencil here
				
			if (!((setup.specular_mapping) && (texture->specularmaps[frame].gl_id!=-1))) {
				glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
			}
			else {
				glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
			}

				// draw lighting

			gl_texture_tesseled_lighting_set(-1,poly->dark_factor);
			glStencilFunc(GL_EQUAL,poly->draw.stencil_idx,0xFF);

			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			}

			poly++;
		}
	}

		// end drawing

	gl_texture_tesseled_lighting_end();
}

void render_opaque_portal_lighting_fix(int mesh_cnt,int *mesh_list,int stencil_pass)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// this draws over the entire polygon
		// to catch any pixels that show up between the
		// tesselated triangles

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO,GL_SRC_COLOR);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

	gl_texture_tesseled_lighting_start();

		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

			// skip hilited polygons

		if (mesh->flag.hilite) continue;

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// in stencil pass?

			if (poly->draw.stencil_pass!=stencil_pass) {
				poly++;
				continue;
			}

				// draw lighting

			gl_texture_tesseled_lighting_set(-1,poly->dark_factor);
			glStencilFunc(GL_EQUAL,poly->draw.stencil_idx,0xFF);

			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

			poly++;
		}
	}

		// end drawing

	gl_texture_tesseled_lighting_end();
}

void render_opaque_portal_specular(int mesh_cnt,int *mesh_list,int stencil_pass,bool is_simple_lighting)
{
	int					n,k,frame,ntrig;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

	gl_texture_tesseled_specular_start();
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

			// skip hilited polygons

		if (mesh->flag.hilite) continue;

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// in stencil pass?

			if (poly->draw.stencil_pass!=stencil_pass) {
				poly++;
				continue;
			}

				// get texture

			texture=&map.textures[poly->txt_idx];
			if (texture->shader.on) {
				poly++;
				continue;
			}

			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// specular texture set?

			if (texture->specularmaps[frame].gl_id==-1) {
				poly++;
				continue;
			}

				// draw specular

			gl_texture_tesseled_specular_set(texture->specularmaps[frame].gl_id);
			glStencilFunc(GL_EQUAL,poly->draw.stencil_idx,0xFF);

			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			}

			poly++;
		}
	}

		// end drawing

	gl_texture_tesseled_specular_end();
}

/* =======================================================

      Opaque Map Glows
      
======================================================= */

void render_opaque_portal_glow(int mesh_cnt,int *mesh_list)
{
	int					n,k,frame;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_EQUAL);
	glDepthMask(GL_FALSE);

	gl_texture_opaque_glow_start();
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

			// skip hilited polygons

		if (mesh->flag.hilite) continue;

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture

			texture=&map.textures[poly->txt_idx];
			if (texture->shader.on) {
				poly++;
				continue;
			}

			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// glow texture set?

			if (texture->glowmaps[frame].gl_id==-1) {
				poly++;
				continue;
			}

				// draw polygon

			gl_texture_opaque_glow_set(texture->bitmaps[frame].gl_id,texture->glowmaps[frame].gl_id,texture->glow.current_color);
			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

			poly++;
		}
	}

		// end drawing

	gl_texture_opaque_glow_end();
}

/* =======================================================

      Opaque Map Shaders
      
======================================================= */

void render_opaque_portal_shader(int mesh_cnt,int *mesh_list)
{

	int					n,k,frame;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	gl_shader_program_start();
	gl_texture_shader_start();
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture

			texture=&map.textures[poly->txt_idx];
			if (!texture->shader.on) {
				poly++;
				continue;
			}

			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// setup shader

			gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
			gl_shader_set_program(texture->shader.program_obj);
				
			gl_shader_set_variables(texture->shader.program_obj,&poly->box.mid,texture);

				// draw polygon

			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

			poly++;
		}
	}

		// end drawing

	gl_texture_shader_end();
	gl_shader_program_end();
}

/* =======================================================

      Opaque Map Stenciling Setup
      
======================================================= */

int render_opaque_mesh_stencil_mark(int mesh_cnt,int *mesh_list)
{
	int					n,k,stencil_pass,stencil_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	stencil_pass=0;
	stencil_idx=stencil_poly_start;

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.portals[mesh_list[n]/1000].mesh.meshes[mesh_list[n]%1000];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

			poly->draw.stencil_pass=stencil_pass;
			poly->draw.stencil_idx=stencil_idx;

			stencil_idx++;
			if (stencil_idx>stencil_poly_end) {
				stencil_idx=stencil_poly_start;
				stencil_pass++;
			}

			poly++;
		}
	}

	return(stencil_pass);
}

/* =======================================================

      Opaque Map Rendering
      
======================================================= */

void render_opaque_map(int mesh_cnt,int *mesh_list)
{
	int					n,stencil_pass,stencil_pass_cnt;
	bool				is_simple_lighting;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

		// texture binding optimization

	gl_texture_bind_start();

		// detect simple lighting

	is_simple_lighting=fog_solid_on();

		// create stencil passes

	stencil_pass_cnt=render_opaque_mesh_stencil_mark(mesh_cnt,mesh_list);

			// attach map complied open gl list

	portal_compile_gl_list_attach(0);
		
		// might need multiple passes to
		// get under stencil resolution limit

	glEnable(GL_STENCIL_TEST);

	for (stencil_pass=0;stencil_pass<=stencil_pass_cnt;stencil_pass++) {

		render_opaque_portal_normal(mesh_cnt,mesh_list,stencil_pass);
		if (setup.bump_mapping) render_opaque_portal_bump(mesh_cnt,mesh_list,stencil_pass,is_simple_lighting);

		if (!hilite_on) {
			render_opaque_portal_lighting(mesh_cnt,mesh_list,stencil_pass,is_simple_lighting);
			if (setup.specular_mapping) render_opaque_portal_specular(mesh_cnt,mesh_list,stencil_pass,is_simple_lighting);
			render_opaque_portal_lighting_fix(mesh_cnt,mesh_list,stencil_pass);
		}

	}

	glDisable(GL_STENCIL_TEST);

		// glow maps and shaders happen outside
		// of stenciling

	render_opaque_portal_glow(mesh_cnt,mesh_list);
	render_opaque_portal_shader(mesh_cnt,mesh_list);

		// dettach any attached lists

	portal_compile_gl_list_dettach();
}

