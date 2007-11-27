/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Segment Changes

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

/* =======================================================

      Move Segments
      
======================================================= */

void segment_move(int group_idx,int xmove,int zmove,int ymove)
{
	int					n,rn,idx;
	short				*seg_list;
	bool				move_objs;
	segment_type		*seg;
	group_type			*group;

		// can this group move objects?

	move_objs=(xmove!=0) || (zmove!=0);
	
		// move the segments

	group=&map.groups[group_idx];
	
	seg_list=group->seg_list;
	
	for (n=0;n!=group->seg_count;n++) {
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
		
		seg_list++;
	}

		// forcing a lighting recalculation for moved portal

	if (group->seg_count>0) {
		seg_list=group->seg_list;
		rn=map.segments[*seg_list].rn;

		map_portal_light_check_changes_reset(&map.portals[rn]);
	}
}

/* =======================================================

      Run Move Over Time
      
======================================================= */

void segment_moves_run(void)
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
			
			segment_move(move->group_idx,x,z,y);
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

void segment_show(int group_idx,bool show)
{
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
}

void segment_solid(int group_idx,bool solid)
{
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
}

/* =======================================================

      Texture Changes
      
======================================================= */

void segment_texture(int group_idx,int index)
{
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
}

void segment_texture_offset(int group_idx,float x_offset,float y_offset)
{
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
}

void segment_texture_shift(int group_idx,float x_shift,float y_shift)
{
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
}

void segment_texture_alpha(int group_idx,float alpha)
{
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
}

