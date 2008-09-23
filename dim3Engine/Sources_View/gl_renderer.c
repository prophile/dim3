/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Render Setup

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

#include "video.h"

extern setup_type			setup;
extern render_info_type		render_info;

SDL_Surface					*surface;

int							orig_scrn_x,orig_scrn_y,orig_scrn_refresh;

/* =======================================================

      Detect if in Windowed Mode
      
======================================================= */

inline bool gl_in_window_mode(void)
{
	return((setup.window) || ((setup.editor_override.on) && (setup.window_editor)));
}

/* =======================================================

      Open and Close OpenGL Context
      
======================================================= */

bool gl_initialize(int screen_wid,int screen_high,bool lock_fps_refresh,int fsaa_mode,bool reset,char *err_str)
{
    GLint				ntxtunit,ntxtsize;
#ifdef D3_OS_MAC
    long				swapint,rect[4];
	CGLContextObj		current_ctx;
	CFDictionaryRef		mode_info;
	CFNumberRef			cf_rate;
#else
	GLenum				glew_error;
#endif
		
		// setup rendering sizes
        
	setup.screen.x_sz=screen_wid;
	setup.screen.y_sz=screen_high;
	
		// normal attributes
		
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	
		// full screen anti-aliasing attributes
		
	switch (fsaa_mode) {
		case fsaa_mode_low:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,2);
			break;
		case fsaa_mode_medium:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
			break;
		case fsaa_mode_high:
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,6);
			break;
	}

		// start window or full screen
		
	if (gl_in_window_mode()) {
		surface=SDL_SetVideoMode(setup.screen.x_sz,setup.screen.y_sz,32,SDL_OPENGL|SDL_HWSURFACE);
	}
	else {
		surface=SDL_SetVideoMode(setup.screen.x_sz,setup.screen.y_sz,32,SDL_OPENGL|SDL_FULLSCREEN);
	}

	if (surface==NULL) {
		sprintf(err_str,"SDL: Could not set video mode (Error: %s)",SDL_GetError());
		return(FALSE);
	}
	
		// work around the dock losing minimize buttons because
		// of SDL luncancy
		
#ifdef D3_OS_MAC
	if (!gl_in_window_mode()) SetSystemUIMode(kUIModeContentSuppressed,0);
#endif
	
		// use glew on linux and windows
		
#ifndef D3_OS_MAC
	glew_error=glewInit();
	if (glew_error!=GL_NO_ERROR) {
		strcpy(err_str,glewGetErrorString(glew_error));
		return(FALSE);
	}
#endif

		// grab openGL attributes
		
	strncpy(render_info.name,(char*)glGetString(GL_RENDERER),64);
	render_info.name[63]=0x0;
	
	strncpy(render_info.ext_string,(char*)glGetString(GL_EXTENSIONS),8192);
	render_info.ext_string[8191]=0x0;
			
	glGetIntegerv(GL_MAX_TEXTURE_UNITS,&ntxtunit);
	render_info.texture_unit_count=(int)ntxtunit;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&ntxtsize);
	render_info.texture_max_size=(int)ntxtsize;

		// in case screen is bigger than desired drawing surface
		
    render_info.monitor_x_sz=surface->w;
	render_info.monitor_y_sz=surface->h;

	render_info.view_x=(render_info.monitor_x_sz-setup.screen.x_sz)>>1;
	render_info.view_y=(render_info.monitor_y_sz-setup.screen.y_sz)>>1;
	
		// determine the referesh rate

	render_info.monitor_refresh_rate=60;				// windows XP has a stuck refresh rate of 60
		
#ifdef D3_OS_MAC
	mode_info=CGDisplayCurrentMode(CGMainDisplayID());
	if (mode_info!=NULL) {
		cf_rate=(CFNumberRef)CFDictionaryGetValue(mode_info,kCGDisplayRefreshRate);
		if (cf_rate) {
			CFNumberGetValue(cf_rate,kCFNumberIntType,&render_info.monitor_refresh_rate);
			if (render_info.monitor_refresh_rate==0) render_info.monitor_refresh_rate=60;
		}
	}
#endif

        // clear the entire window so it doesn't flash
        
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
   
	SDL_GL_SwapBuffers();

        // setup renderer

#ifdef D3_OS_MAC
	current_ctx=CGLGetCurrentContext();
	
	rect[0]=render_info.view_x;
	rect[1]=render_info.view_y;
	rect[2]=setup.screen.x_sz;
	rect[3]=setup.screen.y_sz;
 
 	CGLSetParameter(current_ctx,kCGLCPSwapRectangle,rect);
	CGLEnable(current_ctx,kCGLCESwapRectangle);

	if (lock_fps_refresh) {
		swapint=1;
		CGLSetParameter(current_ctx,kCGLCPSwapInterval,&swapint);
	}	
#endif
    
	glEnable(GL_SCISSOR_TEST);
	glScissor(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz);

	glViewport(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz);
	
		// perspective correction
		
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		
		// smoothing and anti-aliasing
		
	glDisable(GL_DITHER);
	
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	
	if (fsaa_mode!=fsaa_mode_none) glEnable(GL_MULTISAMPLE);
	
		// texture compression
		
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
	glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
	
		// all alphas by 8 bit component
		
	glDisable(GL_ALPHA_TEST);

		// texture utility initialize
		
	gl_texture_initialize();
	
		// do an initial draw
		
	if (!reset) {
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		
		SDL_GL_SwapBuffers();
	}
	
	return(TRUE);
}
	
void gl_shutdown(void)
{
	gl_texture_shutdown();
	
	// SDL_Quit handles all the clean up
}

/* =======================================================

      Check Widescreen
      
======================================================= */

bool gl_is_size_widescreen(int wid,int high)
{
	return(((float)high/(float)wid)<=0.625f);
}

bool gl_is_screen_widescreen(void)
{
	return(gl_is_size_widescreen(setup.screen.x_sz,setup.screen.y_sz));
}

/* =======================================================

      ScreenShots
      
======================================================= */

bool gl_screen_shot(int lft_x,int top_y,int wid,int high,bool thumbnail,char *path)
{
	int					x,y,x_skip,y_skip,
						ss_wid,ss_high,dsz;
	unsigned char		*pixel_buffer,*data,*sptr,*dptr,*s2ptr,*d2ptr;
	bool				ok;
	
	pixel_buffer=(unsigned char*)valloc((wid*3)*high);
	if (pixel_buffer==NULL) return(FALSE);
	
	glReadPixels(lft_x,top_y,wid,high,GL_RGB,GL_UNSIGNED_BYTE,pixel_buffer);
	
		// is there a limiting?
		
	x_skip=y_skip=1;
	ss_wid=wid;
	ss_high=high;
	
	if (thumbnail) {
		x_skip=wid/128;
		y_skip=high/128;
		ss_wid=ss_high=128;
	}
	
		// flip and/or reduce the data
		
	dsz=((ss_wid<<2)*ss_high);
	
	data=(unsigned char*)valloc(dsz);
	if (data==NULL) {
		free(pixel_buffer);
		return(FALSE);
	}
	
	bzero(data,dsz);
	
	sptr=pixel_buffer;
	dptr=data+((ss_high-1)*(ss_wid<<2));

	for (y=0;y!=ss_high;y++) {
	
		s2ptr=sptr;
		d2ptr=dptr;
		
		for (x=0;x!=ss_wid;x++) {
			*d2ptr++=*s2ptr++;
			*d2ptr++=*s2ptr++;
			*d2ptr++=*s2ptr++;
			*d2ptr++=0xFF;
			
			s2ptr+=((x_skip-1)*3);
		}
			
		sptr+=((wid*3)*y_skip);
		dptr-=(ss_wid<<2);
	}

	free(pixel_buffer);

		// save screenshot

	ok=bitmap_write_png_data(data,ss_wid,ss_high,path);
		
	free(data);
	
	return(ok);
}
	
