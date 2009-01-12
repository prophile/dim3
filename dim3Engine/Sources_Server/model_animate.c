/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Animations

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#include "models.h"
#include "consoles.h"
#include "lights.h"
#include "video.h"

extern server_type				server;
extern view_type				view;
extern map_type					map;

extern int game_time_get(void);
extern void map_calculate_light_color_normal(double x,double y,double z,float *cf,float *nf);

/* =======================================================

      Model Fills
      
======================================================= */

void model_change_fill(model_draw *draw,int wfill,int txt)
{
	int					count;
	model_type			*mdl;
	texture_type		*texture;
    
    if ((wfill<0) || (wfill>=max_model_texture)) return;
    
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
	count=model_count_texture_frames(mdl,wfill);
    texture=&mdl->textures[wfill];
    if ((txt<0) || (txt>=count)) return;
    
    draw->cur_texture_frame[wfill]=(unsigned char)txt;
}

/* =======================================================

      Model Start/Stop Animation
      
======================================================= */

void model_get_current_animation_name(model_draw *draw,char *name)
{
	model_type					*mdl;
	model_draw_animation		*draw_animation;
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(-1);

	draw_animation=&draw->animations[draw->script_animation_idx];
	if (draw_animation->animate_idx==-1) {
		name[0]=0x0;
		return;
	}
	
	strcpy(name,mdl->animates[draw_animation->animate_idx].name);
}

int model_find_animation_from_draw(model_draw *draw,char *name)
{
	model_type					*mdl;
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(-1);
	
	return(model_find_animate(mdl,name));
}

bool model_start_animation(model_draw *draw,char *name)
{
	int							idx;
	model_draw_animation		*draw_animation;
	
	idx=model_find_animation_from_draw(draw,name);
	if (idx==-1) return(FALSE);
	
	draw_animation=&draw->animations[draw->script_animation_idx];

		// smooth out changes in animations

	if (draw_animation->mode==am_playing) {
		draw_animation->smooth_animate_idx=draw_animation->animate_idx;
		draw_animation->smooth_pose_move_idx=draw_animation->pose_move_idx;
	}
	else {
		draw_animation->smooth_animate_idx=-1;
		draw_animation->smooth_pose_move_idx=0;
	}

		// set next animation

	draw_animation->tick=game_time_get();

	draw_animation->animate_idx=idx;
	draw_animation->animate_next_idx=-1;
	draw_animation->pose_move_idx=0;

    draw_animation->mode=am_playing;
	
	model_animation_effect_launch(draw,idx,0);
    
	return(TRUE);
}

void model_stop_animation(model_draw *draw)
{
	model_draw_animation	*draw_animation;
	
	draw_animation=&draw->animations[draw->script_animation_idx];
	
	draw_animation->animate_idx=-1;
	draw_animation->animate_next_idx=-1;
	draw_animation->pose_move_idx=0;

	draw_animation->smooth_animate_idx=-1;
	draw_animation->smooth_pose_move_idx=0;
    
    draw_animation->mode=am_stopped;
}

bool model_cancel_animation(model_draw *draw,char *name)
{
	int						idx;
	model_draw_animation	*draw_animation;

	idx=model_find_animation_from_draw(draw,name);
	if (idx==-1) return(FALSE);
	
	draw_animation=&draw->animations[draw->script_animation_idx];
	
	if (draw_animation->animate_idx==idx) model_stop_animation(draw);
	
	return(TRUE);
}

bool model_change_animation(model_draw *draw,char *name)
{
	int						idx;
	model_draw_animation	*draw_animation;

		// if not playing, then start new animation

	draw_animation=&draw->animations[draw->script_animation_idx];
	if (draw_animation->mode!=am_playing) return(model_start_animation(draw,name));

		// chain to next animation

	idx=model_find_animation_from_draw(draw,name);
	if (idx==-1) return(FALSE);

	draw_animation->animate_next_idx=idx;
	
	return(TRUE);
}

bool model_interrupt_animation(model_draw *draw,char *name)
{
	int						old_animate_idx;
	model_draw_animation	*draw_animation;

		// remember current animation

	draw_animation=&draw->animations[draw->script_animation_idx];

	old_animate_idx=draw_animation->animate_idx;

		// start interupted animation

	if (!model_start_animation(draw,name)) return(FALSE);

		// old animation is chained to new one

	draw_animation->animate_next_idx=old_animate_idx;
	
	return(TRUE);
}

/* =======================================================

      Model Run Animation
      
======================================================= */

void model_run_animation_single(model_draw *draw,int animation_idx)
{
	int						mode,tick,nxt_tick;
	model_type				*mdl;
	model_animate_type		*animate;
 	model_draw_animation	*draw_animation;
    
        // stopped or resting animation?

	draw_animation=&draw->animations[animation_idx];
        
    mode=draw_animation->mode;
    if (mode!=am_playing) return;
	
        // get model
        
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// if no poses, then no animation
	
	animate=&mdl->animates[draw_animation->animate_idx];
	if (animate->npose_move==0) return;
	
		// loop until all pose changes for this
		// time frame are accomplished
	
	tick=game_time_get();
	nxt_tick=draw_animation->tick+animate->pose_moves[draw_animation->pose_move_idx].msec;
	
	while (tick>=nxt_tick) {
		draw_animation->pose_move_idx++;

			// turn off animation smoothing once we get
			// past any animation

		draw_animation->smooth_animate_idx=-1;
		draw_animation->smooth_pose_move_idx=0;

			// is there a new animation waiting?

		if (draw_animation->animate_next_idx!=-1) {
			
				// at end of current animation
				// switch to next animation

			if (draw_animation->pose_move_idx>=animate->npose_move) {

					// smooth between the animations

				draw_animation->smooth_animate_idx=draw_animation->animate_idx;
				draw_animation->smooth_pose_move_idx=animate->npose_move-1;
				
					// set the next animation

				draw_animation->animate_idx=draw_animation->animate_next_idx;
				draw_animation->animate_next_idx=-1;
				draw_animation->pose_move_idx=0;
			}

		}

			// no animation waiting, is it looping or non-looping

		else {

				// if looping, stay between loop spots

            if (animate->loop) {
                if (draw_animation->pose_move_idx>animate->loop_end) {
					draw_animation->pose_move_idx=animate->loop_start;
				}
            }
            else {

					// non-looping, go to last pose and then
					// into resting

				if (draw_animation->pose_move_idx>=animate->npose_move) {
					draw_animation->mode=am_resting;
					draw_animation->pose_move_idx=animate->npose_move-1;
					return;
				}
			}
		}

			// next animation tick

		draw_animation->tick=nxt_tick;
		nxt_tick=draw_animation->tick+animate->pose_moves[draw_animation->pose_move_idx].msec;
		
			// any effects

		model_animation_effect_launch(draw,draw_animation->animate_idx,draw_animation->pose_move_idx);
	}
}

void model_run_animation(model_draw *draw)
{
	int						n;
   
		// no model, no animation

	if (draw->uid==-1) return;

		// run all the animations

	for (n=0;n!=max_model_blend_animation;n++) {
		model_run_animation_single(draw,n);
	}
}

/* =======================================================

      Determine the Current Pose Moves for an Animation
      
======================================================= */

model_pose_move_type* model_calc_animation_first_pose(model_type *mdl,model_draw *draw,int blend_idx)
{
	model_draw_animation	*draw_animation;
	model_animate_type		*animate;

	draw_animation=&draw->animations[blend_idx];
	animate=&mdl->animates[draw_animation->animate_idx];

		// use last pose move if resting

	if (draw_animation->mode==am_resting) return(&animate->pose_moves[animate->npose_move-1]);
    
		// calculate the start pose
		// if there was a previous animation chained to this one
		// then tween from the previous pose to the next pose instead
		// of the current pose

	if (draw_animation->smooth_animate_idx!=-1) {
		return(&mdl->animates[draw_animation->smooth_animate_idx].pose_moves[draw_animation->smooth_pose_move_idx]);
	}
	
		// else use the current pose

	return(&animate->pose_moves[draw_animation->pose_move_idx]);
}

model_pose_move_type* model_calc_animation_actual_first_pose(model_type *mdl,model_draw *draw,int blend_idx)
{
	model_draw_animation	*draw_animation;
	model_animate_type		*animate;

	draw_animation=&draw->animations[blend_idx];
	animate=&mdl->animates[draw_animation->animate_idx];

		// use last pose move if resting

	if (draw_animation->mode==am_resting) return(&animate->pose_moves[animate->npose_move-1]);
    
		// else use the current pose

	return(&animate->pose_moves[draw_animation->pose_move_idx]);
}

model_pose_move_type* model_calc_animation_second_pose(model_type *mdl,model_draw *draw,int blend_idx)
{
	int						pose_move_idx;
	model_draw_animation	*draw_animation;
	model_animate_type		*animate;

	draw_animation=&draw->animations[blend_idx];
	animate=&mdl->animates[draw_animation->animate_idx];

		// no second pose move if not playing

	if (draw_animation->mode!=am_playing) return(NULL);

		// figure out next pose move

	pose_move_idx=draw_animation->pose_move_idx+1;

	if ((draw_animation->animate_next_idx==-1) && (animate->loop)) {
		if (pose_move_idx>animate->loop_end) {
			pose_move_idx=animate->loop_start;
		}
	}
	else {
		if (pose_move_idx>=animate->npose_move) {
			pose_move_idx=animate->npose_move-1;
		}
	}

	return(&animate->pose_moves[pose_move_idx]);
}

/* =======================================================

      Calculate Model Animation
      
======================================================= */

void model_calc_animation_poses(model_draw *draw)
{
	int						n,k;
	model_type				*mdl;
	model_draw_animation	*animation;
    model_pose_move_type	*pose_move_1,*pose_move_2;
	model_draw_setup		*setup;
	model_draw_pose_type	*setup_pose;

		// clear animations

	setup=&draw->setup;

	setup_pose=setup->poses;

	for (n=0;n!=max_model_blend_animation;n++) {
		setup_pose->idx_1=setup_pose->idx_2=-1;
		setup_pose->factor=0.0f;
		setup_pose->acceleration=0.0f;
		setup_pose++;
	}

		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// run through all animations

	for (n=0;n!=max_model_blend_animation;n++) {

		animation=&draw->animations[n];
		setup_pose=&setup->poses[n];

			// animation playing?
        
		if (animation->mode==am_stopped) continue;

		pose_move_1=model_calc_animation_first_pose(mdl,draw,n);
		pose_move_2=model_calc_animation_second_pose(mdl,draw,n);

			// single pose?
		
		setup_pose->idx_1=pose_move_1->pose_idx;

		if (pose_move_2==NULL) continue;
    
			// two poses

		setup_pose->idx_2=pose_move_2->pose_idx;

			// acceleration and factors always use the
			// "actual" current pose, as the current pose can be
			// replaced by a previous animations pose when smoothing
			// and we want to get these factors out of the original

		pose_move_1=model_calc_animation_actual_first_pose(mdl,draw,n);

		setup_pose->acceleration=pose_move_1->acceleration;
    
		if (pose_move_1->msec==0) {
			setup_pose->factor=1.0f;
		}
		else {
			k=(1000*(game_time_get()-animation->tick))/pose_move_1->msec;
			if (k<0) k=0;
			if (k>1000) k=1000;

			setup_pose->factor=((float)(1000-k))/1000.0f;
		}
	}
}

void model_calc_animation_move_sway(model_draw *draw)
{
	float					pose_factor;
	model_type				*mdl;
	model_draw_animation	*animation;
    model_pose_move_type	*pose_move_1,*pose_move_2;
	model_draw_setup		*setup;

		// moves and sways are determined from the root animation
		// instead of all animations

		// default is no moves or sways

	setup=&draw->setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0;

		// get animations

	animation=&draw->animations[0];
    if (animation->mode==am_stopped) return;

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

	pose_move_1=model_calc_animation_first_pose(mdl,draw,0);
	pose_move_2=model_calc_animation_second_pose(mdl,draw,0);

		// only a single pose?

	if (pose_move_2==NULL) {
		setup->move.x=pose_move_1->mov.x;
		setup->move.y=pose_move_1->mov.y;
		setup->move.z=pose_move_1->mov.z;
		setup->sway.x=pose_move_1->sway.x;
		setup->sway.y=pose_move_1->sway.y;
		setup->sway.z=pose_move_1->sway.z;
		return;
	}

		// tween moves and sways

	pose_factor=setup->poses[0].factor;

	setup->move.x=pose_move_2->mov.x+((pose_move_1->mov.x-pose_move_2->mov.x)*pose_factor);
	setup->move.y=pose_move_2->mov.y+((pose_move_1->mov.y-pose_move_2->mov.y)*pose_factor);
	setup->move.z=pose_move_2->mov.z+((pose_move_1->mov.z-pose_move_2->mov.z)*pose_factor);
	setup->sway.x=pose_move_2->sway.x+((pose_move_1->sway.x-pose_move_2->sway.x)*pose_factor);
	setup->sway.y=pose_move_2->sway.y+((pose_move_1->sway.y-pose_move_2->sway.y)*pose_factor);
	setup->sway.z=pose_move_2->sway.z+((pose_move_1->sway.z-pose_move_2->sway.z)*pose_factor);
}

void model_calc_animation(model_draw *draw)
{
	model_calc_animation_poses(draw);
	model_calc_animation_move_sway(draw);
}

/* =======================================================

      Calculate Model Draw Bones
      
======================================================= */

void model_calc_draw_bones(model_draw *draw)
{
	model_type			*mdl;
		
	mdl=model_find_uid(draw->uid);
	if (mdl!=NULL) model_create_draw_bones(mdl,&draw->setup);
}

/* =======================================================

      Find Current Pose
      
======================================================= */

int model_get_current_pose(model_draw *draw)
{
	int						animate_idx,pose_move_idx;
	model_type				*mdl;
	model_draw_animation	*animation;

		// any animation playing?

	animation=&draw->animations[draw->script_animation_idx];

	animate_idx=animation->animate_idx;
	pose_move_idx=animation->pose_move_idx;

	if ((animate_idx==-1) || (pose_move_idx==-1)) return(-1);

		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(-1);

		// find pose index

	return(mdl->animates[animate_idx].pose_moves[pose_move_idx].pose_idx);
}

/* =======================================================

      Find A Draw Bone Point, Offset, Brightness
      
======================================================= */

bool model_find_bone_offset(model_draw *draw,char *pose_name,char *bone_name,int *x,int *y,int *z)
{
	int					pose_idx,bone_idx;
	model_tag			tag;
	model_type			*mdl;
		
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(FALSE);
	
		// get pose index
		
	pose_idx=-1;
	if (pose_name[0]!=0x0) {
		pose_idx=model_find_pose(mdl,pose_name);
		if (pose_idx==-1) return(FALSE);
	}
	
		// get bone index
		
	tag=text_to_model_tag(bone_name);
	bone_idx=model_find_bone(mdl,tag);
	if (bone_idx==-1) return(FALSE);
	
		// get bone

	model_calc_draw_bone_position(mdl,&draw->setup,pose_idx,bone_idx,x,y,z);

	return(TRUE);
}

bool model_find_bone_position(model_draw *draw,char *pose_name,char *bone_name,int *x,int *y,int *z)
{
	if (!model_find_bone_offset(draw,pose_name,bone_name,x,y,z)) return(FALSE);
	
	*x=(*x)+draw->pnt.x;
	*y=(*y)+draw->pnt.y;
	*z=(*z)+draw->pnt.z;

	if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),x,y,z);
	
	return(TRUE);
}

bool model_find_bone_position_for_current_animation(model_draw *draw,int bone_idx,int *x,int *y,int *z)
{
	int						animate_idx,animate_pose_move_idx,pose_idx;
	model_type				*mdl;
	model_draw_animation	*animation;
		
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(FALSE);

		// get current pose

	animation=&draw->animations[draw->script_animation_idx];

	animate_idx=animation->animate_idx;
	animate_pose_move_idx=animation->pose_move_idx;

	if ((animate_idx==-1) || (animate_pose_move_idx==-1)) return(FALSE);

	pose_idx=mdl->animates[animate_idx].pose_moves[animate_pose_move_idx].pose_idx;

		// calculate bones

	model_create_draw_bones(mdl,&draw->setup);
	model_get_draw_bone_position(&draw->setup,bone_idx,x,y,z);

	*x=(*x)+draw->pnt.x;
	*y=(*y)+draw->pnt.y;
	*z=(*z)+draw->pnt.z;

		// fix rotation

	if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),x,y,z);

	return(TRUE);
}

bool model_get_bone_brightness(model_draw *draw,char *pose_name,char *bone_name,float *bright)
{
	int				x,y,z;
	float			pc[3],pn[3];
	
		// get bone position
		
	if (!model_find_bone_position(draw,pose_name,bone_name,&x,&y,&z)) return(FALSE);
	
		// light at position

	map_calculate_light_reduce_model(draw);
	map_calculate_light_color_normal((double)x,(double)y,(double)z,pc,pn);
	
	*bright=(pc[0]+pc[1]+pc[2])/3.0f;
	if (*bright<0.0f) *bright=0.0f;
	
	return(TRUE);
}

