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

      Draw Vertexes
      
======================================================= */

void draw_model_selected_vertexes(model_type *model,int mesh_idx)
{
	int				n,nvertex;
	float			*pv;
	
	glColor4f(0,0,0,1);
	glPointSize(5);
	
	nvertex=model->meshes[mesh_idx].nvertex;
	pv=model->meshes[mesh_idx].gl_vertex_array;

	glBegin(GL_POINTS);
	
	for (n=0;n!=nvertex;n++) {
		if ((model_check_sel_mask(model,mesh_idx,n)) && (!model_check_hide_mask(model,mesh_idx,n))) glVertex3f(*pv,*(pv+1),*(pv+2));
		pv+=3;
	}
	
	glEnd();
	glPointSize(1);
}

/* =======================================================

      Draw Model Box
      
======================================================= */

void draw_model_box(model_box_type *box,model_draw_setup *draw_setup,bool draw_floor)
{
	int				i,xsz,zsz,ysz,offx,offz,offy,
					x[8],y[8],z[8];
	
    xsz=box->size.x/2;
    offx=box->offset.x;
    zsz=box->size.z/2;
    offz=box->offset.z;
    ysz=box->size.y;
    offy=box->offset.y;
	
	x[0]=x[1]=x[4]=x[5]=offx-xsz;
	x[2]=x[3]=x[6]=x[7]=offx+xsz;
	y[0]=y[1]=y[2]=y[3]=offy-ysz;
	y[4]=y[5]=y[6]=y[7]=offy;
	z[0]=z[3]=z[4]=z[7]=offz-zsz;
	z[1]=z[2]=z[5]=z[6]=offz+zsz;
	
	for (i=0;i!=8;i++) {
		model_get_point_position(draw_setup,&x[i],&y[i],&z[i]);
	}
    
    glLineWidth(4);
    
    glBegin(GL_LINE_LOOP);
	for (i=0;i!=4;i++) {
		glVertex3i(x[i],y[i],z[i]);
	}
    glEnd();
    
    glBegin(GL_LINE_LOOP);
	for (i=4;i!=8;i++) {
		glVertex3i(x[i],y[i],z[i]);
	}
    glEnd();
    
    glBegin(GL_LINES);
	for (i=0;i!=4;i++) {
		glVertex3i(x[i],y[i],z[i]);
		glVertex3i(x[i+4],y[i+4],z[i+4]);
	}
    glEnd();
		
    glLineWidth(1);
    
    if (!draw_floor) return;
    
    glColor4f(0.75,0.75,0.75,0.5);
    
    glBegin(GL_POLYGON);
	for (i=4;i!=8;i++) {
		glVertex3i(x[i],y[i],z[i]);
	}
    glEnd();
}

void draw_model_boxes(model_type *model,model_draw_setup *draw_setup,int sel_hit_box_idx)
{
	int				n;
	
		// shadow box
		
	glColor4f(0,0,1,0.5);
	draw_model_box(&model->shadow_box,draw_setup,FALSE);
	
		// hit boxes
		
	for (n=0;n<model->nhit_box;n++) {
		if (n==sel_hit_box_idx) {
			glColor4f(1,0,0,0.5);
		}
		else {
			glColor4f(1,1,0,0.5);
		}
		draw_model_box(&model->hit_boxes[n].box,draw_setup,FALSE);
	}
	
		// view box
		
	glColor4f(0,1,0,0.5);
	draw_model_box(&model->view_box,draw_setup,TRUE);
}

/* =======================================================

      Draw Model Normals
      
======================================================= */

void draw_model_normals(model_type *model,int mesh_idx)
{
	int				n,nvertex,
					sx,sy,sz,lsz;
	float			fx,fy,fz,flsz;
	float			*pv,*pn;
	
		// find normal line size
		
	model_get_size(model,&sx,&sy,&sz);
	lsz=sx;
	if (sz>lsz) lsz=sz;
	if (sy>lsz) lsz=sy;
	
	flsz=((float)lsz)/15.0f;
	
		// draw normals
	
	glColor4f(1,0,1,1);
	glLineWidth(2);
	
	nvertex=model->meshes[mesh_idx].nvertex;
	pv=model->meshes[mesh_idx].gl_vertex_array;
	pn=model->meshes[mesh_idx].gl_vertex_normal_array;

	glBegin(GL_LINES);
	
	for (n=0;n!=nvertex;n++) {
		if (model_check_hide_mask(model,mesh_idx,n)) {
			pv+=3;
			pn+=3;
			continue;
		}
		
		fx=*pv++;
		fy=*pv++;
		fz=*pv++;
		glVertex3f(fx,fy,fz);

		fx+=((*pn++)*flsz);
		fy+=((*pn++)*flsz);
		fz+=((*pn++)*flsz);
		glVertex3f(fx,fy,fz);
	}
	
	glEnd();
	glLineWidth(1);
}

/* =======================================================

      Draw Model Axis
      
======================================================= */

void draw_model_axis(model_type *model)
{
	int				n,sz;
	
		// x axis
		
	glColor4f(1,0,0,1);
	
    glBegin(GL_LINES);
	
	glVertex3i(-20000,model->center.y,model->center.z);
	glVertex3i(20000,model->center.y,model->center.z);
	
	for (n=1;n!=20;n++) {
		sz=20;
		if ((n%5)==0) sz=50;
		
		glVertex3i(-(map_enlarge*n),model->center.y,(model->center.z+sz));
		glVertex3i(-(map_enlarge*n),model->center.y,(model->center.z-sz));
		
		glVertex3i((map_enlarge*n),model->center.y,(model->center.z+sz));
		glVertex3i((map_enlarge*n),model->center.y,(model->center.z-sz));
	}
	
	glEnd();
	
		// y axis
		
	glColor4f(0,1,0,1);
	
    glBegin(GL_LINES);
	
	glVertex3i(model->center.x,-20000,model->center.z);
	glVertex3i(model->center.x,20000,model->center.z);

	for (n=1;n!=20;n++) {
		sz=20;
		if ((n%5)==0) sz=50;
		
		glVertex3i((model->center.x+sz),-(map_enlarge*n),model->center.z);
		glVertex3i((model->center.x-sz),-(map_enlarge*n),model->center.z);
		
		glVertex3i((model->center.x+sz),(map_enlarge*n),model->center.z);
		glVertex3i((model->center.x-sz),(map_enlarge*n),model->center.z);
	}

    glEnd();

		// z axis
		
	glColor4f(0,0,1,1);
	
    glBegin(GL_LINES);

	glVertex3i(model->center.x,model->center.y,-20000);
	glVertex3i(model->center.x,model->center.y,20000);
	
	for (n=1;n!=20;n++) {
		sz=20;
		if ((n%5)==0) sz=50;
		
		glVertex3i((model->center.x+sz),model->center.y,-(map_enlarge*n));
		glVertex3i((model->center.x-sz),model->center.y,-(map_enlarge*n));
		
		glVertex3i((model->center.x+sz),model->center.y,(map_enlarge*n));
		glVertex3i((model->center.x-sz),model->center.y,(map_enlarge*n));
	}

    glEnd();
}

