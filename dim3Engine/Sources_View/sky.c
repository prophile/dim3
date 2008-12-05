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

extern void view_next_vertex_object(void);
extern void view_resize_current_vertex_object(int sz);
extern void view_bind_current_vertex_object(void);
extern void view_unbind_current_vertex_object(void);

/* =======================================================

      Draw Background
      
======================================================= */

void draw_background(int cx,int cy,int cz)
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
		
	gx=((float)(cx+cz))*(map.background.x_scroll_fact*0.001f);
	gy=((float)cy)*(map.background.y_scroll_fact*0.001f);
	
		// draw background
		
	texture=&map.textures[map.background.fill];
	txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;
	
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	gl_texture_simple_set(txt_id,FALSE,1,1,1,1);

    glBegin(GL_QUADS);
	glTexCoord2f(gx,gy);
	glVertex2i(0,0);
	glTexCoord2f((gx+1.0f),gy);
	glVertex2i(setup.screen.x_sz,0);
	glTexCoord2f((gx+1.0f),(gy+1.0f));
	glVertex2i(setup.screen.x_sz,setup.screen.y_sz);
	glTexCoord2f(gx,(gy+1.0f));
	glVertex2i(0,setup.screen.y_sz);
    glEnd();
    
	gl_texture_simple_end();
}

/* =======================================================

      Draw Sky
      
======================================================= */

void draw_sky_dome_panoramic(int tick)
{
    int					i,n,k,ty,by,txt_id,radius,
						px[4],pz[4];
	float				txt_x_shift,txt_y_shift,
						gx1,gx2,tgy,bgy;
	double				top_reduce,bot_reduce,d_radius,
						rxz,rxz2,r_add;
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
	gl_3D_rotate(&view.camera.ang);
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

		// dome setup
	
	radius=map.sky.radius;
	d_radius=(double)radius;

	ty=map.sky.dome_y;
	top_reduce=d_radius;

	r_add=ANG_to_RAD*(360/20);
	
		// draw the dome
		
	glBegin(GL_TRIANGLES);

	for (i=0;i!=5;i++) {				// the y
		
		by=ty;
		ty-=(radius/y_fct[i]);

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
		
			px[0]=(int)(-sin(rxz)*bot_reduce);
			px[1]=(int)(-sin(rxz2)*bot_reduce);

			pz[0]=(int)(cos(rxz)*bot_reduce);
			pz[1]=(int)(cos(rxz2)*bot_reduce);
			
			px[2]=(int)(-sin(rxz2)*top_reduce);
			px[3]=(int)(-sin(rxz)*top_reduce);

			pz[2]=(int)(cos(rxz2)*top_reduce);
			pz[3]=(int)(cos(rxz)*top_reduce);
			
			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;

			glTexCoord2f(gx1,bgy);
			glVertex3i(px[0],by,pz[0]);

			glTexCoord2f(gx2,bgy);
			glVertex3i(px[1],by,pz[1]);
			
			glTexCoord2f(gx2,tgy);
			glVertex3i(px[2],ty,pz[2]);
			
			glTexCoord2f(gx2,tgy);
			glVertex3i(px[2],ty,pz[2]);
			
			glTexCoord2f(gx1,tgy);
			glVertex3i(px[3],ty,pz[3]);

			glTexCoord2f(gx1,bgy);
			glVertex3i(px[0],by,pz[0]);

			rxz+=r_add;
		}
	}
	
	glEnd();
	
		// end textures
		
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
		
	gl_texture_simple_end();

		// cap

	glColor4f(texture->col.r,texture->col.g,texture->col.b,1.0f);

	glBegin(GL_TRIANGLES);

	rxz=0.0f;

	for (n=0;n!=20;n++) {			// the x and z
	
		if (n!=19) {
			rxz2=rxz+r_add;
		}
		else {
			rxz2=0.0;
		}
	
		px[0]=(int)(-sin(rxz)*top_reduce);
		px[1]=(int)(-sin(rxz2)*top_reduce);

		pz[0]=(int)(cos(rxz)*top_reduce);
		pz[1]=(int)(cos(rxz2)*top_reduce);

		glVertex3i(px[0],by,pz[0]);
		glVertex3i(px[1],by,pz[1]);
		glVertex3i(0,ty,0);

		rxz+=r_add;
	}
	
	glEnd();
}

void draw_sky_dome_hemisphere(int tick)
{
    int					i,n,k,sz,txt_id,cnt,radius;
	float				txt_x_shift,txt_y_shift,
						gx1,gx2,tgy,bgy,f_ty,f_by;
	float				*vertex_ptr,*uv_ptr;
	double				top_reduce,bot_reduce,d_radius,
						rxz,rxz2,r_add;
	texture_type		*texture;

		// dome setup

	int					y_fct[6]={2,3,4,6,8,11};
	float				gy_fct[7]={1.00f,0.70f,0.40f,0.20f,0.10f,0.01f,0.01f};
	double				reduce_fct[7]={6.6f,6.4f,5.9f,5.2f,4.3f,3.2f};
	
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
	gl_3D_rotate(&view.camera.ang);
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

		// dome setup
	
	radius=map.sky.radius;
	d_radius=(double)radius;

	f_ty=(float)map.sky.dome_y;
	top_reduce=d_radius;

	r_add=ANG_to_RAD*(360/20);

		// construct VBO

	view_next_vertex_object();
	view_bind_current_vertex_object();

	sz=((120*4)*(3+2))*sizeof(float);
	view_resize_current_vertex_object(sz);
					
	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		view_unbind_current_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+((120*4)*3);

		// create the dome vertexes

	cnt=0;
		
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

			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;

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

			if (i!=5) {
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

				*vertex_ptr++=0.0f;
				*vertex_ptr++=f_ty;
				*vertex_ptr++=0.0f;

				*uv_ptr++=0.5f;
				*uv_ptr++=0.01f;
			}

			cnt+=4;
			
			rxz+=r_add;
		}
	}

  	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

		// draw the dome
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
		
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((120*4)*3)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,cnt);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		// end textures
		
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
		
	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_current_vertex_object();
}

void draw_sky_cube(int tick,int y)
{
    int					k,txt_id,radius;
    float				g0,g1,txt_fact,txt_x_shift,txt_y_shift;
	texture_type		*texture;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
					
		// texture sizes
		
	txt_fact=map.sky.txt_fact;
	
	txt_x_shift=((float)tick*0.0005f)*map.sky.txt_x_shift;
	k=(int)txt_x_shift;
	txt_x_shift=txt_x_shift-(float)k;
	
	txt_y_shift=((float)tick*0.0005f)*map.sky.txt_y_shift;
	k=(int)txt_y_shift;
	txt_y_shift=txt_y_shift-(float)k;

		// setup texture
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_TEXTURE);
	glTranslatef(txt_x_shift,txt_y_shift,0.0f);
	glScalef(txt_fact,txt_fact,1.0f);
		
	radius=map.sky.radius;
	
	g0=0.001f;
	g1=0.999f;
		
		// top
		
	if (map.sky.fill!=-1) {
		texture=&map.textures[map.sky.fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
			
		glBegin(GL_QUADS);
		glTexCoord2f(g0,g1);
		glVertex3i(-radius,-radius,-radius);
		glTexCoord2f(g1,g1);
		glVertex3i(radius,-radius,-radius);
		glTexCoord2f(g1,g0);
		glVertex3i(radius,-radius,radius);
		glTexCoord2f(g0,g0);
		glVertex3i(-radius,-radius,radius);
		glEnd();
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		texture=&map.textures[map.sky.bottom_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
			
		glBegin(GL_QUADS);
		glTexCoord2f(g0,g1);
		glVertex3i(-radius,radius,-radius);
		glTexCoord2f(g1,g1);
		glVertex3i(radius,radius,-radius);
		glTexCoord2f(g1,g0);
		glVertex3i(radius,radius,radius);
		glTexCoord2f(g0,g0);
		glVertex3i(-radius,radius,radius);
		glEnd();
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		texture=&map.textures[map.sky.north_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
			
		glBegin(GL_QUADS);
		glTexCoord2f(g0,g0);
		glVertex3i(-radius,-radius,radius);
		glTexCoord2f(g1,g0);
		glVertex3i(radius,-radius,radius);
		glTexCoord2f(g1,g1);
		glVertex3i(radius,radius,radius);
		glTexCoord2f(g0,g1);
		glVertex3i(-radius,radius,radius);
		glEnd();
	}
	
		// east

	if (map.sky.east_fill!=-1) {
		texture=&map.textures[map.sky.east_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);

		glBegin(GL_QUADS);
		glTexCoord2f(g0,g0);
		glVertex3i(radius,-radius,radius);
		glTexCoord2f(g1,g0);
		glVertex3i(radius,-radius,-radius);
		glTexCoord2f(g1,g1);
		glVertex3i(radius,radius,-radius);
		glTexCoord2f(g0,g1);
		glVertex3i(radius,radius,radius);
		glEnd();
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		texture=&map.textures[map.sky.south_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);
		glBegin(GL_QUADS);
		glTexCoord2f(g0,g0);
		glVertex3i(radius,-radius,-radius);
		glTexCoord2f(g1,g0);
		glVertex3i(-radius,-radius,-radius);
		glTexCoord2f(g1,g1);
		glVertex3i(-radius,radius,-radius);
		glTexCoord2f(g0,g1);
		glVertex3i(radius,radius,-radius);
		glEnd();
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		texture=&map.textures[map.sky.west_fill];
		txt_id=texture->bitmaps[texture->animate.current_frame].gl_id;

		gl_texture_simple_set(txt_id,FALSE,1,1,1,1);

		glBegin(GL_QUADS);
		glTexCoord2f(g0,g0);
		glVertex3i(-radius,-radius,-radius);
		glTexCoord2f(g1,g0);
		glVertex3i(-radius,-radius,radius);
		glTexCoord2f(g1,g1);
		glVertex3i(-radius,radius,radius);
		glTexCoord2f(g0,g1);
		glVertex3i(-radius,radius,-radius);
		glEnd();
	}
	
		// restore
		
	glLoadIdentity();
	
	gl_texture_simple_end();
}

void draw_sky(int tick,int y)
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
			draw_sky_cube(tick,y);
			break;
	
	}
}


