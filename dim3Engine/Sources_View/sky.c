/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Sky Boxes

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

#include "consoles.h"
#include "video.h"

extern map_type			map;
extern setup_type		setup;
extern view_type		view;

/* =======================================================

      Draw Background
      
======================================================= */

void draw_background(void)
{
	int					txt_id;
	float				gx,gy;
	texture_type		*texture;
	
		// is there a background?
	
	if (!map.background.on) return;
	if ((map.background.fill<0) || (map.background.fill>=max_map_texture)) return;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_2D_view_screen();
	
		// get scrolling
		
	gx=((float)(view.camera.pnt.x+view.camera.pnt.z))*(map.background.x_scroll_fact*0.001f);
	gy=((float)view.camera.pnt.y)*(map.background.y_scroll_fact*0.001f);

		// draw background
		
	texture=&map.textures[map.background.fill];
	txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;
	
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	gl_texture_simple_start();
	gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
	view_draw_next_vertex_object_2D_texture_screen(setup.screen.x_sz,setup.screen.y_sz,gx,gy);
	gl_texture_simple_end();
}

/* =======================================================

      Draw Dome Panoramic Sky
      
======================================================= */

void draw_sky_dome_panoramic_setup(void)
{
    int					i,n,radius;
	float				gx1,gx2,tgy,bgy,f_ty,f_by;
	float				*vertex_ptr,*uv_ptr;
	double				top_reduce,bot_reduce,d_radius,
						rxz,rxz2,r_add;

		// dome setup

	int					y_fct[5]={2,3,5,6,9};
	float				gy_fct[6]={1.00f,0.80f,0.60f,0.40f,0.20f,0.01f};
	double				reduce_fct[5]={6.8f,6.4f,6.0f,5.5f,5.0f};
	
		// dome setup
	
	radius=map.sky.radius;
	d_radius=(double)radius;

	f_ty=(float)map.sky.dome_y;
	top_reduce=d_radius;

	r_add=ANG_to_RAD*(360/20);
	
		// construct VBO

	vertex_ptr=view_bind_map_sky_vertex_object();
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+((120*4)*3);

		// create the dome vertexes

	for (i=0;i!=5;i++) {				// the y
		
		f_by=f_ty;
		f_ty-=(float)(radius/y_fct[i]);

		rxz=0.0;

		bot_reduce=top_reduce;
		top_reduce-=(d_radius/reduce_fct[i]);
			
		bgy=gy_fct[i];
		tgy=gy_fct[i+1];

		for (n=0;n!=20;n++) {			// the x and z
		
			if (n!=19) {
				rxz2=rxz+r_add;
			}
			else {
				rxz2=0.0;
			}
		
			*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=(float)(cos(rxz)*bot_reduce);

			*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);
			
			*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=(float)(cos(rxz2)*top_reduce);

			*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=(float)(cos(rxz)*top_reduce);
			
			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;

			*uv_ptr++=gx1;
			*uv_ptr++=bgy;

			*uv_ptr++=gx2;
			*uv_ptr++=bgy;

			*uv_ptr++=gx2;
			*uv_ptr++=tgy;

			*uv_ptr++=gx1;
			*uv_ptr++=tgy;

			rxz+=r_add;
		}
	}

		// create the cap vertexes

	rxz=0.0f;

	for (n=0;n!=20;n++) {			// the x and z
	
		if (n!=19) {
			rxz2=rxz+r_add;
		}
		else {
			rxz2=0.0;
		}
	
		*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
		*vertex_ptr++=f_by;
		*vertex_ptr++=(float)(cos(rxz)*top_reduce);

		*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
		*vertex_ptr++=f_by;
		*vertex_ptr++=(float)(cos(rxz2)*top_reduce);

		*vertex_ptr++=0.0f;
		*vertex_ptr++=f_ty;
		*vertex_ptr++=0.0f;

		rxz+=r_add;
	}

		// unmap and unbind vbo

  	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void draw_sky_dome_panoramic(int tick)
{
    int					k,txt_id;
	float				txt_x_shift,txt_y_shift;
	texture_type		*texture;

		// dome setup

	int					y_fct[5]={2,3,5,6,9};
	float				gy_fct[6]={1.00f,0.80f,0.60f,0.40f,0.20f,0.01f};
	double				reduce_fct[5]={6.8f,6.4f,6.0f,5.5f,5.0f};
	
		// texture sizes
		
	txt_x_shift=((float)tick*0.0005f)*map.sky.txt_x_shift;
	k=(int)txt_x_shift;
	txt_x_shift=txt_x_shift-(float)k;
	
	txt_y_shift=((float)tick*0.0005f)*map.sky.txt_y_shift;
	k=(int)txt_y_shift;
	txt_y_shift=txt_y_shift-(float)k;
					
		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(NULL,&view.camera.ang);
	gl_setup_project();
	
		// construct VBO

	view_bind_sky_vertex_object();

		// both outside and cap need vertex list

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);

		// draw textured dome
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_TEXTURE);
	glTranslatef(txt_x_shift,txt_y_shift,0.0f);

	texture=&map.textures[map.sky.fill];
	txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;
	
	gl_texture_simple_set(txt_id,FALSE,1,1,1,1);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((120*4)*3)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,(100*4));

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
		
	gl_texture_simple_end();

		// draw colored cap

	glColor4f(texture->col.r,texture->col.g,texture->col.b,1.0f);
	glDrawArrays(GL_TRIANGLES,(100*4),(20*3));

		// disable vertex array

	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Draw Dome Hemisphere Sky
      
======================================================= */

void draw_sky_dome_hemisphere_setup(void)
{
    int					i,n,radius;
	float				f_ty,f_by,f_ty2,f_by2,gx1,gx2,tgy,bgy;
	float				*vertex_ptr,*uv_ptr;
	double				top_reduce,bot_reduce,d_radius,
						rxz,rxz2,r_add;

		// dome setup

	int					y_fct[6]={2,3,4,6,8,11};
	float				gy_fct[7]={1.00f,0.70f,0.40f,0.20f,0.10f,0.01f,0.01f};
	double				reduce_fct[7]={6.6f,6.4f,5.9f,5.2f,4.3f,3.2f};

		// dome setup
	
	radius=map.sky.radius;
	d_radius=(double)radius;

	f_ty=f_ty2=(float)map.sky.dome_y;
	top_reduce=d_radius;

	r_add=ANG_to_RAD*(360/20);

		// construct VBO

	vertex_ptr=view_bind_map_sky_vertex_object();
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+((200*4)*3);

		// create the dome vertexes

	for (i=0;i!=5;i++) {				// the y
		
		f_by=f_ty;
		f_ty-=((float)(radius/y_fct[i]));

		f_by2=f_ty2;
		f_ty2+=((float)(radius/y_fct[i]));

		rxz=0.0;

		bot_reduce=top_reduce;
		top_reduce-=(d_radius/reduce_fct[i]);

		bgy=gy_fct[i];
		tgy=gy_fct[i+1];
			
		for (n=0;n!=20;n++) {			// the x and z
		
			if (n!=19) {
				rxz2=rxz+r_add;
			}
			else {
				rxz2=0.0;
			}

			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;

				// regular dome

			*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=(float)(cos(rxz)*bot_reduce);

			*uv_ptr++=gx1;
			*uv_ptr++=bgy;

			*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);

			*uv_ptr++=gx2;
			*uv_ptr++=bgy;

			if (i!=4) {
				*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=(float)(cos(rxz2)*top_reduce);

				*uv_ptr++=gx2;
				*uv_ptr++=tgy;

				*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=(float)(cos(rxz)*top_reduce);

				*uv_ptr++=gx1;
				*uv_ptr++=tgy;
			}
			else {
				*vertex_ptr++=0.0f;			// squeeze tops down to triangles
				*vertex_ptr++=f_ty;
				*vertex_ptr++=0.0f;

				*uv_ptr++=0.5f;
				*uv_ptr++=0.01f;
			}

				// flipped dome

			if (map.sky.dome_mirror) {

				*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
				*vertex_ptr++=f_by2;
				*vertex_ptr++=(float)(cos(rxz)*bot_reduce);
			
				*uv_ptr++=gx1;
				*uv_ptr++=bgy;

				*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
				*vertex_ptr++=f_by2;
				*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);
				
				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				if (i!=4) {
					*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=(float)(cos(rxz2)*top_reduce);
					
					*uv_ptr++=gx2;
					*uv_ptr++=tgy;

					*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=(float)(cos(rxz)*top_reduce);
					
					*uv_ptr++=gx1;
					*uv_ptr++=tgy;
				}
				else {
					*vertex_ptr++=0.0f;			// squeeze tops down to triangles
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=0.0f;

					*uv_ptr++=0.5f;
					*uv_ptr++=0.01f;
				}
			}
			
			rxz+=r_add;
		}
	}

		// unmap and unbind vbo

  	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}


void draw_sky_dome_hemisphere(int tick)
{
    int					k,txt_id,dome_cnt,trig_cnt;
	float				txt_x_shift,txt_y_shift;
	texture_type		*texture;

		// texture sizes
		
	txt_x_shift=((float)tick*0.0005f)*map.sky.txt_x_shift;
	k=(int)txt_x_shift;
	txt_x_shift=txt_x_shift-(float)k;
	
	txt_y_shift=((float)tick*0.0005f)*map.sky.txt_y_shift;
	k=(int)txt_y_shift;
	txt_y_shift=txt_y_shift-(float)k;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(NULL,&view.camera.ang);
	gl_setup_project();
	
		// setup texture
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_TEXTURE);
	glTranslatef(txt_x_shift,txt_y_shift,0.0f);

	texture=&map.textures[map.sky.fill];
	txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;
	
	gl_texture_simple_set(txt_id,FALSE,1,1,1,1);

		// bind the vbo

	view_bind_sky_vertex_object();

		// quad and trig counts

	dome_cnt=(20*4)*4;
	trig_cnt=20*3;

		// draw the dome
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
		
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((200*4)*3)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,dome_cnt);
	glDrawArrays(GL_TRIANGLES,dome_cnt,trig_cnt);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// end textures
		
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
		
	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Draw Cube Sky
      
======================================================= */

void draw_sky_cube_setup(void)
{
    float				g0,g1,f_radius;
	float				*vertex_ptr,*uv_ptr;

		// construct VBO

	vertex_ptr=view_bind_map_sky_vertex_object();
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+((6*4)*3);

		// setup cube quads
		
	f_radius=(float)map.sky.radius;
	
	g0=0.001f;
	g1=0.999f;
		
		// top
		
	if (map.sky.fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g1;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g1;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g0;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g0;
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g1;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g1;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g0;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g0;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g0;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g0;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g1;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g1;
	}
	
		// east

	if (map.sky.east_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g0;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g0;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g1;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g1;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g0;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g0;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g1;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g1;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g0;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g0;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=g1;
		*uv_ptr++=g1;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=g0;
		*uv_ptr++=g1;
	}
 
		// unmap and unbind vbo

	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void draw_sky_cube(int tick)
{
    int					k,txt_id,offset;
    float				txt_fact,txt_x_shift,txt_y_shift;
	texture_type		*texture;
					
		// texture sizes
		
	txt_fact=map.sky.txt_fact;
	
	txt_x_shift=((float)tick*0.0005f)*map.sky.txt_x_shift;
	k=(int)txt_x_shift;
	txt_x_shift=txt_x_shift-(float)k;
	
	txt_y_shift=((float)tick*0.0005f)*map.sky.txt_y_shift;
	k=(int)txt_y_shift;
	txt_y_shift=txt_y_shift-(float)k;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(NULL,&view.camera.ang);
	gl_setup_project();

		// setup texture
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_TEXTURE);
	glTranslatef(txt_x_shift,txt_y_shift,0.0f);
	glScalef(txt_fact,txt_fact,1.0f);

		// bind VBO

	view_bind_sky_vertex_object();

		// draw cube sides

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
		
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((6*4)*3)*sizeof(float)));

	offset=0;

	if (map.sky.fill!=-1) {
		texture=&map.textures[map.sky.fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
		glDrawArrays(GL_QUADS,0,4);
		
		offset+=4;
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		texture=&map.textures[map.sky.bottom_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
		glDrawArrays(GL_QUADS,offset,4);
		
		offset+=4;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		texture=&map.textures[map.sky.north_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
		glDrawArrays(GL_QUADS,offset,4);
		
		offset+=4;
	}
	
		// east

	if (map.sky.east_fill!=-1) {
		texture=&map.textures[map.sky.east_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
		glDrawArrays(GL_QUADS,offset,4);
		
		offset+=4;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		texture=&map.textures[map.sky.south_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
		glDrawArrays(GL_QUADS,offset,4);
		
		offset+=4;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		texture=&map.textures[map.sky.west_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
		glDrawArrays(GL_QUADS,offset,4);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// end texture
		
	glLoadIdentity();
	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Initialize Sky Drawing
      
======================================================= */

void draw_sky_init(void)
{
		// is there a sky?
	
	if (!map.sky.on) return;

		// init vertex list

	switch (map.sky.type) {
	
		case st_dome_panoramic:
			view_init_sky_vertex_object(((120*4)*(3+2)));
			draw_sky_dome_panoramic_setup();
			break;

		case st_dome_hemisphere:
			view_init_sky_vertex_object(((200*4)*(3+2)));
			draw_sky_dome_hemisphere_setup();
			break;
			
		case st_cube:
			view_init_sky_vertex_object(((6*4)*(3+2)));
			draw_sky_cube_setup();
			break;
	
	}
}

/* =======================================================

      Draw Sky Mainline
      
======================================================= */

void draw_sky(int tick)
{
		// is there a sky?
	
	if (!map.sky.on) return;
	
		// draw specific sky type

	switch (map.sky.type) {
	
		case st_dome_panoramic:
			draw_sky_dome_panoramic(tick);
			break;

		case st_dome_hemisphere:
			draw_sky_dome_hemisphere(tick);
			break;
			
		case st_cube:
			draw_sky_cube(tick);
			break;
	
	}
}


