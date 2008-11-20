/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Bones

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

      Add and Delete Bones
      
======================================================= */

int model_bone_add(model_type *model,int x,int y,int z)
{
	int					n,bone_idx;
	model_bone_type		*bone,*ptr;

		// only allow a maximum number of bones

	if (model->nbone>=max_model_bone) return(-1);

		// create memory for new bones

	ptr=(model_bone_type*)valloc(sizeof(model_bone_type)*(model->nbone+1));
	if (ptr==NULL) return(-1);

	if (model->bones!=NULL) {
		memmove(ptr,model->bones,(sizeof(model_bone_type)*model->nbone));
		free(model->bones);
	}

	model->bones=ptr;

	bone_idx=model->nbone;
	model->nbone++;

		// initialize the bone
	
	bone=&model->bones[bone_idx];
	bzero(bone,sizeof(model_bone_type));
	
	bone->name[0]=0x0;
	bone->tag=model_null_tag;
	bone->parent_idx=-1;
	bone->pnt.x=x;
	bone->pnt.y=y;
	bone->pnt.z=z;
	
		// fix any poses
		
	for (n=0;n!=model->npose;n++) {
		model->poses[n].bone_moves[bone_idx].rot.x=0;
		model->poses[n].bone_moves[bone_idx].rot.y=0;
		model->poses[n].bone_moves[bone_idx].rot.z=0;
		model->poses[n].bone_moves[bone_idx].mov.x=1;
		model->poses[n].bone_moves[bone_idx].mov.y=1;
		model->poses[n].bone_moves[bone_idx].mov.z=1;
		model->poses[n].bone_moves[bone_idx].acceleration=0;
		model->poses[n].bone_moves[bone_idx].skip_blended=FALSE;
		model->poses[n].bone_moves[bone_idx].constraint.bone_idx=-1;
		model->poses[n].bone_moves[bone_idx].constraint.offset.x=0;
		model->poses[n].bone_moves[bone_idx].constraint.offset.y=0;
		model->poses[n].bone_moves[bone_idx].constraint.offset.z=0;
	}
	
	model_calculate_parents(model);
	
	return(bone_idx);
}

void model_bone_delete(model_type *model,int bone_idx)
{
	int					i,n,nt,sz;
	model_vertex_type	*vertex;
	model_bone_type		*bone,*ptr;
	model_pose_type		*pose;
	
		// delete all vertex attachments
		
	for (n=0;n!=model->nmesh;n++) {
		nt=model->meshes[n].nvertex;
		vertex=model->meshes[n].vertexes;
		
		for (i=0;i!=nt;i++) {
			if (vertex->major_bone_idx==bone_idx) {
				vertex->major_bone_idx=-1;
			}
			else {
				if (vertex->major_bone_idx>bone_idx) vertex->major_bone_idx--;
			}
			if (vertex->minor_bone_idx==bone_idx) {
				vertex->minor_bone_idx=-1;
			}
			else {
				if (vertex->minor_bone_idx>bone_idx) vertex->minor_bone_idx--;
			}
			vertex++;
		}
	}
	
		// delete all bone attachments
		
	nt=model->nbone;
	bone=model->bones;
	
	for (n=0;n!=nt;n++) {
		if (bone->parent_idx==bone_idx) {
			bone->parent_idx=-1;
		}
		else {
			if (bone->parent_idx>bone_idx) bone->parent_idx--;
		}
		bone++;
	}
	
		// delete pose bone moves
		
	sz=(model->nbone-bone_idx)-1;

	if (sz>0) {
		nt=model->npose;
		pose=model->poses;
		
		for (n=0;n!=nt;n++) {
			memmove(&pose->bone_moves[bone_idx],&pose->bone_moves[bone_idx+1],(sz*sizeof(model_bone_move_type)));
			pose++;
		}
	}

		// is the list completely empty?

	if (model->nbone==1) {
		free(model->bones);
		model->bones=NULL;
		model->nbone=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(model_bone_type*)valloc(sizeof(model_bone_type)*(model->nbone-1));

	if (ptr==NULL) {
		if (bone_idx<(model->nbone-1)) {
			memmove(&model->bones[bone_idx],&model->bones[bone_idx+1],(sizeof(model_bone_type)*((model->nbone-bone_idx)-1)));
		}
	}
	else {

		if (bone_idx>0) {
			memmove(ptr,model->bones,(sizeof(model_bone_type)*bone_idx));
		}
		if (bone_idx<(model->nbone-1)) {
			memmove(&ptr[bone_idx],&model->bones[bone_idx+1],(sizeof(model_bone_type)*((model->nbone-bone_idx)-1)));
		}

		free(model->bones);
		model->bones=ptr;
	}
	
	model->nbone--;
		
		// reset bones

	model_calculate_parents(model);
}

/* =======================================================

      Check for Circular Bones
      
======================================================= */

bool model_check_bone_duplicate_tag(model_type *model,model_bone_type *bone)
{
	int				n,nt;
	model_bone_type	*chk_bone;
	
	nt=model->nbone;
	chk_bone=model->bones;
	
	for (n=0;n!=nt;n++) {
		if (chk_bone!=bone) {
			if (chk_bone->tag==bone->tag) return(TRUE);
		}
		chk_bone++;
	}
	
	return(FALSE);
}

bool model_check_bone_circular(model_type *model,model_bone_type *bone)
{
	int				parent_idx;
	model_bone_type	*nxt_bone;
	
	nxt_bone=bone;
	
	while (TRUE) {
	
		parent_idx=nxt_bone->parent_idx;
		if (parent_idx==-1) return(FALSE);
		
		nxt_bone=&model->bones[parent_idx];
		if (nxt_bone==bone) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Move Bones
      
======================================================= */

void model_bone_move(model_type *model,int bone_idx,int x,int y,int z,bool nudge_children,bool nudge_vertexes)
{
	int					i,k,nmesh,nt;
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	
		// nudge bone
		
	bone=&model->bones[bone_idx];
	bone->pnt.x+=x;
	bone->pnt.y+=y;
	bone->pnt.z+=z;
	
		// nudge vertexes
		
	if (nudge_vertexes) {
	
		nmesh=model->nmesh;
		
		for (k=0;k!=nmesh;k++) {
			nt=model->meshes[k].nvertex;
			vertex=model->meshes[k].vertexes;
			
			for (i=0;i!=nt;i++) {
				if (vertex->major_bone_idx==bone_idx) {
					vertex->pnt.x+=(int)(((float)x)*vertex->bone_factor);
					vertex->pnt.y+=(int)(((float)y)*vertex->bone_factor);
					vertex->pnt.z+=(int)(((float)z)*vertex->bone_factor);
				}
				if (vertex->minor_bone_idx==bone_idx) {
					vertex->pnt.x+=(int)(((float)x)*(1.0f-vertex->bone_factor));
					vertex->pnt.y+=(int)(((float)y)*(1.0f-vertex->bone_factor));
					vertex->pnt.z+=(int)(((float)z)*(1.0f-vertex->bone_factor));
				}
				vertex++;
			}
		}
	}
		
		// nudge children
		
	if (nudge_children) {
		nt=model->nbone;
		bone=model->bones;
		
		for (i=0;i!=nt;i++) {
			if (bone->parent_idx==bone_idx) model_bone_move(model,i,x,y,z,TRUE,nudge_vertexes);
			bone++;
		}
	}
	
	model_calculate_parents(model);
}
