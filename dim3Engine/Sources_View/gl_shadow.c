/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shadow Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"

int							shadow_pbuffer_pixel_size;
unsigned long				shadow_fbo_id,shadow_fbo_txt_id;
bool						shadow_on;

extern setup_type			setup;
extern render_info_type		render_info;

// supergumba -- special hack for missing symbols in 10.3.9

#ifdef D3_OS_MAC
	#ifndef GL_EXT_framebuffer_object
		#define GL_FRAMEBUFFER_EXT                 0x8D40
		#define GL_COLOR_ATTACHMENT0_EXT           0x8CE0
		#define GL_FRAMEBUFFER_COMPLETE_EXT        0x8CD5
		void glBindFramebufferEXT(GLenum target, GLuint framebuffer) {}
		void glGenFramebuffersEXT(GLsizei n, GLuint *framebuffers) {}
		void glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {}
		GLenum glCheckFramebufferStatusEXT(GLenum target) { return(0); }
		void glDeleteFramebuffersEXT(GLsizei n, const GLuint *framebuffers) {}
	#endif
#endif

/* =======================================================

      Open and Close Shadow OpenGL Context
      
======================================================= */

bool gl_shadow_initialize(char *err_str)
{
	int				status;
	GLuint			gl_id;

		// check if shadows are enabled
		
	shadow_on=(gl_check_frame_buffer_ok()) && (setup.shadow_on);
	if (!shadow_on) return(TRUE);
	
		// best size for shadow

	shadow_pbuffer_pixel_size=shadow_pbuffer_min_pixel_size;

	if (setup.quality_mode==quality_mode_high) {
		shadow_pbuffer_pixel_size=render_info.texture_max_size;
	}
	else {
		shadow_pbuffer_pixel_size=shadow_pbuffer_min_pixel_size;
	}

	if (shadow_pbuffer_pixel_size>shadow_pbuffer_max_pixel_size) shadow_pbuffer_pixel_size=shadow_pbuffer_max_pixel_size;

		// create the frame buffer object

	glGenFramebuffersEXT(1,&gl_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,gl_id);

	shadow_fbo_id=gl_id;

		// create texture

	glGenTextures(1,&gl_id);
	glBindTexture(GL_TEXTURE_2D,gl_id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size,0,GL_RGBA,GL_UNSIGNED_BYTE,0);

	shadow_fbo_txt_id=gl_id;

		// attach texture to frame buffer object

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,shadow_fbo_txt_id,0);

		// is object OK?

	status=glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

	if (status!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		strcpy(err_str,"Unable to create frame buffer shadow buffer");
		return(FALSE);
	}

	return(TRUE);
}

void gl_shadow_shutdown(void)
{
	GLuint			gl_id;
	
	if (!shadow_on) return;

		// destroy frame buffer

	gl_id=shadow_fbo_id;
	glDeleteFramebuffersEXT(1,&gl_id);

		// destroy texture

	gl_id=shadow_fbo_txt_id;
	glDeleteTextures(1,&gl_id);
}

/* =======================================================

      Render Shadow to Texture
      
======================================================= */

inline bool gl_shadow_texture_render_start(void)
{
	if (!shadow_on) return(FALSE);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,shadow_fbo_id);
	return(TRUE);
}

inline void gl_shadow_texture_render_end(void)
{
	if (!shadow_on) return;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
}

/* =======================================================

      Bind Shadow to Texture
      
======================================================= */

inline bool gl_shadow_texture_bind_start(float alpha)
{
	GLfloat					col4[4];

	if (!shadow_on) return(FALSE);
	
		// bind the texture
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,shadow_fbo_txt_id);

		// setup the rgb combine
		
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
		// alpha combine

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
	
	col4[0]=col4[1]=col4[2]=1.0f;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
	
		// filters

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
		// clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	return(TRUE);
}

inline void gl_shadow_texture_bind_end(void)
{
	if (!shadow_on) return;

		// remove clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
		// turn off texturing
		
	glDisable(GL_TEXTURE_2D);
}
