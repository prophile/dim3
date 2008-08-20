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
bool						shadow_on,shadow_use_frame_buffer;

extern render_info_type		render_info;

/* =======================================================

      Open and Close Shadow OpenGL Context
      
======================================================= */

bool gl_shadow_initialize(int shadow_mode,char *err_str)
{
		// defaults

	shadow_on=FALSE;
	shadow_pbuffer_pixel_size=shadow_pbuffer_min_pixel_size;

		// is shadow on?

	shadow_on=FALSE;

	if (!gl_check_shadow_ok()) return(TRUE);
		
	shadow_on=(shadow_mode!=shadow_mode_none);
	if (!shadow_on) return(TRUE);

		// best size for shadow

	if (shadow_mode==shadow_mode_low) {
		shadow_pbuffer_pixel_size=shadow_pbuffer_min_pixel_size;
	}
	else {
		shadow_pbuffer_pixel_size=render_info.texture_max_size;
	}

	if (shadow_pbuffer_pixel_size>shadow_pbuffer_max_pixel_size) shadow_pbuffer_pixel_size=shadow_pbuffer_max_pixel_size;

		// get proper shadow drawing mode

	shadow_use_frame_buffer=gl_check_frame_buffer_ok();
	
	fprintf(stdout,"frame buffer = %s\n",shadow_use_frame_buffer?"true":"false");

#ifdef D3_OS_MAC
	shadow_use_frame_buffer=FALSE;		// supergumba -- hack to work around 10.3.9 problem, remove after going to 10.4 only
#endif

		// run proper method

	if (shadow_use_frame_buffer) {
		return(gl_shadow_initialize_frame_buffer(shadow_mode,err_str));
	}

	return(gl_shadow_initialize_pbuffer(shadow_mode,err_str));
}

void gl_shadow_shutdown(void)
{
	if (!shadow_on) return;

	if (shadow_use_frame_buffer) {
		gl_shadow_shutdown_frame_buffer();
	}
	else {
		gl_shadow_shutdown_pbuffer();
	}
}

/* =======================================================

      Render Shadow to Texture
      
======================================================= */

inline bool gl_shadow_texture_render_start(void)
{
	if (!shadow_on) return(FALSE);

	if (shadow_use_frame_buffer) {
		return(gl_shadow_texture_render_start_frame_buffer());
	}

	return(gl_shadow_texture_render_start_pbuffer());
}

inline void gl_shadow_texture_render_end(void)
{
	if (!shadow_on) return;

	if (shadow_use_frame_buffer) {
		gl_shadow_texture_render_end_frame_buffer();
	}
	else {
		gl_shadow_texture_render_end_pbuffer();
	}
}

/* =======================================================

      Bind Shadow to Texture
      
======================================================= */

inline bool gl_shadow_texture_bind_start(float alpha)
{
	if (!shadow_on) return(FALSE);

	if (shadow_use_frame_buffer) {
		return(gl_shadow_texture_bind_start_frame_buffer(alpha));
	}

	return(gl_shadow_texture_bind_start_pbuffer(alpha));
}

inline void gl_shadow_texture_bind_end(void)
{
	if (!shadow_on) return;

	if (shadow_use_frame_buffer) {
		gl_shadow_texture_bind_end_frame_buffer();
	}
	else {
		gl_shadow_texture_bind_end_pbuffer();
	}
}
