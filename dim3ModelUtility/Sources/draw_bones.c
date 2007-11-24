/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
        Rocco Bowling for Comulative Deform Mode
 Usage: Bone Routines

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

      Create Rotations/Moves from Neutral
      
======================================================= */

void model_create_draw_bone_from_neutral(model_type *model,model_draw_bone_type *draw_bones)
{
	int						n,nbone;
	model_bone_type			*bone;
	model_draw_bone_type   *draw_bone;

	bone=model->bones;
	draw_bone=draw_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
		draw_bone->fpnt.x=(float)bone->pnt.x;
		draw_bone->fpnt.z=(float)bone->pnt.z;
		draw_bone->fpnt.y=(float)bone->pnt.y;
		
		draw_bone->rot.x=0.0f;
		draw_bone->rot.z=0.0f;
		draw_bone->rot.y=0.0f;

		matrix_identity(&draw_bone->rot_mat);
		
		draw_bone->touch=TRUE;				// auto-touch all bones, no changes necessary
	
		bone++;
		draw_bone++;
	}
}

/* =======================================================

      Create Draw Bones (Non-Comulative)
      
======================================================= */

void model_create_draw_bone_from_pose(model_type *model,model_draw_bone_type *draw_bones,int pose_1)
{
	int						n,nbone;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone;
	model_bone_move_type	*bone_move;
	
		// build draw list from model bones
	
	bone=model->bones;
	bone_move=model->poses[pose_1].bone_moves;
	draw_bone=draw_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
		draw_bone->rot.x=bone_move->rot.x;
        draw_bone->rot.y=bone_move->rot.y;
        draw_bone->rot.z=bone_move->rot.z;

		draw_bone->parent_dist.x=bone->parent_dist.x*bone_move->mov.x;
		draw_bone->parent_dist.y=bone->parent_dist.y*bone_move->mov.y;
		draw_bone->parent_dist.z=bone->parent_dist.z*bone_move->mov.z;
		
			// auto touch bone with no parent

		if (bone->parent_idx==-1) {
			draw_bone->touch=TRUE;
			
			draw_bone->fpnt.x=(float)bone->pnt.x;
			draw_bone->fpnt.y=(float)bone->pnt.y;
			draw_bone->fpnt.z=(float)bone->pnt.z;
		}
		else {
			draw_bone->touch=FALSE;
		}
		
		bone++;
		draw_bone++;
		bone_move++;
	}
}

void model_create_draw_bone_from_pose_factor(model_type *model,model_draw_bone_type *draw_bones,int pose_1,int pose_2,float pose_factor)
{
	int						n,nbone;
	float					accel_pose_factor,
							mov_x_start,mov_z_start,mov_y_start,
							mov_x_end,mov_z_end,mov_y_end,
							rot_x_start,rot_z_start,rot_y_start,
							rot_x_end,rot_z_end,rot_y_end;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone;
	model_bone_move_type	*bone_move_start,*bone_move_end;
	
		// find the bone moves to interept from
		
	bone_move_start=model->poses[pose_1].bone_moves;
	bone_move_end=model->poses[pose_2].bone_moves;
	
		// build draw list from model bones
	
	bone=model->bones;
	draw_bone=draw_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
			// find the bone moves
		
		rot_x_start=bone_move_start->rot.x;
		rot_y_start=bone_move_start->rot.y;
		rot_z_start=bone_move_start->rot.z;

		mov_x_start=bone_move_start->mov.x;
		mov_y_start=bone_move_start->mov.y;
		mov_z_start=bone_move_start->mov.z;
		
		rot_x_end=bone_move_end->rot.x;
		rot_y_end=bone_move_end->rot.y;
		rot_z_end=bone_move_end->rot.z;
			
		mov_x_end=bone_move_end->mov.x;
		mov_y_end=bone_move_end->mov.y;
		mov_z_end=bone_move_end->mov.z;
		
			// change the factor based on acceleration

		accel_pose_factor=acceleration_calculate(pose_factor,bone_move_start->acceleration);
	
			// factor the bone moves
			
		draw_bone->rot.x=rot_x_end+((rot_x_start-rot_x_end)*accel_pose_factor);
		draw_bone->rot.y=rot_y_end+((rot_y_start-rot_y_end)*accel_pose_factor);
		draw_bone->rot.z=rot_z_end+((rot_z_start-rot_z_end)*accel_pose_factor);
		
		mov_x_start=mov_x_end+((mov_x_start-mov_x_end)*accel_pose_factor);
		draw_bone->parent_dist.x=bone->parent_dist.x*mov_x_start;
		mov_y_start=mov_y_end+((mov_y_start-mov_y_end)*accel_pose_factor);
		draw_bone->parent_dist.y=bone->parent_dist.y*mov_y_start;
		mov_z_start=mov_z_end+((mov_z_start-mov_z_end)*accel_pose_factor);
		draw_bone->parent_dist.z=bone->parent_dist.z*mov_z_start;
			
			// auto touch bone with no parent

		if (draw_bone->parent_idx==-1) {
			draw_bone->touch=TRUE;
			
			draw_bone->fpnt.x=(float)bone->pnt.x;
			draw_bone->fpnt.y=(float)bone->pnt.y;
			draw_bone->fpnt.z=(float)bone->pnt.z;
		}
		else {
			draw_bone->touch=FALSE;
		}

		bone++;
		draw_bone++;
		bone_move_start++;
		bone_move_end++;
	}
}

/* =======================================================

      Create Draw Bones (Comulative)
      
======================================================= */

void model_comulative_create_draw_bone_from_pose(model_type *model,model_draw_bone_type *draw_bones,int pose_1)
{
	int						n,nbone;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone;
	model_bone_move_type	*bone_move;
	
		// build draw list from model bones
	
	bone=model->bones;
	bone_move=model->poses[pose_1].bone_moves;
	draw_bone=draw_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
		draw_bone->rot.x=bone_move->rot.x;
        draw_bone->rot.y=bone_move->rot.y;
        draw_bone->rot.z=bone_move->rot.z;
		
		draw_bone->parent_dist.x=bone->parent_dist.x*bone_move->mov.x;
		draw_bone->parent_dist.y=bone->parent_dist.y*bone_move->mov.y;
		draw_bone->parent_dist.z=bone->parent_dist.z*bone_move->mov.z;
		
			// auto touch bone with no parent
		
		if (bone->parent_idx==-1) {
			draw_bone->touch=TRUE;
			
			draw_bone->fpnt.x=(float)bone->pnt.x;
			draw_bone->fpnt.z=(float)bone->pnt.z;
			draw_bone->fpnt.y=(float)bone->pnt.y;
		}
		else {
			draw_bone->touch=FALSE;
		}
		
		bone++;
		draw_bone++;
		bone_move++;
	}
}

void model_comulative_create_draw_bone_from_pose_factor(model_type *model,model_draw_bone_type *draw_bones,int pose_1,int pose_2,float pose_factor)
{
	int						n,nbone;
	float					accel_pose_factor,
							mov_x_start,mov_z_start,mov_y_start,
							mov_x_end,mov_z_end,mov_y_end,
							rot_x_start,rot_z_start,rot_y_start,
							rot_x_end,rot_z_end,rot_y_end;
	model_bone_type			*bone;
	model_draw_bone_type	*draw_bone;
	model_bone_move_type	*bone_move_start,*bone_move_end;
	
		// find the bone moves to interept from
	
	bone_move_start=model->poses[pose_1].bone_moves;
	bone_move_end=model->poses[pose_2].bone_moves;
	
		// build draw list from model bones
	
	bone=model->bones;
	draw_bone=draw_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		draw_bone->parent_idx=bone->parent_idx;
		
			// find the bone moves
		
		rot_x_start=bone_move_start->rot.x;
		rot_z_start=bone_move_start->rot.z;
		rot_y_start=bone_move_start->rot.y;
		
		mov_x_start=bone_move_start->mov.x;
		mov_z_start=bone_move_start->mov.z;
		mov_y_start=bone_move_start->mov.y;
		
		rot_x_end=bone_move_end->rot.x;
		rot_z_end=bone_move_end->rot.z;
		rot_y_end=bone_move_end->rot.y;
		
		mov_x_end=bone_move_end->mov.x;
		mov_z_end=bone_move_end->mov.z;
		mov_y_end=bone_move_end->mov.y;
		
			// change the factor based on acceleration

		accel_pose_factor=acceleration_calculate(pose_factor,bone_move_start->acceleration);
		
			// factor the bone moves
		
		draw_bone->rot.x=rot_x_end+((rot_x_start-rot_x_end)*accel_pose_factor);
		draw_bone->rot.z=rot_z_end+((rot_z_start-rot_z_end)*accel_pose_factor);
		draw_bone->rot.y=rot_y_end+((rot_y_start-rot_y_end)*accel_pose_factor);
		
		mov_x_start=mov_x_end+((mov_x_start-mov_x_end)*accel_pose_factor);
		draw_bone->parent_dist.x=bone->parent_dist.x*mov_x_start;
		mov_y_start=mov_y_end+((mov_y_start-mov_y_end)*accel_pose_factor);
		draw_bone->parent_dist.y=bone->parent_dist.y*mov_y_start;
		mov_z_start=mov_z_end+((mov_z_start-mov_z_end)*accel_pose_factor);
		draw_bone->parent_dist.z=bone->parent_dist.z*mov_z_start;
		
			// auto touch bone with no parent
		
		if (draw_bone->parent_idx==-1) {
			draw_bone->touch=TRUE;
			
			draw_bone->fpnt.x=(float)bone->pnt.x;
			draw_bone->fpnt.z=(float)bone->pnt.z;
			draw_bone->fpnt.y=(float)bone->pnt.y;
		}
		else {
			draw_bone->touch=FALSE;
		}
		
		bone++;
		draw_bone++;
		bone_move_start++;
		bone_move_end++;
	}
}

/* =======================================================

      Combine Model Rotations Through Parents (Non-Comulative)
      
======================================================= */

void model_combine_draw_bone_add_rotate(model_draw_bone_type *draw_bones,int parent_idx,float *rot_x,float *rot_y,float *rot_z)
{
	model_draw_bone_type		*draw_bone;

	if (parent_idx==-1) return;
	
	draw_bone=&draw_bones[parent_idx];
	
	*rot_x=(*rot_x)+draw_bone->rot.x;
	*rot_y=(*rot_y)+draw_bone->rot.y;
	*rot_z=(*rot_z)+draw_bone->rot.z;
	
	model_combine_draw_bone_add_rotate(draw_bones,draw_bone->parent_idx,rot_x,rot_y,rot_z);
}

void model_combine_draw_bone_rotations(model_type *model,model_draw_bone_type *draw_bones)
{
	int						n,nbone;
	float					rot_x,rot_y,rot_z;
	model_draw_bone_type	*draw_bone;
	
	draw_bone=draw_bones;

	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		rot_x=draw_bone->rot.x;
		rot_y=draw_bone->rot.y;
		rot_z=draw_bone->rot.z;

		if (!draw_bone->touch) model_combine_draw_bone_add_rotate(draw_bones,draw_bone->parent_idx,&rot_x,&rot_y,&rot_z);

		matrix_rotate_xyz(&draw_bone->rot_mat,rot_x,rot_y,rot_z);

		draw_bone++;
	}
}

/* =======================================================

      Combine Model Rotations Through Parents (Comulative)
      
======================================================= */

void model_comulative_combine_draw_bone_combine_rotate_by_matrix(bool * touched, model_draw_bone_type *draw_bones,int my_idx)
{
	model_draw_bone_type		*my_draw_bone,*parent_draw_bone;
	matrix_type					local_matrix;
	
	my_draw_bone=&draw_bones[my_idx];
	
	if (my_draw_bone->parent_idx!=-1) {

		parent_draw_bone=&draw_bones[my_draw_bone->parent_idx];
		
		if (touched[my_draw_bone->parent_idx]==0) {
			model_comulative_combine_draw_bone_combine_rotate_by_matrix(touched,draw_bones,my_draw_bone->parent_idx);
		}
		
		memcpy(&my_draw_bone->rot_mat,&parent_draw_bone->rot_mat,sizeof(matrix_type));
		
		my_draw_bone->rot_mat.data[0][3]=0.0f;
		my_draw_bone->rot_mat.data[1][3]=0.0f;
		my_draw_bone->rot_mat.data[2][3]=0.0f;
	}
	
	matrix_rotate_z(&local_matrix, my_draw_bone->rot.z);
	matrix_multiply(&my_draw_bone->rot_mat, &local_matrix);
	
	matrix_rotate_y(&local_matrix, my_draw_bone->rot.y);
	matrix_multiply(&my_draw_bone->rot_mat, &local_matrix);
	
	matrix_rotate_x(&local_matrix, my_draw_bone->rot.x);
	matrix_multiply(&my_draw_bone->rot_mat, &local_matrix);
	
	touched[my_idx]=1;
}

void model_comulative_combine_draw_bone_rotations(model_type *model,model_draw_bone_type *draw_bones)
{
	int						n,nbone;
	bool					touched[max_model_bone];
	model_draw_bone_type	*draw_bone;
	
	draw_bone=draw_bones;
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		matrix_identity(&draw_bone->rot_mat);
		touched[n]=0;
		draw_bone++;
	}
	
	draw_bone=draw_bones;

	for (n=0;n!=nbone;n++) {	
		if (touched[n]==0) {
			model_comulative_combine_draw_bone_combine_rotate_by_matrix(touched,draw_bones,n);
		}
		draw_bone++;
	}
}

/* =======================================================

      Move The Draw Bones to their Matrixes
      
======================================================= */

void model_move_single_draw_bone(model_type *model,model_draw_bone_type *draw_bones,model_draw_bone_type *draw_bone)
{
	int						n;
	float					fx,fz,fy;
	model_draw_bone_type	*parent_bone;
	
		// already moved?
		
	if (draw_bone->touch) return;
	
	draw_bone->touch=TRUE;

		// move parents first
	
	n=draw_bone->parent_idx;
	parent_bone=&draw_bones[n];
	model_move_single_draw_bone(model,draw_bones,parent_bone);

		// rotate the bone
	
	fx=draw_bone->parent_dist.x;
	fy=draw_bone->parent_dist.y;
	fz=draw_bone->parent_dist.z;
	
	matrix_vertex_multiply(&parent_bone->rot_mat,&fx,&fy,&fz);
	
	draw_bone->fpnt.x=parent_bone->fpnt.x+fx;
	draw_bone->fpnt.y=parent_bone->fpnt.y+fy;
	draw_bone->fpnt.z=parent_bone->fpnt.z+fz;
}

void model_move_draw_bones(model_type *model,model_draw_bone_type *draw_bones)
{
	int						n,nbone;
	model_draw_bone_type	*draw_bone;
	
	draw_bone=draw_bones;
	
	nbone=model->nbone;
	
	for (n=0;n!=nbone;n++) {
		model_move_single_draw_bone(model,draw_bones,draw_bone);
		draw_bone++;
	}
}

/* =======================================================

      Create Drawing Bones
      
======================================================= */

void model_create_draw_bones_single(model_type *model,model_draw_setup *draw_setup,model_draw_bone_type *bones)
{
	int			pose_1,pose_2;
	float		pose_factor;
	
		// get poses

	pose_1=draw_setup->poses[0].idx_1;
	pose_2=draw_setup->poses[0].idx_2;
	pose_factor=draw_setup->poses[0].factor;

		// no poses get simple neutral bones

	if (pose_1==-1) {
		model_create_draw_bone_from_neutral(model,bones);
		return;
	}

		// change pose factor for animation based acceleration

	if (pose_2!=-1) {
		pose_factor=acceleration_calculate(pose_factor,draw_setup->poses[0].acceleration);
	}

			// create bones

	switch(model->deform_mode) {
	
		case deform_mode_single_rotate:
				
				// single or tween poses
			
			if (pose_2==-1) {
				model_create_draw_bone_from_pose(model,bones,pose_1);
			}
			else {
				model_create_draw_bone_from_pose_factor(model,bones,pose_1,pose_2,pose_factor);
			}

				// combine rotations and create draw bone matrixes and positions

			model_combine_draw_bone_rotations(model,bones);
			model_move_draw_bones(model,bones);
			break;
		
		case deform_mode_comulative_rotate:
			
				// single or tween poses
			
			if (pose_2==-1) {
				model_comulative_create_draw_bone_from_pose(model,bones,pose_1);
			}
			else {
				model_comulative_create_draw_bone_from_pose_factor(model,bones,pose_1,pose_2,pose_factor);
			}

				// combine rotations and create draw bone matrixes and positions
			
			model_comulative_combine_draw_bone_rotations(model,bones);
			model_move_draw_bones(model,bones);
			break;
	}
	
}

void model_create_draw_bones(model_type *model,model_draw_setup *draw_setup)
{
	int						n,i,nbone,cnt,
							idx,avg_mat_cnt[max_model_bone];
	float					f_cnt;
	matrix_type				avg_mats[max_model_bone][max_model_blend_animation];
	model_draw_bone_type	bones[max_model_blend_animation][max_model_bone];

		// count number of animations

	cnt=0;

	for (n=0;n!=max_model_blend_animation;n++) {
		if (draw_setup->poses[n].idx_1!=-1) cnt++;
	}

		// if only one animation, just pass through
		// to setup bone set

	if (cnt<=1) {
		model_create_draw_bones_single(model,draw_setup,draw_setup->bones);
		return;
	}

		// run all animations and gather bone sets

	for (n=0;n!=max_model_blend_animation;n++) {
		if (draw_setup->poses[n].idx_1!=-1) {
			model_create_draw_bones_single(model,draw_setup,bones[n]);
		}
	}

		// use settings from first blend as default

	nbone=model->nbone;

	memmove(&draw_setup->bones,&bones[0],(sizeof(model_draw_bone_type)*nbone));

		// clear out position and matrix count
		// to get ready to average out the positions

	for (n=0;n!=nbone;n++) {
		draw_setup->bones[n].fpnt.x=0.0f;
		draw_setup->bones[n].fpnt.y=0.0f;
		draw_setup->bones[n].fpnt.z=0.0f;
		avg_mat_cnt[n]=0;
	}

		// add up all the positions and matrixes
		// note that we skip matrixes that have no
		// rotational data in them (which is why we
		// need a separate count for each bone so
		// we can average them correctly)

	for (n=0;n!=max_model_blend_animation;n++) {

		if (draw_setup->poses[n].idx_1!=-1) {

				// average position and matrixes

			for (i=0;i!=nbone;i++) {

				if (matrix_has_rotation(&draw_setup->bones[i].rot_mat)) {

					idx=avg_mat_cnt[n];

					draw_setup->bones[i].fpnt.x+=bones[idx][i].fpnt.x;
					draw_setup->bones[i].fpnt.y+=bones[idx][i].fpnt.y;
					draw_setup->bones[i].fpnt.z+=bones[idx][i].fpnt.z;

					memmove(&avg_mats[i][idx],&draw_setup->bones[i].rot_mat,sizeof(matrix_type));

					avg_mat_cnt[n]++;
				}
			}
		}
	}

	for (n=0;n!=nbone;n++) {
		f_cnt=(float)avg_mat_cnt[n];

		draw_setup->bones[n].fpnt.x/=f_cnt;
		draw_setup->bones[n].fpnt.y/=f_cnt;
		draw_setup->bones[n].fpnt.z/=f_cnt;

		matrix_average(&draw_setup->bones[n].rot_mat,cnt,(matrix_type*)&avg_mats[n]);
	}
}
