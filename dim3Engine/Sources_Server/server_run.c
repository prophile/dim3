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

#include "network.h"
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

      Mesh Triggers/Messages
  
======================================================= */

void mesh_triggers(obj_type *obj,int old_mesh_idx,int mesh_idx)
{
	map_mesh_type			*mesh;

	if (mesh_idx!=-1) {	
		mesh=&map.mesh.meshes[mesh_idx];
		
			// entry messages
			
		if (mesh->msg.entry_on) {
			scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_course,mesh->msg.entry_id);
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_course,mesh->msg.entry_id);
		}
		
			// map change messages
			
		if (mesh->msg.map_change_on) {
			strcpy(map.info.name,mesh->msg.map_name);
			strcpy(map.info.player_start_name,mesh->msg.map_spot_name);
			strcpy(map.info.player_start_type,mesh->msg.map_spot_type);
			map.info.in_load=FALSE;

			server.map_change=TRUE;
		}
		
			// base watch messages
			
		if (mesh->msg.base_on) object_watch_base_alert(mesh,obj,TRUE);
	}
	
	if (old_mesh_idx!=-1) {
	
		mesh=&map.mesh.meshes[old_mesh_idx];
	
			// exit messages
			
		if (mesh->msg.exit_on) {
			scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_course,mesh->msg.exit_id);
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_course,mesh->msg.exit_id);
		}
		
			// base watch messages
			
		if (mesh->msg.base_on) object_watch_base_alert(mesh,obj,FALSE);
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
	
	object_thrust(obj);

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
		object_ducking(obj);

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
	int				n,mesh_idx;
	d3pnt			old_pnt;
	obj_type		*obj;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if (!obj->scenery.on) {
		
				// remember current position if not suspended
				// so we can check for mesh changes
				
			if (!obj->suspend) {
				old_pnt.x=obj->pnt.x;
				old_pnt.y=obj->pnt.y;
				old_pnt.z=obj->pnt.z;
			}

				// run object

			if (obj->remote.on) {
				run_object_remote(obj);
			}
			else {
				run_object_normal(obj,tick);
			}

				// trigger any mesh changes if not suspended
			
			if (!obj->suspend) {
				if ((old_pnt.x!=obj->pnt.x) || (old_pnt.y!=obj->pnt.y) || (old_pnt.z!=obj->pnt.z)) {
					mesh_idx=map_find_mesh(&map,&obj->pnt);
					if (obj->mesh.cur_mesh_idx!=mesh_idx) {
						mesh_triggers(obj,obj->mesh.cur_mesh_idx,mesh_idx);
						obj->mesh.cur_mesh_idx=mesh_idx;
					}
				}
			}
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

