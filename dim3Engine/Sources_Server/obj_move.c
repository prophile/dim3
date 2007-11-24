/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Movement

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

// supergumba -- temporary until new move routines
extern void object_motion_lock(obj_type *obj,float *xmove,float *ymove,float *zmove);
extern void object_motion_set_script_property(obj_type *obj,float xmove,float ymove,float zmove);

/* =======================================================

      Get Motion Movement
      
======================================================= */

void get_motion_movement(float ang,obj_movement *move,float *xmove,float *zmove)
{
    angle_get_movement_float(ang,move->speed,xmove,zmove);
    if (move->reverse) {
        *xmove=-(*xmove);
        *zmove=-(*zmove);
    }
}

void object_motion_setup(obj_type *obj,float *xmove,float *ymove,float *zmove)
{
	float			ang,xadd,zadd;
	
		// forward motion
		
	ang=obj->motion.ang.y;
	get_motion_movement(ang,&obj->forward_move,xmove,zmove);
	
		// side motion
		
	if (obj->side_step) {
		get_motion_movement(angle_add(ang,270),&obj->side_move,&xadd,&zadd);
		*xmove=(*xmove)+xadd;
		*zmove=(*zmove)+zadd;
	}
	
		// vertical motion
		
	*ymove=0;
	
	if ((obj->fly) || (obj->liquid_mode==lm_under)) {
	
			// in auto-walk?
			
		if (obj->auto_walk.mode!=aw_none) {
			if (obj->forward_move.moving) {
				angle_get_movement_float(obj->vert_move.seek_ang,obj->vert_move.max_walk_speed,&xadd,&zadd);
				*ymove=xadd;
			}
		}
		
			// regular seeking
			
		else {
			*ymove=obj->vert_move.speed;
			if (obj->vert_move.reverse) {
				*ymove=-(*ymove);
			}
		}
	}

		// add in outside forces

	*xmove=(*xmove)+obj->force.vct.x;
	*ymove=(*ymove)+obj->force.vct.y;
	*zmove=(*zmove)+obj->force.vct.z;
}

/* =======================================================

      Object Speed Control
      
======================================================= */

void object_movement(obj_type *obj,obj_movement *move)
{
    float		speed,max_speed,accelerate,decelerate;
    
    speed=move->speed;
	
		// single speed mode?
		
	if (obj->single_speed) {
		max_speed=move->max_walk_speed;
		accelerate=move->accelerate;
		decelerate=move->decelerate;
	}
	
		// variable speeds
		
	else {
    
		if (obj->duck.mode!=dm_stand) {
			max_speed=move->max_crawl_speed;
			accelerate=move->accelerate;
			decelerate=move->decelerate;
		}
		else {
			if ((obj->air_mode!=am_ground) || (obj->fly) || (obj->liquid_mode==lm_under)) {
				max_speed=move->max_air_speed;
				accelerate=move->air_accelerate;
				decelerate=move->air_decelerate;
			}
			else {
				if (obj->forward_move.running) {
					max_speed=move->max_run_speed;
				}
				else {
					max_speed=move->max_walk_speed;
				}
				accelerate=move->accelerate;
				decelerate=move->decelerate;
			}
		}
	}
    
		// Accelerating
	
	if (move->moving) {
		
        if (speed==max_speed) return;
        
        if (speed<max_speed) {							// if max speed has changed to be less, auto-decelerate
            speed+=(accelerate*map.settings.resistance);
            if (speed>max_speed) speed=max_speed;
		
            move->speed=speed;
            return;
        }
	}
	
		// Decelerating

	speed-=(decelerate*map.settings.resistance);
	if (speed<0) speed=0;
    
	move->speed=speed;
}

void object_simple_movement(obj_type *obj,obj_movement *move)
{
    float		speed,max_speed,accelerate,decelerate;
    
    speed=move->speed;
	
	max_speed=move->max_walk_speed;
	accelerate=move->accelerate;
	decelerate=move->decelerate;

		// Accelerating
	
	if (move->moving) {
		
        if (speed==max_speed) return;
        
        if (speed<max_speed) {							// if max speed has changed to be less, auto-decelerate
            speed+=(accelerate*map.settings.resistance);
            if (speed>max_speed) speed=max_speed;
		
            move->speed=speed;
            return;
        }
	}
	
		// Decelerating

	speed-=(decelerate*map.settings.resistance);
	if (speed<0) speed=0;
    
	move->speed=speed;
}

/* =======================================================

      Increase Gravity
      
======================================================= */

void object_gravity(obj_type *obj)
{
    float		weight,gravity,gravity_max_power,
				liq_speed_alter;
    
		// reset gravity if flying or on ground

	if ((obj->fly) || (obj->air_mode==am_ground)) {
		obj->force.gravity=gravity_start_power;
		return;
	}

		// under liquids changes gravity

	weight=(float)obj->size.weight;
	gravity_max_power=map.settings.gravity_max_power;

	if (obj->liquid_mode==lm_under) {
		liq_speed_alter=object_liquid_alter_speed(obj);

		weight*=liq_speed_alter;
		gravity_max_power*=liq_speed_alter;
	}

		// run gravity
    
    gravity=obj->force.gravity;
	
		// increase the gravity
		
	if (gravity<gravity_max_power) {
		gravity+=(weight*(map.settings.gravity/gravity_factor));
		if (gravity>gravity_max_power) gravity=map.settings.gravity_max_power;
        
        obj->force.gravity=gravity;
	}
}

void object_fix_force(obj_type *obj)
{
	float			decelerate,gravity_max_speed;
	d3vct			*vct;
	
		// get the deceleration
		
	decelerate=map.settings.resistance;
	
		// reduce the x/z by resistance
		
	vct=&obj->force.vct;
	
	if (vct->x!=0) {
		if (vct->x>0) {
			vct->x-=decelerate;
			if (vct->x<0) vct->x=0;
		}
		else {
			vct->x+=decelerate;
			if (vct->x>0) vct->x=0;
		}
	}
	
	if (vct->z!=0) {
		if (vct->z>0) {
			vct->z-=decelerate;
			if (vct->z<0) vct->x=0;
		}
		else {
			vct->z+=decelerate;
			if (vct->z>0) vct->z=0;
		}
	}

		// gravity different under liquid
		// we are fudging this a bit with the extra
		// 0.5 for some bounancy

	gravity_max_speed=map.settings.gravity_max_speed;
	if (obj->liquid_mode==lm_under) gravity_max_speed*=(object_liquid_alter_speed(obj)*0.5f);
	
		// reduce the y by gravity
		
	if (obj->force.gravity>0) {
		vct->y+=obj->force.gravity;
		if (vct->y>gravity_max_speed) vct->y=gravity_max_speed;
	}
}

/* =======================================================

      Object Alterations
      
======================================================= */

void object_alter_speed(obj_type *obj,float alt_speed)
{
    obj->forward_move.speed*=alt_speed;
    obj->side_move.speed*=alt_speed;
}

void object_alter_gravity(obj_type *obj,float alt_gravity)
{
    obj->force.vct.y*=alt_gravity;
    obj->force.gravity=gravity_start_power;
}

/* =======================================================

      Object Bump-Ups
      
======================================================= */

bool object_bump_up(obj_type *obj,int xmove,int zmove)
{
	int				uid,idx,ydif,ymove;
    obj_type		*hit_obj;
	segment_type	*seg;
	
	if (!obj->bump.on) return(FALSE);
	if (obj->air_mode!=am_ground) return(FALSE);			// can't bump up in air
    
    ydif=0;
    
        // bump up an object?
    
	uid=obj->contact.obj_uid;
    if (uid!=-1) {
        hit_obj=object_find_uid(uid);
        ydif=obj->pos.y-(hit_obj->pos.y-hit_obj->size.y);
    }
    
        // bump up a wall
	
	idx=obj->contact.wall_seg_idx;
	if (idx!=-1) {
        seg=&map.segments[idx];
        ydif=obj->pos.y-seg->data.wall.ty;
    }
    
		// get bump move
		
	if ((ydif<=0) || (ydif>obj->bump.high)) return(FALSE);

	ymove=pin_upward_movement_obj(obj,-ydif);
	
		// check if bump will be OK
		
	if (!move_obj_check_bump(obj,xmove,zmove,ymove)) return(FALSE);
	
		// do bump
	
	obj->contact.wall_seg_idx=-1;
	
	obj->pos.y+=ymove;
	obj->bump.smooth_offset+=abs(ymove);					// bump moves player up, but offset pushes them down and smoothes out the bump
	
	return(obj->contact.ceiling_seg_idx==-1);
}

/* =======================================================

      Object Falling
      
======================================================= */

void object_fall(obj_type *obj)
{
	int				y,fy,ymove,uid;
	obj_type		*hit_obj;
	
		// suspended objects don't fall
		
	if (obj->suspend) return;
	
		// check standing on floors

	y=obj->pos.y;
		
	fy=pin_downward_movement_obj(obj,floor_slop);
	fy+=y;

		// deal with floor contacts
		
	if (obj->contact.floor_seg_idx!=-1) {
	
		obj->stand_obj_uid=-1;

			// below or on the floor?

		if (y>=fy) {
			obj->air_mode=am_ground;
			obj->pos.y=fy;
			return;
		}
		
			// above the floor

		ymove=(int)obj->force.vct.y;
		if ((y>(fy-floor_slop)) || ((y+ymove)>fy)) {		// next floor or slop will go past floor
			if (ymove>=0) {
				obj->air_mode=am_ground;
				obj->pos.y=fy;
			}
			return;
		}
	}
	
		// check standing on objects

	uid=collide_find_object_for_standing_object(obj);
	if (uid!=-1) {
		hit_obj=object_find_uid(uid);
		
		obj->contact.obj_uid=uid;
		obj->pos.y=hit_obj->pos.y-hit_obj->size.y;
		obj->air_mode=am_ground;
		
			// stand on script
			
		if (obj->stand_obj_uid!=uid) {
			object_setup_touch(hit_obj,obj,TRUE);
			scripts_post_event_console(&hit_obj->attach,sd_event_touch,0,0);
		}
		
		obj->stand_obj_uid=uid;
		return;
	}

	obj->stand_obj_uid=-1;
	
		// find air mode
		
	if (obj->force.vct.y<0) {
		obj->air_mode=am_up;
	}
	else {
		obj->air_mode=am_falling;
	}
}

/* =======================================================

      X/Z Movement
      
======================================================= */

bool object_move_xz(obj_type *obj,int xmove,int zmove)
{
	bool			hit;
	d3pos			savepos;
	
	if ((xmove==0) && (zmove==0)) return(FALSE);
	
	memmove(&savepos,&obj->pos,sizeof(d3pos));
	
	if (obj->no_slide) {
		hit=map_obj_move_xz(obj,xmove,zmove);
	}
	else {
		hit=move_obj_check_xz_map_slide(obj,xmove,zmove);
	}
	
	if (!map_find_portal_by_pos(&map,&obj->pos)) {			// might have moved into a new portal
		memmove(&obj->pos,&savepos,sizeof(d3pos));
		return(TRUE);
	}

	return(hit);
}

void object_move_xz_bounce(obj_type *obj)
{
	if (obj->contact.wall_seg_idx==-1) {
		obj->bounce.wall_seg_idx=-1;
		return;
	}

		// only bounce if bounce factor != 0 and not same wall segment
		
	if (obj->bounce.factor==0.0f) return;
	if (obj->bounce.wall_seg_idx==obj->contact.wall_seg_idx) return;
	
		// bounce
		
	obj->force.vct.x-=(obj->motion.vct.x*obj->bounce.factor);
	obj->force.vct.z-=(obj->motion.vct.z*obj->bounce.factor);
}

void object_to_object_push(obj_type *obj,float xmove,float zmove)
{
	int				weight_dif;
	float			f;
	obj_type		*pushed_obj;
	
	if (obj->contact.obj_uid==-1) return;
	
		// get object to push

	pushed_obj=object_find_uid(obj->contact.obj_uid);
	if (pushed_obj==NULL) return;

	if (!pushed_obj->contact.pushable) return;

		// compare weights

	weight_dif=obj->size.weight-pushed_obj->size.weight;
	if (weight_dif<0) return;

		// reduce movement for weight

	if (weight_dif>500) weight_dif=500;
	
	f=1.0f+(((float)weight_dif)/1000.0f);
	
		// add in vector
	
	pushed_obj->force.vct.x=xmove*f;
	pushed_obj->force.vct.z=zmove*f;
}

/* =======================================================

      Y Movement
      
======================================================= */

void object_move_y(obj_type *obj,int ymove)
{
	int				fy,by,up_move;
	
		// moving up
		
	if (ymove<0) {
	
		obj->air_mode=am_up;
		
			// have any floors come up from below

		fy=pin_downward_movement_obj(obj,floor_slop);
		if (fy<0) {
			obj->pos.y+=fy;
			ymove+=fy;
			if (ymove>=0) return;
		}

			// go upwards

		up_move=pin_upward_movement_obj(obj,ymove);
		obj->pos.y+=up_move;
		
		if (up_move>ymove) {
			obj->force.vct.y=0;
			obj->force.gravity=gravity_start_power;
			return;
		}

			// check for above map

		if (obj->pos.y<=0) {
			obj->pos.y=0;
			obj->force.vct.y=0;
			obj->force.gravity=gravity_start_power;
			return;
		}

		return;
	}
	
		// check for falling

	object_fall(obj);
	
		// fix objects gravity and view angles
		
	if (obj->air_mode==am_ground) {
 		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}
	
		// moving down
		
	if (ymove<=0) return;
	
	fy=pin_downward_movement_obj(obj,ymove);
	obj->pos.y+=fy;
	
	if (obj->air_mode==am_falling) obj->fall.dist+=fy;
	
		// check for below portal
		
	by=map.portals[obj->pos.rn].by;
	if (obj->pos.y>=by) {
		obj->pos.y=by;
		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
	}
}

void object_fly_y(obj_type *obj,int ymove)
{
	int				by;
	
		// moving up
		
	if (ymove<0) {
		obj->pos.y+=pin_upward_movement_obj(obj,ymove);
		if (obj->contact.ceiling_seg_idx!=-1) return;

			// check for above map
			
		if (obj->pos.y<=0) {
			obj->pos.y=0;
			return;
		}

		return;
	}
	
		// moving down
		
	if (ymove<=0) return;
	
	obj->pos.y+=pin_downward_movement_obj(obj,ymove);
	
		// check for below portal
		
	by=map.portals[obj->pos.rn].by;
	if (obj->pos.y>=by) {
		obj->pos.y=by;
	}
}

/* =======================================================

      Object Slope Speed Alterations
      
======================================================= */

void object_motion_slope_alter_movement_single(float *mv,float slope_y,float slope_mv)
{
	bool			same_dir;
	
		// are we going in same direction as slope?
		
	same_dir=FALSE;
	
	if ((*mv)!=0.0f) {
		same_dir=(((*mv)<0.0f) && (slope_mv>=0.0f)) || (((*mv)>=0.0f) && (slope_mv<=0.0f));
	}
	
		// if slope is greater than max gravity,
		// then if going against slope, stop all movement, otherwise always go down

	if (slope_y>=gravity_slope_max_y) {
		if (same_dir) {
			*mv=-slope_mv;
		}
		else {
			*mv=0.0f;
		}
		return;
	}

		// if going in same direction, then cut gravity effect

	if (same_dir) slope_mv*=gravity_slope_down_cut;
	
		// subtract slope push from movement

	*mv-=slope_mv;
}

void object_motion_slope_alter_movement(obj_type *obj,float *xmove,float *zmove)
{
	int				rn,x,y,z,sy,seg_idx;
	fc_segment_data	*fc;

		// if not on ground or ignoring slope gravity, then no speed reduction
		
	if (!obj->slope_gravity) return;
	if (obj->air_mode!=am_ground) return;
	if (obj->stand_obj_uid!=-1) return;
	
		// get floor going to
		
	x=obj->pos.x+(int)(*xmove);
	y=obj->pos.y;
	z=obj->pos.z+(int)(*zmove);

	rn=map_find_portal_hint(&map,obj->pos.rn,x,y,z);
	
	sy=find_fc_for_downward_point(rn,x,y,z,obj->size.y,&seg_idx);
	if (seg_idx==-1) return;

		// ignore flat floors

	fc=&map.segments[seg_idx].data.fc;
	if (fc->flat) return;

		// if less then min slope, no gravity effects

	if (fc->slope_y<gravity_slope_min_y) return;

		// apply gravity
	
	object_motion_slope_alter_movement_single(xmove,fc->slope_y,fc->slope_move_x);
	object_motion_slope_alter_movement_single(zmove,fc->slope_y,fc->slope_move_z);
}

/* =======================================================

      Do Movement Mainline
      
======================================================= */

void object_move_normal(obj_type *obj)
{
	int				i_xmove,i_zmove,xchg,zchg,
					fall_damage;
    float			xmove,zmove,ymove;
	bool			oldfalling;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_slope_alter_movement(obj,&xmove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);
    
		// falling
		
	oldfalling=(obj->air_mode==am_falling);

		// move Y

	object_move_y(obj,(int)ymove);
	
		// if on ground, stop all downward motion
		
	if (obj->air_mode==am_ground) {
		if (obj->force.vct.y>0) obj->force.vct.y=0;			// all external forces downward y forces are stopped
		obj->force.gravity=gravity_start_power;
	}
	
		// get exact movement
		
	xchg=obj->pos.x;
	zchg=obj->pos.z;
	
        // moving x/z
		
	i_xmove=(int)xmove;
	i_zmove=(int)zmove;

	while (TRUE) {
			
			// move

		if (!object_move_xz(obj,i_xmove,i_zmove)) break;

			// do any pushing
			
		object_to_object_push(obj,xmove,zmove);
		
			// if bumped up, move again

		if (object_bump_up(obj,i_xmove,i_zmove)) {
			ymove=0;
			continue;	
		}
		
		break;
	}
	
		// force move other objects
		
	xchg=obj->pos.x-xchg;
	zchg=obj->pos.z-zchg;
	
	if ((xchg!=0) || (zchg!=0)) {
		object_move_with_object(obj,xchg,zchg);
	}
	
		// bounces
		
	object_move_xz_bounce(obj);
	
		// objects have finished falling
	
    if (obj->air_mode!=am_falling) {
    
        if (oldfalling) {
            if (obj->fall.dist>map_enlarge) {
			
					// get damage
					
				if (obj->fall.dist>=obj->fall.damage_minimum_height) {
					fall_damage=(int)(((float)(obj->fall.dist-obj->fall.damage_minimum_height))*obj->fall.damage_factor);
					if (fall_damage!=0) object_damage(obj,NULL,NULL,NULL,NULL,fall_damage);
				}
				
					// send the land events
					
                scripts_post_event_console(&obj->attach,sd_event_land,0,0);
                object_post_move_animation_event(obj,sd_event_animation_object_land);
            }
        }
        
        obj->fall.dist=0;
        obj->fall.change=FALSE;

		return;
	}
	
		// objects are currently falling
	
	if ((obj->fall.dist>map_enlarge) && (!obj->fall.change)) {
		scripts_post_event_console(&obj->attach,sd_event_fall,0,0);
		scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_fall,0);
		obj->fall.change=TRUE;
	}
}

void object_move_fly(obj_type *obj)
{
	int				xchg,zchg;
    float			xmove,zmove,ymove;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);
    
		// get exact movement
		
	xchg=obj->pos.x;
	zchg=obj->pos.z;
	
        // moving x/z
		
	if (object_move_xz(obj,(int)xmove,(int)zmove)) {
		object_to_object_push(obj,xmove,zmove);
	}
	
		// force move other objects
		
	xchg=obj->pos.x-xchg;
	zchg=obj->pos.z-zchg;
	
	if ((xchg!=0) || (zchg!=0)) {
		object_move_with_object(obj,xchg,zchg);
	}
	
		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_fly_y(obj,(int)ymove);
}

void object_move_swim(obj_type *obj)
{
    float			xmove,zmove,ymove,liq_speed_alter;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);

		// speed cuts

	liq_speed_alter=object_liquid_alter_speed(obj);
	
	xmove*=liq_speed_alter;
  	zmove*=liq_speed_alter;
	ymove*=liq_speed_alter;

		// falling in water

	if (ymove>=0) object_fall(obj);

	if ((obj->air_mode==am_ground) || (ymove<0)) {
		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
	}

        // moving x/z
		
	if (object_move_xz(obj,(int)xmove,(int)zmove)) {
		object_to_object_push(obj,xmove,zmove);
	}
	
		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_fly_y(obj,(int)ymove);
}

/* =======================================================

      Do Movement Mainline
      
======================================================= */

void object_move(obj_type *obj)
{
		// clear all contacts

	object_clear_contact(&obj->contact);

		// call proper movement

	if (obj->fly) {
		object_move_fly(obj);
	}
	else {
		if (obj->liquid_mode==lm_under) {
			object_move_swim(obj);
		}
		else {
			object_move_normal(obj);

		//	object_move_normal_2(obj);		// supergumba -- NEW COLLIDE -- more collision edits
		}
	}

		// check for contacts

	if (obj->contact.wall_seg_idx!=-1) {
		if (!obj->in_collide_event) {
			obj->in_collide_event=TRUE;
			scripts_post_event_console(&obj->attach,sd_event_collide,0,0);
		}
	}
	else {
		obj->in_collide_event=FALSE;
	}
}

/* =======================================================

      Stop Object Movement
      
======================================================= */

void object_move_start(obj_type *obj)
{
	obj->forward_move.moving=TRUE;
}

void object_move_stop(obj_type *obj)
{
	obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;
	
	object_auto_walk_stop(obj);
}

/* =======================================================

      Objects Moving with Segments/Objects
      
======================================================= */

inline bool object_move_with_move(obj_type *obj,int xmove,int zmove)
{
	if (obj->lock.x) xmove=0;
	if (obj->lock.z) zmove=0;
	
	return(object_move_xz(obj,xmove,zmove));
}

bool object_move_with_wall_segment_check_wall(d3box *box,int seg_idx,int xmove,int zmove)
{
	int					lx,rx,lz,rz;
	wall_segment_data	*wall;
	
	wall=&map.segments[seg_idx].data.wall;
		
		// completely out of Ys?
		
	if ((box->min_y>=wall->by) || (box->max_y<=wall->ty)) return(FALSE);
		
		// get wall points plus movement
			
	lx=wall->lx+xmove;
	rx=wall->rx+xmove;
	lz=wall->lz+zmove;
	rz=wall->rz+zmove;
		
		// check x/z (2D) wall
			
	return(box_collide_2D_line(box,lx,rx,lz,rz));
}

void object_move_with_wall_segment(int seg_idx,int xmove,int zmove)
{
	int			i;
	d3box		box;
	obj_type	*obj;

	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {

		if (!obj->remote.on) {

				// has wall moved into object?

			box_create_from_object(&box,obj);

			if (object_move_with_wall_segment_check_wall(&box,seg_idx,xmove,zmove)) {
				if (object_move_with_move(obj,xmove,zmove)) {
					object_crush(obj,TRUE);					// if move fails, then auto-crush
				}
			}
		}

		obj++;
	}
}

void object_move_with_floor_segment(int seg_idx,int xmove,int zmove)
{
	int			i;
	obj_type	*obj;
	
	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {

		if (!obj->remote.on) {

				// are we standing on this floor?

			if (obj->contact.floor_seg_idx==seg_idx) object_move_with_move(obj,xmove,zmove);

		}
		obj++;
	}
}

void object_move_with_object(obj_type *obj,int xmove,int zmove)
{
	int			i;
	obj_type	*obj_check;
	
	obj_check=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		if (!obj_check->remote.on) {
			if (obj_check->stand_obj_uid==obj->uid) {
				if (!obj_check->suspend) {
					object_move_with_move(obj_check,xmove,zmove);
				}
			}
			obj_check++;
		}
	}
}

/* =======================================================

      Reset Object to Original Spot
      
======================================================= */

void object_reset_prepare(obj_type *obj)
{
	memmove(&obj->org_pos,&obj->pos,sizeof(d3pos));
	memmove(&obj->org_ang,&obj->ang,sizeof(d3ang));
}

void object_reset(obj_type *obj)
{
	memmove(&obj->pos,&obj->org_pos,sizeof(d3pos));
	map_find_portal_by_pos(&map,&obj->pos);
	
	memmove(&obj->ang,&obj->org_ang,sizeof(d3ang));
	obj->motion.ang.y=obj->turn.ang_to.y=obj->ang.y;
}

/* =======================================================

      Objects Ducking/Standing Up
      
======================================================= */

void object_duck(obj_type *obj)
{
    int			duck_mode,y_change;
    
    duck_mode=obj->duck.mode;
    if ((duck_mode==dm_stand) || (duck_mode==dm_duck)) return;
    
    
    if (duck_mode==dm_stand_up) {
    
        obj->duck.y_move-=obj->duck.y_add;
        if (obj->duck.y_move>0) return;
        
        obj->duck.y_move=0;
        obj->duck.mode=dm_stand;
        object_post_move_animation_event(obj,sd_event_animation_object_standing);
        return;
    }
    
    if (duck_mode==dm_duck_down) {
    
        obj->duck.y_move+=obj->duck.y_add;
        y_change=obj->duck.y_change;
        if (obj->duck.y_move<y_change) return;
        
        obj->duck.y_move=y_change;
        obj->duck.mode=dm_duck;
        object_post_move_animation_event(obj,sd_event_animation_object_ducking);
        return;
    }
}

/* =======================================================

      Objects Smooth Bumping
      
======================================================= */

void object_fix_bump_smooth(obj_type *obj)
{
	int			k;
	
	if (obj->bump.smooth_offset==0) return;
	
		// slowly remove bump offset
		
	k=(int)(((float)obj->bump.smooth_offset)*obj->bump.smooth_factor);
	if (k<10) k=10;
	
	obj->bump.smooth_offset-=k;
	if (obj->bump.smooth_offset<0) obj->bump.smooth_offset=0;
}

