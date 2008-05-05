/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Move Over Time

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

#include "scripts.h"

extern js_type			js;

/* =======================================================

      Add/Delete Move Over Time
      
======================================================= */

bool script_move_add(int group_idx,int movement_idx,d3pnt *mov,d3ang *rot,int count,int user_id)
{
	float			f_count;
	move_type		*move;
	
		// no negative move counts
		
	if (count<=0) return(FALSE);
	
		// setup the move over time
		
	if (js.count.move>=max_moves) return(FALSE);
	
	move=&js.moves[js.count.move];
	js.count.move++;
	
	move->count=count;
	
	f_count=(float)move->count;
	move->mov_add.x=((float)mov->x)/f_count;
	move->mov_add.y=((float)mov->y)/f_count;
	move->mov_add.z=((float)mov->z)/f_count;
	
	move->rot_add.x=rot->x/f_count;
	move->rot_add.y=rot->y/f_count;
	move->rot_add.z=rot->z/f_count;

	move->cur_mov.x=move->cur_mov.y=move->cur_mov.z=0.0f;
	
	move->has_mov=((mov->x!=0) || (mov->y!=0) || (mov->z!=0));
	move->has_rot=((rot->x!=0.0f) || (rot->y!=0.0f) || (rot->z!=0.0f));
		
	move->group_idx=group_idx;
	move->movement_idx=movement_idx;		// -1 if launched from a script
	move->user_id=user_id;
	
	move->freeze=FALSE;
	
	memmove(&move->attach,&js.attach,sizeof(attach_type));

	return(TRUE);
}

void script_move_dispose(int idx)
{
	if (idx<(js.count.move-1)) {
		memmove(&js.moves[idx],&js.moves[idx+1],(sizeof(move_type)*((js.count.move-idx)-1)));
	}
	
	js.count.move--;
}

void script_move_clear_all(void)
{
	js.count.move=0;
}

/* =======================================================

      Check if group already has move
      
======================================================= */

int script_move_find(int group_idx)
{
	int				n;
	move_type		*move;

		// run all moves
		
	move=js.moves;
	
	for (n=0;n!=js.count.move;n++) {
		if (move->group_idx==group_idx) return(n);
		move++;
	}
	
	return(-1);
}

bool script_move_on(int group_idx)
{
	return(script_move_find(group_idx)!=-1);
}

/* =======================================================

      Freeze and UnFreeze
      
======================================================= */

void script_move_freeze(int group_idx,bool freeze)
{
	int				idx;
	
	idx=script_move_find(group_idx);
	if (idx==-1) return;
	
	js.moves[idx].freeze=freeze;
}

bool script_move_frozen(int group_idx)
{
	int				idx;
	
	idx=script_move_find(group_idx);
	if (idx==-1) return(FALSE);
	
	return(js.moves[idx].freeze);
}
