/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Drawing

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

#include "model.h"

extern int					draw_type,cur_mesh,cur_bone,cur_hit_box,shift_x,shift_y,magnify_z,
							gl_view_x_sz,gl_view_y_sz;
extern float				ang_y,ang_x;
extern bool					fileopen,play_animate,model_bump_on,model_normal_on,model_bone_drag_on,model_show_first_mesh,
							model_box_on,drag_sel_on;
extern Rect					drag_sel_box;
extern AGLContext			ctx;
extern WindowRef			model_wind;

extern model_draw_setup		draw_setup;

/* =======================================================

      Setup GL Draw
      
======================================================= */

void draw_model_gl_setup(model_type *model)
{
	int				yoff,sz,bsz;
	float			aspect;
	Rect			wbox;
	
		// model viewport
		
	GetWindowPortBounds(model_wind,&wbox);
	
	glViewport(wbox.left,texture_palette_height,gl_view_x_sz,gl_view_y_sz);
	
	glEnable(GL_SCISSOR_TEST);
	glScissor(wbox.left,texture_palette_height,gl_view_x_sz,gl_view_y_sz);

		// model perspective
		
	aspect=(float)gl_view_x_sz/(float)gl_view_y_sz;
	
	sz=model->view_box.size.x;
	if (model->view_box.size.z>sz) sz=model->view_box.size.z;
	if (model->view_box.size.y>sz) sz=model->view_box.size.y;
    
	bsz=sz>>2;
	
	yoff=-(model->view_box.size.y/2);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45,aspect,(bsz>>1),(sz*10));
	glScalef(1,-1,1);						// y is reversed
	
	sz+=magnify_z;
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef((float)shift_x,-(float)(yoff+shift_y),-(((float)sz)*((1.0f/aspect)+0.5f)));
	
	glRotatef(ang_x,1,0,0);
	glRotatef(ang_y,0,1,0);
	
	glColor4f(1.0f,1.0f,1.0f,0.0f);
	
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Draw Model Bone and Vertex Setup
      
======================================================= */

void draw_model_setup_bones_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
		// create the drawing bones
	
	model_create_draw_bones(model,draw_setup);
	
		// calculate vertexes for first mesh if "show first mesh" is on
		
	if ((model_show_first_mesh) && (mesh_idx!=0)) {
		model_create_draw_vertexes(model,0,draw_setup);
		model_create_draw_normals(model,0,draw_setup);
	}

		// calculate vertexes for drawing mesh
		
	model_create_draw_vertexes(model,mesh_idx,draw_setup);
	model_create_draw_normals(model,mesh_idx,draw_setup);
}

/* =======================================================

      Draw Model Window
      
======================================================= */

void draw_model_wind(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
		// setup transformation to fit model in middle of screen
		
	draw_model_gl_setup(model);

		// clear the buffer
		
	glClearColor(0.9,0.9,0.9,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	if (!fileopen) {
		aglSwapBuffers(ctx);
		return;
	}
	
		// draw the center
		
	draw_model_axis(model);

		// create the drawing bones, vertex arrays and normal arrays
		
	draw_model_setup_bones_vertexes(model,mesh_idx,draw_setup);
	
		// draw the mesh(es) in the current view
	
	switch (draw_type) {	
		case dt_model:
			if ((model_show_first_mesh) && (mesh_idx!=0)) draw_model(model,0,draw_setup);
			draw_model(model,mesh_idx,draw_setup);
			draw_model_selected_vertexes(model,mesh_idx);
			break;
		case dt_mesh:
			if ((model_show_first_mesh) && (mesh_idx!=0)) draw_model_mesh(model,0);
			draw_model_mesh(model,mesh_idx);
			draw_model_selected_vertexes(model,mesh_idx);
			break;
		case dt_bones:
			draw_model_bones(model,draw_setup,cur_bone);
			break;
		case dt_mesh_bones:
			if ((model_show_first_mesh) && (mesh_idx!=0)) draw_model_mesh(model,0);
			draw_model_mesh(model,mesh_idx);
			draw_model_selected_vertexes(model,mesh_idx);
			draw_model_bones(model,draw_setup,cur_bone);
			break;
	}
	
		// draw the normals
		
	if (model_normal_on) draw_model_normals(model,mesh_idx);
  
        // draw the view, shadow, and hit boxes
    
    if (model_box_on) draw_model_boxes(model,draw_setup,cur_hit_box);
	
		// draw the drag selection
		
	if (drag_sel_on) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,gl_view_x_sz,0,gl_view_y_sz,-1,1);
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		glColor4f(0.8,0.8,0.8,0.4);
		glBegin(GL_POLYGON);
		glVertex3i(drag_sel_box.left,(gl_view_y_sz-drag_sel_box.top),-0.1);
		glVertex3i(drag_sel_box.right,(gl_view_y_sz-drag_sel_box.top),-0.1);
		glVertex3i(drag_sel_box.right,(gl_view_y_sz-drag_sel_box.bottom),-0.1);
		glVertex3i(drag_sel_box.left,(gl_view_y_sz-drag_sel_box.bottom),-0.1);
		glEnd();
	}
   
	aglSwapBuffers(ctx);
}

void draw_model_setup_pose(model_type *model,model_draw_setup *draw_setup,int wpose)
{
	model_clear_draw_setup(model,draw_setup);

    draw_setup->poses[0].idx_1=wpose;
	draw_setup->poses[0].idx_2=-1;
    draw_setup->poses[0].factor=0;
	draw_setup->poses[0].acceleration=0;

	draw_setup->ang.x=0;
	draw_setup->ang.y=0;
	draw_setup->ang.z=0;
	
    draw_setup->sway.x=draw_setup->sway.z=draw_setup->sway.y=0;
    draw_setup->move.x=draw_setup->move.z=draw_setup->move.y=0;
}

void draw_model_wind_pose(model_type *model,int wmesh,int wpose)
{
	draw_model_setup_pose(model,&draw_setup,wpose);
	draw_model_wind(model,wmesh,&draw_setup);
}
