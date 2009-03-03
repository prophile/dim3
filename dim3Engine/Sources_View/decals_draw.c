/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Decals

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
#include "lights.h"
#include "video.h"

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

extern int game_time_get(void);

/* =======================================================

      Decals
      
======================================================= */

void decal_render_stencil(int stencil_idx,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int			n;
	d3pnt		*pt;

		// already stenciled for decal?

	if (mesh_poly->draw.decal_stencil_idx!=0) return;

		// stencil

	glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);

	glBegin(GL_POLYGON);

	for (n=0;n!=mesh_poly->ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		glVertex3i(pt->x,pt->y,pt->z);
	}
	
	glEnd();

		// remember stencil

	mesh_poly->draw.decal_stencil_idx=stencil_idx;
}

void decal_render_mark(int stencil_idx,decal_type *decal)
{
	int				k,tick,fade_out_start_tick;
	float			alpha,g_size,gx,gy,cf[3];
	mark_type		*mark;
	
		// get the alpha
	
	mark=&server.marks[decal->mark_idx];
		
	alpha=decal->alpha;
	tick=game_time_get()-decal->start_tick;
	
	if (tick<mark->fade_in_msec) {
		alpha=(alpha*(float)tick)/(float)mark->fade_in_msec;
	}
	else {
		fade_out_start_tick=mark->fade_in_msec+mark->life_msec;
		if (tick>=fade_out_start_tick) {
			k=tick-fade_out_start_tick;
			alpha=(alpha*(float)(mark->fade_out_msec-k))/(float)mark->fade_out_msec;
		}
	}
	
		// get the decal image
		
	effect_image_animate_get_uv(tick,&mark->animate,&gx,&gy,&g_size);

		// get lighting
		
	gl_lights_calc_vertex((double)decal->x[0],(double)decal->y[0],(double)decal->z[0],cf);
	
         // draw the polygon
			
	glStencilFunc(GL_EQUAL,stencil_idx,0xFF);
	gl_texture_decal_set(view_images_get_gl_id(server.marks[decal->mark_idx].image_idx),cf[0],cf[1],cf[2],alpha);
	
	glBegin(GL_QUADS);
    glTexCoord2f(gx,gy);
    glVertex3i(decal->x[0],decal->y[0],decal->z[0]);
    glTexCoord2f((gx+g_size),gy);
    glVertex3i(decal->x[1],decal->y[1],decal->z[1]);
    glTexCoord2f((gx+g_size),(gy+g_size));
    glVertex3i(decal->x[2],decal->y[2],decal->z[2]);
    glTexCoord2f(gx,(gy+g_size));
    glVertex3i(decal->x[3],decal->y[3],decal->z[3]);
    glEnd();
}

void decal_render(int mesh_draw_count,int *mesh_draw_list)
{
	int					n,k,stencil_idx;
	decal_type			*decal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;

	if (server.count.decal==0) return;

		// clear all rendering stencil marks
		// and detect if in view

	decal=server.decals;

	for (n=0;n!=server.count.decal;n++) {
	
		decal->in_view=FALSE;
		
		for (k=0;k!=mesh_draw_count;k++) {
			if (mesh_draw_list[k]==decal->mesh_idx) {
				decal->in_view=TRUE;
			}
		}
		
		if (decal->in_view) {
			mesh_poly=&map.mesh.meshes[decal->mesh_idx].polys[decal->poly_idx];
			mesh_poly->draw.decal_stencil_idx=0;
		}
		
		decal++;
	}

		// decals use stenciling

	glEnable(GL_STENCIL_TEST);

		// stencil decal segments

	glDisable(GL_BLEND);
			
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	stencil_idx=stencil_poly_start;
	decal=server.decals;

	for (n=0;n!=server.count.decal;n++) {

		if (decal->in_view) {
			mesh=&map.mesh.meshes[decal->mesh_idx];
			mesh_poly=&mesh->polys[decal->poly_idx];
			decal_render_stencil(stencil_idx,mesh,mesh_poly);
			stencil_idx++;
		}
		
		decal++;
	}

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

		// draw decals to stencils

	gl_texture_decal_start();
				
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glDisable(GL_DEPTH_TEST);
			
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

	decal=server.decals;

	for (n=0;n!=server.count.decal;n++) {
	
		if (decal->in_view) {
			mesh_poly=&map.mesh.meshes[decal->mesh_idx].polys[decal->poly_idx];
			decal_render_mark(mesh_poly->draw.decal_stencil_idx,decal);
		}
		
		decal++;
	}

	gl_texture_decal_end();
	
	glDisable(GL_STENCIL_TEST);
}

