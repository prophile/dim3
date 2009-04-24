/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Draw Normal Routines

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

      Position Model Normals
      
======================================================= */

void model_move_single_normal(model_draw_bone_type *draw_bones,model_vertex_type *vertex,float *nx,float *nz,float *ny)
{
	int						n;
	float					bone_factor,
							majx,majz,majy,minx,minz,miny;
	model_draw_bone_type	*major_bone,*minor_bone;

		// find rotations
		
	n=vertex->major_bone_idx;
	if (n==-1) {
		*nx=vertex->normal.x;
		*nz=vertex->normal.z;
		*ny=vertex->normal.y;
		return;
	}
	major_bone=&draw_bones[n];

		// find major bone rotation
	
	majx=vertex->normal.x;
	majy=vertex->normal.y;
	majz=vertex->normal.z;
	
	matrix_vertex_multiply_ignore_transform(&major_bone->rot_mat,&majx,&majy,&majz);	// commulative has a transform that needs to be ignored

		// find minor bone
		
	bone_factor=vertex->bone_factor;
		
	n=vertex->minor_bone_idx;
	if ((n==-1) || (bone_factor==1)) {
		*nx=majx;
		*nz=majz;
		*ny=majy;
		return;
	}
	minor_bone=&draw_bones[n];
		
		// find minor bone rotation
		
	minx=vertex->normal.x;
	miny=vertex->normal.y;
	minz=vertex->normal.z;
	
	minor_bone->rot_mat.data[0][3]=0;
	minor_bone->rot_mat.data[1][3]=0;
	minor_bone->rot_mat.data[2][3]=0;
	
	matrix_vertex_multiply_ignore_transform(&minor_bone->rot_mat,&minx,&miny,&minz);	// commulative has a transform that needs to be ignored

		// average the points
		
	*nx=minx+((majx-minx)*bone_factor);
	*nz=minz+((majz-minz)*bone_factor);
	*ny=miny+((majy-miny)*bone_factor);
}

/* =======================================================

      Build Normal Lists
      
======================================================= */

void model_create_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						i,nt;
	float					x,y,z;
	float					*pn;
	bool					no_sway;
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
	matrix_type				rot_mat,sway_mat;
	
	mesh=&model->meshes[mesh_idx];
	
		// rotations and sways
		
	matrix_rotate_zyx(&rot_mat,draw_setup->ang.x,draw_setup->ang.y,draw_setup->ang.z);
		
		// setup list
		
	nt=mesh->nvertex;
	vertex=mesh->vertexes;
	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array;

		// normals with sways
		
	no_sway=((draw_setup->sway.x==0) && (draw_setup->sway.y==0) && (draw_setup->sway.z==0));

	if (!no_sway) {
	
		matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
		
		for (i=0;i!=nt;i++) {
			model_move_single_normal(draw_setup->bones,vertex,&x,&z,&y);
			matrix_vertex_multiply(&sway_mat,&x,&z,&y);
			matrix_vertex_multiply(&rot_mat,&x,&y,&z);

			*pn++=x;
			*pn++=y;
			*pn++=z;
			
			vertex++;
		}
		
		return;
	}
	
		// normals with no sways
		
	for (i=0;i!=nt;i++) {
		model_move_single_normal(draw_setup->bones,vertex,&x,&z,&y);
		matrix_vertex_multiply(&rot_mat,&x,&y,&z);

		*pn++=x;
		*pn++=y;
		*pn++=z;
		
		vertex++;
	}
}
