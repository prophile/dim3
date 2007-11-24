/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Win32 PBuffer Shadow Routines

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

#ifdef D3_OS_WINDOWS

extern int						shadow_pbuffer_pixel_size;

unsigned long					shadow_pbuffer_txt_id;

HDC								shadow_hdc,shadow_orig_hdc;
HGLRC							shadow_ctx,shadow_orig_ctx;
HPBUFFERARB						shadow_pbuffer;

/* =======================================================

      Open and Close Shadow OpenGL Context
      
======================================================= */

bool gl_shadow_initialize_pbuffer(int shadow_mode,char *err_str)
{
	int				pf,pwid,phigh,
					*att,attribs[16];
	unsigned int	count;
	GLuint			gl_id;
	HDC				current_hdc;
	HGLRC			current_ctx;

		// get current context

	current_hdc=wglGetCurrentDC();
	current_ctx=wglGetCurrentContext();

		// create pbuffer

	att=attribs;
	*att++=WGL_SUPPORT_OPENGL_ARB;
	*att++=TRUE;
	*att++=WGL_ACCELERATION_ARB;
	*att++=WGL_FULL_ACCELERATION_ARB;
	*att++=WGL_DRAW_TO_PBUFFER_ARB;
	*att++=TRUE;
	*att++=WGL_DOUBLE_BUFFER_ARB;
	*att++=FALSE;
	*att++=WGL_COLOR_BITS_ARB;
	*att++=24;
	*att++=WGL_ALPHA_BITS_ARB;
	*att++=8;
	*att++=WGL_DEPTH_BITS_ARB;
	*att++=0;
	*att=0x0;

	if (!wglChoosePixelFormatARB(current_hdc,attribs,NULL,1,&pf,&count)) {
		strcpy(err_str,"Unable to create pbuffer shadow pixel format");
		return(FALSE);
	}

	att=attribs;
	*att++=WGL_TEXTURE_FORMAT_ARB;
	*att++=WGL_TEXTURE_RGBA_ARB;
	*att++=WGL_TEXTURE_TARGET_ARB;
	*att++=WGL_TEXTURE_2D_ARB;
	*att++=WGL_PBUFFER_LARGEST_ARB;
	*att++=1;
	*att=0x0;

	shadow_pbuffer=wglCreatePbufferARB(current_hdc,pf,shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size,attribs);
	if (shadow_pbuffer==NULL) {
		strcpy(err_str,"Unable to create pbuffer shadow buffer");
		return(FALSE);
	}

	shadow_hdc=wglGetPbufferDCARB(shadow_pbuffer);
	shadow_ctx=wglCreateContext(shadow_hdc);

		// is this pbuffer OK?
		// these potentially leak the context, but I don't really
		// expect these to fail on almost any hardware, basically
		// just here as sanity checks to stop further crashes

	wglQueryPbufferARB(shadow_pbuffer,WGL_PBUFFER_HEIGHT_ARB,&phigh);
	wglQueryPbufferARB(shadow_pbuffer,WGL_PBUFFER_WIDTH_ARB,&pwid);

	if ((phigh!=shadow_pbuffer_pixel_size) || (pwid!=shadow_pbuffer_pixel_size)) {
		strcpy(err_str,"Unable to create pbuffer shadow of correct size");
		return(FALSE);
	}

	if (!wglShareLists(current_ctx,shadow_ctx)) {
		strcpy(err_str,"Unable to share pbuffer shadow lists");
		return(FALSE);
	}

		// setup pbuffer drawing

	wglMakeCurrent(shadow_hdc,shadow_ctx);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_SCISSOR_TEST);
	glScissor(0,0,shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,0,0,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
		// return to original context
		
	wglMakeCurrent(current_hdc,current_ctx);

		// pbuffer texture

	glGenTextures(1,&gl_id);
	glBindTexture(GL_TEXTURE_2D,gl_id);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size,0,GL_RGBA,GL_UNSIGNED_BYTE,0);

	shadow_pbuffer_txt_id=gl_id;

	return(TRUE);
}

void gl_shadow_shutdown_pbuffer(void)
{
	GLuint			gl_id;
	HDC				current_hdc;
	HGLRC			current_ctx;
	SDL_SysWMinfo	info;

	current_hdc=wglGetCurrentDC();
	current_ctx=wglGetCurrentContext();

		// destroy pbuffer

	wglMakeCurrent(shadow_hdc,shadow_ctx);
	wglDeleteContext(shadow_ctx);
	wglReleasePbufferDCARB(shadow_pbuffer,shadow_hdc);
	wglDestroyPbufferARB(shadow_pbuffer);

    SDL_GetWMInfo(&info);
	ReleaseDC(info.window,shadow_hdc);

		// return to original context

	wglMakeCurrent(current_hdc,current_ctx);

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
		
	shadow_orig_hdc=wglGetCurrentDC();
	shadow_orig_ctx=wglGetCurrentContext();
	
		// render to texture
	
	wglMakeCurrent(shadow_hdc,shadow_ctx);

	return(TRUE);
}

void gl_shadow_texture_render_end_pbuffer(void)
{
		// note that this is a slow way to do this
		// as must renders don't seem to support a more
		// direct way (use the frame buffer objects instead)

	glBindTexture(GL_TEXTURE_2D,shadow_pbuffer_txt_id);
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size);
	
	wglMakeCurrent(shadow_orig_hdc,shadow_orig_ctx);
}

/* =======================================================

      Bind Shadow to Texture
      
======================================================= */

bool gl_shadow_texture_bind_start_pbuffer(float alpha)
{
	GLfloat					col4[4];
	
		// bind the texture
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,shadow_pbuffer_txt_id);

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

void gl_shadow_texture_bind_end_pbuffer(void)
{
		// remove clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
		// turn off texturing
		
	glDisable(GL_TEXTURE_2D);
}

#endif