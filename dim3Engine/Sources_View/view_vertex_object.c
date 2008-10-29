/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Vertex Objects

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

// supergumba -- move this

#define view_vertex_object_count			8

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							cur_vbo_cache_idx,
							vbo_map_sz,vbo_cache_sz[view_vertex_object_count];
GLuint						vbo_map,vbo_cache[view_vertex_object_count];

/* =======================================================

      Create and Dispose Vertex Objects
      
======================================================= */

void view_create_vertex_objects(void)
{
	int			n;

		// map vbo

	glGenBuffersARB(1,&vbo_map);

		// misc vbos

	glGenBuffersARB(view_vertex_object_count,vbo_cache);

		// initial sizes
		// we only remap the buffer is the size is greater
		// while this cost us memory, we don't get the cost of
		// reseting these buffers constantly

	vbo_map_sz=-1;

	for (n=0;n!=view_vertex_object_count;n++) {
		vbo_cache_sz[n]=-1;
	}

		// start at first misc vbo

	cur_vbo_cache_idx=0;
}

void view_dispose_vertex_objects(void)
{
	glDeleteBuffersARB(1,&vbo_map);
	glDeleteBuffersARB(view_vertex_object_count,vbo_cache);
}

/* =======================================================

      Map Vertex Object
      
======================================================= */

inline void view_resize_map_vertex_object(int sz)
{
	if (sz>vbo_map_sz) {
		vbo_map_sz=sz;
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);
	}
}

inline void view_bind_map_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_map);
}

inline void view_unbind_map_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Model, Particle, Etc Vertexe Objects
      
======================================================= */

inline void view_next_vertex_object(void)
{
	cur_vbo_cache_idx++;
	if (cur_vbo_cache_idx==view_vertex_object_count) cur_vbo_cache_idx=0;
}

inline void view_resize_current_vertex_object(int sz)
{
	if (sz>vbo_cache_sz[cur_vbo_cache_idx]) {
		vbo_cache_sz[cur_vbo_cache_idx]=sz;
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);
	}
}


inline void view_bind_current_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_cache[cur_vbo_cache_idx]);
}

inline void view_unbind_current_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}
