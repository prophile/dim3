/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Check Rendering Features

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

extern bool					arch_ppc_g3;

extern setup_type			setup;
extern render_info_type		render_info;

/* =======================================================

      GL Checks
      
======================================================= */

bool gl_check_bump_ok(void)
{
	if (strstr(render_info.ext_string,"GL_ARB_texture_env_dot3")==NULL) return(FALSE);
	return(render_info.texture_unit_count>=3);
}

bool gl_check_specular_ok(void)
{
	return(render_info.texture_unit_count>=3);
}

bool gl_check_glow_ok(void)
{
	return(render_info.texture_unit_count>=3);
}

bool gl_check_frame_buffer_ok(void)
{

	// frame buffer routines not defined in 10.3.9, so we need to check this in OS X
	
#ifdef D3_OS_MAC
	#ifndef GL_EXT_framebuffer_object
		return(FALSE);
	#endif
#endif

		// otherwise just check the string
		
	return(strstr(render_info.ext_string,"GL_EXT_framebuffer_object")!=NULL);
}

bool gl_check_fsaa_ok(void)
{
	return(strstr(render_info.ext_string,"GL_ARB_multisample")!=NULL);
}

bool gl_check_texture_compress_ok(void)
{
	if (strstr(render_info.name,"NVIDIA")!=NULL) return(FALSE);
	return(strstr(render_info.ext_string,"GL_ARB_texture_compression")!=NULL);
}

bool gl_check_texture_anisotropic_filter_ok(void)
{
	return(strstr(render_info.ext_string,"GL_EXT_texture_filter_anisotropic")!=NULL);
}

bool gl_check_texture_generate_mipmaps_ok(void)
{
	if (strstr(render_info.name,"NVIDIA")!=NULL) return(FALSE);
	return(strstr(render_info.ext_string,"GL_SGIS_generate_mipmap")!=NULL);
}

bool gl_check_point_sprite_ok(void)
{
	return(strstr(render_info.ext_string,"GL_ARB_point_sprite")!=NULL);
}

bool gl_check_shader_ok(void)
{
	if (strstr(render_info.ext_string,"GL_ARB_shader_objects")==NULL) return(FALSE);
	if (strstr(render_info.ext_string,"GL_ARB_fragment_shader")==NULL) return(FALSE);
	if (strstr(render_info.ext_string,"GL_ARB_vertex_shader")==NULL) return(FALSE);
	if (strstr(render_info.ext_string,"GL_ARB_shading_language_100")==NULL) return(FALSE);

	return(TRUE);
}


