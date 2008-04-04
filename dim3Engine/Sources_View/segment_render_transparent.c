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

extern void portal_compile_gl_list_attach(int rn,int txt_unit_count);
extern void portal_compile_gl_list_dettach(void);

/* =======================================================

      Transparent Mesh Sorting
      
======================================================= */

float segment_render_transparent_segments_far_z(map_mesh_type *mesh,map_mesh_poly_type *mesh_poly,int cx,int cy,int cz)
{
	int				n,kx,ky,kz;
	float			d,dist;
	d3pnt			*pt;

		// calculate the farest z
		// that is on screen

	dist=0.0f;

	for (n=0;n!=mesh_poly->ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		kx=pt->x-cx;
		ky=pt->y-cy;
		kz=pt->z-cz;
		if (gl_rotate_point_on_screen(kx,ky,kz)) {
			d=gl_project_point_z(kx,ky,kz);
			if (d>dist) dist=d;
		}
	}

	return(dist);
}

void segment_render_transparent_sort(int rn,int cx,int cy,int cz)
{
	int							n,k,i,sort_cnt,sort_idx;
	float						dist;
	portal_type					*portal;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*mesh_poly;
	map_mesh_poly_sort_type		*sort_list;

	portal=&map.portals[rn];
	sort_list=portal->mesh.draw.sort_list;

		// create sort list

	sort_cnt=0;

	mesh=portal->mesh.meshes;
		
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		if (!mesh->draw.has_transparent) {
			mesh++;
			continue;
		}
		
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			if (mesh_poly->draw.draw_type!=map_mesh_poly_draw_transparent) {
				mesh_poly++;
				continue;
			}

				// find distance from camera

			dist=segment_render_transparent_segments_far_z(mesh,mesh_poly,cx,cy,cz);

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
				memmove(&sort_list[sort_idx+1],&sort_list[sort_idx],((sort_cnt-sort_idx)*sizeof(map_mesh_poly_sort_type)));
			}

			sort_list[sort_idx].mesh_idx=n;
			sort_list[sort_idx].poly_idx=k;
			sort_list[sort_idx].dist=dist;

			sort_cnt++;

			mesh_poly++;
		}
	
		mesh++;
	}

	portal->mesh.draw.sort_cnt=sort_cnt;
}

/* =======================================================

      Transparent Mesh Drawing
      
======================================================= */

void segment_render_transparent_portal_mesh(portal_type *portal)
{
	int						n,sort_cnt,frame;
	unsigned long			txt_id;
	float					alpha;
	bool					txt_setup_reset;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	map_mesh_poly_sort_type	*sort_list;
	texture_type			*texture;

	sort_cnt=portal->mesh.draw.sort_cnt;
	sort_list=portal->mesh.draw.sort_list;

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
		mesh=&portal->mesh.meshes[sort_list[n].mesh_idx];
		mesh_poly=&mesh->polys[sort_list[n].poly_idx];

		texture=&map.textures[mesh_poly->txt_idx];
		frame=mesh_poly->draw.cur_frame;

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

		if ((texture->bitmaps[frame].gl_id!=txt_id) || (mesh_poly->alpha!=alpha)) {
		
			txt_id=texture->bitmaps[frame].gl_id;
			alpha=mesh_poly->alpha;
			gl_texture_transparent_set(txt_id,alpha);
			
			if (texture->additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}

		glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);

			// draw any specular on the transparent segment

		if (mesh_poly->draw.is_specular) {
			
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

			gl_texture_transparent_specular_set(texture->specularmaps[frame].gl_id,mesh_poly->alpha);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);

				// end specular drawing and force a transparencies reset

			gl_texture_transparent_specular_end();
			txt_setup_reset=TRUE;

		}

			// draw any glow on the transparent segment

		if (mesh_poly->draw.is_glow) {

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

			gl_texture_transparent_glow_set(texture->bitmaps[frame].gl_id,texture->glowmaps[frame].gl_id,mesh_poly->alpha);
			gl_texture_transparent_glow_color(texture->glow.current_color);

			glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);

				// end glow drawing and force a transparencies reset

			gl_texture_transparent_glow_end();
			txt_setup_reset=TRUE;
		}
	}

	gl_texture_transparent_end();
}

void segment_render_transparent_portal_shader(portal_type *portal)
{
	int						n,sort_cnt,frame;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	map_mesh_poly_sort_type	*sort_list;
	texture_type			*texture;
	light_spot_type			*lspot;

	gl_shader_start();
	gl_texture_shader_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
				
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	sort_cnt=portal->mesh.draw.sort_cnt;
	sort_list=portal->mesh.draw.sort_list;
	
	for (n=0;n!=sort_cnt;n++) {
		mesh=&portal->mesh.meshes[sort_list[n].mesh_idx];
	
		if (!mesh->draw.has_transparent_shader) continue;
		
		mesh_poly=&mesh->polys[sort_list[n].poly_idx];
		if (mesh_poly->draw.draw_type!=map_mesh_poly_draw_transparent_shader) continue;

		texture=&map.textures[mesh_poly->txt_idx];
		frame=mesh_poly->draw.cur_frame;

		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
		
		lspot=map_portal_find_closest_light(portal,(double)mesh_poly->box.mid.x,(double)mesh_poly->box.mid.y,(double)mesh_poly->box.mid.z,NULL);
		gl_shader_set_variables(texture->shader.program_obj,&mesh_poly->box.mid,texture,lspot);
		
		glNormal3f(mesh_poly->draw.normal[0],mesh_poly->draw.normal[1],mesh_poly->draw.normal[2]);

		glDrawElements(GL_POLYGON,mesh_poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)mesh_poly->draw.portal_v);
	}

	gl_texture_shader_end();
	gl_shader_end();
}

void segment_render_transparent_portal(int rn)
{
	portal_type			*portal;

	portal=&map.portals[rn];
		
		// if no transparent segments, then just skip
		
	if (!portal->mesh.draw.has_transparent) return;

		// attach compiled vertex lists

	if (portal->mesh.draw.has_opaque_shader) {
		portal_compile_gl_list_attach(rn,3);
	}
	else {
		portal_compile_gl_list_attach(rn,2);
	}

		// sort meshes

	segment_render_transparent_sort(rn,view.camera.pos.x,view.camera.pos.y,view.camera.pos.z);

		// transparent meshes

	segment_render_transparent_portal_mesh(portal);
	if (portal->mesh.draw.has_transparent_shader) segment_render_transparent_portal_shader(portal);
}

/* =======================================================

      Transparent Drawing Mainline
      
======================================================= */

void segment_render_transparent(int portal_cnt,int *portal_list)
{
	int				n;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
	
		// run through portals
		// we want to go from furthest away to closest
		// for the transparency sorting
		
	for (n=0;n<portal_cnt;n++) {
		segment_render_transparent_portal(portal_list[n]);
	}

		// dettach any attached lists

	portal_compile_gl_list_dettach();
}

