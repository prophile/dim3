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

/* =======================================================

      Opaque Stencil Pass Pieces Drawing
      
======================================================= */

void segment_render_opaque_stencil_portal_normal_mesh(portal_type *portal,int stencil_pass)
{
	int					n,k,frame;
	unsigned long		txt_id;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

	gl_texture_opaque_start();

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	
	txt_id=-1;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if ((!mesh->draw.has_stencil_normal) || (mesh->draw.stencil_pass_start>stencil_pass) || (mesh->draw.stencil_pass_end<stencil_pass)) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			if ((mesh_poly->draw.draw_type!=map_mesh_poly_draw_normal) || (mesh_poly->draw.stencil_pass!=stencil_pass)) {
				mesh_poly++;
				continue;
			}

			texture=&map.textures[mesh_poly->txt_idx];
			frame=mesh_poly->draw.cur_frame;

			if (texture->bitmaps[frame].gl_id!=txt_id) {
				txt_id=texture->bitmaps[frame].gl_id;
				gl_texture_opaque_set(txt_id);
			}
			
			glStencilFunc(GL_ALWAYS,mesh_poly->draw.stencil_idx,0xFF);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
		mesh++;
	}

	glDisable(GL_STENCIL_TEST);

	gl_texture_opaque_end();
}

void segment_render_opaque_stencil_portal_bump_mesh(portal_type *portal,int stencil_pass)
{
	int					n,k,frame;
	unsigned long		txt_id,bump_id;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

	gl_texture_opaque_bump_start();

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

	txt_id=bump_id=-1;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if ((!mesh->draw.has_stencil_bump) || (mesh->draw.stencil_pass_start>stencil_pass) || (mesh->draw.stencil_pass_end<stencil_pass)) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			if ((mesh_poly->draw.draw_type!=map_mesh_poly_draw_bump) || (mesh_poly->draw.stencil_pass!=stencil_pass)) {
				mesh_poly++;
				continue;
			}

			texture=&map.textures[mesh_poly->txt_idx];
			frame=mesh_poly->draw.cur_frame;

			if ((texture->bitmaps[frame].gl_id!=txt_id) || (texture->bumpmaps[frame].gl_id!=bump_id)) {
				txt_id=texture->bitmaps[frame].gl_id;
				bump_id=texture->bumpmaps[frame].gl_id;
				gl_texture_opaque_bump_set(txt_id,bump_id);
			}
		
			gl_texture_opaque_bump_factor(mesh_poly->draw.normal);
			
			glStencilFunc(GL_ALWAYS,mesh_poly->draw.stencil_idx,0xFF);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
		mesh++;
	}

	glDisable(GL_STENCIL_TEST);

	gl_texture_opaque_bump_end();
}

void segment_render_opaque_stencil_portal_lighting_mesh(portal_type *portal,int stencil_pass)
{
	int					n,k,ntrig;
	float				dark_factor;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;

	gl_texture_tesseled_lighting_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO,GL_SRC_COLOR);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
			
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

	dark_factor=1.0f;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if ((!mesh->draw.has_stencil_lighting) || (mesh->draw.stencil_pass_start>stencil_pass) || (mesh->draw.stencil_pass_end<stencil_pass)) {
			mesh++;
			continue;
		}
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			if ((!mesh_poly->draw.is_stencil_lighting) || (mesh_poly->draw.stencil_pass!=stencil_pass)) {
				mesh_poly++;
				continue;
			}

			glStencilFunc(GL_EQUAL,mesh_poly->draw.stencil_idx,0xFF);

			if (dark_factor!=mesh_poly->dark_factor) {
				dark_factor=mesh_poly->dark_factor;
				gl_texture_tesseled_lighting_factor(dark_factor);
			}

			// supergumba -- testing
			/*
			if (!mesh_poly->draw.simple_lighting) {
				glLineWidth(2.0f);
				ntrig=mesh_poly->light.trig_count;
				for (t=0;t!=ntrig;t++) {
					glDrawElements(GL_LINE_LOOP,3,GL_UNSIGNED_INT,(GLvoid*)&mesh_poly->light.trig_vertex_idx[t*3]);
				}
				glLineWidth(1.0f);
			}
			*/

			if (mesh_poly->draw.is_simple_lighting) {
				glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
			}
			else {
				ntrig=mesh_poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)mesh_poly->light.trig_vertex_idx);
				if ((mesh_poly->ptsz-2)!=ntrig) glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
			}

			mesh_poly++;
		}
	
		mesh++;
	}

	glDisable(GL_STENCIL_TEST);

	gl_texture_tesseled_lighting_end();
}

/* =======================================================

      Opaque Non-Stencil Pass Pieces Drawing
      
======================================================= */

void segment_render_opaque_simple_portal_normal_mesh(portal_type *portal)
{
	int					n,k,frame;
	unsigned long		txt_id;
	float				dark_factor;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

	gl_texture_opaque_lighting_start();

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
	txt_id=-1;
	dark_factor=1.0f;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if (!mesh->draw.has_simple_normal) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			if (mesh_poly->draw.draw_type!=map_mesh_poly_draw_normal) {
				mesh_poly++;
				continue;
			}

			texture=&map.textures[mesh_poly->txt_idx];
			frame=mesh_poly->draw.cur_frame;

			if (texture->bitmaps[frame].gl_id!=txt_id) {
				txt_id=texture->bitmaps[frame].gl_id;
				gl_texture_opaque_lighting_set(txt_id);
			}

			if (dark_factor!=mesh_poly->dark_factor) {
				dark_factor=mesh_poly->dark_factor;
				gl_texture_opaque_lighting_factor(dark_factor);
			}
			
			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
		mesh++;
	}

	gl_texture_opaque_lighting_end();
}

void segment_render_opaque_simple_portal_bump_mesh(portal_type *portal)
{
	int					n,k,frame;
	unsigned long		txt_id,bump_id;
	float				dark_factor;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

	gl_texture_opaque_bump_lighting_start();

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	txt_id=bump_id=-1;
	dark_factor=1.0f;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if (!mesh->draw.has_simple_bump) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			if (mesh_poly->draw.draw_type!=map_mesh_poly_draw_bump) {
				mesh_poly++;
				continue;
			}

			texture=&map.textures[mesh_poly->txt_idx];
			frame=mesh_poly->draw.cur_frame;

			if ((texture->bitmaps[frame].gl_id!=txt_id) || (texture->bumpmaps[frame].gl_id!=bump_id)) {
				txt_id=texture->bitmaps[frame].gl_id;
				bump_id=texture->bumpmaps[frame].gl_id;
				gl_texture_opaque_bump_lighting_set(txt_id,bump_id);
			}
		
			gl_texture_opaque_bump_lighting_factor(mesh_poly->draw.normal);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
		mesh++;
	}

	gl_texture_opaque_bump_lighting_end();
}

/* =======================================================

      Opaque Effect Pass Pieces Drawing
      
======================================================= */

void segment_render_opaque_portal_specular_mesh(portal_type *portal)
{
	int					n,k,frame;
	unsigned long		specular_id;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

	gl_texture_opaque_specular_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	
	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_EQUAL);
	glDepthMask(GL_FALSE);

  	specular_id=-1;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if (!mesh->draw.has_specular) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			if (!mesh_poly->draw.is_specular) {
				mesh_poly++;
				continue;
			}

			texture=&map.textures[mesh_poly->txt_idx];
			frame=mesh_poly->draw.cur_frame;

			if (specular_id!=texture->specularmaps[frame].gl_id) {
				specular_id=texture->specularmaps[frame].gl_id;
				gl_texture_opaque_specular_set(specular_id);
			}
			
			gl_texture_opaque_specular_factor(texture->specular.factor);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
		mesh++;
	}

	gl_texture_opaque_specular_end();
}

void segment_render_opaque_portal_glow_mesh(portal_type *portal)
{
	int					n,k,frame;
	unsigned long		txt_id,glow_id;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

	gl_texture_opaque_glow_start();
	
	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_EQUAL);
	glDepthMask(GL_FALSE);

	txt_id=-1;
	glow_id=-1;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if (!mesh->draw.has_glow) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			if (!mesh_poly->draw.is_glow) {
				mesh_poly++;
				continue;
			}

			texture=&map.textures[mesh_poly->txt_idx];
			frame=mesh_poly->draw.cur_frame;

			if ((txt_id!=texture->bitmaps[frame].gl_id) || (glow_id!=texture->glowmaps[frame].gl_id)) {
				txt_id=texture->bitmaps[frame].gl_id;
				glow_id=texture->glowmaps[frame].gl_id;
				gl_texture_opaque_glow_set(txt_id,glow_id);
			}

			gl_texture_opaque_glow_color(texture->glow.current_color);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
		mesh++;
	}

	gl_texture_opaque_glow_end();
}

void segment_render_opaque_portal_shader_mesh(portal_type *portal)
{
	int					n,k,frame;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
	light_spot_type		*lspot;

	gl_shader_start();
	gl_texture_shader_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
		
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
		
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if (!mesh->draw.has_opaque_shader) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			if (mesh_poly->draw.draw_type!=map_mesh_poly_draw_opaque_shader) {
				mesh_poly++;
				continue;
			}

			texture=&map.textures[mesh_poly->txt_idx];
			frame=mesh_poly->draw.cur_frame;

			gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
			gl_shader_set_program(texture->shader.program_obj);
			
			lspot=map_portal_find_closest_light(portal,(double)mesh_poly->box.mid.x,(double)mesh_poly->box.mid.y,(double)mesh_poly->box.mid.z,NULL);
			gl_shader_set_variables(texture->shader.program_obj,&mesh_poly->box.mid,texture,lspot);
			
			glNormal3f(mesh_poly->draw.normal[0],mesh_poly->draw.normal[1],mesh_poly->draw.normal[2]);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
		
			mesh_poly++;
		}
	
		mesh++;
	}

	gl_texture_shader_end();
	gl_shader_end();
}

/* =======================================================

      Opaque Segment Drawing
      
======================================================= */

int segment_render_opaque_portal(int rn,int pass_last)
{
	int							stencil_pass,
								pass_start,pass_end;
	portal_type					*portal;

	portal=&map.portals[rn];

		// attach compiled vertex lists

	if ((portal->mesh.draw.has_glow) || (portal->mesh.draw.has_specular) || (portal->mesh.draw.has_opaque_shader)) {
		portal_compile_gl_list_attach(rn,3);
	}
	else {
		portal_compile_gl_list_attach(rn,2);
	}

		// meshes that use stencil-based lighting

		// need to potentially run multiple passes
		// so 8-bit stencil buffer can be used for more
		// than 256 polygons in a portal.
	
		// The stencil buffer is used to mask the
		// tesseled lighting passes

	pass_start=portal->opaque_stencil_pass_start;
	pass_end=portal->opaque_stencil_pass_end;

	for (stencil_pass=pass_start;stencil_pass<=pass_end;stencil_pass++) {
	
			// clear stencil buffer when passes change

		if (stencil_pass!=pass_last) {
			glClear(GL_STENCIL_BUFFER_BIT);
			pass_last=stencil_pass;
		}

			// normal, bump, and tesseled lighting

		if (portal->mesh.draw.has_stencil_normal) segment_render_opaque_stencil_portal_normal_mesh(portal,stencil_pass);
		if (portal->mesh.draw.has_stencil_bump) segment_render_opaque_stencil_portal_bump_mesh(portal,stencil_pass);
		if ((!hilite_on) && (portal->mesh.draw.has_stencil_lighting)) segment_render_opaque_stencil_portal_lighting_mesh(portal,stencil_pass);
	}

		// meshes that do not use stencil based lighting

	if (portal->mesh.draw.has_simple_normal) segment_render_opaque_simple_portal_normal_mesh(portal);
	if (portal->mesh.draw.has_simple_bump) segment_render_opaque_simple_portal_bump_mesh(portal);

		// specular, glows, and shaders

	if (portal->mesh.draw.has_specular) segment_render_opaque_portal_specular_mesh(portal);
	if (portal->mesh.draw.has_glow) segment_render_opaque_portal_glow_mesh(portal);
	if (portal->mesh.draw.has_opaque_shader) segment_render_opaque_portal_shader_mesh(portal);

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
		
		// remember the last stencil pass set so we
		// only clear the stencil when necessary

	pass_last=0;
		
	for (i=(portal_cnt-1);i>=0;i--) {
		pass_last=segment_render_opaque_portal(portal_list[i],pass_last);
	}

		// dettach any attached lists

	portal_compile_gl_list_dettach();
}
