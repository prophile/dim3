/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Actions

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

#include "client.h"
#include "scripts.h"
#include "objects.h"
#include "cameras.h"
#include "sounds.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern network_setup_type	net_setup;

/* =======================================================

      Animations
      
======================================================= */

void object_post_move_animation_event(obj_type *obj,int sub_event)
{
	obj->last_move_animation_event=sub_event;
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sub_event,0);
}

bool object_post_move_animation_event_change(obj_type *obj,int sub_event)
{
	if (obj->last_move_animation_event==sub_event) return(FALSE);
	
	obj->last_move_animation_event=sub_event;
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sub_event,0);
	
	return(TRUE);
}

void object_post_turn_animation_event(obj_type *obj,int sub_event)
{
	obj->last_turn_animation_event=sub_event;
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sub_event,0);
}

bool object_post_turn_animation_event_change(obj_type *obj,int sub_event)
{
	if (obj->last_turn_animation_event==sub_event) return(FALSE);
	
	obj->last_turn_animation_event=sub_event;
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sub_event,0);
	
	return(TRUE);
}

void object_event_animations(obj_type *obj)
{
	int				last_event;
	bool			moving,forward_moving,side_moving,
					in_air,standing,ducked,ducking,running,forward_reverse,side_reverse;
	
		// current states
		
	forward_moving=obj->forward_move.moving;
	side_moving=obj->side_move.moving;
	moving=forward_moving||side_moving;
	
	in_air=obj->air_mode!=am_ground;
	standing=obj->duck.mode==dm_stand;
	ducked=obj->duck.mode==dm_duck;
	ducking=(obj->duck.mode==dm_stand_up) || (obj->duck.mode==dm_duck_down);
	running=obj->forward_move.running;
	
	forward_reverse=obj->forward_move.reverse;
	side_reverse=obj->side_move.reverse;
	
	last_event=obj->last_move_animation_event;

		// turning movement

	if (obj->turn.ang_add.y==0.0f) {
		object_post_turn_animation_event_change(obj,sd_event_animation_object_turn_stop);
	}
	else {
		if (obj->turn.ang_add.y<0.0f) {
			object_post_turn_animation_event_change(obj,sd_event_animation_object_turn_left);
		}
		else {
			object_post_turn_animation_event_change(obj,sd_event_animation_object_turn_right);
		}
	}
		
		// not moving, only stop events
		
	if (!moving) {

		if ((!in_air) && (!ducking)) {
			object_post_move_animation_event_change(obj,sd_event_animation_object_stop);
			return;
		}

		return;
	}

		// walking

	if ((forward_moving) && (!ducked) && (!running)) {

			// walking forward

		if (!forward_reverse) {

				// walking forward to side

			if (side_moving) {

					// walking forward to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_left_forward);
					return;
				}

					// walking forward to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_right_forward);
					return;
				}
			}

				// walking straight forward

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_walk);
				return;
			}
		}

			// walking backwards

		else {

				// walking backwards to side

			if (side_moving) {

					// walking backwards to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_left_back);
					return;
				}

					// walking backwards to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_right_back);
					return;
				}
			}

				// walking straight backwards

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_walk_back);
				return;
			}
		}

		return;
	}

		// running

	if ((forward_moving) && (!ducked) && (running)) {

			// running forward

		if (!forward_reverse) {

				// running forward to side

			if (side_moving) {

					// running forward to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_left_forward);
					return;
				}

					// running forward to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_right_forward);
					return;
				}
			}

				// running straight forward

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_run);
				return;
			}
		}

			// running backwards

		else {

				// running backwards to side

			if (side_moving) {

					// running backwards to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_left_back);
					return;
				}

					// running backwards to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_right_back);
					return;
				}
			}

				// running straight backwards

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_run_back);
				return;
			}
		}

		return;
	}

		// crawling

	if ((forward_moving) && (ducked)) {

			// crawling forward

		if (!forward_reverse) {

				// crawling forward to side

			if (side_moving) {

					// crawling forward to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_left_forward);
					return;
				}

					// crawling forward to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_right_forward);
					return;
				}
			}

				// crawling straight forward

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl);
				return;
			}
		}

			// crawling backwards

		else {

				// crawling backwards to side

			if (side_moving) {

					// crawling backwards to left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_left_back);
					return;
				}

					// crawling backwards to right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_right_back);
					return;
				}
			}

				// crawling straight backwards

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_back);
				return;
			}
		}

		return;
	}

		// left and right only movement

	if ((!forward_moving) && (side_moving)) {

			// crawling left/right

		if (ducked) {

				// crawl left side

			if (!side_reverse) {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_left);
				return;
			}

				// crawl right side

			else {
				object_post_move_animation_event_change(obj,sd_event_animation_object_crawl_right);
				return;
			}
		}

		else {

				// walking left/right

			if (!running) {

					// walk left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_left);
					return;
				}

					// walk right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_walk_right);
					return;
				}
			}

				// running left/right

			else {

					// running left side

				if (!side_reverse) {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_left);
					return;
				}

					// running right side

				else {
					object_post_move_animation_event_change(obj,sd_event_animation_object_run_right);
					return;
				}
			}
		}

		return;
	}
}

/* =======================================================

      Jumping
      
======================================================= */

void object_start_jump(obj_type *obj)
{
	poly_pointer_type	*poly_ptr;
	map_mesh_poly_type	*mesh_poly;
	
		// can't jump if flying or not on ground
		
	if ((obj->fly) || (!obj->jump.on)) return;
	if ((obj->liquid_mode==lm_under) || (obj->air_mode!=am_ground)) return;
	
		// if current standing slope is too high to climb, you can't jump
		
	if (obj->slope_gravity) {

		poly_ptr=&obj->contact.stand_poly;

		if (poly_ptr->portal_idx!=-1) {
			mesh_poly=&map.portals[poly_ptr->portal_idx].mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];
			if (!mesh_poly->box.flat) {
				if (mesh_poly->slope.y>gravity_slope_max_y) return;
			}
		}

	}
	
		// start jump
    
	scripts_post_event_console(&obj->attach,sd_event_jump,0,0);
    
	obj->force.vct.y=-(float)obj->jump.y_add;
    obj->force.gravity=gravity_start_power;
    
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_jump,0);
}

void object_liquid_jump(obj_type *obj)
{
	int				jump_add;
	
		// small jump if no polygon contact

	jump_add=obj->jump.y_add;
	if (obj->contact.hit_poly.portal_idx==-1) jump_add/=2;

		// jump out of water

	scripts_post_event_console(&obj->attach,sd_event_jump,0,0);
    
	obj->force.vct.y=-(float)jump_add;
    obj->force.gravity=gravity_start_power;
    
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_jump,0);
}

/* =======================================================

      Ducking
      
======================================================= */

void object_start_stand(obj_type *obj)
{
	int			old_mode;
	bool		stand_ok;
	
	if ((obj->fly) || (!obj->duck.on)) return;
    if ((obj->duck.mode==dm_stand) || (obj->duck.mode==dm_stand_up)) return;
	
		// check if object can stand up
		
	old_mode=obj->duck.mode;
	obj->duck.mode=dm_stand;

	stand_ok=!map_stand_check_object(obj);
	
	obj->duck.mode=old_mode;
	
	if (!stand_ok) return;
	
		// stand up
    
    obj->duck.mode=dm_stand_up;
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_stand_up,0);
}

void object_start_duck(obj_type *obj)
{
	if ((obj->fly) || (!obj->duck.on)) return;
	if (obj->air_mode!=am_ground) return;
    if ((obj->duck.mode==dm_duck) || (obj->duck.mode==dm_duck_down)) return;

    obj->duck.mode=dm_duck_down;
	scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_duck_down,0);
}

void object_ducking(obj_type *obj)
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

      Attach Sounds To Objects
      
======================================================= */

void object_set_ambient(obj_type *obj,char *name,float pitch)
{
	strcpy(obj->ambient.name,name);
	obj->ambient.pitch=pitch;
	obj->ambient.on=TRUE;

	obj->ambient.buffer_idx=al_find_buffer(name);
}

void object_change_ambient_pitch(obj_type *obj,float pitch)
{
	obj->ambient.pitch=pitch;
}

void object_clear_ambient(obj_type *obj)
{
	obj->ambient.on=FALSE;
}

/* =======================================================

      Enter/Exit Vehicles
      
======================================================= */

bool object_enter_vehicle(obj_type *obj,char *err_str)
{
	int				uid,x,z,y,sz;
	obj_type		*vehicle_obj;
	obj_vehicle		*vehicle;

		// find a vehicle within right distance
		
	sz=(obj->size.z*3)>>1;
	
	angle_get_movement(obj->motion.ang.y,obj->size.z,&x,&z);
	uid=collide_find_object_for_object_move(obj,x,z);
		
	if (uid==-1) {
		if (err_str!=NULL) strcpy(err_str,"No object nearby to enter");
		return(FALSE);
	}

	vehicle_obj=object_find_uid(uid);
	if (!vehicle_obj->vehicle.on) {
		if (err_str!=NULL) strcpy(err_str,"Nearby object is not a vehicle");
		return(FALSE);
	}
	
		// can't enter a vehicle already entered
		
	vehicle=&vehicle_obj->vehicle;

	if (vehicle->attach_obj_uid!=-1) {
		if (err_str!=NULL) strcpy(err_str,"Vehicle has already been entered");
		return(FALSE);
	}

		// get enter offset

	x=vehicle_obj->pos.x-obj->pos.x;
	z=vehicle_obj->pos.z-obj->pos.z;
	y=vehicle_obj->pos.y-obj->pos.y;
		
	rotate_2D_point_center(&x,&z,-vehicle_obj->ang.y);
		
	vehicle->attach_offset.x=x;
	vehicle->attach_offset.z=z;
	vehicle->attach_offset.y=y;
	
		// send events
		
	scripts_post_event_console(&obj->attach,sd_event_vehicle,sd_event_vehicle_enter,0);
	scripts_post_event_console(&vehicle_obj->attach,sd_event_vehicle,sd_event_vehicle_enter,0);
	
		// hide the object
		
	obj->hidden=TRUE;
	object_stop(obj);
	
		// attach object

	vehicle->attach_obj_uid=obj->uid;

		// if this object was the camera object, then reconnect

	if (camera.obj_uid==obj->uid) camera_connect(vehicle_obj);
	
		// if this object was the player object, move to vehicle

	if (server.player_obj_uid==obj->uid) {
		vehicle_obj->player=TRUE;
		obj->player=FALSE;
		server.player_obj_uid=vehicle_obj->uid;
	}

	return(TRUE);
}

bool object_exit_vehicle(obj_type *vehicle_obj,bool ignore_errors,char *err_str)
{
	int				x,z,y;
	bool			old_ok,empty;
	obj_type		*orig_obj;
	obj_vehicle		*vehicle;

		// don't exit moving vehicles

	if (!ignore_errors) {
		if ((vehicle_obj->forward_move.moving) || (vehicle_obj->side_move.moving) || (vehicle_obj->vert_move.moving)) {
			if (err_str!=NULL) strcpy(err_str,"Can not exit moving vehicle");
			return(FALSE);
		}
	}

		// get original object
	
	orig_obj=object_find_uid(vehicle_obj->vehicle.attach_obj_uid);
	
		// find exit point
		
	vehicle=&vehicle_obj->vehicle;
	
	x=vehicle->attach_offset.x;
	z=vehicle->attach_offset.z;
	y=vehicle->attach_offset.y;
	
	rotate_2D_point_center(&x,&z,vehicle_obj->ang.y);
	
	orig_obj->pos.x=vehicle_obj->pos.x-x;
	orig_obj->pos.z=vehicle_obj->pos.z-z;
	orig_obj->pos.y=vehicle_obj->pos.y-y;

	orig_obj->ang.y=angle_find(orig_obj->pos.x,orig_obj->pos.z,vehicle_obj->pos.x,vehicle_obj->pos.z);
	
		// can we exit?
		
	if (!map_find_portal_by_pos(&map,&orig_obj->pos)) {
	
		if (!ignore_errors) {
			if (err_str!=NULL) strcpy(err_str,"No space in map to exit");
			return(FALSE);
		}
		
		memmove(&orig_obj->pos,&vehicle_obj->pos,sizeof(d3pos));
	}
	
	old_ok=vehicle_obj->run_setup.collision_ok;				// don't collide with vehicle
	vehicle_obj->run_setup.collision_ok=FALSE;
	
	empty=!map_spot_empty_object(orig_obj);
	
	vehicle_obj->run_setup.collision_ok=old_ok;
	
	if ((!empty) && (!ignore_errors)) {
		if (err_str!=NULL) strcpy(err_str,"No space in map to exit");
		return(FALSE);
	}

		// send events
		
	scripts_post_event_console(&vehicle_obj->attach,sd_event_vehicle,sd_event_vehicle_exit,0);
	scripts_post_event_console(&orig_obj->attach,sd_event_vehicle,sd_event_vehicle_exit,0);
	
		// exit
	
	vehicle->attach_obj_uid=-1;
	object_stop(vehicle_obj);

		// if this vehicle was the camera object, then reconnect

	if (camera.obj_uid==vehicle_obj->uid) camera_connect(orig_obj);

		// if this vehicle was the player object, move to object

	if (server.player_obj_uid==vehicle_obj->uid) {
		vehicle_obj->player=FALSE;
		orig_obj->player=TRUE;
		server.player_obj_uid=orig_obj->uid;
	}

		// unhide the original object
		
	orig_obj->hidden=FALSE;

	return(TRUE);
}
