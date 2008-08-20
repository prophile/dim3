/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Generic Frame Buffer Object Shadow Routines

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

extern int					shadow_pbuffer_pixel_size;

//
// supergumba
//
// 10.3.9 SDK doesn't have these symbols.
//
// Right now this is ONLY 10.4, so we need to lock this out until we move
// up to 10.4 only.  This means INTEL only for right now, and we'll hack
// fix this by using the BIG_ENDIAN define.
//
// This all needs to be fixed up for 10.4 only version
//

unsigned long					shadow_fbo_id,shadow_fbo_txt_id;

/* =======================================================

      Open and Close Shadow OpenGL Context
      
======================================================= */

bool gl_shadow_initialize_frame_buffer(int shadow_mode,char *err_str)
{
#ifndef __BIG_ENDIAN__
	int				status;
	GLuint			gl_id;

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
#endif

	return(TRUE);
}

void gl_shadow_shutdown_frame_buffer(void)
{
#ifndef __BIG_ENDIAN__
	GLuint			gl_id;

		// destroy frame buffer

	gl_id=shadow_fbo_id;
	glDeleteFramebuffersEXT(1,&gl_id);

		// destroy texture

	gl_id=shadow_fbo_txt_id;
	glDeleteTextures(1,&gl_id);
#endif
}

/* =======================================================

      Render Shadow to Texture
      
======================================================= */

bool gl_shadow_texture_render_start_frame_buffer(void)
{
#ifndef __BIG_ENDIAN__
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,shadow_fbo_id);
	return(TRUE);
#else
	return(FALSE);
#endif
}

void gl_shadow_texture_render_end_frame_buffer(void)
{
#ifndef __BIG_ENDIAN__
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
#endif
}

/* =======================================================

      Bind Shadow to Texture
      
======================================================= */

bool gl_shadow_texture_bind_start_frame_buffer(float alpha)
{
#ifndef __BIG_ENDIAN__
	GLfloat					col4[4];
	
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
#else
	return(FALSE);
#endif
}

void gl_shadow_texture_bind_end_frame_buffer(void)
{
#ifndef __BIG_ENDIAN__

		// remove clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
		// turn off texturing
		
	glDisable(GL_TEXTURE_2D);
#endif
}
