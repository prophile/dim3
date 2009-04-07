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

extern bool view_draw_node(int tick,node_type *node,int pixel_size);

/* =======================================================

      Initialize Back Renderer
      
======================================================= */

bool gl_back_render_initialize(char *err_str)
{
		// check if back rendering is enabled
		
	back_render_on=gl_check_frame_buffer_ok();
	if (!back_render_on) return(TRUE);
	
		// create depth buffer object
	
	glGenTextures(1,&back_render_fbo_depth_id);
	glBindTexture(GL_TEXTURE_2D,back_render_fbo_depth_id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,back_render_texture_pixel_size,back_render_texture_pixel_size,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);

		// create the frame buffer object and attach depth

	glGenFramebuffersEXT(1,&back_render_fbo_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,back_render_fbo_id);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D,back_render_fbo_depth_id,0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

	return(TRUE);
}

void gl_back_render_shutdown(void)
{
	if (!back_render_on) return;

		// destroy frame buffer and depth

	glDeleteFramebuffersEXT(1,&back_render_fbo_id);
	glDeleteTextures(1,&back_render_fbo_depth_id);
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

      Back Render Texture
      
======================================================= */

GLuint gl_back_render_create_texture(void)
{
	GLuint				gl_id;
	
	glGenTextures(1,&gl_id);
	glBindTexture(GL_TEXTURE_2D,gl_id);
	
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,back_render_texture_pixel_size,back_render_texture_pixel_size,0,GL_RGBA,GL_UNSIGNED_BYTE,0);

		// animsotropic modes
		
	switch (setup.anisotropic_mode) {
	
		case anisotropic_mode_none:
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.0);
			break;
			
		case anisotropic_mode_low:
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,2.0);
			break;
			
		case anisotropic_mode_medium:
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,4.0);
			break;
			
		case anisotropic_mode_high:
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,8.0);
			break;
			
	}

		// mipmap modes
		
	switch (setup.mipmap_mode) {
	
		case mipmap_mode_none:
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;
			
		case mipmap_mode_bilinear:
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;
			
		case mipmap_mode_trilinear:
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;
			
	}
	
	glBindTexture(GL_TEXTURE_2D,0);
	
	return(gl_id);
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
		node->back_render.txt_id=gl_back_render_create_texture();
	}

		// setup fbo

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,back_render_fbo_id);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,node->back_render.txt_id,0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,back_render_fbo_depth_id);
	
	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		return;
	}

		// draw back buffer
		
	view_draw_node(tick,node,back_render_texture_pixel_size);
	glFlush();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

		// generate mipmaps
		
	glBindTexture(GL_TEXTURE_2D,node->back_render.txt_id);
	glGenerateMipmapEXT(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,0);
	
		// mark as rendered for this frame
		
	node->back_render.render=TRUE;
}

void gl_back_render_frame_start(int tick)
{
	int					n,k,mesh_idx;
	node_type			*node;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
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
}

/* =======================================================

      Back Render Texture
      
======================================================= */

bool gl_back_render_get_texture(char *node_name,GLuint *txt_id)
{
	int				node_idx;
	node_type		*node;
	
	if (node_name[0]==0x0) return(FALSE);

		// get node
		
	node_idx=map_find_node(&map,node_name);
	if (node_idx==-1) return(FALSE);
	
	node=&map.nodes[node_idx];
	
		// get back render texture
		
	if (!node->back_render.render) return(FALSE);

	*txt_id=node->back_render.txt_id;
	return(TRUE);
}


