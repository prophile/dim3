/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Mesh

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

      Add Mesh
      
======================================================= */

int model_mesh_add(model_type *model)
{
	int					nmesh,i;
	model_mesh_type		*mesh;
	
	nmesh=model->nmesh;
	if (nmesh>=max_model_mesh) return(-1);
		
		// create the new mesh
	
	mesh=&model->meshes[nmesh];
	
		// memory
		
	mesh->vertexes=valloc(max_model_vertex*sizeof(model_vertex_type));
	mesh->trigs=valloc(max_model_trig*sizeof(model_trig_type));
	mesh->materials=valloc(max_model_texture*sizeof(model_material_type));
	mesh->vertex_sel_mask=valloc(vertex_sel_hide_mask_sz);
	mesh->vertex_hide_mask=valloc(vertex_sel_hide_mask_sz);
	mesh->draw.gl_vertex_array=valloc((max_model_vertex*3)*sizeof(float));
	mesh->draw.gl_color_array=valloc((max_model_vertex*3)*sizeof(float));
	mesh->draw.gl_vertex_normal_array=valloc((max_model_vertex*3)*sizeof(float));
	mesh->draw.gl_light_normal_array=valloc((max_model_vertex*3)*sizeof(float));
	
	bzero(mesh->vertexes,(max_model_vertex*sizeof(model_vertex_type)));
	bzero(mesh->trigs,(max_model_trig*sizeof(model_trig_type)));
	bzero(mesh->materials,(max_model_texture*sizeof(model_material_type)));
	bzero(mesh->vertex_sel_mask,vertex_sel_hide_mask_sz);
	bzero(mesh->vertex_hide_mask,vertex_sel_hide_mask_sz);
	bzero(mesh->draw.gl_vertex_array,(max_model_vertex*3)*sizeof(float));
	bzero(mesh->draw.gl_color_array,(max_model_vertex*3)*sizeof(float));
	bzero(mesh->draw.gl_vertex_normal_array,(max_model_vertex*3)*sizeof(float));
	bzero(mesh->draw.gl_light_normal_array,(max_model_vertex*3)*sizeof(float));
	
		// initialize
		
	mesh->nvertex=0;
	mesh->ntrig=0;
	
	for (i=0;i!=max_model_texture;i++) {
		mesh->materials[i].trig_start=mesh->materials[i].trig_count=0;
	}
	
	strcpy(mesh->name,"New Mesh");
	mesh->no_lighting=FALSE;
	mesh->blend_add=FALSE;
	mesh->tintable=FALSE;
	
	model->nmesh++;
	
	return(model->nmesh-1);
}

/* =======================================================

      Duplicate Mesh
      
======================================================= */

int model_mesh_duplicate(model_type *model,int mesh_idx)
{
	int					idx;
	model_mesh_type		*org_mesh,*mesh;
	
		// add new mesh
		
	idx=model_mesh_add(model);
	if (idx==-1) return(-1);
	
		// duplicate
	
	org_mesh=&model->meshes[mesh_idx];
	mesh=&model->meshes[idx];

	memmove(mesh->vertexes,org_mesh->vertexes,(max_model_vertex*sizeof(model_vertex_type)));
	memmove(mesh->trigs,org_mesh->trigs,(max_model_trig*sizeof(model_trig_type)));
	memmove(mesh->materials,org_mesh->materials,(max_model_texture*sizeof(model_material_type)));
	
	mesh->nvertex=org_mesh->nvertex;
	mesh->ntrig=org_mesh->ntrig;

	mesh->no_lighting=org_mesh->no_lighting;
	mesh->blend_add=org_mesh->blend_add;
	mesh->tintable=org_mesh->tintable;
	
	return(idx);
}

/* =======================================================

      Copy Mesh
      
======================================================= */

void model_mesh_copy(model_type *model,int copy_mesh_idx,int mesh_idx)
{
	model_mesh_type		*org_mesh,*mesh;
	
	org_mesh=&model->meshes[copy_mesh_idx];
	mesh=&model->meshes[mesh_idx];

	memmove(mesh->vertexes,org_mesh->vertexes,(max_model_vertex*sizeof(model_vertex_type)));
	memmove(mesh->trigs,org_mesh->trigs,(max_model_trig*sizeof(model_trig_type)));
	memmove(mesh->materials,org_mesh->materials,(max_model_texture*sizeof(model_material_type)));
	
	mesh->nvertex=org_mesh->nvertex;
	mesh->ntrig=org_mesh->ntrig;

	mesh->no_lighting=org_mesh->no_lighting;
	mesh->blend_add=org_mesh->blend_add;
	mesh->tintable=org_mesh->tintable;
}

/* =======================================================

      Delete Mesh
      
======================================================= */

bool model_mesh_delete(model_type *model,int mesh_idx)
{
	int					sz;
	model_mesh_type		*mesh;
	
		// always need one mesh
		
	if (model->nmesh<=1) return(FALSE);
	
		// free the memory
		
	mesh=&model->meshes[mesh_idx];
		
	free(mesh->vertexes);
	free(mesh->trigs);
	free(mesh->materials);
	free(mesh->vertex_sel_mask);
	free(mesh->vertex_hide_mask);
	free(mesh->draw.gl_vertex_array);
	free(mesh->draw.gl_color_array);
	free(mesh->draw.gl_vertex_normal_array);
	free(mesh->draw.gl_light_normal_array);
	
		// delete current mesh
				
	sz=(model->nmesh-mesh_idx)-1;
	if (sz>0) memmove(&model->meshes[mesh_idx],&model->meshes[mesh_idx+1],(sz*sizeof(model_mesh_type)));

	model->nmesh--;
	
	return(TRUE);
}


