/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Group Changes

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

extern map_type			map;
extern js_type			js;

extern bool map_movement_next_move(int movement_idx,attach_type *attach);

/* =======================================================

      Find Groups
      
======================================================= */

int group_find_by_index(char *name)
{
	int				n;
	group_type		*group;
	
	group=map.groups;
	
	for (n=0;n!=map.ngroup;n++) {
		if (strcasecmp(group->name,name)==0) return(n);
		group++;
	}
	
	return(-1);
}

/* =======================================================

      Move/Rotate Groups
      
======================================================= */

void group_move(int group_idx,int xmove,int zmove,int ymove)
{
	int					n,unit_cnt;
	bool				move_objs;
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;

		// can this group move objects?

	move_objs=(xmove!=0) || (zmove!=0);
	
		// move the meshes

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		portal=&map.portals[unit_list->portal_idx];

		switch (unit_list->type) {

			case group_type_mesh:

					// is mesh moveable?

				mesh=&portal->mesh.meshes[unit_list->idx];
				if (!mesh->flag.moveable) break;

					// move mesh and mark as
					// touched so it can be saved with games

				map_portal_mesh_move(&map,unit_list->portal_idx,unit_list->idx,TRUE,xmove,ymove,zmove);
				mesh->flag.touched=TRUE;

					// move objects on segment
					// supergumba
				
				if (move_objs) {
					//		object_move_with_wall_segment((int)(*seg_list),xmove,zmove);
					object_move_with_standing_mesh(unit_list->portal_idx,unit_list->idx,xmove,zmove);
				}

					// move decals with segments!

				decal_move_for_mesh(unit_list->portal_idx,unit_list->idx,xmove,ymove,zmove);

					// force a lighting recalc if mesh moved in a portal

				map_portal_light_check_changes_reset(portal);
				break;

			case group_type_liquid:

				liq=&portal->liquid.liquids[unit_list->idx];

				liq->lft+=xmove;
				liq->rgt+=xmove;
				liq->top+=zmove;
				liq->bot+=zmove;
				liq->y+=ymove;

				break;

		}
		
		unit_list++;
	}
}

void group_rotate(int group_idx,float x,float y,float z)
{
	int					n,unit_cnt;
	bool				move_objs;
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_mesh_type		*mesh;

		// can this group move objects?

	move_objs=(y!=0.0f);
	
		// move the meshes

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		portal=&map.portals[unit_list->portal_idx];

		if (unit_list->type==group_type_mesh) {

				// is mesh moveable?

			mesh=&portal->mesh.meshes[unit_list->idx];
			if (!mesh->flag.moveable) break;

				// move mesh and mark as
				// touched so it can be saved with games

			map_portal_mesh_rotate(&map,unit_list->portal_idx,unit_list->idx,TRUE,x,y,z);
			mesh->flag.touched=TRUE;

				// move objects on segment
				// supergumba
			
			if (move_objs) {
				//		object_move_with_wall_segment((int)(*seg_list),xmove,zmove);
			//	object_move_with_standing_mesh(unit_list->portal_idx,unit_list->idx,xmove,zmove);
			}

				// move decals with segments!

		//	decal_move_for_mesh(unit_list->portal_idx,unit_list->idx,xmove,ymove,zmove);

				// force a lighting recalc if mesh moved in a portal

			map_portal_light_check_changes_reset(portal);
		}
		
		unit_list++;
	}
}

/* =======================================================

      Run Move Over Time
      
======================================================= */

void group_moves_run(void)
{
	int				n,x,y,z,user_id;
	move_type		*move;

		// run all moves
		
	move=js.moves;
	
	for (n=0;n!=js.count.move;n++) {
	
		if (!move->freeze) {
		
				// movements
				
			if (move->has_mov) {
				move->cur_mov.x+=move->mov_add.x;
				move->cur_mov.y+=move->mov_add.y;
				move->cur_mov.z+=move->mov_add.z;
					
				x=(int)move->cur_mov.x;
				y=(int)move->cur_mov.y;
				z=(int)move->cur_mov.z;
					
				move->cur_mov.x-=(float)x;
				move->cur_mov.y-=(float)y;
				move->cur_mov.z-=(float)z;
					
				group_move(move->group_idx,x,z,y);
			}
			
				// rotations
				
			if (move->has_rot) {
				group_rotate(move->group_idx,move->rot_add.x,move->rot_add.y,move->rot_add.z);
			}
			
			move->count--;
		}
		
		move++;
	}
	
		// delete finished moves
	
	n=0;
	
	while (n<js.count.move) {
		move=&js.moves[n];
		if (move->count!=0) {
			n++;
			continue;
		}
		
		user_id=move->user_id;
	
			// post the finished event
			
		scripts_post_event_console(&move->attach,sd_event_move,sd_event_move_done,user_id);
		
			// if from a movement, then change
			
		if (move->movement_idx!=-1) {
			if (map_movement_next_move(move->movement_idx,&move->attach)) {
				scripts_post_event_console(&move->attach,sd_event_move,sd_event_move_loop,user_id);
			}
		}
	
			// delete move
			
		script_move_dispose(n);

			// exit if no more moves

		if (js.count.move==0) break;
	}
}

/* =======================================================

      Show or Set Passthrough
      
======================================================= */

void group_show(int group_idx,bool show)
{
	int					n,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_mesh_type		*mesh;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		portal=&map.portals[unit_list->portal_idx];
		mesh=&portal->mesh.meshes[unit_list->idx];

		mesh->flag.on=show;

		unit_list++;
	}
}

void group_solid(int group_idx,bool solid)
{
	int					n,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_mesh_type		*mesh;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		portal=&map.portals[unit_list->portal_idx];
		mesh=&portal->mesh.meshes[unit_list->idx];

		mesh->flag.pass_through=!solid;

		unit_list++;
	}
}

/* =======================================================

      Texture Changes
      
======================================================= */

void group_texture(int group_idx,int index)
{
	int					n,k,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		portal=&map.portals[unit_list->portal_idx];
		mesh=&portal->mesh.meshes[unit_list->idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			poly->draw.txt_frame_offset=index;
			poly++;
		}

		unit_list++;
	}
}

void group_texture_shift(int group_idx,float x_shift,float y_shift)
{
	int					n,k,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		portal=&map.portals[unit_list->portal_idx];
		mesh=&portal->mesh.meshes[unit_list->idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			poly->x_shift=x_shift;
			poly->y_shift=y_shift;
			poly++;
		}

		unit_list++;
	}
}

void group_texture_alpha(int group_idx,float alpha)
{
	int					n,k,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		portal=&map.portals[unit_list->portal_idx];
		mesh=&portal->mesh.meshes[unit_list->idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			poly->alpha=alpha;
			poly++;
		}

		unit_list++;
	}
}

