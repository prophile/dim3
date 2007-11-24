/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Arrays for Certain Drawing Operations

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

int							render_array_current_idx;
render_array_type			render_arrays[render_array_max_count];

/* =======================================================

      Setup Rendering Arrays
      
======================================================= */

bool gl_render_arrays_initialize(char *err_str)
{
	int				n,sz,sz2,bsz;
	
		// find largest needed arrays for all
		// drawing operations that use these
		// arrays
		
	sz=max_model_trig*3;								// models: 3 vertexes per triangle
	
	sz2=36*4;											// rings: number of vertexes per slice
	if (sz2>sz) sz=sz2;
	
	sz2=(max_particle_count*max_particle_trail)*4;		// particles: total quads
	if (sz2>sz) sz=sz2;
	
		// create arrays
		
	for (n=0;n!=render_array_max_count;n++) {
		
		bsz=(sz*3)*sizeof(float);
		render_arrays[n].vertex=valloc(bsz);
		render_arrays[n].color=valloc(bsz);
		render_arrays[n].normal=valloc(bsz);
		
		bsz=(sz*2)*sizeof(float);
		render_arrays[n].coord=valloc(bsz);
		
		render_arrays[n].fence=n+1;
		
		if ((render_arrays[n].vertex==NULL) || (render_arrays[n].color==NULL) || (render_arrays[n].normal==NULL) || (render_arrays[n].coord==NULL)) {
			strcpy(err_str,"Out of Memory");
			return(FALSE);
		}
	}

	return(TRUE);
}

void gl_render_arrays_shutdown(void)
{
	int				n;
	
	for (n=0;n!=render_array_max_count;n++) {
		free(render_arrays[n].vertex);
		free(render_arrays[n].color);
		free(render_arrays[n].normal);
		free(render_arrays[n].coord);
	}
}

/* =======================================================

      Render Array Frame Setup
      
======================================================= */

void gl_render_arrays_frame_start(void)
{
	render_array_current_idx=-1;
		
		// set initial token

#ifdef D3_OS_MAC
	glSetFenceAPPLE(render_arrays[0].fence);
#endif
}

void gl_render_arrays_frame_finish(void)
{
#ifdef D3_OS_MAC
	int			n;
	
		// force all tokens to be completed
		
	for (n=0;n!=render_array_max_count;n++) {
		glFinishFenceAPPLE(render_arrays[n].fence);
	}
#endif
}

/* =======================================================

      Get Proper Rendering Array
      
======================================================= */

inline void gl_render_array_start(void)
{
	render_array_current_idx++;
	if (render_array_current_idx==render_array_max_count) render_array_current_idx=0;
	
#ifdef D3_OS_MAC
	glFinishFenceAPPLE(render_arrays[render_array_current_idx].fence);
#endif
}

inline float* gl_render_array_get_current_vertex(void)
{
	return(render_arrays[render_array_current_idx].vertex);
}

inline float* gl_render_array_get_current_coord(void)
{
	return(render_arrays[render_array_current_idx].coord);
}

inline float* gl_render_array_get_current_color(void)
{
	return(render_arrays[render_array_current_idx].color);
}

inline float* gl_render_array_get_current_normal(void)
{
	return(render_arrays[render_array_current_idx].normal);
}

inline void gl_render_array_stop(void)
{
#ifdef D3_OS_MAC
	glSetFenceAPPLE(render_arrays[render_array_current_idx].fence);
#endif
}

