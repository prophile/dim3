/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Undo

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#define undo_type_bone_move		0

extern model_type				model;

int								undo_type,undo_pose_idx,undo_bone_idx;
model_bone_move_type			undo_bone_move;

extern void redraw_pose_row(void);

/* =======================================================

      Model Undo
      
======================================================= */

void undo_clear(void)
{
	DisableMenuItem(GetMenuRef(editmenu),1);
}

void undo_set_bone_move(int pose_idx,int bone_idx)
{
	undo_type=undo_type_bone_move;
	undo_pose_idx=pose_idx;
	undo_bone_idx=bone_idx;
	memmove(&undo_bone_move,&model.poses[pose_idx].bone_moves[bone_idx],sizeof(model_bone_move_type));

	EnableMenuItem(GetMenuRef(editmenu),1);
}

void undo_run(void)
{
	switch (undo_type) {
	
		case undo_type_bone_move:
			memmove(&model.poses[undo_pose_idx].bone_moves[undo_bone_idx],&undo_bone_move,sizeof(model_bone_move_type));
			redraw_pose_row();
			break;
			
	}
	
	undo_clear();
}

