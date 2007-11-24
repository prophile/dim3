/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Turning

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

#include "objects.h"
#include "weapons.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Find Turn Speeds for Object
      
======================================================= */

float object_get_turn_speed(obj_type *obj)
{
		// ground speeds
		
    if (obj->air_mode==am_ground) {
		if (obj->duck.mode==dm_duck) return(obj->turn.crawl_speed);
		if (obj->forward_move.running) return(obj->turn.run_speed);
		return(obj->turn.walk_speed);
    }
	
		// air speeds
		
	return(obj->turn.air_speed);
}

float object_get_turn_motion(obj_type *obj)
{
		// ground speeds
		
    if (obj->air_mode==am_ground) {
		if (obj->duck.mode==dm_duck) return(obj->turn.crawl_motion_speed);
		if (obj->forward_move.running) return(obj->turn.run_motion_speed);
		return(obj->turn.walk_motion_speed);
    }
	
		// air speed
	
	return(obj->turn.air_motion_speed);
}

/* =======================================================

      Object Turning
      
======================================================= */

void object_turn(obj_type *obj)
{
	float		ang,rang,turn_add,subway,addway;
	
	turn_add=obj->turn.ang_add.y;
	if (turn_add==0) return;
	
	ang=obj->ang.y;
	if (obj->turn.ang_to.y==ang) return;
	
	rang=obj->turn.ang_to.y;
	
	if (ang>rang) {
		subway=ang-rang;
		addway=360-(ang-rang);
	}
	else {
		subway=360-(rang-ang);
		addway=rang-ang;
	}
	
	if (turn_add<0) {
		if (fabs(turn_add)>=subway) {
			obj->ang.y=rang;
			return;
		}
	}
	else {
		if (turn_add>=addway) {
			obj->ang.y=rang;
			return;
		}
	}
	
	obj->ang.y=angle_add(ang,turn_add);
}

void object_turn_stop(obj_type *obj)
{
	obj->turn.ang_to.y=(float)obj->pos.y;
	obj->turn.ang_add.y=0.0f;

	object_auto_walk_stop(obj);
}

void object_fix_motion(obj_type *obj)
{
	float			turn_max;
    d3ang			*ang;

		// fix motion to facing angle
	
    ang=&obj->ang;
	if (ang->y==obj->motion.ang.y) return;

		// if no motion whatsoever, auto reset motion angle

	if ((obj->forward_move.speed==0.0f) && (obj->side_move.speed==0.0f)) {
		obj->motion.ang.y=ang->y;
		return;
	}

		// no motion changes when flying and no thrust

	if ((obj->fly) && (!obj->forward_move.moving) && (!obj->side_move.moving)) return;

		// fix motion angle to facing angle
		
	turn_max=object_get_turn_motion(obj)*map.settings.resistance;
    obj->motion.ang.y=angle_turn_toward(obj->motion.ang.y,ang->y,turn_max);
}

/* =======================================================

      Player Turning
      
======================================================= */

void object_player_turn(obj_type *obj)
{
	float			turn_add,turn_restrict;
	weapon_type		*weap;
	
		// can it turn?
		
	if ((obj->turn.only_when_moving) && (!obj->forward_move.moving)) {
		obj->turn.ang_add.y=0;
		return;
	}
	
		// get turn speed
	
	obj->turn.fix_ang_add.y=0;
	
	turn_add=obj->turn.ang_add.y;
    if (turn_add==0) return;
	
		// zoom factors
		
	weap=weapon_find_current(obj);
	if (weap!=NULL) {
		if ((weap->zoom.on) && (weap->zoom.active)) {
			if (obj->duck.mode!=dm_duck) {
				turn_add*=weap->zoom.turn_factor;
			}
			else {
				turn_add*=weap->zoom.crawl_turn_factor;
			}
		}
	}
	
		// restriction
		
	if (obj->turn.restrict_player_turning) {
		turn_restrict=object_get_turn_speed(obj);
		
		if (turn_add<-turn_restrict) turn_add=-turn_restrict;
		if (turn_add>turn_restrict) turn_add=turn_restrict;
	}
	
		// turning
		
    obj->ang.y=angle_add(obj->ang.y,turn_add);
	obj->turn.fix_ang_add.y=turn_add;
}

void object_player_turn_direct(obj_type *obj,float ang)
{
		// turned around?
		
	if (obj->ang.y!=ang) obj->forward_move.speed=0;

		// face correct direction
		
	obj->forward_move.moving=TRUE;
	obj->forward_move.reverse=FALSE;
	obj->turn.ang_add.y=0.0f;
	obj->ang.y=obj->motion.ang.y=ang;
}

/* =======================================================

      Player Looking
      
======================================================= */

float object_player_look_constrain(obj_type *obj,weapon_type *weap,float ang_x)
{
	float			up_angle,down_angle;

	up_angle=obj->look.up_angle;
	down_angle=obj->look.down_angle;

	if (weap!=NULL) {
		if ((weap->zoom.on) && (weap->zoom.active)) {
			up_angle*=weap->zoom.look_factor;
			down_angle*=weap->zoom.look_factor;
		}
	}
		
	if (ang_x<-up_angle) ang_x=-up_angle;
	if (ang_x>down_angle) ang_x=down_angle;

	return(ang_x);
}

void object_player_look(obj_type *obj)
{
	float			look_add;
	weapon_type		*weap;
	
	look_add=obj->look.ang_add;
    if (look_add==0) return;
	
		// inverted looking
		
	if (setup.invert_look) look_add=-look_add;
	
		// zoom factors

	weap=weapon_find_current(obj);
	if (weap!=NULL) {
		if ((weap->zoom.on) && (weap->zoom.active)) {
			look_add*=weap->zoom.look_factor;
		}
	}
	
		// change look

	obj->view_ang.x=object_player_look_constrain(obj,weap,(obj->view_ang.x+look_add));
}

void object_player_look_stop(obj_type *obj)
{
	obj->look.ang_add=0.0f;
}

void object_player_look_fix_zoom(obj_type *obj,weapon_type *weap)
{
	obj->view_ang.x=object_player_look_constrain(obj,weap,obj->view_ang.x);
}

void object_fly_reset_angle(obj_type *obj)
{
	if ((obj->player) && (obj->input_mode==im_fly)) obj->ang.x=-obj->view_ang.x;
}

