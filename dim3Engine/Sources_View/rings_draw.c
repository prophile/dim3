/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ring Drawing Routines

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

#include "objects.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "effects.h"
#include "video.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Draw Ring Position
      
======================================================= */

void ring_draw_position(effect_type *effect,int count,int *x,int *y,int *z)
{
	float				m_tick;
	ring_type			*ring;
	ring_effect_data	*eff_ring;
	
	eff_ring=&effect->data.ring;
	ring=&server.rings[eff_ring->ring_idx];

	m_tick=((float)count)/10.0f;
	
	*x=effect->pnt.x+(int)(ring->vct.x*m_tick);
	*y=effect->pnt.y+(int)(ring->vct.y*m_tick);
	*z=effect->pnt.z+(int)(ring->vct.z*m_tick);
}

/* =======================================================

      Draw Rings
      
======================================================= */

void ring_draw(effect_type *effect,int count)
{
	int						n,px,py,pz,nvertex,
							life_tick;
	float					mx,my,mz,fx,fy,fz,
							outer_sz,inner_sz,
							color_dif,alpha,gx,gy,g_size,
							f_count,f_tick;
	float					*vl,*vt,*vertex_ptr,*vertex_array,*coord_array;
	double					rd,rd2;
	d3col					col;
	ring_type				*ring;
	ring_effect_data		*eff_ring;
	matrix_type				mat_x,mat_y,mat_z;
	
	eff_ring=&effect->data.ring;
	ring=&server.rings[eff_ring->ring_idx];
	
		// get size
		
	life_tick=effect->life_tick;

	f_tick=(float)life_tick;
	f_count=(float)count;
	
	outer_sz=(float)(ring->end_outer_size-ring->start_outer_size);
	outer_sz=((outer_sz*f_count)/f_tick)+(float)ring->start_outer_size;

	inner_sz=(float)(ring->end_inner_size-ring->start_inner_size);
	inner_sz=((inner_sz*f_count)/f_tick)+(float)ring->start_inner_size;

		// color and alpha

	color_dif=ring->end_color.r-ring->start_color.r;
    col.r=ring->start_color.r+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.g-ring->start_color.g;
    col.g=ring->start_color.g+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.b-ring->start_color.b;
    col.b=ring->start_color.b+((color_dif*f_count)/f_tick);

	col.r*=eff_ring->tint.r;
	col.g*=eff_ring->tint.g;
	col.b*=eff_ring->tint.b;

	alpha=ring->end_alpha-ring->start_alpha;
	alpha=((alpha*f_count)/f_tick)+ring->start_alpha;
	
		// setup images
		
	effect_image_animate_get_uv(count,&ring->animate,&gx,&gy,&g_size);
	
		// position and ring rotation

	ring_draw_position(effect,count,&px,&py,&pz);

	mx=(float)px;
	my=(float)py;
	mz=(float)pz;

	fx=f_count*ring->rot.x;
	fx+=(fx*ring->rot_accel.x);
	fx=angle_add(eff_ring->ang.x,fx);

	fy=f_count*ring->rot.y;
	fy+=(fy*ring->rot_accel.y);
	fy=angle_add(eff_ring->ang.y,fy);

	fz=f_count*ring->rot.z;
	fz+=(fz*ring->rot_accel.z);
	fz=angle_add(eff_ring->ang.z,fz);

	matrix_rotate_x(&mat_x,-fx);
	matrix_rotate_z(&mat_z,fz);
	matrix_rotate_y(&mat_y,fy);

		// construct VBO

	nvertex=36*4;

	vertex_ptr=view_bind_map_next_vertex_object((nvertex*(3+2)));
	if (vertex_ptr==NULL) return;

		// set ring arrays

	vl=vertex_array=vertex_ptr;
	vt=coord_array=vertex_ptr+(nvertex*3);

	for (n=0;n!=360;n+=10) {
		rd=(double)n*ANG_to_RAD;
		rd2=((double)(n+10))*ANG_to_RAD;

			// quad 1

		fx=(float)cos(rd)*outer_sz;
		fy=-((float)sin(rd)*outer_sz);
		fz=0.0f;

		*vt++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*vt++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
		
		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*vl++=mx+fx;
		*vl++=my+fy;
		*vl++=mz+fz;

			// quad 2

		fx=(float)cos(rd2)*outer_sz;
		fy=-((float)sin(rd2)*outer_sz);
		fz=0.0f;

		*vt++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*vt++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
		
		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*vl++=mx+fx;
		*vl++=my+fy;
		*vl++=mz+fz;

			// quad 3

		fx=(float)cos(rd2)*inner_sz;
		fy=-((float)sin(rd2)*inner_sz);
		fz=0.0f;

		*vt++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*vt++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
		
		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*vl++=mx+fx;
		*vl++=my+fy;
		*vl++=mz+fz;

			// quad 4

		fx=(float)cos(rd)*inner_sz;
		fy=-((float)sin(rd)*inner_sz);
		fz=0.0f;

		*vt++=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		*vt++=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
		
		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		*vl++=mx+fx;
		*vl++=my+fy;
		*vl++=mz+fz;
	}

		// unmap vertex object

	view_unmap_current_vertex_object();

		// draw ring
		
	gl_texture_simple_start();
	gl_texture_simple_set(view_images_get_gl_id(ring->image_idx),FALSE,col.r,col.g,col.b,alpha);
	
	glEnable(GL_BLEND);
	
	if (ring->blend_add) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((nvertex*3)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,nvertex);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDepthMask(GL_TRUE);
	
	gl_texture_simple_end();

		// unbind vertex object
		
	view_unbind_current_vertex_object();
}

