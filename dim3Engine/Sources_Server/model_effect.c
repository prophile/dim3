/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Effects (Sounds, Particles)

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "models.h"
#include "effects.h"
#include "sounds.h"
#include "network.h"
#include "consoles.h"
#include "video.h"

extern server_type				server;
extern view_type				view;
extern map_type					map;
extern network_setup_type		net_setup;

/* =======================================================

      Model Effect Setup
      
======================================================= */

void model_animation_effect_setup(model_type *mdl)
{
	int						n,k,t,nanimate,npose_move;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

		// get sound, particle, and ring indexes
		// so they don't have to be looked up every time
		// they are launched

	nanimate=mdl->nanimate;
	animate=mdl->animates;

	for (n=0;n!=nanimate;n++) {

		npose_move=animate->npose_move;
		pose_move=animate->pose_moves;

		for (k=0;k!=npose_move;k++) {

			pose_move->sound.buffer_idx=-1;
			if (pose_move->sound.name[0]!=0x0) pose_move->sound.buffer_idx=al_find_buffer(pose_move->sound.name);

			for (t=0;t!=pose_move->particle.count;t++) {
				pose_move->particle.particles[t].particle_idx=-1;
				if (pose_move->particle.particles[t].name[0]!=0x0) pose_move->particle.particles[t].particle_idx=particle_find_index(pose_move->particle.particles[t].name);
			}
			
			for (t=0;t!=pose_move->ring.count;t++) {
				pose_move->ring.rings[t].ring_idx=-1;
				if (pose_move->ring.rings[t].name[0]!=0x0) pose_move->ring.rings[t].ring_idx=ring_find_index(pose_move->ring.rings[t].name);
			}

			pose_move++;
		}

		animate++;
	}
}

/* =======================================================

      Model Effect Bone Position
      
======================================================= */

void model_animation_effect_launch_bone_position(model_draw *draw,int animate_idx,int pose_idx,int bone_idx,d3pnt *pt)
{
	model_type				*mdl;
	model_draw_setup		*setup;
	model_animate_type		*animate;

		// default to center of model

	pt->x=draw->pnt.x;
	pt->y=draw->pnt.y;
	pt->z=draw->pnt.z;

		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

	setup=&draw->setup;
	
	animate=&mdl->animates[animate_idx];
	if ((pose_idx<0) || (pose_idx>=animate->npose_move)) return;

	if (bone_idx!=-1) {
		model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,&pt->x,&pt->y,&pt->z);
		pt->x+=draw->pnt.x;
		pt->y+=draw->pnt.y;
		pt->z+=draw->pnt.z;

		if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&pt->x,&pt->y,&pt->z);
	}
}

/* =======================================================

      Model Effect Particles
      
======================================================= */

void model_animation_effect_launch_particle(model_particle_type *particle,d3pnt *pt,d3ang *ang,model_draw_connect *connect,int bone_idx)
{
	d3pnt			spt;
	particle_rotate	rot;
	particle_motion	motion;
	
	memmove(&spt,pt,sizeof(d3pnt));
	
		// make particle slop
		
	if (particle->slop.x!=0) spt.x+=random_int(particle->slop.x*2)-particle->slop.x;
	if (particle->slop.y!=0) spt.y+=random_int(particle->slop.y*2)-particle->slop.y;
	if (particle->slop.z!=0) spt.z+=random_int(particle->slop.z*2)-particle->slop.z;
			
		// setup particle

	rot.on=FALSE;

	motion.bone_idx=-1;
	motion.vct.x=motion.vct.y=motion.vct.z=0.0f;

	if (particle->rotate) {
		rot.on=TRUE;
		memmove(&rot.ang,ang,sizeof(d3ang));
	}

	if (particle->motion) {
		memmove(&motion.vct,&connect->motion_vct,sizeof(d3vct));
		motion.vct.x*=particle->motion_factor;
		motion.vct.y*=particle->motion_factor;
		motion.vct.z*=particle->motion_factor;
	}

	if (particle->stick) {
		motion.obj_uid=connect->obj_uid;
		motion.weap_uid=connect->weap_uid;
		motion.proj_uid=connect->proj_uid;
		motion.bone_idx=bone_idx;
	}

		// spawn particle

	particle_spawn(particle->particle_idx,&spt,&rot,&motion);
}

/* =======================================================

      Model Effect Rings
      
======================================================= */

void model_animation_effect_launch_ring(model_ring_type *ring,d3pnt *pt,d3ang *ang)
{
	d3pnt			spt;

		// make ring slop

	memmove(&spt,pt,sizeof(d3pnt));
		
	if (ring->slop.x!=0) spt.x+=random_int(ring->slop.x*2)-ring->slop.x;
	if (ring->slop.y!=0) spt.y+=random_int(ring->slop.y*2)-ring->slop.y;
	if (ring->slop.z!=0) spt.z+=random_int(ring->slop.z*2)-ring->slop.z;
	
		// spawn ring
		
	if (ring->angle) {
		ring_spawn(ring->ring_idx,&spt,ang);
	}
	else {
		ring_spawn(ring->ring_idx,&spt,NULL);
	}
}

/* =======================================================

      Model Effect Launches
      
======================================================= */

void model_animation_effect_launch(model_draw *draw,int animate_idx,int pose_idx)
{
	int						t;
	d3pnt					pt;
	model_type				*mdl;
	model_draw_setup		*setup;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

	setup=&draw->setup;
	
	animate=&mdl->animates[animate_idx];
	if ((pose_idx<0) || (pose_idx>=animate->npose_move)) return;
	
	pose_move=&animate->pose_moves[pose_idx];

		// sounds
		
	if (pose_move->sound.buffer_idx!=-1) {
		model_animation_effect_launch_bone_position(draw,animate_idx,pose_idx,pose_move->sound.bone_idx,&pt);

		al_play_source(pose_move->sound.buffer_idx,pt.x,pt.y,pt.z,pose_move->sound.pitch,FALSE,FALSE,FALSE,draw->player);
		object_watch_sound_alert(pt.x,pt.y,pt.z,draw->connect.obj_uid,pose_move->sound.name);	// sound watches

		if ((net_setup.client.joined) && (draw->connect.net_sound)) net_client_send_sound(net_setup.client.remote_uid,pt.x,pt.y,pt.z,pose_move->sound.pitch,pose_move->sound.name);
	}

		// mesh fades

	if (pose_move->mesh_fade.mesh_idx!=-1) {
		model_mesh_fade_start(draw,pose_move->mesh_fade.mesh_idx,pose_move->mesh_fade.fade_in_msec,pose_move->mesh_fade.fade_life_msec,pose_move->mesh_fade.fade_out_msec);
	}
	
		// flash
		
	if (pose_move->flash.intensity!=0) {
		model_animation_effect_launch_bone_position(draw,animate_idx,pose_idx,pose_move->flash.bone_idx,&pt);
		effect_spawn_flash(&pt,&pose_move->flash.col,pose_move->flash.intensity,pose_move->flash.flash_msec,pose_move->flash.fade_msec);
	}
	
		// shake
		
	if (pose_move->shake.distance!=0) {
		effect_spawn_shake(&pt,pose_move->shake.distance,pose_move->shake.size,pose_move->shake.life_msec);
	}
	
		// particles
		
	for (t=0;t!=pose_move->particle.count;t++) {
		if (pose_move->particle.particles[t].particle_idx!=-1) {
			model_animation_effect_launch_bone_position(draw,animate_idx,pose_idx,pose_move->particle.particles[t].bone_idx,&pt);
			model_animation_effect_launch_particle(&pose_move->particle.particles[t],&pt,&setup->ang,&draw->connect,pose_move->particle.particles[t].bone_idx);
		}
	}

		// rings
		
	for (t=0;t!=pose_move->ring.count;t++) {
		if (pose_move->ring.rings[t].ring_idx!=-1) {
			model_animation_effect_launch_bone_position(draw,animate_idx,pose_idx,pose_move->ring.rings[t].bone_idx,&pt);
			model_animation_effect_launch_ring(&pose_move->ring.rings[t],&pt,&setup->ang);
		}
	}
}

