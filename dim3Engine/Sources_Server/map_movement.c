/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Movements

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

#include "scripts.h"
#include "objects.h"
#include "cameras.h"
#include "sounds.h"

extern map_type			map;
extern server_type		server;
extern camera_type		camera;
extern js_type			js;

extern bool group_move_start(int group_idx,int movement_idx,int movement_move_idx,d3pnt *mov,d3ang *rot,int count,int user_id,bool main_move);
extern void group_move_freeze(int group_idx,bool freeze);
extern bool group_move_frozen(int group_idx);
extern bool group_move_object_stand(int group_idx,int stand_mesh_idx);

/* =======================================================

      Calculate Movement Centers
      
======================================================= */

void map_movements_get_center(movement_type *movement,int *cx,int *cy,int *cz)
{
	int			x,y,z,x2,y2,z2;
	
	map_group_get_center(&map,movement->group_idx,&x,&y,&z);
	
	if (movement->reverse_group_idx!=-1) {
		map_group_get_center(&map,movement->reverse_group_idx,&x2,&y2,&z2);
		x=(x+x2)>>1;
		y=(y+y2)>>1;
		z=(z+z2)>>1;
	}
		
	*cx=x;
	*cy=y;
	*cz=z;
}

/* =======================================================

      Turn Movements On/Off
      
======================================================= */

void map_movements_start(int movement_idx,int move_idx,attach_type *attach)
{
	int						x,y,z,msec,buffer_idx;
	d3pnt					rev_mov;
	d3ang					rev_rot;
	movement_type			*movement;
	movement_move_type		*move;
	attach_type				bkup_attach;

	movement=&map.movements[movement_idx];
	if (movement->nmove==0) return;
	
		// started
		
	movement->started=TRUE;
	
	move=&movement->moves[move_idx];
	
		// save current script attach and use course
			
	memmove(&bkup_attach,&js.attach,sizeof(attach_type));
	memmove(&js.attach,attach,sizeof(attach_type));
	
		// get reverse movement
		
	rev_mov.x=-move->mov.x;
	rev_mov.y=-move->mov.y;
	rev_mov.z=-move->mov.z;
	
	rev_rot.x=-move->rot.x;
	rev_rot.y=-move->rot.y;
	rev_rot.z=-move->rot.z;
	
		// start move over time
	
	msec=move->msec/10;
	
	if (!movement->reverse) {
		group_move_start(movement->group_idx,movement_idx,move_idx,&move->mov,&move->rot,msec,move->user_id,TRUE);
		if (movement->reverse_group_idx!=-1) group_move_start(movement->reverse_group_idx,movement_idx,move_idx,&rev_mov,&rev_rot,msec,move->user_id,FALSE);
	}
	else {
		group_move_start(movement->group_idx,movement_idx,move_idx,&rev_mov,&rev_rot,msec,move->user_id,TRUE);
		if (movement->reverse_group_idx!=-1) group_move_start(movement->reverse_group_idx,movement_idx,move_idx,&move->mov,&move->rot,msec,move->user_id,FALSE);
	}
	
		// restore old attach
		
	memmove(&js.attach,&bkup_attach,sizeof(attach_type));
	
		// play any sound
		
	if (move->sound_name[0]==0x0) return;
	
	map_movements_get_center(movement,&x,&y,&z);
	
	buffer_idx=al_find_buffer(move->sound_name);
	if (buffer_idx==-1) return;
	
	al_play_source(buffer_idx,x,y,z,move->sound_pitch,FALSE,FALSE,FALSE,FALSE);

		// sound watches

	object_watch_sound_alert(x,y,z,-1,move->sound_name);
}

/* =======================================================

      Initialize Movements
      
======================================================= */

void map_movements_initialize(void)
{
	int				n,nmovement;
	movement_type	*movement;
	
	nmovement=map.nmovement;
	movement=map.movements;
	
	for (n=0;n!=nmovement;n++) {
	
			// start-up flags
			
		movement->started=FALSE;
		movement->opened=FALSE;
		
			// get center
			
		map_movements_get_center(movement,&movement->pnt.x,&movement->pnt.y,&movement->pnt.z);		
			
			// run auto-starts
		
		if (movement->auto_start) {
			movement->reverse=FALSE;
			map_movements_start(n,0,&js.course_attach);
		}
		
		movement++;
	}
}

/* =======================================================

      Script Movements
      
======================================================= */

void map_movements_script_start(int movement_idx,bool reverse)
{
	movement_type	*movement;
	
	movement=&map.movements[movement_idx];

	if (movement->started) return;
	
	if (!reverse) {
		movement->reverse=FALSE;
		map_movements_start(movement_idx,0,&js.attach);
	}
	else {
		movement->reverse=TRUE;
		map_movements_start(movement_idx,(movement->nmove-1),&js.attach);
	}
}

void map_movements_script_freeze(int movement_idx)
{
	movement_type	*movement;
	
	movement=&map.movements[movement_idx];
	
	group_move_freeze(movement->group_idx,TRUE);
	if (movement->reverse_group_idx!=-1) group_move_freeze(movement->reverse_group_idx,TRUE);
}

void map_movements_script_thaw(int movement_idx)
{
	movement_type	*movement;
	
	movement=&map.movements[movement_idx];
	
	group_move_freeze(movement->group_idx,FALSE);
	if (movement->reverse_group_idx!=-1) group_move_freeze(movement->reverse_group_idx,FALSE);
}

void map_movements_script_start_or_thaw(int movement_idx)
{
	movement_type	*movement;
	
	movement=&map.movements[movement_idx];

		// is movement started?
		
	if (movement->started) {
		if (group_move_frozen(movement->group_idx)) {
			group_move_freeze(movement->group_idx,FALSE);		// if frozen, then thaw
		}
	}
	else {
		movement->reverse=FALSE;
		map_movements_start(movement_idx,0,&js.attach);
	}
}

bool map_movements_script_is_looping(int movement_idx)
{	
	return(map.movements[movement_idx].loop);
}

/* =======================================================

      Run Movements
      
======================================================= */

bool map_movement_next_move(int movement_idx,int move_idx,attach_type *attach)
{
	movement_type	*movement;
	
	movement=&map.movements[movement_idx];
	
		// get next movement
		
	if (!movement->reverse) {
		
		move_idx++;
		
		if (move_idx>=movement->nmove) {
		
			if ((movement->auto_open) && (movement->auto_open_never_close)) {
				movement->auto_open=FALSE;
				movement->started=FALSE;
				return(TRUE);				// if auto-open and never close, then stop movement and turn off auto-open
			}
				
			if (!movement->loop) {
				movement->started=FALSE;
				return(TRUE);				// if looping is off, then stop
			}
			
			move_idx=0;
		}
	}
	else {
	
		move_idx--;
		
		if (move_idx<0) {
		
			if (!movement->loop) {
				movement->started=FALSE;
				return(TRUE);				// if looping is off, then stop
			}
			
			move_idx=movement->nmove-1;
		}
	}
	
		// start next movement
		
	map_movements_start(movement_idx,move_idx,attach);
	
	return(FALSE);
}

/* =======================================================

      Auto-Open Movements
      
======================================================= */

void map_movements_auto_open(void)
{
	int				n,i,nmovement;
	bool			obj_in_range;
	d3pnt			pnt;
	movement_type	*movement;
	obj_type		*obj;

		// check movements
		
	nmovement=map.nmovement;
	movement=map.movements;
	
	for (n=0;n!=nmovement;n++) {
	
			// can't check started or non-auto-open movements
			
		if ((movement->started) || ((!movement->auto_open) && (!movement->auto_open_stand))) {
			movement++;
			continue;
		}

			// find any objects close to auto_open
			
		obj_in_range=FALSE;

		if (movement->auto_open) {
			
			obj=server.objs;

			for (i=0;i!=server.count.obj;i++) {
				if ((obj->player) || (obj->open_doors)) {
					if (distance_check(obj->pnt.x,(obj->pnt.y-(obj->size.y>>1)),obj->pnt.z,movement->pnt.x,movement->pnt.y,movement->pnt.z,movement->auto_open_distance)) {
						obj_in_range=TRUE;
						break;
					}
				}
				obj++;
			}

				// check for camera node walks

			if ((camera.mode==cv_static) && (camera.auto_walk.on) && (camera.auto_walk.open_doors)) {
				camera_static_get_position(&pnt,NULL);
				obj_in_range=distance_check(pnt.x,pnt.y,pnt.z,movement->pnt.x,movement->pnt.y,movement->pnt.z,movement->auto_open_distance);
			}

		}

			// check for standing on

		if (movement->auto_open_stand) {

			obj=server.objs;

			for (i=0;i!=server.count.obj;i++) {

				if ((obj->player) || (obj->open_doors)) {

					if (obj->contact.stand_poly.mesh_idx!=-1) {
						if (group_move_object_stand(movement->group_idx,obj->contact.stand_poly.mesh_idx)) {
							obj_in_range=TRUE;
							break;
						}
						if (group_move_object_stand(movement->reverse_group_idx,obj->contact.stand_poly.mesh_idx)) {
							obj_in_range=TRUE;
							break;
						}
					}
				}

				obj++;
			}

		}
		
			// time to open or close?
			
		if (!movement->opened) {
			if (obj_in_range) {
				movement->reverse=FALSE;
				movement->opened=TRUE;
				map_movements_start(n,0,&js.course_attach);
			}
		}
		else {
			if (!obj_in_range) {
				movement->reverse=TRUE;
				movement->opened=FALSE;
				map_movements_start(n,(movement->nmove-1),&js.course_attach);
			}
		}
		
		movement++;
	}
}
