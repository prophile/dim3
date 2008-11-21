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
	int					animate_idx;
	model_animate_type	*animate,*ptr;

		// only allow a maximum number of animations

	if (model->nanimate>=max_model_animate) return(-1);

		// create memory for new animation

	ptr=(model_animate_type*)malloc(sizeof(model_animate_type)*(model->nanimate+1));
	if (ptr==NULL) return(-1);

	if (model->animates!=NULL) {
		memmove(ptr,model->animates,(sizeof(model_animate_type)*model->nanimate));
		free(model->animates);
	}

	model->animates=ptr;

	animate_idx=model->nanimate;
	model->nanimate++;

		// initialize animate

	animate=&model->animates[animate_idx];
	bzero(animate,sizeof(model_animate_type));

	strcpy(animate->name,"New Animation");
	animate->loop=TRUE;
	
	animate->npose_move=0;
	animate->loop_start=-1;	
	animate->loop_end=-1;	
	
	return(animate_idx);
}

int model_animate_duplicate(model_type *model,int animate_idx)
{
	int					dup_animate_idx;
	
	dup_animate_idx=model_animate_add(model);
	if (dup_animate_idx==-1) return(-1);
	
	memmove(&model->animates[dup_animate_idx],&model->animates[animate_idx],sizeof(model_animate_type));
	strcat(model->animates[dup_animate_idx].name," copy");
	
	return(dup_animate_idx);
}

void model_animate_delete(model_type *model,int animate_idx)
{
	model_animate_type	*ptr;

		// is the list completely empty?

	if (model->nanimate==1) {
		free(model->animates);
		model->animates=NULL;
		model->nanimate=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(model_animate_type*)malloc(sizeof(model_animate_type)*(model->nanimate-1));

	if (ptr==NULL) {
		if (animate_idx<(model->nanimate-1)) {
			memmove(&model->animates[animate_idx],&model->animates[animate_idx+1],(sizeof(model_animate_type)*((model->nanimate-animate_idx)-1)));
		}
	}
	else {

		if (animate_idx>0) {
			memmove(ptr,model->animates,(sizeof(model_animate_type)*animate_idx));
		}
		if (animate_idx<(model->nanimate-1)) {
			memmove(&ptr[animate_idx],&model->animates[animate_idx+1],(sizeof(model_animate_type)*((model->nanimate-animate_idx)-1)));
		}

		free(model->animates);
		model->animates=ptr;
	}
	
	model->nanimate--;
}

/* =======================================================

      Insert and Delete Animation Poses
      
======================================================= */

int model_animate_pose_insert(model_type *model,int animate_idx,int animate_pose_idx,int pose_idx)
{
	int					k,t,npose,sz;
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
 	
	animate->pose_moves[k].sound.name[0]=0x0;
	animate->pose_moves[k].sound.pitch=1.0f;
	animate->pose_moves[k].sound.no_position=FALSE;
	
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

  	animate->pose_moves[k].particle.count=0;

	for (t=0;t!=max_model_animate_particle;t++) {
		animate->pose_moves[k].particle.particles[t].bone_idx=-1;
		animate->pose_moves[k].particle.particles[t].name[0]=0x0;
		animate->pose_moves[k].particle.particles[t].motion=FALSE;
		animate->pose_moves[k].particle.particles[t].motion_factor=1.0f;
		animate->pose_moves[k].particle.particles[t].stick=FALSE;
		animate->pose_moves[k].particle.particles[t].slop.x=0;
		animate->pose_moves[k].particle.particles[t].slop.y=0;
		animate->pose_moves[k].particle.particles[t].slop.z=0;
	}

  	animate->pose_moves[k].ring.count=0;

	for (t=0;t!=max_model_animate_ring;t++) {
		animate->pose_moves[k].ring.rings[t].bone_idx=-1;
		animate->pose_moves[k].ring.rings[t].name[0]=0x0;
		animate->pose_moves[k].ring.rings[t].angle=FALSE;
		animate->pose_moves[k].ring.rings[t].slop.x=0;
		animate->pose_moves[k].ring.rings[t].slop.y=0;
		animate->pose_moves[k].ring.rings[t].slop.z=0;
	}

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

/* =======================================================

      Particles
      
======================================================= */

int model_animate_add_particle(model_type *model,int animate_idx,int animate_pose_idx)
{
	int									idx;
	model_pose_move_particle_type		*pose_particle;
	model_particle_type					*particle;
	
	pose_particle=&model->animates[animate_idx].pose_moves[animate_pose_idx].particle;
	if (pose_particle->count>=max_model_animate_particle) return(-1);
	
	idx=pose_particle->count;
	pose_particle->count++;
	
	particle=&pose_particle->particles[idx];
	particle->bone_idx=-1;
	particle->name[0]=0x0;
	particle->motion=FALSE;
	particle->motion_factor=1.0f;
	particle->stick=FALSE;
	particle->slop.x=0;
	particle->slop.y=0;
	particle->slop.z=0;
	
	return(idx);
}

void model_animate_delete_particle(model_type *model,int animate_idx,int animate_pose_idx,int idx)
{
	int									sz;
	model_pose_move_particle_type		*pose_particle;
	
	pose_particle=&model->animates[animate_idx].pose_moves[animate_pose_idx].particle;
	if (pose_particle->count==0) return;
	
	sz=((pose_particle->count-idx)-1)*sizeof(model_particle_type);
	if (sz>0) memmove(&pose_particle->particles[idx],&pose_particle->particles[idx+1],sz);
	
	pose_particle->count--;
}

/* =======================================================

      Rings
      
======================================================= */

int model_animate_add_ring(model_type *model,int animate_idx,int animate_pose_idx)
{
	int									idx;
	model_pose_move_ring_type			*pose_ring;
	model_ring_type						*ring;
	
	pose_ring=&model->animates[animate_idx].pose_moves[animate_pose_idx].ring;
	if (pose_ring->count>=max_model_animate_ring) return(-1);
	
	idx=pose_ring->count;
	pose_ring->count++;
	
	ring=&pose_ring->rings[idx];
	ring->bone_idx=-1;
	ring->name[0]=0x0;
	ring->angle=FALSE;
	ring->slop.x=0;
	ring->slop.y=0;
	ring->slop.z=0;
	
	return(idx);
}

void model_animate_delete_ring(model_type *model,int animate_idx,int animate_pose_idx,int idx)
{
	int									sz;
	model_pose_move_ring_type			*pose_ring;
	
	pose_ring=&model->animates[animate_idx].pose_moves[animate_pose_idx].ring;
	if (pose_ring->count==0) return;
	
	sz=((pose_ring->count-idx)-1)*sizeof(model_ring_type);
	if (sz>0) memmove(&pose_ring->rings[idx],&pose_ring->rings[idx+1],sz);
	
	pose_ring->count--;
}
