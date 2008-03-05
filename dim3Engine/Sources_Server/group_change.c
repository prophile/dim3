/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Group Changes

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
#include "effects.h"

extern map_type			map;
extern js_type			js;

extern bool map_movement_next_move(int movement_idx,attach_type *attach);

// supergumba -- rename this file

/* =======================================================

      Move Groups
      
======================================================= */

void group_move(int group_idx,int xmove,int zmove,int ymove)
{
	int					n,unit_cnt;
	bool				move_objs;
	unsigned char		portal_hit[max_portal];
	group_type			*group;
	group_unit_type		*unit_list;
	portal_type			*portal;
	map_liquid_type		*liq;

		// will need to recalc lighting in
		// portals with moved segments

	bzero(portal_hit,max_portal);

		// can this group move objects?

	move_objs=(xmove!=0) || (zmove!=0);
	
		// move the segments

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		portal=&map.portals[unit_list->portal_idx];

		switch (unit_list->type) {

			case group_type_mesh:

		//		if (portal->mesh.meshes[
				// supergumba mesh moveable flag

				map_portal_mesh_move(&map,unit_list->portal_idx,unit_list->idx,TRUE,xmove,ymove,zmove);

					// supergumba -- move objects with segments!

					// supergumba -- move decals with segments!

			//	if (seg->decal_count!=0) decal_move_for_segment(idx,xmove,ymove,zmove);

					// force a lighting recalc if mesh moved in a portal

				if (portal_hit[unit_list->portal_idx]==0x0) {
					portal_hit[unit_list->portal_idx]=0x1;
					map_portal_light_check_changes_reset(portal);
				}

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

				/*


		idx=*seg_list;
		seg=&map.segments[idx];
		
		if (seg->moveable) {
			
				// move objects on segment
				
			if (move_objs) {
				switch (seg->type) {
					case sg_wall:
						object_move_with_wall_segment((int)(*seg_list),xmove,zmove);
						break;
					case sg_floor:
						object_move_with_floor_segment((int)(*seg_list),xmove,zmove);
						break;
				}
			}
		
				// move segment
				
			map_segment_move(&map,seg,xmove,ymove,zmove);
			if (seg->decal_count!=0) decal_move_for_segment(idx,xmove,ymove,zmove);
		}

*/
		
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
		
				// get the next move
				
			move->x+=move->dx;
			move->y+=move->dy;
			move->z+=move->dz;
			
			x=(int)move->x;
			y=(int)move->y;
			z=(int)move->z;
			
			move->x-=(float)x;
			move->y-=(float)y;
			move->z-=(float)z;
			
			group_move(move->group_idx,x,z,y);
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
	/* supergumba
	int				n;
	short			*seg_list;
	segment_type	*seg;
	group_type		*group;
	
	group=&map.groups[group_idx];
	seg_list=group->seg_list;
	
	for (n=0;n!=group->seg_count;n++) {
		seg=&map.segments[*seg_list];
		seg->on=show;
		seg->touched=TRUE;
		map_portal_set_segment_list_changes(&map,seg->rn);
		seg_list++;
	}
	*/
}

void group_solid(int group_idx,bool solid)
{
	/* supergumba
	int				n;
	short			*seg_list;
	segment_type	*seg;
	group_type		*group;
	
	group=&map.groups[group_idx];
	seg_list=group->seg_list;
	
	for (n=0;n!=group->seg_count;n++) {
		seg=&map.segments[*seg_list];
		seg->pass_through=!solid;
		seg->touched=TRUE;
		map_portal_set_segment_list_changes(&map,seg->rn);
		seg_list++;
	}
	*/
}

/* =======================================================

      Texture Changes
      
======================================================= */

void group_texture(int group_idx,int index)
{
	/* supergumba
	int				n;
	short			*seg_list;
	segment_type	*seg;
	group_type		*group;
	
	group=&map.groups[group_idx];
	seg_list=group->seg_list;
	
	for (n=0;n!=group->seg_count;n++) {
		seg=&map.segments[*seg_list];
		seg->txt_offset=index;
		seg->touched=TRUE;
		seg_list++;
	}
	*/
}

void group_texture_offset(int group_idx,float x_offset,float y_offset)
{
	/* supergumba
	int				n;
	short			*seg_list;
	segment_type	*seg;
	group_type		*group;
	
	group=&map.groups[group_idx];
	seg_list=group->seg_list;
	
	for (n=0;n!=group->seg_count;n++) {
		seg=&map.segments[*seg_list];
		seg->x_txtoff=x_offset;
		seg->y_txtoff=y_offset;
		seg->touched=TRUE;
		seg_list++;
	}
	*/
}

void group_texture_shift(int group_idx,float x_shift,float y_shift)
{
	/* supergumba
	int				n;
	short			*seg_list;
	segment_type	*seg;
	group_type		*group;
	
	group=&map.groups[group_idx];
	seg_list=group->seg_list;
	
	for (n=0;n!=group->seg_count;n++) {
		seg=&map.segments[*seg_list];
		seg->x_shift=x_shift;
		seg->y_shift=y_shift;
		seg->touched=TRUE;
		seg_list++;
	}
	*/
}

void group_texture_alpha(int group_idx,float alpha)
{
	/* supergumba
	int				n;
	short			*seg_list;
	segment_type	*seg;
	group_type		*group;
	
	group=&map.groups[group_idx];
	seg_list=group->seg_list;
	
	for (n=0;n!=group->seg_count;n++) {
		seg=&map.segments[*seg_list];
		seg->alpha=alpha;
		seg->touched=TRUE;
		seg_list++;
	}
	*/
}

