/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Rending Setup

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

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern int game_time_get(void);
extern bool fog_solid_on(void);
																						
/* =======================================================

      Setup Map Polygons
      
======================================================= */

void render_map_setup(void)
{
	int					n,k,frame;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;
	
		// run through the meshes

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		
			// clear mesh flags

		mesh->draw.has_shader=FALSE;
		mesh->draw.has_opaque=FALSE;
		mesh->draw.has_transparent=FALSE;
		mesh->draw.has_glow=FALSE;
		
			// run through the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// get texture and frame

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// set the flags

			poly->draw.frame=frame;
			poly->draw.shader_on=(texture->shader_idx!=-1);
			poly->draw.transparent_on=((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f));
			poly->draw.glow_on=(texture->frames[frame].glowmap.gl_id!=-1);

				// or to the mesh flags

			mesh->draw.has_opaque|=(!poly->draw.transparent_on);
			mesh->draw.has_transparent|=poly->draw.transparent_on;
			mesh->draw.has_shader|=poly->draw.shader_on;
			mesh->draw.has_no_shader|=(!poly->draw.shader_on);
			mesh->draw.has_glow|=poly->draw.glow_on;

			poly++;
		}
	}
}

