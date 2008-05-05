/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Movements

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
	#include "dim3maputility.h"
#endif

/* =======================================================

      Find Map Movements
      
======================================================= */

int map_movement_find(map_type *map,char *name)
{
	int				n,nmovement;
	movement_type	*movement;
	
	nmovement=map->nmovement;
	movement=map->movements;
	
	for (n=0;n!=nmovement;n++) {
		if (strcmp(movement->name,name)==0) return(n);
		movement++;
	}
	
	return(-1);
}

/* =======================================================

      Map Movements
      
======================================================= */

int map_movement_add(map_type *map)
{
	movement_type		*movement;
	
	if (map->nmovement>=max_movement) return(-1);
	
	movement=&map->movements[map->nmovement];
	map->nmovement++;
	
	strcpy(movement->name,"New Movement");
	movement->group_idx=0;
	movement->reverse_group_idx=-1;
	movement->auto_start=FALSE;
	movement->auto_open=FALSE;
	movement->auto_open_never_close=FALSE;
	movement->auto_open_distance=20000;
	movement->loop=FALSE;
	
	movement->nmove=0;
	
	return(map->nmovement-1);
}

void map_movement_delete(map_type *map,int movement_idx)
{
	if (movement_idx<(map->nmovement-1)) {
		memmove(&map->movements[movement_idx],&map->movements[movement_idx+1],(sizeof(movement_type)*((map->nmovement-movement_idx)-1)));
	}
	map->nmovement--;
}

/* =======================================================

      Map Movements Moves
      
======================================================= */

int map_movement_move_add(map_type *map,int movement_idx)
{
	movement_type		*movement;
	movement_move_type	*move;
	
	movement=&map->movements[movement_idx];
	if (movement->nmove>=max_movement_move) return(-1);
	
	move=&movement->moves[movement->nmove];
	movement->nmove++;
	
	move->msec=100;
	move->mov.x=move->mov.y=move->mov.z=0;
	move->rot.x=move->rot.y=move->rot.z=0.0f;
	move->user_id=0;
	move->sound_name[0]=0x0;
	move->sound_pitch=1.0f;

	return(movement->nmove-1);
}

void map_movement_move_delete(map_type *map,int movement_idx,int move_idx)
{
	movement_type		*movement;
	
	movement=&map->movements[movement_idx];
	
	if (move_idx<(movement->nmove-1)) {
		memmove(&movement->moves[move_idx],&movement->moves[move_idx+1],(sizeof(movement_move_type)*((movement->nmove-move_idx)-1)));
	}
	movement->nmove--;
}
