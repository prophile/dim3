/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Rending Setup

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

      Setup Map Polygons
      
======================================================= */

void render_map_setup(int mesh_cnt,int *mesh_list)
{
	int					n,k,frame;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;
	
		// run through the meshes

	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.mesh.meshes[mesh_list[n]];

			// clear mesh flags

		mesh->render.has_shader=FALSE;
		mesh->render.has_opaque=FALSE;
		mesh->render.has_transparent=FALSE;
		mesh->render.has_glow=FALSE;
		
			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture and frame

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// set the flags

			poly->render.frame=frame;
			poly->render.shader_on=(texture->shader_idx!=-1);
			poly->render.transparent_on=((texture->bitmaps[frame].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f));
			poly->render.glow_on=(texture->glowmaps[frame].gl_id!=-1);

				// or to the mesh flags

			mesh->render.has_opaque|=(!poly->render.transparent_on);
			mesh->render.has_transparent|=poly->render.transparent_on;
			mesh->render.has_shader|=poly->render.shader_on;
			mesh->render.has_no_shader|=(!poly->render.shader_on);
			mesh->render.has_glow|=poly->render.glow_on;

			poly++;
		}
	}
}

