/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server Periodic Processes

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
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "scenery.h"
#include "models.h"
#include "effects.h"
#include "cameras.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void group_moves_run(void);
extern void map_movements_auto_open(void);

/* =======================================================

      Portal Triggers/Messages
  
======================================================= */

void portal_triggers(obj_type *obj,int old_rn,int rn)
{
	portal_type		*portal;

	if (rn!=-1) {	
		portal=&map.portals[rn];
		
			// entry messages
			
		if (portal->msg.entry_on) {
			scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_course,portal->msg.entry_id);
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_course,portal->msg.entry_id);
		}
		
			// map change messages
			
		if (portal->msg.map_change_on) {
			strcpy(map.info.name,portal->msg.map_name);
			strcpy(map.info.player_start_name,portal->msg.map_spot_name);
			strcpy(map.info.player_start_type,portal->msg.map_spot_type);
			map.info.in_load=FALSE;

			server.map_change=TRUE;
		}
		
			// portal watch messages
			
		object_watch_portal_alert(obj,TRUE);
		
			// base watch messages
			
		if (portal->msg.base_on) object_watch_base_alert(portal,obj,TRUE);
	}
	
	if (old_rn!=-1) {
	
			// exit messages
			
		portal=&map.portals[old_rn];
		if (portal->msg.exit_on) {
			scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_course,portal->msg.exit_id);
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_course,portal->msg.exit_id);
		}
		
			// portal watch messages
			
		object_watch_portal_alert(obj,FALSE);
		
			// base watch messages
			
		if (portal->msg.base_on) object_watch_base_alert(portal,obj,FALSE);
	}
}

/* =======================================================

      Setup Objects
  
======================================================= */

void setup_objects(void)
{
	int			n;
	obj_type	*obj;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if (obj->scenery.on) {
			obj->run_setup.stand_ok=obj->contact.on;
			obj->run_setup.collision_ok=obj->contact.on;
			obj->run_setup.push_ok=FALSE;
			obj->run_setup.pickup_ok=FALSE;
			obj->run_setup.melee_ok=FALSE;
		}
		else {
			obj->run_setup.stand_ok=(!obj->hidden)&&(obj->contact.on)&&(!obj->pickup.on);
			obj->run_setup.collision_ok=(!obj->hidden)&&(obj->contact.on)&&(!obj->pickup.on);
			obj->run_setup.push_ok=(!obj->hidden)&&(!obj->suspend)&&(obj->contact.on);
			obj->run_setup.pickup_ok=(!obj->hidden)&&(obj->pickup.on)&&(obj->contact.on);
			obj->run_setup.melee_ok=(!obj->hidden)&&(obj->contact.on);
		}

		obj++;
	}
}

/* =======================================================

      Run Specific Objects
  
======================================================= */

void run_object_remote(obj_type *obj)
{
	remote_predict_move(obj);
	item_pickup_check(obj);
}

void run_object_normal(obj_type *obj,int tick)
{
		// spawning
		
	if (obj->spawning) object_spawn(obj);

		// item counts
		
	obj->count++;
	if (obj->item_count!=0) obj->item_count--;
	
		// turning and looking
		
	if (obj->player) {
		if (!obj->input_freeze) {
			object_player_turn(obj);
			object_player_look(obj);
		}
		else {
			if (!obj->suspend) {
				object_turn(obj);
			}
		}
		object_fs_effect_run(tick,obj);
	}
	else {
		if (!obj->suspend) {
			object_turn(obj);
		}
	}

		// watches

	object_watch(obj);

		// health recover

	object_health_recover(obj);

		// movement
	
	if (!obj->suspend) {
		object_auto_walk(obj);
	
		object_fix_motion(obj);
		object_movement(obj,&obj->forward_move);
		object_movement(obj,&obj->side_move);
		object_simple_movement(obj,&obj->vert_move);
		
		object_gravity(obj);
		object_fix_force(obj);
		object_move(obj);
		
		object_fix_bump_smooth(obj);
		object_duck(obj);

		object_touch(obj);
		object_liquid(tick,obj);

		object_crush(obj,FALSE);

		item_pickup_check(obj);
	}

		// auto-growing

	object_grow_run(obj);
	
		// animation events

	if (obj->player) {
		object_event_animations(obj);
	}
	
		// death check
		
	object_death(obj);
}


/* =======================================================

      Run Objects
  
======================================================= */

void run_objects_slice(int tick)
{
	int				n,old_rn;
	obj_type		*obj;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if (!obj->scenery.on) {
		
				// remember current portal

			old_rn=obj->pos.rn;

				// run object

			if (obj->remote.on) {
				run_object_remote(obj);
			}
			else {
				run_object_normal(obj,tick);
			}

				// trigger any portal changes
			
			if (old_rn!=obj->pos.rn) portal_triggers(obj,old_rn,obj->pos.rn);
		}
		
		obj++;
	}
}

void run_objects_no_slice(int tick)
{
	int				n;
	obj_type		*obj;
	weapon_type		*weap;


	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		model_draw_setup_object(tick,obj);
		model_run_animation(&obj->draw);

		if (!obj->scenery.on) {

				// fades

			model_fade_run(tick,&obj->draw);
			model_mesh_fade_run(tick,&obj->draw);

				// held weapons

			if (obj->player) {
				weap=weapon_find_current(obj);
				if (weap!=NULL) {
					model_draw_setup_weapon(tick,obj,weap,FALSE);
					weapon_run_hand(obj,tick);
				}
			}
		}
			
		obj++;
	}
}

/* =======================================================

      Run Projectiles
      
======================================================= */

void run_projectiles_slice(int tick)
{
	int				n;
	proj_type		*proj;
	
	for (n=0;n!=server.count.proj;n++) {
		proj=&server.projs[n];

		object_clear_contact(&proj->contact);
	   
			// projectile counts
			
		proj->count++;
		if (proj->parent_grace>0) proj->parent_grace--;

			// moving projectiles
			
		if (!proj->stick) {
			projectile_speed(proj);

			if (projectile_move(proj)) {
				projectile_mark_dispose(proj);
				continue;
			}
			
			projectile_gravity(proj);
		}
		
			// stuck projectiles
			
		else {
			projectile_stick(proj);
		}
		
		projectile_collision(proj);
		
		if (projectile_hit(tick,proj,FALSE)) {
			projectile_mark_dispose(proj);
		}
	}
		
	projectile_dispose();
}

void run_projectiles_no_slice(int tick)
{
	int				n;
	proj_type		*proj;

	proj=server.projs;
	
	for (n=0;n!=server.count.proj;n++) {
		model_draw_setup_projectile(tick,proj);
		model_run_animation(&proj->draw);
		model_fade_run(tick,&proj->draw);
		model_mesh_fade_run(tick,&proj->draw);
		proj++;
	}
}

/* =======================================================

      Run Server Processes
      
======================================================= */

void server_run(int tick)
{
	obj_type		*obj;
	
		// get player object
		
	obj=object_find_uid(server.player_obj_uid);
	
		// time to run tasks
		
	if (tick>=server.time.run_tick) {
	
			// tasks that require 1/100th of
			// a second operation
			
		while (tick>=server.time.run_tick) {
			server.time.run_tick+=10;

			group_moves_run();
			
			setup_objects();
			run_objects_slice(tick);
			
			run_projectiles_slice(tick);
			
			map_movements_auto_open();
			
			camera_run();
			weapon_hand_bounce(obj);
			
			particle_map_run();
		}

			// tasks that do not require
			// 1/100th of an operation but a specific
			// tick count

		run_objects_no_slice(tick);
		run_projectiles_no_slice(tick);

			// effects and decal time-outs
		
		effect_dispose();
		decal_dispose();
	}
}

