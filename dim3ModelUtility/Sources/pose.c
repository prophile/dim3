/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Poses

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

      Pose Add, Duplicate and Delete
      
======================================================= */

int model_pose_add(model_type *model)
{
	int						n,pose_idx;
	model_pose_type			*pose,*ptr;
	model_bone_move_type	*bone_move;

		// only allow a maximum number of poses

	if (model->npose>=max_model_pose) return(-1);

		// create memory for new pose

	ptr=(model_pose_type*)valloc(sizeof(model_pose_type)*(model->npose+1));
	if (ptr==NULL) return(-1);

	if (model->poses!=NULL) {
		memmove(ptr,model->poses,(sizeof(model_pose_type)*model->npose));
		free(model->poses);
	}

	model->poses=ptr;

	pose_idx=model->npose;
	model->npose++;

		// initialize the pose
	
	pose=&model->poses[pose_idx];
	bzero(pose,sizeof(model_pose_type));
	
	strcpy(pose->name,"New Pose");
	
	bone_move=pose->bone_moves;
	
	for (n=0;n!=model->nbone;n++) {
		bone_move->acceleration=0;
		bone_move->skip_blended=FALSE;
		
		bone_move->rot.x=bone_move->rot.y=bone_move->rot.z=0.0f;
		bone_move->mov.x=bone_move->mov.y=bone_move->mov.z=1.0f;
		
		bone_move->constraint.bone_idx=-1;
		bone_move->constraint.offset.x=bone_move->constraint.offset.y=bone_move->constraint.offset.z=0;
		
		bone_move++;
	}
	
	return(pose_idx);
}

int model_pose_duplicate(model_type *model,int pose_idx)
{
	int				dup_pose_idx;

	dup_pose_idx=model_pose_add(model);
	if (dup_pose_idx==-1) return(-1);
	
	memmove(&model->poses[dup_pose_idx],&model->poses[pose_idx],sizeof(model_pose_type));
	strcat(model->poses[dup_pose_idx].name," copy");
	
	return(dup_pose_idx);
}

void model_pose_delete(model_type *model,int pose_idx)
{
	int					n,k,nanimate,npose_move;
	model_pose_type		*ptr;
	model_animate_type	*animate;
    
        // fix pose indexes
        
    nanimate=model->nanimate;
    animate=model->animates;
    
    for (n=0;n!=nanimate;n++) {
        npose_move=animate->npose_move;
		
        for (k=0;k!=npose_move;k++) {
            if (animate->pose_moves[k].pose_idx>=pose_idx) animate->pose_moves[k].pose_idx--;
        }
        animate++;
    }

		// is the list completely empty?

	if (model->npose==1) {
		free(model->poses);
		model->poses=NULL;
		model->npose=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(model_pose_type*)valloc(sizeof(model_pose_type)*(model->npose-1));

	if (ptr==NULL) {
		if (pose_idx<(model->npose-1)) {
			memmove(&model->poses[pose_idx],&model->poses[pose_idx+1],(sizeof(model_pose_type)*((model->npose-pose_idx)-1)));
		}
	}
	else {

		if (pose_idx>0) {
			memmove(ptr,model->poses,(sizeof(model_pose_type)*pose_idx));
		}
		if (pose_idx<(model->npose-1)) {
			memmove(&ptr[pose_idx],&model->poses[pose_idx+1],(sizeof(model_pose_type)*((model->npose-pose_idx)-1)));
		}

		free(model->poses);
		model->poses=ptr;
	}
	
	model->npose--;
}

/* =======================================================

      Clear Pose
      
======================================================= */

void model_pose_clear(model_type *model,int pose_idx)
{
	int						n,nbone;
	model_bone_move_type	*bone_move;
	
	nbone=model->nbone;
	bone_move=model->poses[pose_idx].bone_moves;
	
	for (n=0;n!=nbone;n++) {
		bone_move->acceleration=0;
		bone_move->skip_blended=FALSE;
		
		bone_move->rot.x=bone_move->rot.y=bone_move->rot.z=0.0f;
		bone_move->mov.x=bone_move->mov.y=bone_move->mov.z=1.0f;
		
		bone_move->constraint.bone_idx=-1;
		bone_move->constraint.offset.x=bone_move->constraint.offset.y=bone_move->constraint.offset.z=0;
				
		bone_move++;
	}
}

/* =======================================================

      Is Pose Used
      
======================================================= */

bool model_check_pose_in_animation(model_type *model,int pose_idx)
{
	int					n,k,nanimate,npose_move;
	model_animate_type	*animate;
    
    nanimate=model->nanimate;
    animate=model->animates;
    
    for (n=0;n!=nanimate;n++) {
        npose_move=animate->npose_move;
		
        for (k=0;k!=npose_move;k++) {
            if (animate->pose_moves[k].pose_idx==pose_idx) return(TRUE);
		}
        animate++;
    }
	
	return(FALSE);
}

/* =======================================================

      Blending Skip Settings
      
======================================================= */

void model_pose_blend_set_all(model_type *model,int pose_idx,bool skip_blended)
{
	int						n,nbone;
	model_bone_move_type	*bone_move;
	
	nbone=model->nbone;
	bone_move=model->poses[pose_idx].bone_moves;
	
	for (n=0;n!=nbone;n++) {
		bone_move->skip_blended=skip_blended;
		bone_move++;
	}
}

