/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Fog

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

#include "consoles.h"
#include "video.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Textured Fog
      
======================================================= */

void fog_draw_textured(int tick)
{
	int					n,k,count,outer_radius,inner_radius,
						radius_add,radius,frame;
	unsigned long		gl_id;
	float				r_ang,r_ang_2,r_add,fx_1,fx_2,fz_1,fz_2,f_ty,f_by,
						txt_x_off,txt_x_turn,txt_x_off_add,
						gx,gx_add;
	float				*vertex_ptr,*uv_ptr;
	double				d_radius;
	d3ang				ang;
	texture_type		*texture;
	
		// textured fog on?
		
	if ((!map.fog.on) || (map.fog.use_solid_color)) return;

		// setup viewpoint
		
	memmove(&ang,&view.render->camera.ang,sizeof(d3ang));
	ang.y=180.0f;
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&ang);
	gl_setup_project();

		// drawing layers
	
	count=map.fog.count;
	outer_radius=map.fog.outer_radius;
	inner_radius=map.fog.inner_radius;

	radius_add=(inner_radius-outer_radius)/count;

		// construct VBO

	vertex_ptr=view_bind_map_next_vertex_object((((8*4)*count)*(3+2)));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(((8*4)*count)*3);

		// get drawing setup

	txt_x_off=((float)(tick>>7))*map.fog.speed;
	txt_x_turn=(map.fog.txt_x_fact*(view.render->camera.ang.y/360.0f));			// change texture offset with camera rotation
	txt_x_off_add=1.0f/(float)count;

	r_add=ANG_to_RAD*(float)(180/8);

	radius=outer_radius;

	f_ty=-(float)map.fog.high;
	f_by=(float)map.fog.drop;

	gx_add=map.fog.txt_x_fact/8.0f;

		// create the fog vertexes

	for (n=0;n!=count;n++) {

		if ((n&0x1)==0x0) {
			gx=txt_x_turn+txt_x_off;
		}
		else {
			gx=txt_x_turn-txt_x_off;
		}
		txt_x_off+=txt_x_off_add;

		r_ang=ANG_to_RAD*(-90.0f);

		for (k=0;k!=8;k++) {

			d_radius=(double)radius;

			r_ang_2=r_ang+r_add;

			fx_1=(float)(-sin(r_ang)*d_radius);
			fx_2=(float)(-sin(r_ang_2)*d_radius);

			fz_1=-(float)(cos(r_ang)*d_radius);
			fz_2=-(float)(cos(r_ang_2)*d_radius);

			*vertex_ptr++=fx_1;
			*vertex_ptr++=f_ty;
			*vertex_ptr++=fz_1;

			*uv_ptr++=gx;
			*uv_ptr++=0.0f;

			*vertex_ptr++=fx_2;
			*vertex_ptr++=f_ty;
			*vertex_ptr++=fz_2;

			*uv_ptr++=gx+gx_add;
			*uv_ptr++=0.0f;

			*vertex_ptr++=fx_2;
			*vertex_ptr++=f_by;
			*vertex_ptr++=fz_2;

			*uv_ptr++=gx+gx_add;
			*uv_ptr++=map.fog.txt_y_fact;

			*vertex_ptr++=fx_1;
			*vertex_ptr++=f_by;
			*vertex_ptr++=fz_1;

			*uv_ptr++=gx;
			*uv_ptr++=map.fog.txt_y_fact;

			r_ang=r_ang_2;
			gx+=gx_add;
		}

		radius+=radius_add;
	}

  	view_unmap_current_vertex_object();

		// setup drawing
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	texture=&map.textures[map.fog.texture_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;
	gl_id=texture->frames[frame].bitmap.gl_id;

	gl_texture_simple_start();
	gl_texture_simple_set(gl_id,FALSE,1.0f,1.0f,1.0f,map.fog.alpha);

		// draw the fog
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
		
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((((8*4)*count)*3)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,((8*4)*count));

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// end texture

	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_current_vertex_object();
}

/* =======================================================

      Solid Fog
      
======================================================= */

inline bool fog_solid_on(void)
{
	return((map.fog.on) && (map.fog.use_solid_color));
}

void fog_solid_start(void)
{
	GLfloat		col[4];

	glEnable(GL_FOG);

	col[0]=map.fog.col.r;
	col[1]=map.fog.col.g;
	col[2]=map.fog.col.b;
	col[3]=1.0f;

	glFogfv(GL_FOG_COLOR,col);
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogi(GL_FOG_START,map.fog.inner_radius);
	glFogi(GL_FOG_END,map.fog.outer_radius);
}

void fog_solid_end(void)
{
	glDisable(GL_FOG);
}

