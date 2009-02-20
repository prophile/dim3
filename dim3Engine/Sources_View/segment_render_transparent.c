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

#include "lights.h"
#include "effects.h"
#include "consoles.h"
#include "video.h"

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

extern bool fog_solid_on(void);
extern void view_compile_gl_list_attach(void);
extern void view_compile_gl_list_dettach(void);
extern void view_compile_gl_list_switch_to_color(void);
extern void view_compile_gl_list_switch_to_normal(void);
extern void view_compile_gl_list_switch_to_specular(void);

/* =======================================================

      Transparent Mesh Drawing
      
======================================================= */

void render_transparent_mesh_simple(void)
{
	int						n,sort_cnt,frame,cur_mesh_idx;
	unsigned long			txt_id;
	float					alpha;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_poly_sort_item_type	*sort_list;
	texture_type			*texture;

		// setup transparent drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
				
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

		// sorted transparency list

	sort_cnt=map.sort.count;
	sort_list=map.sort.list;

		// keep track of certain settings so
		// we can optimize state changes

	txt_id=-1;
	alpha=0.0f;
	cur_mesh_idx=-1;

		// draw transparent meshes

	gl_texture_transparent_start();

	for (n=0;n!=sort_cnt;n++) {
		mesh=&map.mesh.meshes[sort_list[n].mesh_idx];
		poly=&mesh->polys[sort_list[n].poly_idx];

			// reduce the lighting list

		if (cur_mesh_idx!=sort_list[n].mesh_idx) {
			cur_mesh_idx=sort_list[n].mesh_idx;
			map_calculate_light_reduce_mesh(mesh);
		}
	
			// get texture

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			// do we need texture switching?

		if ((texture->bitmaps[frame].gl_id!=txt_id) || (poly->alpha!=alpha)) {
		
			txt_id=texture->bitmaps[frame].gl_id;
			alpha=poly->alpha;

			if (texture->additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}

			gl_texture_transparent_set(txt_id,alpha);
			
		}

			// setup the lights

		gl_build_lights_from_reduced_light_list(&poly->box.mid);

			// draw the polygon

		glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
	}

	gl_texture_transparent_end();
}

void render_transparent_mesh_shader(void)
{
	int						n,sort_cnt,frame,nlight,cur_mesh_idx;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_poly_sort_item_type	*sort_list;
	texture_type			*texture;

		// set transparent shader calls

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
				
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

		// sorted transparent poly list

	sort_cnt=map.sort.count;
	sort_list=map.sort.list;

		// keep track of state

	cur_mesh_idx=-1;

		// start shaders

	gl_shader_program_start(max_map_texture,map.textures);
	gl_texture_shader_start();
	
	for (n=0;n!=sort_cnt;n++) {
		mesh=&map.mesh.meshes[sort_list[n].mesh_idx];
		poly=&mesh->polys[sort_list[n].poly_idx];

			// get texture

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			// is shader

		if (!texture->shader.on) continue;

			// reduce the lighting list

		if (cur_mesh_idx!=sort_list[n].mesh_idx) {
			cur_mesh_idx=sort_list[n].mesh_idx;
			map_calculate_light_reduce_mesh(mesh);
		}

			// setup the lights

		nlight=gl_build_lights_from_reduced_light_list(&poly->box.mid);

			// draw shader

		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
		
		gl_shader_set_variables(texture->shader.program_obj,&poly->box.mid,nlight,poly->dark_factor,texture);

		glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
	}

	gl_texture_shader_end();
	gl_shader_program_end();
}


void render_transparent_mesh_glow(void)
{
	int						n,sort_cnt,frame;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_poly_sort_item_type	*sort_list;
	texture_type			*texture;

		// setup glow mapping

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	sort_cnt=map.sort.count;
	sort_list=map.sort.list;

	gl_texture_glow_start();

	for (n=0;n!=sort_cnt;n++) {
		mesh=&map.mesh.meshes[sort_list[n].mesh_idx];
		poly=&mesh->polys[sort_list[n].poly_idx];

			// skip hilited polygons
			// or meshes with no speculars

		if ((!mesh->flag.has_glow) || (mesh->flag.hilite)) continue;

			// get texture

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			// any glow on texture

		if (texture->glowmaps[frame].gl_id==-1) continue;
		
			// draw glow

		gl_texture_glow_set(texture->glowmaps[frame].gl_id,texture->glow.current_color);
		glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
	}

	gl_texture_glow_end();
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
		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.pnt,&view.camera.ang);
	gl_setup_project();

		// texture binding optimization

	gl_texture_bind_start();
	
		// attach compiled vertex lists

	view_compile_gl_list_attach();

		// sort meshes

	render_transparent_sort(mesh_cnt,mesh_list,&view.camera.pnt);

		// transparent meshes

	if (dim3_debug) {
		render_transparent_mesh_shader();
	}
	else {
		render_transparent_mesh_simple();
	}

	render_transparent_mesh_glow();

		// dettach any attached lists

	view_compile_gl_list_dettach();
}

