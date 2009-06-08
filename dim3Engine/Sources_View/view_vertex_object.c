/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Vertex Objects

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

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							cur_vbo_cache_idx,cur_vbo_cache_index_idx,
							vbo_cache_sz[view_vertex_object_count],
							vbo_cache_index_sz[view_vertex_object_count];
GLuint						vbo_map,vbo_map_index,
							vbo_liquid,vbo_liquid_index,
							vbo_sky,
							vbo_cache[view_vertex_object_count],
							vbo_cache_index[view_vertex_object_count];

/* =======================================================

      Create and Dispose Vertex Objects
      
======================================================= */

void view_create_vertex_objects(void)
{
	int			n;

		// map, liquid and sky vbo

	glGenBuffersARB(1,&vbo_map);
	glGenBuffersARB(1,&vbo_map_index);

	glGenBuffersARB(1,&vbo_liquid);
	glGenBuffersARB(1,&vbo_liquid_index);

	glGenBuffersARB(1,&vbo_sky);

		// misc vbos

	glGenBuffersARB(view_vertex_object_count,vbo_cache);
	glGenBuffersARB(view_vertex_object_count,vbo_cache_index);

		// initial sizes
		// we only remap the buffer is the size is greater
		// while this cost us memory, we don't get the cost of
		// reseting these buffers constantly

	for (n=0;n!=view_vertex_object_count;n++) {
		vbo_cache_sz[n]=-1;
		vbo_cache_index_sz[n]=-1;
	}

		// start at first misc vbo

	cur_vbo_cache_idx=0;
	cur_vbo_cache_index_idx=0;
}

void view_dispose_vertex_objects(void)
{
	glDeleteBuffersARB(1,&vbo_map);
	glDeleteBuffersARB(1,&vbo_map_index);

	glDeleteBuffersARB(1,&vbo_liquid);
	glDeleteBuffersARB(1,&vbo_liquid_index);

	glDeleteBuffersARB(1,&vbo_sky);

	glDeleteBuffersARB(view_vertex_object_count,vbo_cache);
	glDeleteBuffersARB(view_vertex_object_count,vbo_cache_index);
}

/* =======================================================

      Map VBOs
      
======================================================= */

void view_init_map_vertex_object(int sz)
{
		// create map geometery buffer

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_map);

	sz*=sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_DYNAMIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

inline float* view_bind_map_map_vertex_object(void)
{
	float		*vertex_ptr;

		// bind to map specific VBO

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_map);

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(vertex_ptr);
}

inline void view_bind_map_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_map);
}

inline void view_unmap_map_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_map_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

void view_init_map_index_object(int sz)
{
		// create map index buffer

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_map_index);

	sz*=sizeof(unsigned int);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,sz,NULL,GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

inline unsigned int* view_bind_map_map_index_object(void)
{
	unsigned int		*index_ptr;

		// bind to map specific VBO

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_map_index);

		// map pointer

	index_ptr=(unsigned int*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (index_ptr==NULL) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(index_ptr);
}

inline void view_bind_map_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_map_index);
}

inline void view_unmap_map_index_object(void)
{
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
}

inline void view_unbind_map_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Liquid VBOs
      
======================================================= */

void view_init_liquid_vertex_object(int sz)
{
		// create liquid geometery buffer

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_liquid);

	sz*=sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_DYNAMIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

inline float* view_bind_map_liquid_vertex_object(void)
{
	float		*vertex_ptr;

		// bind to liquid specific VBO

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_liquid);

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(vertex_ptr);
}

inline void view_unmap_liquid_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_liquid_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

void view_init_liquid_index_object(int sz)
{
		// create liquid index buffer

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_liquid_index);

	sz*=sizeof(unsigned int);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,sz,NULL,GL_DYNAMIC_DRAW_ARB);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

inline unsigned int* view_bind_map_liquid_index_object(void)
{
	unsigned int		*index_ptr;

		// bind to liquid specific VBO

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_liquid_index);

		// map pointer

	index_ptr=(unsigned int*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (index_ptr==NULL) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(index_ptr);
}

inline void view_unmap_liquid_index_object(void)
{
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
}

inline void view_unbind_liquid_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Sky VBOs
      
======================================================= */

void view_init_sky_vertex_object(int sz)
{
		// create sky geometery buffer

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_sky);

	sz*=sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_DYNAMIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

inline float* view_bind_map_sky_vertex_object(void)
{
	float		*vertex_ptr;

		// bind to sky specific VBO

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_sky);

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

	return(vertex_ptr);
}

inline void view_bind_sky_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_sky);
}

inline void view_unmap_sky_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_sky_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Model, Particle, Etc VBOs
      
======================================================= */

float* view_bind_map_next_vertex_object(int sz)
{
	float			*vertex_ptr;

		// bind it

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_cache[cur_vbo_cache_idx]);

		// resize it if more memory needed
		// we only grow up, this *might* be a problem
		// but it seems more optimal for now.  Will require
		// more testing

		// size is in floats

	sz*=sizeof(float);

	if (sz>vbo_cache_sz[cur_vbo_cache_idx]) {

			// we need to grab the pointer first so if the data
			// is still being used then we'll be stalled

		if (vbo_cache_sz[cur_vbo_cache_idx]!=-1) {
			vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
			glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
		}

			// now we can change the size

		vbo_cache_sz[cur_vbo_cache_idx]=sz;
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);
	}

		// map pointer

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

		// get next object

	cur_vbo_cache_idx++;
	if (cur_vbo_cache_idx==view_vertex_object_count) cur_vbo_cache_idx=0;

	return(vertex_ptr);
}

inline void view_unmap_current_vertex_object(void)
{
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
}

inline void view_unbind_current_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

unsigned short* view_bind_map_next_index_object(int sz)
{
	unsigned short		*index_ptr;

		// bind it

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_cache_index[cur_vbo_cache_index_idx]);

		// size is in unsigned short

	sz*=sizeof(unsigned short);

	if (sz>vbo_cache_index_sz[cur_vbo_cache_index_idx]) {

			// we need to grab the pointer first so if the data
			// is still being used then we'll be stalled

		if (vbo_cache_index_sz[cur_vbo_cache_index_idx]!=-1) {
			index_ptr=(unsigned short*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
			glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
		}

			// now we can change the size

		vbo_cache_index_sz[cur_vbo_cache_index_idx]=sz;
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);
	}

		// map pointer

	index_ptr=(unsigned short*)glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (index_ptr==NULL) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
		return(NULL);
	}

		// get next object

	cur_vbo_cache_index_idx++;
	if (cur_vbo_cache_index_idx==view_vertex_object_count) cur_vbo_cache_index_idx=0;

	return(index_ptr);
}

inline void view_unmap_current_index_object(void)
{
	glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
}

inline void view_unbind_current_index_object(void)
{
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);
}

/* =======================================================

      Some VBO Utility Routines
      
======================================================= */

void view_draw_next_vertex_object_2D_texture_screen(int wid,int high,float gx,float gy)
{
	float			*vertex_ptr,*uv_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(4*2);

		// get the vertexes

	*vertex_ptr++=0.0f;
	*vertex_ptr++=0.0f;

	*uv_ptr++=gx;
	*uv_ptr++=gy;

	*vertex_ptr++=(float)wid;
	*vertex_ptr++=0.0f;

	*uv_ptr++=gx+1.0f;
	*uv_ptr++=gy;

	*vertex_ptr++=(float)wid;
	*vertex_ptr++=(float)high;

	*uv_ptr++=gx+1.0f;
	*uv_ptr++=gy+1.0f;

	*vertex_ptr++=0.0f;
	*vertex_ptr++=(float)high;

	*uv_ptr++=gx;
	*uv_ptr++=gy+1.0f;

  	view_unmap_current_vertex_object();

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_tint_screen(void)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*2);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=0.0f;
	*vertex_ptr++=0.0f;

	*vertex_ptr++=(float)setup.screen.x_sz;
	*vertex_ptr++=0.0f;

	*vertex_ptr++=(float)setup.screen.x_sz;
	*vertex_ptr++=(float)setup.screen.y_sz;

	*vertex_ptr++=0.0f;
	*vertex_ptr++=(float)setup.screen.y_sz;

  	view_unmap_current_vertex_object();

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_texture_quad(int lft,int rgt,int top,int bot)
{
	float			*vertex_ptr,*uv_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(4*2);

		// get the vertexes

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)top;

	*uv_ptr++=0.0f;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)top;

	*uv_ptr++=1.0f;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=1.0f;
	*uv_ptr++=1.0f;

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=0.0f;
	*uv_ptr++=1.0f;

  	view_unmap_current_vertex_object();

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_color_quad(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha)
{
	float			*vertex_ptr,*col_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+4));
	if (vertex_ptr==NULL) return;

	col_ptr=vertex_ptr+(4*2);

		// get the vertexes

	*vertex_ptr++=(float)x0;
	*vertex_ptr++=(float)y0;

	*col_ptr++=col0->r;
	*col_ptr++=col0->g;
	*col_ptr++=col0->b;
	*col_ptr++=alpha;

	*vertex_ptr++=(float)x1;
	*vertex_ptr++=(float)y1;

	*col_ptr++=col1->r;
	*col_ptr++=col1->g;
	*col_ptr++=col1->b;
	*col_ptr++=alpha;

	*vertex_ptr++=(float)x2;
	*vertex_ptr++=(float)y2;

	*col_ptr++=col2->r;
	*col_ptr++=col2->g;
	*col_ptr++=col2->b;
	*col_ptr++=alpha;

	*vertex_ptr++=(float)x3;
	*vertex_ptr++=(float)y3;

	*col_ptr++=col3->r;
	*col_ptr++=col3->g;
	*col_ptr++=col3->b;
	*col_ptr++=alpha;

  	view_unmap_current_vertex_object();

		// draw the quad
		
	if (alpha!=1.0f) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

 	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_BLEND);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void view_draw_next_vertex_object_2D_line_quad(int lft,int rgt,int top,int bot)
{
	float			*vertex_ptr;

	vertex_ptr=view_bind_map_next_vertex_object(4*2);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)top;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)top;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)bot;

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)bot;

  	view_unmap_current_vertex_object();

		// draw the quad
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_LINE_LOOP,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_current_vertex_object();
}
