/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Group Movements

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
#include "objects.h"
#include "effects.h"
#include "consoles.h"

extern map_type			map;
extern js_type			js;

extern bool map_movement_next_move(int movement_idx,int move_idx,attach_type *attach);

/* =======================================================

      Start Group Movement
      
======================================================= */

bool group_move_start(int group_idx,int movement_idx,int movement_move_idx,d3pnt *mov,d3ang *rot,int count,int user_id,bool main_move)
{
	int					n,unit_cnt;
	float				f_count;
	char				err_str[256];
	group_type			*group;
	group_move_type		*move;
	group_unit_type		*unit_list;

	group=&map.groups[group_idx];

		// no negative move counts
		
	if (count<=0) {
		sprintf(err_str,"Attemping to move group '%s' with negative time count",group->name);
		console_add_error(err_str);
		return(FALSE);
	}
	
		// check for certain error states
		// this check is a little bit heavy but these are
		// common errors with users that are hard to track down

	unit_cnt=group->unit_count;

	if (unit_cnt==0) {
		sprintf(err_str,"Attemping to move group '%s' which has no meshes attached to it",group->name);
		console_add_error(err_str);
		return(FALSE);
	}
	
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {
		if (unit_list->type==group_type_mesh) {
			if (!map.mesh.meshes[unit_list->idx].flag.moveable) {
				sprintf(err_str,"Attemping to move a mesh in group '%s' which does not have the moveable flag set",group->name);
				console_add_error(err_str);
				return(FALSE);
			}
		}
		unit_list++;
	}
	
		// setup move over time
		
	move=&group->move;

	move->count=count;
	
	move->mov_add.x=mov->x/count;
	move->mov_add.y=mov->y/count;
	move->mov_add.z=mov->z/count;

	f_count=(float)move->count;
	move->rot_add.x=rot->x/f_count;
	move->rot_add.y=rot->y/f_count;
	move->rot_add.z=rot->z/f_count;
		
	move->movement_idx=movement_idx;
	move->movement_move_idx=movement_move_idx;
	move->user_id=user_id;
	
	move->main_move=main_move;
	
	move->freeze=FALSE;
	move->on=TRUE;
	
		// the movement attach is a duplicate of the script
		// attach, this is done to fix some references in header files

	memmove(&move->attach,&js.attach,sizeof(attach_type));

	return(TRUE);
}

void group_move_clear_all(void)
{
	int				n;
	group_type		*group;

	group=map.groups;

	for (n=0;n!=map.ngroup;n++) {

			// no movements yet

		group->move.on=FALSE;
		group->move.was_moved=FALSE;
		group->move.cuml_mov_add.x=group->move.cuml_mov_add.y=group->move.cuml_mov_add.z=0;
		group->move.cuml_rot_add.x=group->move.cuml_rot_add.y=group->move.cuml_rot_add.z=0;

			// get group center.  We need to
			// keep a separate center to stop
			// rotations from floating

		map_group_get_center(&map,n,&group->center_pnt);

		group++;
	}
}

/* =======================================================

      Freeze and UnFreeze
      
======================================================= */

void group_move_freeze(int group_idx,bool freeze)
{
	map.groups[group_idx].move.freeze=freeze;
}

bool group_move_frozen(int group_idx)
{
	return(map.groups[group_idx].move.freeze);
}

/* =======================================================

      Check if Object is Standing on Group
      
======================================================= */

bool group_move_object_stand(int group_idx,int stand_mesh_idx)
{
	int					n,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;

	group=&map.groups[group_idx];

	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			if (unit_list->idx==stand_mesh_idx) return(TRUE);
		}
		
		unit_list++;
	}

	return(FALSE);
}

/* =======================================================

      Move Groups
      
======================================================= */

void group_move(group_type *group,d3pnt *mpnt)
{
	int					n,unit_cnt;
	bool				move_objs;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;

		// any movement?

	if ((mpnt->x==0) && (mpnt->y==0) && (mpnt->z==0)) return;

		// can this group move objects?

	move_objs=(mpnt->x!=0) || (mpnt->z!=0);
	
		// move the meshes

	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		switch (unit_list->type) {

			case group_type_mesh:

					// is mesh moveable?

				mesh=&map.mesh.meshes[unit_list->idx];
				if (!mesh->flag.moveable) break;

					// move mesh and mark as
					// touched so it can be saved with games

				map_mesh_move(&map,unit_list->idx,mpnt,FALSE);
				mesh->flag.touched=TRUE;

					// move objects and decals with mesh

				if (move_objs) object_move_with_mesh(unit_list->idx,mpnt->x,mpnt->z);
				decal_move_with_mesh(unit_list->idx,mpnt->x,mpnt->y,mpnt->z);
				break;

			case group_type_liquid:

				liq=&map.liquid.liquids[unit_list->idx];

				liq->lft+=mpnt->x;
				liq->rgt+=mpnt->x;
				liq->top+=mpnt->z;
				liq->bot+=mpnt->z;
				liq->y+=mpnt->y;

				break;

		}
		
		unit_list++;
	}

		// keep the cumlative moves

	group->move.cuml_mov_add.x+=mpnt->x;
	group->move.cuml_mov_add.y+=mpnt->y;
	group->move.cuml_mov_add.z+=mpnt->z;

		// group center changes

	group->center_pnt.x+=mpnt->x;
	group->center_pnt.y+=mpnt->y;
	group->center_pnt.z+=mpnt->z;
}

/* =======================================================

      Rotate Groups
      
======================================================= */

void group_rotate(group_type *group,d3ang *fpnt)
{
	int					n,unit_cnt;
	bool				move_objs;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;

		// any rotation?

	if ((fpnt->x==0.0f) && (fpnt->y==0.0f) && (fpnt->z==0.0f)) return;

		// can this group move objects?

	move_objs=(fpnt->y!=0.0f);
	
		// move the meshes

	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {

				// is mesh moveable?

			mesh=&map.mesh.meshes[unit_list->idx];
			if (!mesh->flag.moveable) break;

				// move mesh and mark as
				// touched so it can be saved with games

			if (mesh->flag.rot_independent) {
				map_mesh_rotate(&map,unit_list->idx,&mesh->box.mid,fpnt,FALSE);
			}
			else {
				map_mesh_rotate(&map,unit_list->idx,&group->center_pnt,fpnt,FALSE);
			}
			
			mesh->flag.touched=TRUE;

				// rotate objects and decals with mesh
			
			if (move_objs) object_rotate_with_mesh(unit_list->idx,fpnt->y);
			decal_rotate_with_mesh(unit_list->idx,fpnt->y);
		}
		
		unit_list++;
	}

		// keep the cumlative rotations

	group->move.cuml_rot_add.x+=fpnt->x;
	group->move.cuml_rot_add.y+=fpnt->y;
	group->move.cuml_rot_add.z+=fpnt->z;
}

/* =======================================================

      Run Moves
      
======================================================= */

void group_moves_run(bool run_events)
{
	int				n,user_id;
	group_type		*group;
	group_move_type	*move;

		// run all moves
		
	for (n=0;n!=map.ngroup;n++) {

		group=&map.groups[n];
		move=&group->move;
		if ((!move->on) || (move->freeze)) continue;
		
		group_move(group,&move->mov_add);
		group_rotate(group,&move->rot_add);

		move->was_moved=TRUE;
		
		move->count--;
	}
	
		// handle any finished moves
		// we do this separately as multiple group moves
		// hooked up to a move can interfere with each other
		
	for (n=0;n!=map.ngroup;n++) {

		move=&map.groups[n].move;
		if ((!move->on) || (move->freeze) || (move->count!=0)) continue;

		move->on=FALSE;

			// only post events and single map movements if
			// this move is the main move of a group of movements
			
		if (!move->main_move) continue;

			// post the finished event
	
		user_id=move->user_id;
		if (run_events) scripts_post_event_console((attach_type*)&move->attach,sd_event_move,sd_event_move_done,user_id);

			// signal back to the original map movement
	
		if (map_movement_next_move(move->movement_idx,move->movement_move_idx,(attach_type*)&move->attach)) {
			if (run_events) scripts_post_event_console((attach_type*)&move->attach,sd_event_move,sd_event_move_loop,user_id);
		}
	}
}

/* =======================================================

      Synch Group Movements with Loaded File
      
======================================================= */

void group_moves_synch_with_load(void)
{
	int				n;
	group_type		*group;
	group_move_type	*move;

	group=map.groups;

	for (n=0;n!=map.ngroup;n++) {

		if (move->was_moved) {
			group_move(group,&group->move.cuml_mov_add);
			group_rotate(group,&group->move.cuml_rot_add);
		}
		
		group++;
	}
}

/* =======================================================

      Synch Group Movements with Host
      
======================================================= */

void group_moves_synch_with_client(int group_idx,network_reply_group_synch *synch)
{
	int				flags;
	group_type		*group;

		// setup the synch for the client

	synch->group_idx=htons((short)group_idx);

	group=&map.groups[group_idx];

	flags=0;
	if (group->move.on) flags|=net_group_synch_flag_on;
	if (group->move.freeze) flags|=net_group_synch_flag_freeze;
	if (group->move.main_move) flags|=net_group_synch_flag_main_move;

	synch->flags=htonl(flags);

	synch->count=htons((short)group->move.count);
	synch->user_id=htons((short)group->move.user_id);
	synch->movement_idx=htons((short)group->move.movement_idx);
	synch->movement_move_idx=htons((short)group->move.movement_move_idx);

	synch->mov_add_x=htonl(group->move.mov_add.x);
	synch->mov_add_y=htonl(group->move.mov_add.y);
	synch->mov_add_z=htonl(group->move.mov_add.z);

	synch->fp_rot_add_x=htonf(group->move.rot_add.x);
	synch->fp_rot_add_y=htonf(group->move.rot_add.y);
	synch->fp_rot_add_z=htonf(group->move.rot_add.z);

	synch->cuml_mov_add_x=htonl(group->move.cuml_mov_add.x);
	synch->cuml_mov_add_y=htonl(group->move.cuml_mov_add.y);
	synch->cuml_mov_add_z=htonl(group->move.cuml_mov_add.z);

	synch->fp_cuml_rot_add_x=htonf(group->move.cuml_rot_add.x);
	synch->fp_cuml_rot_add_y=htonf(group->move.cuml_rot_add.y);
	synch->fp_cuml_rot_add_z=htonf(group->move.cuml_rot_add.z);
}

void group_moves_synch_with_host(network_reply_group_synch *synch)
{
	int				group_idx,flags;
	d3pnt			cuml_mov_add;
	d3ang			cuml_rot_add;
	group_type		*group;

		// get the group

	group_idx=(int)ntohs(synch->group_idx);
	group=&map.groups[group_idx];

		// fix the movement

	flags=ntohl(synch->flags);

	group->move.on=((flags&net_group_synch_flag_on)!=0);
	group->move.freeze=((flags&net_group_synch_flag_freeze)!=0);
	group->move.main_move=((flags&net_group_synch_flag_main_move)!=0);

	group->move.count=(int)ntohs(synch->count);
	group->move.user_id=(int)ntohs(synch->user_id);
	group->move.movement_idx=(int)ntohs(synch->movement_idx);
	group->move.movement_move_idx=(int)ntohs(synch->movement_move_idx);

	group->move.mov_add.x=ntohl(synch->mov_add_x);
	group->move.mov_add.y=ntohl(synch->mov_add_y);
	group->move.mov_add.z=ntohl(synch->mov_add_z);

	group->move.rot_add.x=ntohf(synch->fp_rot_add_x);
	group->move.rot_add.y=ntohf(synch->fp_rot_add_y);
	group->move.rot_add.z=ntohf(synch->fp_rot_add_z);

		// synch the commulative moves

		// if there's already been movement, get rid
		// of that before synching all commulative
		// movement

	cuml_mov_add.x=ntohl(synch->cuml_mov_add_x)-group->move.cuml_mov_add.x;
	cuml_mov_add.y=ntohl(synch->cuml_mov_add_y)-group->move.cuml_mov_add.y;
	cuml_mov_add.z=ntohl(synch->cuml_mov_add_z)-group->move.cuml_mov_add.z;

	group_move(group,&cuml_mov_add);

	cuml_rot_add.x=ntohf(synch->fp_cuml_rot_add_x)-group->move.cuml_rot_add.x;
	cuml_rot_add.y=ntohf(synch->fp_cuml_rot_add_y)-group->move.cuml_rot_add.y;
	cuml_rot_add.z=ntohf(synch->fp_cuml_rot_add_z)-group->move.cuml_rot_add.z;

	group_rotate(group,&cuml_rot_add);
}

