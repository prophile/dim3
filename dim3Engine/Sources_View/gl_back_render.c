/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Back Render Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"

GLuint						back_render_fbo_id,back_render_fbo_depth_id;
bool						back_render_on;

extern map_type				map;
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

/* =======================================================

      Initialize Back Renderer
      
======================================================= */

bool gl_back_render_initialize(char *err_str)
{
	int				status;

		// check if back rendering is enabled
		
	back_render_on=gl_check_frame_buffer_ok();
	if (!back_render_on) return(TRUE);
	
		// create depth buffer object
		
	glGenFramebuffersEXT(1,&back_render_fbo_depth_id);
	glBindFramebufferEXT(GL_RENDERBUFFER_EXT,back_render_fbo_depth_id);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_COMPONENT,back_render_texture_pixel_size,back_render_texture_pixel_size);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,0);

		// create the frame buffer object and attach depth

	glGenFramebuffersEXT(1,&back_render_fbo_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,back_render_fbo_id);

	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,back_render_fbo_depth_id);

		// is object OK?

	status=glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

	if (status!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		strcpy(err_str,"Unable to create back render FBO");
		return(FALSE);
	}

	return(TRUE);
}

void gl_back_render_shutdown(void)
{
	if (!back_render_on) return;

		// destroy frame buffer and depth

	glDeleteFramebuffersEXT(1,&back_render_fbo_id);
	glDeleteFramebuffersEXT(1,&back_render_fbo_depth_id);
}

/* =======================================================

      Per Map Back rendering
      
======================================================= */

void gl_back_render_map_start(void)
{
	int				n;
	node_type		*node;

	if (!back_render_on) return;

	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		node->back_render.txt_id=-1;
		node++;
	}
}

void gl_back_render_map_end(void)
{
	int				n;
	node_type		*node;

	if (!back_render_on) return;

	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		if (node->back_render.txt_id!=-1) {
			glDeleteTextures(1,&node->back_render.txt_id);
		}
		node++;
	}
}

/* =======================================================

      Per Frame Back rendering
      
======================================================= */

void gl_back_render_frame_node(int tick,char *node_name)
{
	int				node_idx;
	node_type		*node;

		// get node

	node_idx=map_find_node(&map,node_name);
	if (node_idx==-1) return;
	
	node=&map.nodes[node_idx];

		// alread rendered?

	if (node->back_render.render) return;

		// need to create a texture?

	if (node->back_render.txt_id==-1) {
		glGenTextures(1,&node->back_render.txt_id);
		glBindTexture(GL_TEXTURE_2D,node->back_render.txt_id);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,back_render_texture_pixel_size,back_render_texture_pixel_size,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
	}

		// render

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,back_render_fbo_id);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,node->back_render.txt_id,0);
/*
	if (!view_draw_setup_node_start(tick,node,back_render_texture_pixel_size)) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		return;
	}

	view_draw(tick);

		// finish up and return to view camera
		
	glFlush();
		
	view_draw_setup_node_finish();
*/
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
}

void gl_back_render_frame_start(int tick)
{
	int					n,k,mesh_idx,liq_idx;
	node_type			*node;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;

	if (!back_render_on) return;

		// flag rendering so we don't draw twice

	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		node->back_render.render=FALSE;
		node++;
	}

		// run through all the meshes
	
	for (n=0;n!=view.render->mesh_draw.count;n++) {
		mesh_idx=view.render->mesh_draw.items[n].idx;

		mesh=&map.mesh.meshes[mesh_idx];
		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			if (poly->camera[0]!=0x0) gl_back_render_frame_node(tick,poly->camera);
			poly++;
		}
	}

		// run through all liquids

	for (n=0;n!=view.render->liquid_draw.count;n++) {
		liq_idx=view.render->liquid_draw.items[n].idx;

		liq=&map.liquid.liquids[liq_idx];
		if (liq->camera[0]!=0x0) gl_back_render_frame_node(tick,liq->camera);
	}

}


