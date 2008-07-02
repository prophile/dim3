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

      Opaque Segment Drawing
      
======================================================= */



/*
void segment_render_opaque_portal_mesh_poly(portal_type *portal,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int					frame,ntrig;
	bool				is_hilite,is_specular,
						is_simple_lighting,is_poly_simple_lighting;
	texture_type		*texture;

		// get the texture and setup

	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

		// shaders

	if (texture->shader.on) {
		gl_shader_program_start();
		gl_texture_shader_start();

		glDisable(GL_BLEND);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		
		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
			
		gl_shader_set_variables(texture->shader.program_obj,&poly->box.mid,texture);
			
		glNormal3f(poly->draw.normal[0],poly->draw.normal[1],poly->draw.normal[2]);

		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

		gl_texture_shader_end();
		gl_shader_program_end();

		return;
	}

		// non-shader regular drawing

		// if no hilite, then need to turn on
		// stenciling for the lighting and specular
		// passes

	is_hilite=(mesh->flag.hilite) || (hilite_on);


		// detect simple lighting

	is_simple_lighting=(!setup.high_quality_lighting) || (fog_solid_on());
	is_poly_simple_lighting=(is_simple_lighting) || (poly->draw.simple_tessel);

		// draw the texture

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	gl_texture_opaque_start();
	gl_texture_opaque_set(texture->bitmaps[frame].gl_id);

	if (is_poly_simple_lighting) {
		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
	}
	else {
		ntrig=poly->light.trig_count;
		glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
	}

	gl_texture_opaque_end();





	}
//	if (!((setup.bump_mapping) && (texture->bumpmaps[frame].gl_id!=-1))) {
		gl_texture_opaque_bump_start();
		gl_texture_opaque_bump_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,poly->draw.normal,poly->draw.normal_dist_factor);
		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
		gl_texture_opaque_bump_end();
	}


		// lighting

	if (!is_hilite) {
	
			// specular?
			
		is_specular=(setup.specular_mapping) && (texture->specularmaps[frame].gl_id!=-1);

			// tesseled lighting mesh

		gl_texture_tesseled_lighting_start();

		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO,GL_SRC_COLOR);

		glDisable(GL_ALPHA_TEST);
		glDisable(GL_DEPTH_TEST);
		
		if (!is_specular) {
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);			// if no specular, clear the stencil now
		}
		else {
			glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
		}
		
		glStencilFunc(GL_EQUAL,0x55,0xFF);

		gl_texture_tesseled_lighting_set(-1,poly->dark_factor);

		if (is_poly_simple_lighting) {
			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
		}
		else {
			ntrig=poly->light.trig_count;
			glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			if ((poly->ptsz-2)!=ntrig) glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
		}

		gl_texture_tesseled_lighting_end();

			// speculars hilights

		if (is_specular) {

			gl_texture_tesseled_specular_start();

			glBlendFunc(GL_ONE,GL_ONE);
		
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
			glStencilFunc(GL_EQUAL,0x56,0xFF);

			gl_texture_tesseled_specular_set(texture->specularmaps[frame].gl_id);

			if (is_poly_simple_lighting) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
				if ((poly->ptsz-2)!=ntrig) glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
		
			gl_texture_tesseled_specular_end();
		}
	}

		// glow mapping

	if ((setup.glow_mapping) && (texture->glowmaps[frame].gl_id!=-1)) {

		glDisable(GL_STENCIL_TEST);
		
		gl_texture_opaque_glow_start();

		glDisable(GL_BLEND);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);
		glDepthMask(GL_FALSE);

		gl_texture_opaque_glow_set(texture->bitmaps[frame].gl_id,texture->glowmaps[frame].gl_id,texture->glow.current_color);
		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

		gl_texture_opaque_glow_end();
	}
	
}

*/



// supergumba -- should rename all these
// supergumba -- 0x55, 0x56, search out for stencil constant
// supergumba -- the starts/ends of textures will do to much enable/disable work
// supergumba -- erase all stencil idx stuff



/*
void segment_render_opaque_portal_mesh_poly(portal_type *portal,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int					frame,ntrig;
	bool				is_hilite,is_specular,
						is_simple_lighting,is_poly_simple_lighting;
	texture_type		*texture;

		// get the texture and setup

	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

		// shaders

	if (texture->shader.on) {
		gl_shader_program_start();
		gl_texture_shader_start();

		glDisable(GL_BLEND);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		
		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
			
		gl_shader_set_variables(texture->shader.program_obj,&poly->box.mid,texture);
			
		glNormal3f(poly->draw.normal[0],poly->draw.normal[1],poly->draw.normal[2]);

		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

		gl_texture_shader_end();
		gl_shader_program_end();

		return;
	}

		// non-shader regular drawing

		// if no hilite, then need to turn on
		// stenciling for the lighting and specular
		// passes

	is_hilite=(mesh->flag.hilite) || (hilite_on);

	if (!is_hilite) {
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
		glStencilFunc(GL_ALWAYS,0x55,0xFF);
	}
	else {
		glDisable(GL_STENCIL_TEST);
	}

		// detect simple lighting

	is_simple_lighting=(!setup.high_quality_lighting) || (fog_solid_on());

		// draw the texture

	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	if (!((setup.bump_mapping) && (texture->bumpmaps[frame].gl_id!=-1))) {
		gl_texture_opaque_start();
		gl_texture_opaque_set(texture->bitmaps[frame].gl_id);
		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
		gl_texture_opaque_end();
	}
	else {
		gl_texture_opaque_bump_start();
		gl_texture_opaque_bump_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,poly->draw.normal,poly->draw.normal_dist_factor);
		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
		gl_texture_opaque_bump_end();
	}

		// lighting

	if (!is_hilite) {

			// simple lighting

		is_poly_simple_lighting=(is_simple_lighting) || (poly->draw.simple_tessel);
	
			// specular?
			
		is_specular=(setup.specular_mapping) && (texture->specularmaps[frame].gl_id!=-1);

			// tesseled lighting mesh

		gl_texture_tesseled_lighting_start();

		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO,GL_SRC_COLOR);

		glDisable(GL_ALPHA_TEST);
		glDisable(GL_DEPTH_TEST);
		
		if (!is_specular) {
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);			// if no specular, clear the stencil now
		}
		else {
			glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
		}
		
		glStencilFunc(GL_EQUAL,0x55,0xFF);

		gl_texture_tesseled_lighting_set(-1,poly->dark_factor);

		if (is_poly_simple_lighting) {
			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
		}
		else {
			ntrig=poly->light.trig_count;
			glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			if ((poly->ptsz-2)!=ntrig) glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
		}

		gl_texture_tesseled_lighting_end();

			// speculars hilights

		if (is_specular) {

			gl_texture_tesseled_specular_start();

			glBlendFunc(GL_ONE,GL_ONE);
		
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
			glStencilFunc(GL_EQUAL,0x56,0xFF);

			gl_texture_tesseled_specular_set(texture->specularmaps[frame].gl_id);

			if (is_poly_simple_lighting) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
				if ((poly->ptsz-2)!=ntrig) glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
		
			gl_texture_tesseled_specular_end();
		}
	}

		// glow mapping

	if ((setup.glow_mapping) && (texture->glowmaps[frame].gl_id!=-1)) {

		glDisable(GL_STENCIL_TEST);
		
		gl_texture_opaque_glow_start();

		glDisable(GL_BLEND);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);
		glDepthMask(GL_FALSE);

		gl_texture_opaque_glow_set(texture->bitmaps[frame].gl_id,texture->glowmaps[frame].gl_id,texture->glow.current_color);
		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

		gl_texture_opaque_glow_end();
	}
}
*/







void render_opaque_portal_normal(int portal_idx,bool is_simple_lighting)
{
	int					n,k,frame,ntrig;
	portal_type			*portal;
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

	gl_texture_opaque_start();
	
		// run through the portal meshes

	portal=&map.portals[portal_idx];
	mesh=portal->mesh.meshes;

	for (n=0;n!=portal->mesh.nmesh;n++) {

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
			gl_texture_opaque_set(texture->bitmaps[frame].gl_id);

				// draw polygon

//			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

			// supergumba -- a test
			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			}

			poly++;
		}

		mesh++;
	}

		// end drawing

	gl_texture_opaque_end();
}








// supergumba
// if 			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
// is true, then don't stencil!



void render_opaque_portal_stencil_poly(map_mesh_poly_type *poly,texture_type *texture,int frame)
{
	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
			
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilFunc(GL_ALWAYS,0x55,0xFF);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
			
	glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}


void render_opaque_portal_bump(int portal_idx,bool is_simple_lighting)
{
	int					n,k,frame,ntrig;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// need to use normal map in color array

	portal=&map.portals[portal_idx];
	glColorPointer(3,GL_FLOAT,0,portal->vertexes.pnormal);

		// setup drawing

	glEnable(GL_STENCIL_TEST);
	
		// run through the portal meshes

	mesh=portal->mesh.meshes;

	for (n=0;n!=portal->mesh.nmesh;n++) {

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

				// bump texture set?

			if (texture->bumpmaps[frame].gl_id==-1) {
				poly++;
				continue;
			}
			
				// stencil polygon

			render_opaque_portal_stencil_poly(poly,texture,frame);

				// blend in the bump

			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO,GL_SRC_COLOR);

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_ALPHA_TEST);

			glStencilFunc(GL_EQUAL,0x55,0xFF);
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

			gl_texture_opaque_tesseled_bump_start();
			gl_texture_opaque_tesseled_bump_set(texture->bumpmaps[frame].gl_id,0);

			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			}

			gl_texture_opaque_tesseled_bump_end();

			poly++;
		}

		mesh++;
	}

		// end drawing

	glDisable(GL_STENCIL_TEST);

		// restore original color array

	glColorPointer(3,GL_FLOAT,0,portal->vertexes.pcolor);
}



void render_opaque_portal_lighting(int portal_idx,bool is_simple_lighting)
{
	int					n,k,frame,ntrig;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// setup drawing

//	glEnable(GL_STENCIL_TEST);
	
		// run through the portal meshes

	portal=&map.portals[portal_idx];
	mesh=portal->mesh.meshes;

	for (n=0;n!=portal->mesh.nmesh;n++) {

			// skip hilited polygons

		if (mesh->flag.hilite) {
			mesh++;
			continue;
		}

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
			
				// stencil polygon

//			render_opaque_portal_stencil_poly(poly,texture,frame);

				// draw lighting

			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO,GL_SRC_COLOR);

//			glDisable(GL_DEPTH_TEST);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

			
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);

//			glStencilFunc(GL_EQUAL,0x55,0xFF);
//			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

			gl_texture_tesseled_lighting_start();
			gl_texture_tesseled_lighting_set(texture->bitmaps[frame].gl_id,poly->dark_factor);

			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
//				if ((poly->ptsz-2)!=ntrig) glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			
			gl_texture_tesseled_lighting_end();

			poly++;
		}

		mesh++;
	}

		// end drawing

	glDisable(GL_STENCIL_TEST);
}






void render_opaque_portal_specular(int portal_idx,bool is_simple_lighting)
{
	int					n,k,frame,ntrig;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

		// setup drawing

	glEnable(GL_STENCIL_TEST);
	
		// run through the portal meshes

	portal=&map.portals[portal_idx];
	mesh=portal->mesh.meshes;

	for (n=0;n!=portal->mesh.nmesh;n++) {

			// skip hilited polygons

		if (mesh->flag.hilite) {
			mesh++;
			continue;
		}

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

				// specular texture set?

			if (texture->specularmaps[frame].gl_id==-1) {
				poly++;
				continue;
			}
			
				// stencil polygon

			render_opaque_portal_stencil_poly(poly,texture,frame);

				// draw specular

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_ALPHA_TEST);

			glStencilFunc(GL_EQUAL,0x55,0xFF);
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

			gl_texture_tesseled_specular_start();
			gl_texture_tesseled_specular_set(texture->specularmaps[frame].gl_id);

			if ((is_simple_lighting) || (poly->draw.simple_tessel)) {
				glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
			}
			else {
				ntrig=poly->light.trig_count;
				glDrawElements(GL_TRIANGLES,(ntrig*3),GL_UNSIGNED_INT,(GLvoid*)poly->light.trig_vertex_idx);
			}
			
			gl_texture_tesseled_specular_end();

			poly++;
		}

		mesh++;
	}

		// end drawing


	glDisable(GL_STENCIL_TEST);
}


void render_opaque_portal_glow(int portal_idx)
{
	int					n,k,frame;
	portal_type			*portal;
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
	
		// run through the portal meshes

	portal=&map.portals[portal_idx];
	mesh=portal->mesh.meshes;

	for (n=0;n!=portal->mesh.nmesh;n++) {

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

				// specular texture set?

			if (texture->glowmaps[frame].gl_id==-1) {
				poly++;
				continue;
			}

				// draw polygon

			gl_texture_opaque_glow_set(texture->bitmaps[frame].gl_id,texture->glowmaps[frame].gl_id,texture->glow.current_color);
			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

			poly++;
		}

		mesh++;
	}

		// end drawing

	gl_texture_opaque_glow_end();
}








void segment_render_opaque(int portal_cnt,int *portal_list)
{
	int				n,portal_idx;
	bool			is_simple_lighting;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

		// texture binding optimization

	gl_texture_bind_start();

		// detect simple lighting

	is_simple_lighting=(!setup.high_quality_lighting) || (fog_solid_on());
		
		// run through portals
		// we want to go from closest to furthest to
		// catch as much z-buffer eliminates as possible

	for (n=(portal_cnt-1);n>=0;n--) {
		portal_idx=portal_list[n];
		portal_compile_gl_list_attach(portal_idx);
		render_opaque_portal_normal(portal_idx,is_simple_lighting);
	//	if (setup.bump_mapping) render_opaque_portal_bump(portal_idx,is_simple_lighting);
		if (!hilite_on) {
			render_opaque_portal_lighting(portal_idx,is_simple_lighting);
		//	render_opaque_portal_specular(portal_idx,is_simple_lighting);
		//	render_opaque_portal_glow(portal_idx);
		}
	}

		// dettach any attached lists

	portal_compile_gl_list_dettach();
}

