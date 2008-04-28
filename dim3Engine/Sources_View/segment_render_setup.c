/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Segment Drawing Setup

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

extern bool				hilite_on;

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern void portal_compile_gl_lists(int tick,int rn);
extern bool fog_solid_on(void);

/* =======================================================

      Normal Smoothing Init
      
======================================================= */

void mesh_normal_smooth_init(void)
{
	int						i,n,k;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;

	portal=map.portals;

	for (i=0;i!=map.nportal;i++) {

		mesh=portal->mesh.meshes;

		for (n=0;n!=portal->mesh.nmesh;n++) {
			
			mesh_poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				mesh_poly->draw.normal[0]=0.5f;
				mesh_poly->draw.normal[1]=0.5f;
				mesh_poly->draw.normal[2]=1.0f;
				
				mesh_poly++;
			}
		
			mesh++;
		}
			
		portal++;
	}
}

/* =======================================================

      Setup Meshes To Render
      
======================================================= */

void mesh_render_setup(int tick,int portal_cnt,int *portal_list)
{
	int							n,k,i,rn,frame,
								lod_dist,stencil_pass,stencil_idx;
	bool						is_transparent,light_changed,global_light_simple;
	portal_type					*portal;
	texture_type				*texture;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*mesh_poly;
	
		// setup portal and mesh drawing
		// compile gl lists for portal and per mesh and
		// poly flags from rendering
		
	for (i=(portal_cnt-1);i>=0;i--) {
		rn=portal_list[i];
		portal=&map.portals[rn];

			// create vertex, normal, and color lists
			
		portal_compile_gl_lists(tick,rn);

			// has lighting changed in this portal?
			
		light_changed=map_portal_light_check_changes(portal);
		
			// combined global simple lighting
			
		global_light_simple=(!setup.high_quality_lighting) || (fog_solid_on());

			// per-portal flags

		portal->mesh.draw.has_transparent=FALSE;
		portal->mesh.draw.has_glow=FALSE;
		portal->mesh.draw.has_specular=FALSE;
		portal->mesh.draw.has_opaque_shader=FALSE;
		portal->mesh.draw.has_transparent_shader=FALSE;

			// run through the meses

		mesh=portal->mesh.meshes;

		for (n=0;n!=portal->mesh.nmesh;n++) {
		
			mesh->draw.has_stencil_normal=FALSE;
			mesh->draw.has_stencil_bump=FALSE;
			mesh->draw.has_transparent=FALSE;
			mesh->draw.has_glow=FALSE;
			mesh->draw.has_specular=FALSE;
			mesh->draw.has_opaque_shader=FALSE;
			mesh->draw.has_transparent_shader=FALSE;
			
			mesh_poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				
				lod_dist=abs(mesh_poly->box.mid.x-view.camera.pos.x)+abs(mesh_poly->box.mid.y-view.camera.pos.y)+abs(mesh_poly->box.mid.z-view.camera.pos.z);

				texture=&map.textures[mesh_poly->txt_idx];
				frame=(texture->animate.current_frame+mesh_poly->draw.txt_frame_offset)&max_texture_frame_mask;

				mesh_poly->draw.is_simple_lighting=(global_light_simple) || (lod_dist>map.optimizations.lod_light_distance) || (mesh_poly->draw.simple_tessel);

					// is texture transparent?

				is_transparent=(mesh_poly->alpha!=1.0f) || (texture->bitmaps[frame].alpha_mode==alpha_mode_transparent);

					// is shader

				if (texture->shader.on) {
				
					if (is_transparent) {
						mesh_poly->draw.draw_type=map_mesh_poly_draw_transparent_shader;
						mesh->draw.has_transparent_shader=TRUE;
						portal->mesh.draw.has_transparent_shader=TRUE;
					}
					else {
						mesh_poly->draw.draw_type=map_mesh_poly_draw_opaque_shader;
						mesh->draw.has_opaque_shader=TRUE;
						portal->mesh.draw.has_opaque_shader=TRUE;
					}
				}
				else {

						// is transparent?
				
					if (is_transparent) {
						mesh->draw.has_transparent=TRUE;
						mesh_poly->draw.draw_type=map_mesh_poly_draw_transparent;
						portal->mesh.draw.has_transparent=TRUE;
					}
					else {

							// is bump?

						if ((setup.bump_mapping) && (lod_dist<map.optimizations.lod_bump_distance) && (texture->bumpmaps[frame].gl_id!=-1)) {
							if (!mesh_poly->draw.is_simple_lighting) {
								mesh_poly->draw.draw_type=map_mesh_poly_draw_stencil_bump;
								mesh->draw.has_stencil_bump=TRUE;
								portal->mesh.draw.has_stencil_bump=TRUE;
								portal->mesh.draw.has_stencil_lighting=TRUE;
							}
							else {
								mesh_poly->draw.draw_type=map_mesh_poly_draw_simple_bump;
								mesh->draw.has_simple_bump=TRUE;
								portal->mesh.draw.has_simple_bump=TRUE;
								portal->mesh.draw.has_simple_lighting=TRUE;
							}
							if ((!light_changed) || (mesh->flag.moveable)) map_portal_calculate_normal_vector_smooth(portal,(double)mesh_poly->box.mid.x,(double)mesh_poly->box.mid.y,(double)mesh_poly->box.mid.z,mesh_poly->draw.normal);
						}

							// is normal?

						else {
							if (!mesh_poly->draw.is_simple_lighting) {
								mesh_poly->draw.draw_type=map_mesh_poly_draw_stencil_normal;
								mesh->draw.has_stencil_normal=TRUE;
								portal->mesh.draw.has_stencil_normal=TRUE;
								portal->mesh.draw.has_stencil_lighting=TRUE;
							}
							else {
								mesh_poly->draw.draw_type=map_mesh_poly_draw_simple_normal;
								mesh->draw.has_simple_normal=TRUE;
								portal->mesh.draw.has_simple_normal=TRUE;
								portal->mesh.draw.has_simple_lighting=TRUE;
							}
						}
					}

						// check for glow and specular

					mesh_poly->draw.is_glow=((setup.glow_mapping) && (lod_dist<map.optimizations.lod_glow_distance) && (texture->glowmaps[frame].gl_id!=-1));
					mesh_poly->draw.is_specular=((setup.specular_mapping) && (lod_dist<map.optimizations.lod_specular_distance) && (texture->specularmaps[frame].gl_id!=-1));

					mesh->draw.has_glow|=mesh_poly->draw.is_glow;
					portal->mesh.draw.has_glow|=mesh_poly->draw.is_glow;

					mesh->draw.has_specular|=mesh_poly->draw.is_specular;
					portal->mesh.draw.has_specular|=mesh_poly->draw.is_specular;
				}

				mesh_poly->draw.cur_frame=frame;

				mesh_poly++;
			}

			mesh++;
		}
			
		portal++;
	}

		// setup the opaque per-portal stencil passes
		// there might be more than 256 polygons per portal, so we need to run in passes
		// transparent items don't use stencils
		
		// each polygon gets a stencil pass and stencil index within that pass
		// the mesh itself remembers the first and last stencil pass for
		// optimization purposes
		
	stencil_pass=0;
	stencil_idx=stencil_segment_start;
	
	for (i=(portal_cnt-1);i>=0;i--) {

		rn=portal_list[i];
		portal=&map.portals[rn];

		portal->opaque_stencil_pass_start=stencil_pass;
		
		mesh=portal->mesh.meshes;
	
		for (n=0;n!=portal->mesh.nmesh;n++) {
		
			if ((mesh->draw.has_stencil_normal) || (mesh->draw.has_stencil_bump)) {
				
				mesh->draw.stencil_pass_start=stencil_pass;

				mesh_poly=mesh->polys;
				
				for (k=0;k!=mesh->npoly;k++) {

					if ((mesh_poly->draw.draw_type!=map_mesh_poly_draw_stencil_normal) || (mesh_poly->draw.draw_type!=map_mesh_poly_draw_stencil_bump)) {
						mesh_poly->draw.stencil_pass=stencil_pass;
						mesh_poly->draw.stencil_idx=stencil_idx;
						
						stencil_idx++;
						if (stencil_idx>stencil_segment_end) {
							stencil_idx=stencil_segment_start;
							stencil_pass++;
						}
					}
				
					mesh_poly++;
				}
				
				mesh->draw.stencil_pass_end=stencil_pass;
			}

		
			mesh++;
		}

		portal->opaque_stencil_pass_end=stencil_pass;
	}
}
