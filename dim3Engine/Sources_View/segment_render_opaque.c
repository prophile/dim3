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

#include "lights.h"
#include "effects.h"
#include "consoles.h"
#include "video.h"

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern int game_time_get(void);
extern bool fog_solid_on(void);
extern void view_compile_gl_list_attach(void);
extern void view_compile_gl_list_switch_to_color(void);
extern void view_compile_gl_list_switch_to_normal(void);
extern void view_compile_gl_list_switch_to_specular(void);
extern void view_compile_gl_list_dettach(void);
																						
/* =======================================================

      Opaque Map Shaders
      
======================================================= */



void render_opaque_mesh_shader(int mesh_cnt,int *mesh_list)
{
	int					n,k,frame,nlight;
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

	gl_lights_start();

	gl_shader_draw_start();
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.mesh.meshes[mesh_list[n]];

			// run through the polys
			
		map_calculate_light_reduce_mesh(mesh);		// supergumba -- reduce lights before running mesh

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// shaders happen outside of stenciling
				// so we need to check for transparencies here

			if ((texture->bitmaps[frame].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f)) {
				poly++;
				continue;
			}
			
			nlight=gl_lights_build_from_reduced_light_list(&poly->box.mid);

				// setup shader

			gl_shader_draw_execute(texture,frame,nlight,poly->dark_factor);

				// dark factor

			glColor4f(poly->dark_factor,poly->dark_factor,poly->dark_factor,1.0f);

				// draw polygon

			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
			
			poly++;
		}
	}

		// end drawing

	gl_shader_draw_end();

	gl_lights_end();
}












// supergumba -- light testing






void render_opaque_mesh_old(int mesh_cnt,int *mesh_list)
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

	gl_texture_opaque_start(TRUE);

		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.mesh.meshes[mesh_list[n]];

		map_calculate_light_reduce_mesh(mesh);		// supergumba -- reduce lights before running mesh
		
			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture

			texture=&map.textures[poly->txt_idx];

			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if ((texture->bitmaps[frame].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f)) {
				poly++;
				continue;
			}
			

			gl_texture_opaque_set(texture->bitmaps[frame].gl_id);
			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
			
			poly++;
		}
	}

		// end drawing

	gl_texture_opaque_end();
}







void render_opaque_mesh_simple(int mesh_cnt,int *mesh_list)
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

	gl_lights_start();

	gl_texture_opaque_start(TRUE);
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.mesh.meshes[mesh_list[n]];

		map_calculate_light_reduce_mesh(mesh);
		
			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture

			texture=&map.textures[poly->txt_idx];

			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if ((texture->bitmaps[frame].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f)) {
				poly++;
				continue;
			}
			
				// setup lights

			gl_lights_build_from_reduced_light_list(&poly->box.mid);
			
				// dark factor

			glColor4f(poly->dark_factor,poly->dark_factor,poly->dark_factor,1.0f);

				// draw polygon

			gl_texture_opaque_set(texture->bitmaps[frame].gl_id);
			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);

			poly++;
		}
	}

		// end drawing

	gl_texture_opaque_end();

	gl_lights_end();
}









void render_opaque_mesh_glow(int mesh_cnt,int *mesh_list)
{
	int					n,k,frame;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;
	
		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	gl_texture_glow_start();
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.mesh.meshes[mesh_list[n]];

			// skip hilited polygons
			// or meshes with no speculars

		if ((!mesh->flag.has_glow) || (mesh->flag.hilite)) continue;

			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// glow texture set?

			if (texture->glowmaps[frame].gl_id==-1) {
				poly++;
				continue;
			}

				// draw glow

			gl_texture_glow_set(texture->glowmaps[frame].gl_id,texture->glow.current_color);
			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);

			poly++;
		}
	}

		// end drawing

	glDepthMask(GL_TRUE);

	gl_texture_glow_end();
}













/* =======================================================

      Opaque Map Rendering
      
======================================================= */

void render_opaque_map(int mesh_cnt,int *mesh_list)
{
		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.pnt,&view.camera.ang);
	gl_setup_project();

		// texture binding optimization

	gl_texture_bind_start();

		// attach map complied open gl list

	view_compile_gl_list_attach();
		
	if (dim3_debug) {
	//	render_opaque_portal_light_old(mesh_cnt,mesh_list);
		render_opaque_mesh_shader(mesh_cnt,mesh_list);
	}
	else {
		render_opaque_mesh_simple(mesh_cnt,mesh_list);
	}

	render_opaque_mesh_glow(mesh_cnt,mesh_list);

		// dettach any attached lists

	view_compile_gl_list_dettach();
}

