/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object In Liquids

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
#include "physics.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Liquid Contacts
      
======================================================= */

void object_liquid_contact(obj_type *obj)
{
	int					n,nliquid,
						rn,y,eye_y,dy,lft,rgt,top,bot;
	d3box				box;
	map_liquid_type		*liq;
	
	obj->liquid_mode=lm_out;
	obj->contact.liquid_idx=-1;

	rn=obj->pos.rn;
	box_create_from_object(&box,obj);
	
	lft=box.min_x;
	rgt=box.max_x;
	top=box.min_z;
	bot=box.max_z;

	nliquid=map.liquid.nliquid;
	
	for (n=0;n!=nliquid;n++) {
		liq=&map.liquid.liquids[n];
	
		if (rgt<liq->lft) continue;
		if (lft>liq->rgt) continue;
		if (bot<liq->top) continue;
		if (top>liq->bot) continue;

		y=obj->pos.y;
		dy=liq->y;
		
		if (y>=dy) {
			eye_y=(y+obj->duck.y_move)+obj->size.eye_offset;
			
 			if (eye_y>dy) {
				obj->liquid_mode=lm_under;
			}
			else {
				obj->liquid_mode=lm_in;
			}
            
			obj->contact.liquid_idx=n;
        }
	}
}

/* =======================================================

      Get Alter Speed for Liquids
      
======================================================= */

float object_liquid_alter_speed(obj_type *obj)
{
	if (obj->liquid_mode!=lm_under) return(1.0f);
	if (obj->contact.liquid_idx==-1) return(1.0f);

	return(map.liquid.liquids[obj->contact.liquid_idx].speed_alter);
}

/* =======================================================

      Objects in Liquid
      
======================================================= */

void object_liquid(int tick,obj_type *obj)
{
	int					harm,old_liquid_mode;
	map_liquid_type		*liq;
	
    old_liquid_mode=obj->liquid_mode;
	
	object_liquid_contact(obj);
    
        // leaving liquid
     
    if (obj->contact.liquid_idx==-1) {
        if (old_liquid_mode!=lm_out) scripts_post_event_console(&obj->attach,sd_event_liquid,sd_event_liquid_out,0);
        return;
    }
    
        // entering or moving in liquid
        
    liq=&map.liquid.liquids[obj->contact.liquid_idx];

	switch (obj->liquid_mode) {
	
		case lm_in:
			if (old_liquid_mode==lm_out) {
				obj->status.liquid_harm_count=0;
				scripts_post_event_console(&obj->attach,sd_event_liquid,sd_event_liquid_in,0);
				break;
			}
			if (old_liquid_mode==lm_under) {
				scripts_post_event_console(&obj->attach,sd_event_liquid,sd_event_liquid_surface,0);
				object_liquid_jump(obj);			// jump out of liquid when surfacing
				break;
			}
			break;
			
		case lm_under:
			if (old_liquid_mode==lm_out) {
				obj->status.liquid_harm_count=0;
			}
			if (old_liquid_mode!=lm_under) {
				obj->status.liquid_under_tick=tick;
				obj->status.liquid_drown_count=0;
				scripts_post_event_console(&obj->attach,sd_event_liquid,sd_event_liquid_submerge,0);
			}
			break;
			
	}
	
		// downing

	if (obj->liquid_mode==lm_under) {

		if ((tick-obj->status.liquid_under_tick)>liq->harm.drown_tick) {

			if (obj->status.liquid_drown_count==0) {
				obj->status.liquid_drown_count=100;

				harm=liq->harm.drown_harm;
				if (harm>0) object_damage(obj,NULL,NULL,NULL,NULL,harm);
			}
			else {
				obj->status.liquid_drown_count--;
			}

		}
	}

		// hurting or healing liquids

	if (obj->liquid_mode!=lm_out) {

		if (obj->status.liquid_harm_count==0) {
			obj->status.liquid_harm_count=100;

			harm=liq->harm.in_harm;
			if (harm!=0) {
				if (harm>0) {
					object_damage(obj,NULL,NULL,NULL,NULL,harm);
				}
				else {
					object_heal(obj,-harm);
				}
			}
		}
		else {
			obj->status.liquid_harm_count--;
		}
	}
}

