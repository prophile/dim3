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
	map_mesh_poly_type		*poly;

	portal=map.portals;

	for (i=0;i!=map.nportal;i++) {

		mesh=portal->mesh.meshes;

		for (n=0;n!=portal->mesh.nmesh;n++) {
			
			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				poly->draw.normal[0]=0.5f;
				poly->draw.normal[1]=0.5f;
				poly->draw.normal[2]=1.0f;
				poly->draw.normal_dist_factor=0.0f;
				
				poly++;
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
	int							i;
	
	for (i=(portal_cnt-1);i>=0;i--) {
		portal_compile_gl_lists(tick,portal_list[i]);
	}

}
