/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Mac PBuffer Shadow Routines

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

#include "video.h"

#ifdef D3_OS_MAC

extern int						shadow_pbuffer_pixel_size;

unsigned long					shadow_pbuffer_txt_id;

CGLContextObj					shadow_ctx,shadow_orig_ctx;
CGLPBufferObj					shadow_pbuffer;

/* =======================================================

      Open and Close Shadow OpenGL Context
      
======================================================= */

bool gl_shadow_initialize_pbuffer(int shadow_mode,char *err_str)
{
	long					npix,screen,rect[4];
	GLuint					gl_id;
	CGLError				err;
	CGDirectDisplayID		display;
	CGLContextObj			current_ctx;
	CGLPixelFormatAttribute	*att,attribs[16];
	CGLPixelFormatObj		pf;

		// get current context
		
	current_ctx=CGLGetCurrentContext();

		// create the pbuffer
		
	display=CGMainDisplayID();
		
	att=attribs;
	*att++=kCGLPFADisplayMask;
	*att++=CGDisplayIDToOpenGLDisplayMask(display);
	*att++=kCGLPFANoRecovery;
	*att++=kCGLPFAAccelerated;
	*att++=kCGLPFAColorSize;
	*att++=24;
	*att++=kCGLPFAAlphaSize;
	*att++=8;
	*att++=kCGLPFAPBuffer;
	*att=0x0;
	
	err=CGLChoosePixelFormat(attribs,&pf,&npix);
	if (err!=0) {
        sprintf(err_str,"OpenGL: Could not create shadow pixel format (Error: %s)",CGLErrorString(err));
        return(FALSE);
    }

	err=CGLCreateContext(pf,NULL,&shadow_ctx);
	CGLDestroyPixelFormat(pf);
	if (err!=0) {
        sprintf(err_str,"OpenGL: Could not create shadow context (Error: %s)",CGLErrorString(err));
        return(FALSE);
    }
	
	CGLSetCurrentContext(shadow_ctx);
		
	err=CGLCreatePBuffer(shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size,GL_TEXTURE_2D,GL_RGBA,0,&shadow_pbuffer);
	if (err!=0) {
        sprintf(err_str,"OpenGL: Could not create shadow pixel buffer (Error: %s)",CGLErrorString(err));
		CGLSetCurrentContext(NULL);
		CGLClearDrawable(shadow_ctx);
		CGLDestroyContext(shadow_ctx);
        return(FALSE);
    }
		
	CGLGetVirtualScreen(current_ctx,&screen);
	
	err=CGLSetPBuffer(shadow_ctx,shadow_pbuffer,0,0,screen);
	if (err!=0) {
        sprintf(err_str,"OpenGL: Could not attach shadow pixel buffer (Error: %s)",CGLErrorString(err));
		CGLSetCurrentContext(current_ctx);
		CGLClearDrawable(shadow_ctx);
		CGLDestroyContext(shadow_ctx);
		CGLDestroyPBuffer(shadow_pbuffer);
        return(FALSE);
    }
	
		// setup pbuffer drawing

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	rect[0]=rect[1]=0;
	rect[2]=rect[3]=shadow_pbuffer_pixel_size;
 
	CGLSetParameter(shadow_ctx,kCGLCPSwapRectangle,rect);
	CGLEnable(shadow_ctx,kCGLCESwapRectangle);
    
	glEnable(GL_SCISSOR_TEST);
	glScissor(0,0,shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,0,0,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glVertexArrayParameteriAPPLE(GL_VERTEX_ARRAY_STORAGE_HINT_APPLE,GL_STORAGE_SHARED_APPLE);
	
		// return to original context
		
	CGLSetCurrentContext(current_ctx);

		// pbuffer texture

	glGenTextures(1,&gl_id);
	shadow_pbuffer_txt_id=gl_id;

	return(TRUE);
}

void gl_shadow_shutdown_pbuffer(void)
{
	GLuint					gl_id;
	CGLContextObj			current_ctx;

	current_ctx=CGLGetCurrentContext();
	
		// destroy pbuffer
	
	CGLSetCurrentContext(shadow_ctx);
	CGLClearDrawable(shadow_ctx);
	CGLDestroyPBuffer(shadow_pbuffer);
	CGLDestroyContext(shadow_ctx);

		// return to original context
	
	CGLSetCurrentContext(current_ctx);

		// destroy pbuffer texture

	gl_id=shadow_pbuffer_txt_id;
	glDeleteTextures(1,&gl_id);
}

/* =======================================================

      Render Shadow to Texture
      
======================================================= */

bool gl_shadow_texture_render_start_pbuffer(void)
{
		// remember earlier context
		
	shadow_orig_ctx=CGLGetCurrentContext();
	
		// render to texture
	
	CGLSetCurrentContext(shadow_ctx);

	return(TRUE);
}

void gl_shadow_texture_render_end_pbuffer(void)
{
	CGLSetCurrentContext(shadow_orig_ctx);
}

/* =======================================================

      Bind Shadow to Texture
      
======================================================= */

bool gl_shadow_texture_bind_start_pbuffer(float alpha)
{
	GLfloat					col4[4];
	CGLContextObj			current_ctx;
	
		// bind the texture
		
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,shadow_pbuffer_txt_id);
	
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

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
	
	col4[0]=col4[1]=col4[2]=1;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
	
		// clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

		// set texture
	
	current_ctx=CGLGetCurrentContext();
	CGLTexImagePBuffer(current_ctx,shadow_pbuffer,GL_FRONT);

	return(TRUE);
}

void gl_shadow_texture_bind_end_pbuffer(void)
{
		// remove clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
		// turn off texturing
		
	glDisable(GL_TEXTURE_2D);
}

#endif