/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Mesh, Bones, Vertexes, Boxes

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

/* =======================================================

      Draw Mesh
      
======================================================= */

void draw_model_mesh(model_type *model,int mesh_idx)
{
	int					n,ntrig;
    model_trig_type		*trig;
	model_mesh_type		*mesh;
	
		// draw the mesh
		
	mesh=&model->meshes[mesh_idx];
		
	glVertexPointer(3,GL_FLOAT,0,mesh->draw.gl_vertex_array);
	glLockArraysEXT(0,mesh->nvertex);
	
	glColor4f(0.75,0.75,0.75,1);
    
	ntrig=mesh->ntrig;
	trig=mesh->trigs;
    
    for (n=0;n!=ntrig;n++) {
	
		if (!model_check_hide_mask_trig(model,mesh_idx,trig)) {
			glBegin(GL_LINE_LOOP);
			glArrayElement(trig->v[0]);
			glArrayElement(trig->v[1]);
			glArrayElement(trig->v[2]);
			glEnd();
		}
		
		trig++;
    }
    
	glUnlockArraysEXT();
}

/* =======================================================

      Draw Bones Drag Handles
      
======================================================= */

float draw_model_bones_drag_handle_offset(model_type *model)
{
	float				f;
	
	f=(float)(model->view_box.size.x+model->view_box.size.z+model->view_box.size.y)/3.0f;
	f*=0.15;
	
	if (f<20.0f) return(20.0f);
	if (f>250.0f) return(250.0f);
	
	return(f);
}

void draw_model_bones_drag_handle_calc(float x,float y,float z,d3vct *vct,d3ang *ang,float *hx,float *hy,float *hz)
{
	matrix_type		mat;
	
		// handle coordinates
	
	if (ang->x!=0) {
		matrix_rotate_x(&mat,ang->x);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	if (ang->y!=0) {
		matrix_rotate_y(&mat,ang->y);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	if (ang->z!=0) {
		matrix_rotate_z(&mat,ang->z);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	*hx=x-vct->x;
	*hy=y-vct->y;
	*hz=z-vct->z;
}

void draw_model_bones_drag_handle(float x,float y,float z,d3vct *vct,d3ang *ang,d3col *col)
{
	float			x2,y2,z2;
	
		// handle coordinates
		
	draw_model_bones_drag_handle_calc(x,y,z,vct,ang,&x2,&y2,&z2);
		
		// line
		
	glColor4f(col->r,col->g,col->b,1);
		
	glLineWidth(2);
	
	glBegin(GL_LINES);
	glVertex3f(x,y,z);
	glVertex3f(x2,y2,z2);
	glEnd();
	
	glLineWidth(1);
	
	glPointSize(10);
	
	glBegin(GL_POINTS);
	glVertex3f(x2,y2,z2);
	glEnd();
	
	glPointSize(1);
}

/* =======================================================

      Find Rotation for Handles
      
======================================================= */

void draw_model_bones_get_handle_rot(model_type *model,model_draw_setup *draw_setup,int bone_idx,d3ang *rot)
{
	int						parent_idx;
	model_draw_bone_type	*draw_bone;
	
	draw_bone=&draw_setup->bones[bone_idx];
	
		// non-comulative rotations
		
	rot->x=draw_bone->rot.x;
	rot->y=draw_bone->rot.y;
	rot->z=draw_bone->rot.z;

	if (model->deform_mode!=deform_mode_comulative_rotate) return;
	
		// cumulative rotations
		
	parent_idx=bone_idx;
	
	while (TRUE) {
		parent_idx=draw_bone->parent_idx;
		if (parent_idx==-1) break;
		
		draw_bone=&draw_setup->bones[parent_idx];

		rot->x+=draw_bone->rot.x;
		rot->y+=draw_bone->rot.y;
		rot->z+=draw_bone->rot.z;
	}
}

/* =======================================================

      Draw Bones
      
======================================================= */

void draw_model_bones(model_type *model,model_draw_setup *draw_setup,int sel_bone_idx)
{
	int						n,nbone,parent_idx;
	float					x_move,z_move,y_move,
							x,z,y,bone_drag_handle_offset;
	d3vct					vct;
	d3ang					ang,rot;
	d3col					col;
	model_draw_bone_type	*draw_bone,*parent_bone;

		// draw the bones

	nbone=model->nbone;
    
    x_move=(float)draw_setup->move.x;
    z_move=(float)draw_setup->move.z;
    y_move=(float)draw_setup->move.y;
	
        // connections
        
	glColor4f(0.2,0.2,0.2,1);
	glLineWidth(2);
	
	glBegin(GL_LINES);
	
	draw_bone=draw_setup->bones;
	
	for (n=0;n!=nbone;n++) {
        parent_idx=draw_bone->parent_idx;
        if (parent_idx!=-1) {
            parent_bone=&draw_setup->bones[parent_idx];
			
			x=draw_bone->fpnt.x+x_move;
			y=draw_bone->fpnt.y+y_move;
			z=draw_bone->fpnt.z+z_move;

            glVertex3f(x,y,z);
			
			x=parent_bone->fpnt.x+x_move;
			y=parent_bone->fpnt.y-y_move;
			z=parent_bone->fpnt.z-z_move;
			
            glVertex3f(x,y,z);
        }
		
		draw_bone++;
	}
	
	glEnd();
	
	glLineWidth(1);
        
        // bones
        
	draw_bone=draw_setup->bones;
	
	bone_drag_handle_offset=draw_model_bones_drag_handle_offset(model);
	
	for (n=0;n!=nbone;n++) {
	
			// bone position
			
		x=draw_bone->fpnt.x+x_move;
		y=draw_bone->fpnt.y+y_move;
		z=draw_bone->fpnt.z+z_move;
	
			// selected or normal
	
		if (sel_bone_idx==n) {

			glPointSize(10);
			
				// draw selected bone
				
			glColor4f(1,0,1,1);
			
			glBegin(GL_POINTS);
			glVertex3f(x,y,(z-10));
			glEnd();
		
				// draw drag bones
				
			glDisable(GL_DEPTH_TEST);
			
			draw_model_bones_get_handle_rot(model,draw_setup,n,&rot);
				
			vct.x=bone_drag_handle_offset;
			vct.y=0;
			vct.z=0;
			ang.x=0;
			ang.y=rot.y;
			ang.z=rot.z;
			col.r=1;
			col.g=0;
			col.b=0;
			draw_model_bones_drag_handle(x,y,z,&vct,&ang,&col);
			
			vct.x=0;
			vct.y=bone_drag_handle_offset;
			vct.z=0;
			ang.x=rot.x;
			ang.y=0;
			ang.z=rot.z;
			col.r=0;
			col.g=1;
			col.b=0;
			draw_model_bones_drag_handle(x,y,z,&vct,&ang,&col);
			
			vct.x=0;
			vct.y=0;
			vct.z=bone_drag_handle_offset;
			ang.x=rot.x;
			ang.y=rot.y;
			ang.z=0;
			col.r=0;
			col.g=0;
			col.b=1;
			draw_model_bones_drag_handle(x,y,z,&vct,&ang,&col);
			
			glEnable(GL_DEPTH_TEST);
		}
		else {
		
				// draw regular bone
				
			glPointSize(8);
			glColor4f(1,0,0,1);
			
			glBegin(GL_POINTS);
			glVertex3f(x,y,(z-10));
			glEnd();
		}
		
		draw_bone++;
	}
	
	glPointSize(1);
}

