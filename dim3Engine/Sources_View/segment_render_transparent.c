/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Transparent Segment Rendering

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

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

extern bool fog_solid_on(void);
extern void view_compile_gl_list_attach(void);
extern void view_compile_gl_list_dettach(void);

/* =======================================================

      Transparent Mesh Drawing
      
======================================================= */

void render_transparent_portal_mesh(bool is_fog_lighting)
{
	int						n,sort_cnt,frame;
	unsigned long			txt_id;
	float					alpha;
	bool					txt_setup_reset;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_poly_sort_item_type	*sort_list;
	texture_type			*texture;

	sort_cnt=map.sort.count;
	sort_list=map.sort.list;

		// this flag tells if we need to reset the
		// texture setup for transparencies.  It starts
		// true so we initially set it up, and goes back to
		// true whenever a specular or glow mapping disrupts
		// the rendering flow

		// speculars and glows need to render when the actual
		// transparent segment renders so the sorting works
		// correctly

	txt_setup_reset=TRUE;

		// texture mapped transparent segments
		// these are lighted by themselves

	txt_id=-1;
	alpha=0.0f;

	for (n=0;n!=sort_cnt;n++) {
		mesh=&map.mesh.meshes[sort_list[n].mesh_idx];
		poly=&mesh->polys[sort_list[n].poly_idx];
	
			// get texture

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			// is shader

		if (texture->shader.on) continue;

			// do we need to get back to rendering for transparencies?
			// this happens when a specular or glow interrupt the normal
			// rendering flow.  This also forces a reset of the current
			// texture and alpha

		if (txt_setup_reset) {
			gl_texture_transparent_start();
						
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);
						
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);

			txt_setup_reset=FALSE;

			txt_id=-1;
			alpha=0.0f;
		}

			// draw the transparent texture

		if ((texture->bitmaps[frame].gl_id!=txt_id) || (poly->alpha!=alpha)) {
		
			txt_id=texture->bitmaps[frame].gl_id;
			alpha=poly->alpha;
			gl_texture_transparent_set(txt_id,alpha);
			
			if (texture->additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}

		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

			// draw any specular on the transparent segment

		if ((!is_fog_lighting) && (texture->specularmaps[frame].gl_id!=-1)) {
			
				// end transparencies drawing and start specular

			gl_texture_transparent_end();
			
			gl_texture_transparent_specular_start();

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);		// speculars are always additive

			glDisable(GL_ALPHA_TEST);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);

				// draw specular

			gl_texture_transparent_specular_set(texture->specularmaps[frame].gl_id,poly->alpha);

				// use lighting mesh as specular is dependant upon the light

			glDrawElements(GL_QUADS,(poly->light.nquad*4),GL_UNSIGNED_INT,(GLvoid*)(mesh->light.quad_indexes+poly->light.quad_index_offset));

				// end specular drawing and force a transparencies reset

			gl_texture_transparent_specular_end();
			txt_setup_reset=TRUE;
		}

			// draw any glow on the transparent segment

		if (texture->glowmaps[frame].gl_id!=-1) {

				// end transparencies drawing and start glow

			gl_texture_transparent_end();
		
			gl_texture_transparent_glow_start();
			
			glEnable(GL_BLEND);
			if (texture->additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);	// glows can also be additive
			}

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);
		
				// draw glow

			gl_texture_transparent_glow_set(texture->bitmaps[frame].gl_id,texture->glowmaps[frame].gl_id,poly->alpha,texture->glow.current_color);

			glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);

				// end glow drawing and force a transparencies reset

			gl_texture_transparent_glow_end();
			txt_setup_reset=TRUE;
		}
	}

	gl_texture_transparent_end();
}

void render_transparent_portal_shader(void)
{
	int						n,sort_cnt,frame;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_poly_sort_item_type	*sort_list;
	texture_type			*texture;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
				
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	gl_shader_program_start(max_map_texture,map.textures);
	gl_texture_shader_start();

	sort_cnt=map.sort.count;
	sort_list=map.sort.list;
	
	for (n=0;n!=sort_cnt;n++) {
		mesh=&map.mesh.meshes[sort_list[n].mesh_idx];
		poly=&mesh->polys[sort_list[n].poly_idx];

			// get texture

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			// is shader

		if (!texture->shader.on) continue;

			// draw shader

		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
		
		gl_shader_set_variables(texture->shader.program_obj,&poly->box.mid,texture);

		glDrawElements(GL_POLYGON,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.portal_v);
	}

	gl_texture_shader_end();
	gl_shader_program_end();
}

/* =======================================================

      Transparent Mesh Sorting
      
======================================================= */

float render_transparent_segments_far_z(map_mesh_type *mesh,map_mesh_poly_type *poly,d3pnt *pnt)
{
	int				n,kx,ky,kz;
	float			d,dist;
	d3pnt			*pt;

		// calculate the farest z
		// that is on screen

	dist=0.0f;

	for (n=0;n!=poly->ptsz;n++) {

		pt=&mesh->vertexes[poly->v[n]];
		kx=pt->x-pnt->x;
		ky=pt->y-pnt->y;
		kz=pnt->z-pt->z;

		if (!gl_project_in_view_z(kx,ky,kz)) continue;

		d=gl_project_get_depth(kx,ky,kz);
		if (d>dist) dist=d;
	}

	return(dist);
}

void render_transparent_sort(int mesh_cnt,int *mesh_list,d3pnt *pnt)
{
	int							n,k,i,frame,sort_cnt,sort_idx;
	float						dist;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	map_poly_sort_item_type		*sort_list;
	texture_type				*texture;

	sort_list=map.sort.list;

		// create sort list

	sort_cnt=0;
		
	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.mesh.meshes[mesh_list[n]];
		
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];

				// get texture

			texture=&map.textures[poly->txt_idx];
			if (texture->shader.on) continue;
			
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// transparent?

			if ((texture->bitmaps[frame].alpha_mode!=alpha_mode_transparent) && (poly->alpha==1.0f)) continue;

				// find distance from camera

			dist=render_transparent_segments_far_z(mesh,poly,pnt);

				// find position in sort list

			sort_idx=sort_cnt;

			for (i=0;i!=sort_cnt;i++) {
				if (dist>sort_list[i].dist) {
					sort_idx=i;
					break;
				}
			}

				// add to sort list

			if (sort_idx<sort_cnt) {
				memmove(&sort_list[sort_idx+1],&sort_list[sort_idx],((sort_cnt-sort_idx)*sizeof(map_poly_sort_item_type)));
			}

			sort_list[sort_idx].mesh_idx=mesh_list[n];
			sort_list[sort_idx].poly_idx=k;
			sort_list[sort_idx].dist=dist;

			sort_cnt++;
			if (sort_cnt>=max_sort_poly) break;

			poly++;
		}

		if (sort_cnt>=max_sort_poly) break;
	}

	map.sort.count=sort_cnt;
}

/* =======================================================

      Transparent Map Rendering
      
======================================================= */

void render_transparent_map(int mesh_cnt,int *mesh_list)
{
	bool			is_fog_lighting;
	
		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

		// texture binding optimization

	gl_texture_bind_start();

		// check for obscure fog lighting

	is_fog_lighting=fog_solid_on();
	
		// attach compiled vertex lists

	view_compile_gl_list_attach();

		// sort meshes

	render_transparent_sort(mesh_cnt,mesh_list,&view.camera.pnt);

		// transparent meshes

	render_transparent_portal_mesh(is_fog_lighting);
	render_transparent_portal_shader();

		// dettach any attached lists

	view_compile_gl_list_dettach();
}

