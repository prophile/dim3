/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Animations

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

      Add, Duplicate and Delete Animations
      
======================================================= */

int model_animate_add(model_type *model)
{
	int				nanimate;
	
	nanimate=model->nanimate;
	if (nanimate>=max_model_animate) return(-1);
	
	strcpy(model->animates[nanimate].name,"New Animation");
	model->animates[nanimate].loop=TRUE;
	
	model->animates[nanimate].npose_move=0;	
	model->animates[nanimate].loop_start=-1;	
	model->animates[nanimate].loop_end=-1;	
	model->nanimate++;
	
	return(nanimate);
}

int model_animate_duplicate(model_type *model,int animate_idx)
{
	int				nanimate;
	
	nanimate=model->nanimate;
	if (nanimate>=max_model_animate) return(-1);
	
	memmove(&model->animates[nanimate],&model->animates[animate_idx],sizeof(model_animate_type));
	strcat(model->animates[nanimate].name," copy");

	model->nanimate++;
	
	return(nanimate);
}

void model_animate_delete(model_type *model,int animate_idx)
{
	int				nanimate,sz;
	
	nanimate=model->nanimate;

    if (animate_idx<(nanimate-1)) {
        sz=((nanimate-1)-animate_idx)*sizeof(model_animate_type);
        memmove(&model->animates[animate_idx],&model->animates[animate_idx+1],sz);
    }
	
	model->nanimate--;
}

/* =======================================================

      Insert and Delete Animation Poses
      
======================================================= */

int model_animate_pose_insert(model_type *model,int animate_idx,int animate_pose_idx,int pose_idx)
{
	int					k,npose,sz;
	model_animate_type	*animate;
	
	animate=&model->animates[animate_idx];
	
	npose=animate->npose_move;
	if (npose>=max_model_animate_pose) return(-1);

        // find a place to add it
        
    if ((animate_pose_idx!=-1) && (animate_pose_idx>=(npose-1))) {
        k=npose;
    }
    else {
        sz=(npose-(animate_pose_idx+1))*sizeof(model_pose_move_type);
        memmove(&animate->pose_moves[animate_pose_idx+2],&animate->pose_moves[animate_pose_idx+1],sz);
    
        k=animate_pose_idx+1;
    }
		
	animate->pose_moves[k].pose_idx=pose_idx;
	animate->pose_moves[k].msec=100;
	
	animate->pose_moves[k].sway.z=0;
	animate->pose_moves[k].sway.x=0;
	animate->pose_moves[k].sway.y=0;
	
	animate->pose_moves[k].mov.z=0;
	animate->pose_moves[k].mov.x=0;
	animate->pose_moves[k].mov.y=0;
	
	animate->pose_moves[k].effect_bone_idx=-1;
 	
	animate->pose_moves[k].sound.name[0]=0x0;
	animate->pose_moves[k].sound.pitch=1.0f;
	
	animate->pose_moves[k].particle.name[0]=0x0;
	animate->pose_moves[k].particle.motion=FALSE;
	animate->pose_moves[k].particle.motion_factor=1.0f;
	animate->pose_moves[k].particle.stick=FALSE;
	animate->pose_moves[k].particle.slop.x=0;
	animate->pose_moves[k].particle.slop.y=0;
	animate->pose_moves[k].particle.slop.z=0;
	
 	animate->pose_moves[k].flash.intensity=0;
 	animate->pose_moves[k].flash.flash_msec=0;
 	animate->pose_moves[k].flash.fade_msec=0;
 	animate->pose_moves[k].flash.intensity=0;
 	animate->pose_moves[k].flash.col.r=1.0f;
 	animate->pose_moves[k].flash.col.g=1.0f;
 	animate->pose_moves[k].flash.col.b=1.0f;
	
	animate->pose_moves[k].shake.distance=0;
	animate->pose_moves[k].shake.size=0;
	animate->pose_moves[k].shake.life_msec=0;
  
	animate->npose_move++;
	
	return(k);
}

void model_animate_pose_delete(model_type *model,int animate_idx,int animate_pose_idx)
{
	int					npose,sz;
	model_animate_type	*animate;
	
	animate=&model->animates[animate_idx];
	
	npose=animate->npose_move;
    if (npose==1) return;
    
    if (animate_pose_idx<(npose-1)) {
        sz=((npose-1)-animate_pose_idx)*sizeof(model_pose_move_type);
        memmove(&animate->pose_moves[animate_pose_idx],&animate->pose_moves[animate_pose_idx+1],sz);
    }
    
    animate->npose_move--;
    
    if (animate->loop_start>=animate->npose_move) animate->loop_start=animate->npose_move-1;
    if (animate->loop_end>=animate->npose_move) animate->loop_end=animate->npose_move-1;
}

/* =======================================================

      Animation Looping
      
======================================================= */

void model_animate_set_loop_start(model_type *model,int animate_idx,int animate_pose_idx)
{
	model_animate_type	*animate;
	
	animate=&model->animates[animate_idx];
	animate->loop_start=animate_pose_idx;
	
	if (animate->loop_end!=-1) {
		if (animate->loop_start>animate->loop_end) animate->loop_end=animate->loop_start;
	}
}

void model_animate_set_loop_end(model_type *model,int animate_idx,int animate_pose_idx)
{
	model_animate_type	*animate;
	
	animate=&model->animates[animate_idx];
	animate->loop_end=animate_pose_idx;
	
	if (animate->loop_start!=-1) {
		if (animate->loop_start>animate->loop_end) animate->loop_start=animate->loop_end;
	}
}
