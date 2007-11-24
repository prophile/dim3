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
	int						n,npose;
	model_bone_move_type	*bone_move;
	
	npose=model->npose;
	if (npose>=max_model_pose) return(-1);
	
	bzero(&model->poses[npose],sizeof(model_pose_type));
	
	strcpy(model->poses[npose].name,"New Pose");
	
	bone_move=model->poses[npose].bone_moves;
	for (n=0;n<model->nbone;n++) {
		bone_move->rot.x=0;
		bone_move->rot.z=0;
		bone_move->rot.y=0;
		bone_move->mov.x=1;
		bone_move->mov.z=1;
		bone_move->mov.y=1;
		bone_move->acceleration=0;
		bone_move++;
	}
	
	model->npose++;
	
	return(npose);
}

int model_pose_duplicate(model_type *model,int pose_idx)
{
	int				npose;

		// duplicate selected pose
		
	npose=model->npose;
	if (npose>=max_model_pose) return(-1);
	
	memmove(&model->poses[npose],&model->poses[pose_idx],sizeof(model_pose_type));
	
	strcat(model->poses[npose].name," copy");

	model->npose++;
	
	return(npose);
}

void model_pose_delete(model_type *model,int pose_idx)
{
	int					n,k,npose,nanimate,npose_move,sz;
	model_animate_type	*animate;
    
        // delete pose
    
    npose=model->npose;
    
    if (pose_idx<(npose-1)) {
        sz=((npose-1)-pose_idx)*sizeof(model_pose_type);
        memmove(&model->poses[pose_idx],&model->poses[pose_idx+1],sz);
    }
    
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
		bone_move->rot.x=0;
		bone_move->rot.z=0;
		bone_move->rot.y=0;
		bone_move->mov.x=1;
		bone_move->mov.z=1;
		bone_move->mov.y=1;
		bone_move->acceleration=0;
		
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

