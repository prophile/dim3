/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Vertex Routines

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

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Position Model Vertexes
      
======================================================= */

void model_move_single_vertex(model_draw_bone_type *draw_bones,model_vertex_type *vertex,float *px,float *py,float *pz)
{
	int						n;
	float					bone_factor,
							majx,majz,majy,minx,minz,miny;
	model_draw_bone_type	*major_bone,*minor_bone;

		// find major bone
		
	n=vertex->major_bone_idx;
	if (n==-1) {
		*px=(float)vertex->pnt.x;
		*py=(float)vertex->pnt.y;
		*pz=(float)vertex->pnt.z;
		return;
	}
	major_bone=&draw_bones[n];

		// find major bone rotation
	
	majx=vertex->major_dist.x;
	majy=vertex->major_dist.y;
	majz=vertex->major_dist.z;
	
	matrix_vertex_multiply(&major_bone->rot_mat,&majx,&majy,&majz);
	
	majx+=major_bone->fpnt.x;
	majy+=major_bone->fpnt.y;
	majz+=major_bone->fpnt.z;

		// find minor bone
		
	bone_factor=vertex->bone_factor;
		
	n=vertex->minor_bone_idx;
	if ((n==-1) || (bone_factor==1)) {
		*px=majx;
		*py=majy;
		*pz=majz;
		return;
	}
	minor_bone=&draw_bones[n];
		
		// find minor bone rotation
		
	minx=vertex->minor_dist.x;
	miny=vertex->minor_dist.y;
	minz=vertex->minor_dist.z;
	
	matrix_vertex_multiply(&minor_bone->rot_mat,&minx,&miny,&minz);
	
	minx+=minor_bone->fpnt.x;
	miny+=minor_bone->fpnt.y;
	minz+=minor_bone->fpnt.z;

		// average the points
		
	*px=minx+((majx-minx)*bone_factor);
	*pz=minz+((majz-minz)*bone_factor);
	*py=miny+((majy-miny)*bone_factor);
}

/* =======================================================

      Setup Model Vertexes
	        
======================================================= */

void model_create_draw_vertexes_normal(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,nvertex;
	float					x,z,y,cx,cz,cy,mov_x,mov_z,mov_y;
	float					*pv;
	matrix_type				rot_x_mat,rot_z_mat,rot_y_mat,sway_mat;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	
		// rotations
		
	cx=(float)draw_setup->center.x;
	cz=(float)draw_setup->center.z;
	cy=(float)draw_setup->center.y;
	
	matrix_rotate_x(&rot_x_mat,draw_setup->ang.x);
	matrix_rotate_z(&rot_z_mat,draw_setup->ang.z);
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	
		// movements
		
	mov_x=draw_setup->move.x+cx;	// restore the center movement
    mov_z=draw_setup->move.z+cz;
    mov_y=draw_setup->move.y+cy;
	
		// sways
		
	matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
	
		// run through the vertexes
		
	mesh=&model->meshes[mesh_idx];
    
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	pv=mesh->draw.gl_vertex_array;
    
	for (n=0;n!=nvertex;n++) {

		model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
		matrix_vertex_multiply(&sway_mat,&x,&y,&z);
		
		x-=cx;
		y-=cy;
		z-=cz;
		
		matrix_vertex_multiply(&rot_x_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_z_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

		*pv++=x+mov_x;
		*pv++=y+mov_y;
		*pv++=z+mov_z;
		
		vertex++;
	}
}

void model_create_draw_vertexes_no_xzrot(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,nvertex;
	float					x,z,y,cx,cz,cy,mov_x,mov_z,mov_y;
	float					*pv;
	matrix_type				rot_y_mat,sway_mat;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	
		// rotations
		
	cx=(float)draw_setup->center.x;
	cz=(float)draw_setup->center.z;
	cy=(float)draw_setup->center.y;
	
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	
		// movements
		
	mov_x=draw_setup->move.x+cx;	// restore the center movement
    mov_z=draw_setup->move.z+cz;
    mov_y=draw_setup->move.y+cy;
	
		// sways
		
	matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
	
		// run through the vertexes
		
	mesh=&model->meshes[mesh_idx];
    
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	pv=mesh->draw.gl_vertex_array;
    
	for (n=0;n!=nvertex;n++) {

		model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
		matrix_vertex_multiply(&sway_mat,&x,&y,&z);
		
		x-=cx;
		y-=cy;
		z-=cz;
		
		matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

		*pv++=x+mov_x;
		*pv++=y+mov_y;
		*pv++=z+mov_z;
		
		vertex++;
	}
}

void model_create_draw_vertexes_no_sway(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,nvertex;
	float					x,z,y,cx,cz,cy,mov_x,mov_z,mov_y;
	float					*pv;
	matrix_type				rot_x_mat,rot_z_mat,rot_y_mat;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	
		// rotations
		
	cx=(float)draw_setup->center.x;
	cz=(float)draw_setup->center.z;
	cy=(float)draw_setup->center.y;
	
	matrix_rotate_x(&rot_x_mat,draw_setup->ang.x);
	matrix_rotate_z(&rot_z_mat,draw_setup->ang.z);
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	
		// movements
		
	mov_x=draw_setup->move.x+cx;	// restore the center movement
    mov_z=draw_setup->move.z+cz;
    mov_y=draw_setup->move.y+cy;
	
		// run through the vertexes
		
	mesh=&model->meshes[mesh_idx];
    
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	pv=mesh->draw.gl_vertex_array;
    
	for (n=0;n!=nvertex;n++) {

		model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
		
		x-=cx;
		y-=cy;
		z-=cz;
		
		matrix_vertex_multiply(&rot_x_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_z_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

		*pv++=x+mov_x;
		*pv++=y+mov_y;
		*pv++=z+mov_z;
		
		vertex++;
	}
}

void model_create_draw_vertexes_no_sway_xzrot(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,nvertex;
	float					x,z,y,cx,cz,cy,mov_x,mov_z,mov_y;
	float					*pv;
	matrix_type				rot_y_mat;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	
		// rotations
		
	cx=(float)draw_setup->center.x;
	cz=(float)draw_setup->center.z;
	cy=(float)draw_setup->center.y;
	
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	
		// movements
		
	mov_x=draw_setup->move.x+cx;	// restore the center movement
    mov_z=draw_setup->move.z+cz;
    mov_y=draw_setup->move.y+cy;
	
		// run through the vertexes
		
	mesh=&model->meshes[mesh_idx];
    
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	pv=mesh->draw.gl_vertex_array;
    
	for (n=0;n!=nvertex;n++) {

		model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
		
		x-=cx;
		y-=cy;
		z-=cz;
		
		matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

		*pv++=x+mov_x;
		*pv++=y+mov_y;
		*pv++=z+mov_z;
		
		vertex++;
	}
}

void model_create_draw_vertexes_no_sway_center_move(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,nvertex;
	float					x,z,y;
	float					*pv;
	matrix_type				rot_x_mat,rot_z_mat,rot_y_mat;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	
		// rotations
		
	matrix_rotate_x(&rot_x_mat,draw_setup->ang.x);
	matrix_rotate_z(&rot_z_mat,draw_setup->ang.z);
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	
		// run through the vertexes
		
	mesh=&model->meshes[mesh_idx];
    
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	pv=mesh->draw.gl_vertex_array;
    
	for (n=0;n!=nvertex;n++) {

		model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
		
		matrix_vertex_multiply(&rot_x_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_z_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

		*pv++=x;
		*pv++=y;
		*pv++=z;
		
		vertex++;
	}
}

void model_create_draw_vertexes_no_sway_center_move_xzrot(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,nvertex;
	float					x,z,y;
	float					*pv;
	matrix_type				rot_y_mat;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	
		// rotations
		
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	
		// run through the vertexes
		
	mesh=&model->meshes[mesh_idx];
    
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	pv=mesh->draw.gl_vertex_array;
    
	for (n=0;n!=nvertex;n++) {

		model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
		
		matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

		*pv++=x;
		*pv++=y;
		*pv++=z;
		
		vertex++;
	}
}

void model_create_draw_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	bool			no_sway,no_center,no_move,no_xzrot;
	
		// check model setup
		
	no_sway=((draw_setup->sway.x==0) && (draw_setup->sway.y==0) && (draw_setup->sway.z==0));
	no_center=((draw_setup->center.x==0) && (draw_setup->center.y==0) && (draw_setup->center.z==0));
	no_move=((draw_setup->move.x==0) && (draw_setup->move.y==0) && (draw_setup->move.z==0));
	no_xzrot=((draw_setup->ang.x==0) && (draw_setup->ang.z==0));
	
		// pick best optimized vertex creator
		
	if ((no_sway) && (no_center) && (no_move)) {
		if (no_xzrot) {
			model_create_draw_vertexes_no_sway_center_move_xzrot(model,mesh_idx,draw_setup);
			return;
		}
		model_create_draw_vertexes_no_sway_center_move(model,mesh_idx,draw_setup);
		return;
	}
	
	if (no_sway) {
		if (no_xzrot) {
			model_create_draw_vertexes_no_sway_xzrot(model,mesh_idx,draw_setup);
			return;
		}
		model_create_draw_vertexes_no_sway(model,mesh_idx,draw_setup);
		return;
	}
	
	if (no_xzrot) {
		model_create_draw_vertexes_no_xzrot(model,mesh_idx,draw_setup);
		return;
	}
	
	model_create_draw_vertexes_normal(model,mesh_idx,draw_setup);
}

/* =======================================================

      Create Flattened Model Vertexes
      
======================================================= */

void model_create_draw_2D_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup,int x_offset,int y_offset,float x_resize,float y_resize)
{
	int						n,nvertex;
	float					x,z,y,cx,cz,cy,mov_x,mov_y;
	float					*pv;
	bool					no_sway,no_center;
	matrix_type				rot_x_mat,rot_z_mat,rot_y_mat,sway_mat;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	
		// rotations
		
	cx=(float)draw_setup->center.x;
	cz=(float)draw_setup->center.z;
	cy=(float)draw_setup->center.y;
	
	matrix_rotate_x(&rot_x_mat,draw_setup->ang.x);
	matrix_rotate_z(&rot_z_mat,draw_setup->ang.z);
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
	
		// draw offset
		
	mov_x=(float)x_offset;
    mov_y=(float)y_offset;
	
		// setup the list
    
	mesh=&model->meshes[mesh_idx];
	
	nvertex=mesh->nvertex;
	vertex=mesh->vertexes;
	pv=mesh->draw.gl_vertex_array;
	
		// with sways
		
	no_sway=((draw_setup->sway.x==0) && (draw_setup->sway.y==0) && (draw_setup->sway.z==0));
		
	if (!no_sway) {
	
		matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);

		for (n=0;n!=nvertex;n++) {

			model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
			matrix_vertex_multiply(&sway_mat,&x,&y,&z);
			
			x-=cx;
			y-=cy;
			z-=cz;
			
			matrix_vertex_multiply(&rot_x_mat,&x,&y,&z);
			matrix_vertex_multiply(&rot_z_mat,&x,&y,&z);
			matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

			*pv++=((x+mov_x)*x_resize);
			*pv++=((y+mov_y)*y_resize);
			
			vertex++;
		}
		
		return;
	}
	
		// no sway with center
		
	no_center=((cx==0) && (cy==0) && (cz==0));
	
	if (!no_center) {
	
		for (n=0;n!=nvertex;n++) {

			model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
			
			x-=cx;
			y-=cy;
			z-=cz;
			
			matrix_vertex_multiply(&rot_x_mat,&x,&y,&z);
			matrix_vertex_multiply(&rot_z_mat,&x,&y,&z);
			matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

			*pv++=((x+mov_x)*x_resize);
			*pv++=((y+mov_y)*y_resize);
			
			vertex++;
		}
	}
	
		// with no sway, no center

	for (n=0;n!=nvertex;n++) {

		model_move_single_vertex(draw_setup->bones,vertex,&x,&y,&z);
		
		matrix_vertex_multiply(&rot_x_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_z_mat,&x,&y,&z);
		matrix_vertex_multiply(&rot_y_mat,&x,&y,&z);

		*pv++=((x+mov_x)*x_resize);
		*pv++=((y+mov_y)*y_resize);
		
		vertex++;
	}
}

/* =======================================================

      Resize and Flip Model Vertexes
      
======================================================= */

void model_resize_draw_vertex(model_type *model,int mesh_idx,float resize)
{
	int			n,nt,yadd;
	float		*pv;
	
		// resize vertexes
		
	yadd=model->view_box.size.y>>1;
	
	nt=model->meshes[mesh_idx].nvertex;
	pv=model->meshes[mesh_idx].draw.gl_vertex_array;
	
	for (n=0;n!=nt;n++) {
		*pv=((*pv)*resize);
		pv++;
		*pv=(((*pv)+yadd)*resize)-yadd;
		pv++;
		*pv=((*pv)*resize);
		pv++;
	}
}

void model_flip_draw_vertex(model_type *model,int mesh_idx)
{
	int			n,nt;
	float		*pv;
	
	nt=model->meshes[mesh_idx].nvertex;
	pv=model->meshes[mesh_idx].draw.gl_vertex_array;
	
	for (n=0;n!=nt;n++) {
		*pv=-(*pv);
		pv+=3;
	}
}

/* =======================================================

      Translate Vertex Lists to View
      
======================================================= */

void model_translate_draw_vertex(model_type *model,int mesh_idx,int x,int y,int z)
{
	int			n,nt;
	float		fx,fy,fz;
	float		*pv;
	
	nt=model->meshes[mesh_idx].nvertex;
	pv=model->meshes[mesh_idx].draw.gl_vertex_array;
	
	fx=(float)x;
	fy=(float)y;
	fz=(float)z;
	
	for (n=0;n!=nt;n++) {
		*pv+=fx;
		pv++;
		*pv+=fy;
		pv++;
		*pv=-((*pv)+fz);
		pv++;
	}
}

