/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Mesh Palette

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "window.h"
#include "tab.h"
#include "dialog.h"
#include "model.h"

extern int					cur_mesh,cur_pose,gl_view_x_sz,gl_view_y_sz;
extern bool					fileopen;
extern model_draw_setup		draw_setup;
extern model_type			model;

extern AGLContext			ctx;
extern WindowRef			model_wind;

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void mesh_palette_draw_normal_setup(void)
{
	Rect					wbox;

	GetWindowPortBounds(model_wind,&wbox);
	
	glViewport((wbox.left+gl_view_x_sz),0,mesh_palette_width,(gl_view_y_sz+texture_palette_height));
	
	glEnable(GL_SCISSOR_TEST);
	glScissor((wbox.left+gl_view_x_sz),0,mesh_palette_width,(gl_view_y_sz+texture_palette_height));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)0,(GLdouble)mesh_palette_width,(GLdouble)(gl_view_y_sz+texture_palette_height),(GLdouble)0,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mesh_palette_draw_mesh_setup(int mesh_idx,int y,int high)
{
	int					xoff,yoff,ysz,sz,bsz;
	float				aspect;
	d3pnt				min,max;
	Rect				wbox;
	model_draw_setup	draw_setup;
	
		// setup view port
		
	GetWindowPortBounds(model_wind,&wbox);
	
	y=gl_view_y_sz-y;

	glViewport((wbox.left+gl_view_x_sz),y,mesh_palette_width,high);
	
	glEnable(GL_SCISSOR_TEST);
	glScissor((wbox.left+gl_view_x_sz),y,mesh_palette_width,high);
	
	glClearColor(0.8f,0.8f,0.8f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// model perspective
		
	aspect=(float)mesh_palette_width/(float)high;
	
	model_get_vertex_extent(&model,mesh_idx,&min.x,&max.x,&min.z,&max.z,&min.y,&max.y);

	sz=max.x-min.x;
	ysz=max.y-min.y;
	if (ysz>sz) sz=ysz;
	
	bsz=sz>>2;
	
	xoff=-((min.x+max.x)>>1);
	yoff=(min.y+max.y)>>1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45,aspect,(bsz>>1),(sz*10));
	glScalef(1,-1,1);						// y is reversed
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-(float)xoff,-(float)yoff,-(((float)sz)*((1.0f/aspect)+0.5f)));
	glRotatef(180.0f,0.0f,1.0f,0.0f);
	
		// draw model
		
	model_draw_array_initialize(&model);
	
	model_clear_draw_setup(&model,&draw_setup);
	draw_model_setup_pose(&model,&draw_setup,-1);
	draw_model_setup_bones_vertexes(&model,mesh_idx,&draw_setup);
	
	draw_model(&model,mesh_idx,&draw_setup);
	
	model_draw_array_free(&model);
	
	glFinish();
}

void mesh_palette_draw(void)
{
	int						n,y,high;
	
 	if (!fileopen) return;

		// clear mesh palette
		
	mesh_palette_draw_normal_setup();
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
		// get size
		
	high=(gl_view_y_sz+texture_palette_height)/model.nmesh;
	if (high>mesh_palette_width) high=mesh_palette_width;
	
		// draw meshes
	
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
		
	for (n=0;n!=model.nmesh;n++) {
		mesh_palette_draw_mesh_setup(n,(n*high),high);
	}
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
		// borders
		
	mesh_palette_draw_normal_setup();
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	for (n=0;n!=model.nmesh;n++) {
		y=(n*high)+1;
		
		glBegin(GL_LINES);
		glVertex2i(0,y);
		glVertex2i(mesh_palette_width,y);
		glVertex2i(0,(y+high));
		glVertex2i(mesh_palette_width,(y+high));
		glEnd();
	}
		
	glBegin(GL_LINES);
	glVertex2i(1,0);
	glVertex2i(1,(gl_view_y_sz+texture_palette_height));
	glVertex2i(mesh_palette_width,0);
	glVertex2i(mesh_palette_width,(gl_view_y_sz+texture_palette_height));
	glEnd();
	
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
	y=cur_mesh*high;
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(2,(y+2));
	glVertex2i((mesh_palette_width-1),(y+2));
	glVertex2i((mesh_palette_width-1),(y+high));
	glVertex2i(2,(y+high));
	glEnd();
		
	aglSwapBuffers(ctx);
}

/* =======================================================

      Mesh Palette Clicking
      
======================================================= */

void mesh_palette_click(Point pt,bool dblclick)
{
	int					y,high;
	Rect				wbox;
	
    if (!fileopen) return;
 
		// drawing area
		
	GetWindowPortBounds(model_wind,&wbox);
	
	wbox.top+=tool_button_size;
	wbox.bottom-=info_palette_height;

		// find clicked mesh
		
	high=(gl_view_y_sz+texture_palette_height)/model.nmesh;
	if (high>mesh_palette_width) high=mesh_palette_width;
	
	y=(pt.v-wbox.top)/high;
	if ((y<0) || (y>=model.nmesh)) return;
	
	cur_mesh=y;
		
		// redraw
		
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
	mesh_palette_draw();
		
		// handle click
	
	if (dblclick) {
		dialog_mesh_info_run(&model.meshes[cur_mesh]);
	}
}

