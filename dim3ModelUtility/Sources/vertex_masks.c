/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Vertex Sel and Hide Masks

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

      Sel Masks
      
======================================================= */

void model_clear_sel_mask(model_type *model,int mesh_idx)
{
	bzero(model->meshes[mesh_idx].vertex_sel_mask,vertex_sel_hide_mask_sz);
}

void model_set_sel_mask(model_type *model,int mesh_idx,int vertex_idx,bool value)
{
	int				idx,sft;
	
	idx=vertex_idx>>3;
	sft=vertex_idx%8;
	
	if (value) {
		model->meshes[mesh_idx].vertex_sel_mask[idx]|=(0x1<<sft);
	}
	else {
		model->meshes[mesh_idx].vertex_sel_mask[idx]&=((0x1<<sft)^0xFF);
	}
}

void model_set_sel_mask_all(model_type *model,int mesh_idx)
{
	memset(model->meshes[mesh_idx].vertex_sel_mask,0xFF,vertex_sel_hide_mask_sz);
}

bool model_check_sel_mask(model_type *model,int mesh_idx,int vertex_idx)
{
	int				idx,sft;
	
	idx=vertex_idx>>3;
	sft=vertex_idx%8;
	
	return((model->meshes[mesh_idx].vertex_sel_mask[idx]&(0x1<<sft))!=0);
}

/* =======================================================

      Hide Masks
      
======================================================= */

void model_clear_hide_mask(model_type *model,int mesh_idx)
{
	bzero(model->meshes[mesh_idx].vertex_hide_mask,vertex_sel_hide_mask_sz);
}

void model_set_hide_mask(model_type *model,int mesh_idx,int vertex_idx,bool value)
{
	int				idx,sft;
	
	idx=vertex_idx>>3;
	sft=vertex_idx%8;
	
	if (value) {
		model->meshes[mesh_idx].vertex_hide_mask[idx]|=(0x1<<sft);
	}
	else {
		model->meshes[mesh_idx].vertex_hide_mask[idx]&=((0x1<<sft)^0xFF);
	}
}

bool model_check_hide_mask(model_type *model,int mesh_idx,int vertex_idx)
{
	int				idx,sft;
	
	idx=vertex_idx>>3;
	sft=vertex_idx%8;
	
	return((model->meshes[mesh_idx].vertex_hide_mask[idx]&(0x1<<sft))!=0);
}

/* =======================================================

      Hide/Sel Interactions
      
======================================================= */

void model_hide_mask_set_sel_vertexes(model_type *model,int mesh_idx)
{
	int				n,nt;
	
	nt=model->meshes[mesh_idx].nvertex;
	
	for (n=0;n!=nt;n++) {
		if (model_check_sel_mask(model,mesh_idx,n)) model_set_hide_mask(model,mesh_idx,n,TRUE);
	}
	
	model_clear_sel_mask(model,mesh_idx);
}

void model_hide_mask_set_non_sel_vertexes(model_type *model,int mesh_idx)
{
	int				n,nt;
	
	nt=model->meshes[mesh_idx].nvertex;
	
	for (n=0;n!=nt;n++) {
		if (!model_check_sel_mask(model,mesh_idx,n)) model_set_hide_mask(model,mesh_idx,n,TRUE);
	}
}

void model_hide_mask_show_all_vertexes(model_type *model,int mesh_idx)
{
	model_clear_hide_mask(model,mesh_idx);
}

/* =======================================================

      Hide/Trig Interactions
      
======================================================= */

bool model_check_hide_mask_trig(model_type *model,int mesh_idx,model_trig_type *trig)
{
	int				n;
	
	for (n=0;n!=3;n++) {
		if (model_check_hide_mask(model,mesh_idx,trig->v[n])) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Sel/Bone Interactions
      
======================================================= */

void model_set_sel_mask_bone(model_type *model,int mesh_idx,int bone_idx)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
		// clear selection
		
	model_clear_sel_mask(model,mesh_idx);
	
		// find vertexes attached to bone

	nt=model->meshes[mesh_idx].nvertex;
		
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		if ((vertex->major_bone_idx==bone_idx) || (vertex->minor_bone_idx==bone_idx)) {
			if (!model_check_hide_mask(model,mesh_idx,i)) model_set_sel_mask(model,mesh_idx,i,TRUE);
		}
		
		vertex++;
	}
}

void model_set_sel_mask_no_bone(model_type *model,int mesh_idx)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
		// clear selection
		
	model_clear_sel_mask(model,mesh_idx);
	
		// find vertexes to no bone

	nt=model->meshes[mesh_idx].nvertex;
		
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		if ((vertex->major_bone_idx==-1) && (vertex->minor_bone_idx==-1)) {
			if (!model_check_hide_mask(model,mesh_idx,i)) model_set_sel_mask(model,mesh_idx,i,TRUE);
		}
		
		vertex++;
	}
}

void model_set_sel_mask_near_bone(model_type *model,int mesh_idx,int bone_idx,float percentage)
{
	int					i,nt,x,y,z,v_dist,dist;
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	
		// get bone to check
		
	bone=&model->bones[bone_idx];
	
		// clear selection
		
	model_clear_sel_mask(model,mesh_idx);

		// get check distance
		
	x=model->view_box.size.x;
	z=model->view_box.size.z;
	y=model->view_box.size.y;
	
	v_dist=(int)(sqrt((x*x)+(z*z)+(y*y)));
	v_dist=(int)((float)v_dist*percentage);
	
	nt=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		x=labs(bone->pnt.x-vertex->pnt.x);
		z=labs(bone->pnt.z-vertex->pnt.z);
		y=labs(bone->pnt.y-vertex->pnt.y);
		dist=(int)(sqrt((x*x)+(z*z)+(y*y)));
	
		if (dist<v_dist) model_set_sel_mask(model,mesh_idx,i,TRUE);
	
		vertex++;
	}
	
}

/* =======================================================

      Sel/Material Interactions
      
======================================================= */

void model_set_sel_mask_material(model_type *model,int mesh_idx,int material_idx)
{
	int					i,k,nt;
	model_trig_type		*trig;
	model_material_type	*material;
	
		// clear selection
		
	model_clear_sel_mask(model,mesh_idx);
	
		// select vertexes in trigs

	material=&model->meshes[mesh_idx].materials[material_idx];
	trig=&model->meshes[mesh_idx].trigs[material->trig_start];
	
	nt=material->trig_count;
	
	for (i=0;i!=nt;i++) {
	
		for (k=0;k!=3;k++) {
			if (!model_check_hide_mask(model,mesh_idx,trig->v[k])) model_set_sel_mask(model,mesh_idx,trig->v[k],TRUE);
		}
		
		trig++;
	}
}
