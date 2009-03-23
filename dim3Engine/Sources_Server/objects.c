/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Creation/Dispose

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
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "interfaces.h"
#include "consoles.h"

extern int					current_map_spawn_idx;

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern hud_type				hud;
extern js_type				js;

int							ndelayed_obj_spawn,
							game_obj_rule_uid=-1;

delayed_obj_spawn_type		*delayed_obj_spawns;

/* =======================================================

      Initialize Object List
      
======================================================= */

void object_initialize_list(void)
{
	server.objs=NULL;
	server.count.obj=0;
	server.uid.obj=0;

		// script based object spawns need to
		// be delayed so they don't effect the
		// object list in loop

	ndelayed_obj_spawn=0;
	delayed_obj_spawns=(delayed_obj_spawn_type*)malloc(sizeof(delayed_obj_spawn_type)*max_delayed_obj_spawn);
}

void object_free_list(void)
{
	if (delayed_obj_spawns!=NULL) free(delayed_obj_spawns);

	delayed_obj_spawns=NULL;
}

/* =======================================================

      Clear Object Structures
      
======================================================= */

void object_clear_size(obj_size *size)
{
	size->x=size->z=10*map_enlarge;
	size->y=20*map_enlarge;

	size->eye_offset=18*map_enlarge;

	size->radius=10*map_enlarge;
	size->weight=200;
}

void object_clear_position(d3pnt *pnt)
{
	pnt->x=pnt->y=pnt->z=0;
}

void object_clear_angle(d3ang *ang)
{
	ang->x=ang->y=ang->z=0;
}

void object_clear_motion(obj_motion *motion)
{
	motion->last_y_change=0;
	motion->vct.x=motion->vct.z=motion->vct.y=0;
	motion->ang.y=motion->ang.x=motion->ang.z=0;
}

void object_clear_movement(obj_movement *move)
{
    move->speed=0;
    move->max_crawl_speed=move->max_walk_speed=move->max_run_speed=move->max_air_speed=0;
    move->accelerate=move->decelerate=move->air_accelerate=move->air_decelerate=0;
	move->slop=3.0f;
	move->seek_ang=0.0f;
	move->running=FALSE;
    move->moving=FALSE;
    move->reverse=FALSE;
	move->seeking=FALSE;
}

void object_clear_force(obj_force *force)
{
	force->vct.x=force->vct.z=force->vct.y=0.0f;
    force->gravity=gravity_start_power;
}

void object_clear_contact(obj_contact *contact)
{
	contact->hit_poly.mesh_idx=-1;
	contact->stand_poly.mesh_idx=-1;
	contact->head_poly.mesh_idx=-1;

	contact->liquid_idx=-1;
	
	contact->obj_uid=-1;
	contact->proj_uid=-1;
	
	contact->melee=FALSE;
}

void object_clear_touch(obj_touch *touch)
{
	touch->obj_uid=-1;
	
	touch->pnt.x=touch->pnt.y=touch->pnt.z=0;
	touch->ang.x=touch->ang.y=touch->ang.z=0.0f;
	
	touch->stand=FALSE;
}

void object_clear_hit(obj_hit *hit)
{
	hit->obj_uid=-1;
	hit->weap_uid=-1;
	hit->proj_uid=-1;
	
	hit->pnt.x=hit->pnt.y=hit->pnt.z=0;
	hit->ang.x=hit->ang.y=hit->ang.z=0.0f;
	
	hit->damage=0;
}

void object_clear_pickup(obj_pickup *pickup)
{
	pickup->obj_uid=-1;
	pickup->item_uid=-1;
}

void object_clear_remote(obj_remote *remote)
{
	remote->on=FALSE;
	remote->uid=-1;
}

void object_clear_watch(obj_watch *watch)
{
	watch->on=FALSE;
	watch->obj_uid=-1;
	watch->dist=map_enlarge*50;
	watch->restrict_on=FALSE;
	watch->restrict_ang=0.0f;
	memset(watch->obj_flags,0x0,max_object);
}

void object_clear_draw(model_draw *draw)
{
	int				k;
	
		// model draw
		
	draw->on=FALSE;
	draw->uid=-1;
	draw->name[0]=0x0;
	draw->bounce=FALSE;
	draw->face_forward=FALSE;
	draw->offset.x=draw->offset.z=draw->offset.y=0;
	draw->rot.x=draw->rot.z=draw->rot.y=0.0f;
	draw->spin.x=draw->spin.z=draw->spin.y=0.0f;
	draw->spin_tick=0;
	draw->alpha=1;
	draw->resize=1;
	
		// lights
		
	draw->script_light_idx=0;

	for (k=0;k!=max_model_light;k++) {
		draw->lights[k].on=FALSE;
		draw->lights[k].type=lt_normal;
		draw->lights[k].direction=ld_all;
		draw->lights[k].intensity=0;
		draw->lights[k].exponent=1.0f;
		draw->lights[k].col.r=draw->lights[k].col.g=draw->lights[k].col.b=1;
	}
	
		// halos
		
	draw->script_halo_idx=0;

	for (k=0;k!=max_model_halo;k++) {
		draw->halos[k].on=FALSE;
		draw->halos[k].min_dist=20*map_enlarge;
		draw->halos[k].max_dist=350*map_enlarge;
		draw->halos[k].min_size=500;
		draw->halos[k].max_size=200;
		draw->halos[k].min_alpha=0.8f;
		draw->halos[k].max_alpha=0.05f;
		draw->halos[k].no_clip_object=FALSE;
		draw->halos[k].no_clip_self=TRUE;
	}

		// shadow
		
	draw->shadow.on=FALSE;
	draw->shadow.mode=sm_normal;
	draw->shadow.always_in_air=FALSE;
	draw->shadow.static_in_air=FALSE;
	draw->shadow.static_face_angle=0;
	draw->shadow.static_size=0;
	draw->shadow.static_alpha=1.0f;

		// default normal

	draw->normal[0]=0.5f;
	draw->normal[1]=0.5f;
	draw->normal[2]=1.0f;
	
		// animate setup
		
	for (k=0;k!=max_model_blend_animation;k++) {
		draw->animations[k].mode=am_stopped;
		draw->setup.poses[k].idx_1=draw->setup.poses[k].idx_2=-1;
	}

		// mesh and textures

	draw->mesh_mask=0x1;				// default to first mesh
	
	for (k=0;k!=max_model_texture;k++) {
		draw->cur_texture_frame[k]=0;
	}

		// mesh fades

	for (k=0;k!=max_model_mesh;k++) {
		draw->mesh_fades[k].on=FALSE;
	}
}

/* =======================================================

      Object Size and Radius
      
======================================================= */

void object_set_radius(obj_type *obj)
{
	int			radius;
	
	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;
	
	obj->size.radius=radius>>1;
}

/* =======================================================

      Object Positions
      
======================================================= */

void object_set_spawn_mesh(obj_type *obj)
{
	obj->mesh.spawn_mesh_idx=map_mesh_find(&map,&obj->pnt);
	obj->mesh.cur_mesh_idx=obj->mesh.spawn_mesh_idx;
}

void object_set_position(obj_type *obj,int x,int y,int z,float ang_y,float ymove)
{
	d3pnt				*pnt;
	d3ang				*ang;
	obj_motion			*motion;
	
	pnt=&obj->pnt;
	
	pnt->x=x;
	pnt->y=y;
	pnt->z=z;
	
	memmove(&obj->last_pnt,pnt,sizeof(d3pnt));
	
	ang=&obj->ang;
	ang->x=0;
	ang->y=ang_y;
	ang->z=0;
	
	memmove(&obj->last_ang,ang,sizeof(d3ang));
	
	ang=&obj->view_ang;
	ang->x=0;
	ang->y=0;
	ang->z=0;
	
	motion=&obj->motion;
	
	motion->last_y_change=0;
	motion->ang.y=ang_y;
	motion->vct.y=ymove;
	
	obj->turn.ang_to.y=ang_y;
	
	obj->force.gravity=gravity_start_power;

	object_set_spawn_mesh(obj);
}

void object_stop(obj_type *obj)
{
    obj->forward_move.running=FALSE;
	
	object_clear_angle(&obj->turn.ang_add);
	object_clear_angle(&obj->turn.ang_to);
	object_clear_angle(&obj->turn.fix_ang_add);
    obj->look.ang_add=0;
	
	obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;
	
	obj->forward_move.speed=0;
	obj->side_move.speed=0;
	obj->vert_move.speed=0;

	obj->motion.last_y_change=0;
	
	obj->force.vct.x=obj->force.vct.z=obj->force.vct.y=0;
	
	obj->auto_walk.mode=aw_none;
	obj->auto_walk.node_seek_idx=obj->auto_walk.node_dest_idx=-1;
	
	obj->last_move_animation_event=-1;
	obj->last_turn_animation_event=-1;
	
	obj->thrust.vct.x=obj->thrust.vct.y=obj->thrust.vct.z=0.0f;
}   

/* =======================================================

      Create a New Object
      
======================================================= */

int object_reserve_uid(void)
{
	int				uid;

	uid=server.uid.obj;
	server.uid.obj++;

	return(uid);
}

obj_type* object_create(int bind,int reserve_uid)
{
	obj_type		*obj,*ptr;

		// only allow a maximum number of objects

	if (server.count.obj>=max_object) return(NULL);

		// create memory for new object

	ptr=(obj_type*)malloc(sizeof(obj_type)*(server.count.obj+1));
	if (ptr==NULL) return(NULL);

	if (server.objs!=NULL) {
		memmove(ptr,server.objs,(sizeof(obj_type)*server.count.obj));
		free(server.objs);
	}

	server.objs=ptr;

	obj=&server.objs[server.count.obj];
	server.count.obj++;

		// uid and bind

	obj->bind=bind;

	if (reserve_uid==-1) {
		obj->uid=object_reserve_uid();
	}
	else {
		obj->uid=reserve_uid;
	}

		// initialize object
	
	obj->spawn_idx=current_map_spawn_idx;
	current_map_spawn_idx++;
	
	obj->hidden=FALSE;
	obj->player=FALSE;
	obj->bot=FALSE;
	obj->suspend=FALSE;
	obj->scenery.on=FALSE;
	obj->fly=FALSE;
	obj->single_speed=FALSE;
	obj->side_step=TRUE;
	obj->quick_reverse=TRUE;
	obj->no_slide=FALSE;
	obj->crawl=TRUE;
	obj->open_doors=FALSE;
	obj->input_freeze=FALSE;
	obj->hide_all_weapons=FALSE;
	obj->find_on=TRUE;
	obj->contact.object_on=TRUE;
	obj->contact.projectile_on=TRUE;
	obj->contact.force_on=TRUE;
	obj->contact.pushable=FALSE;
	obj->hit_box.on=FALSE;
	obj->pickup.on=FALSE;
	obj->pickup.ignore=FALSE;
	obj->damage.on=FALSE;
	obj->damage.crushable=FALSE;
	obj->damage.invincible=FALSE;
	obj->damage.in_damage=FALSE;
	
	obj->input_mode=im_fpp;
	
	obj->in_collide_event=FALSE;
	obj->death_trigger=FALSE;
	
	obj->lock.x=obj->lock.y=obj->lock.z=FALSE;
	
	obj->stand_obj_uid=-1;
	obj->damage_obj_uid=-1;
	obj->on_ladder=FALSE;
	
	obj->name[0]=0x0;
	obj->team_idx=net_team_none;
	obj->spawn_spot_name[0]=0x0;
	
	obj->click.on=FALSE;
	obj->click.distance=map_enlarge*10;
	obj->click.crosshair_up_idx=-1;
	obj->click.crosshair_down_idx=-1;
	
	object_clear_size(&obj->size);
	object_clear_position(&obj->pnt);
	object_clear_angle(&obj->ang);
	object_clear_motion(&obj->motion);
	object_clear_force(&obj->force);
	object_clear_movement(&obj->forward_move);
    object_clear_movement(&obj->side_move);
	object_clear_movement(&obj->vert_move);
	object_stop(obj);

	obj->sight.side_angle=20.0f;
	obj->sight.look_angle=0.0f;
	obj->sight.side_division=3;
	obj->sight.look_division=1;
	obj->sight.distance=map_enlarge*150;

	obj->rigid_body.on=FALSE;
	obj->rigid_body.max_drop_y=map_enlarge*8;
	obj->rigid_body.draw_offset_y=0;
	obj->rigid_body.reset_factor_y=0.8f;
	obj->rigid_body.smooth_factor_y=0.2f;
	obj->rigid_body.max_ang_x=45.0f;
	obj->rigid_body.reset_factor_x=0.8f;
	obj->rigid_body.smooth_factor_x=0.2f;
	obj->rigid_body.max_ang_z=45.0f;
	obj->rigid_body.reset_factor_z=0.8f;
	obj->rigid_body.smooth_factor_z=0.2f;
	
    obj->bump.on=TRUE;
    obj->bump.high=450;
	obj->bump.smooth_factor=0.1f;
	obj->bump.smooth_offset=0;

	obj->slope_gravity=TRUE;
	
	obj->bounce.mesh_idx=-1;
	obj->bounce.factor=0.0f;
	
	obj->turn.ignore_mouse=FALSE;
	obj->turn.only_when_moving=FALSE;
	obj->turn.restrict_player_turning=FALSE;
	
	obj->thrust.speed=0.5f;
	obj->thrust.max_speed=60.0f;
	obj->thrust.vct.x=obj->thrust.vct.y=obj->thrust.vct.z=0.0f;
	obj->thrust.drag=FALSE;
	
	obj->look.ang_add=0;
	obj->look.up_angle=80;
	obj->look.down_angle=35;
	obj->look.effect_weapons=TRUE;
	
	obj->duck.on=TRUE;
    obj->duck.mode=dm_stand;
    obj->duck.y_move=0;
    obj->duck.y_add=16;
    obj->duck.y_change=300;
	
	obj->jump.on=TRUE;
    obj->jump.y_add=32;
	
	obj->kickback.size=100;

	obj->vehicle.on=FALSE;
	obj->vehicle.attach_obj_uid=-1;

	obj->radar.on=FALSE;
	obj->radar.icon[0]=0x0;
	obj->radar.icon_idx=-1;
	obj->radar.motion_only=FALSE;
	obj->radar.always_visible=FALSE;
	obj->radar.fade_start_tick=0;
	
	obj->status.health=obj->status.start_health=obj->status.max_health=100;
	obj->status.health_recover_tick=obj->status.health_recover_count=0;
	obj->status.health_recover_amount=1;
	
	object_clear_draw(&obj->draw);
	obj->ambient.on=FALSE;
    
    obj->held_weapon.mode=wm_held;
    obj->held_weapon.swap_tick=0;
	obj->held_weapon.current_uid=obj->held_weapon.next_uid=-1;
	obj->held_weapon.bounce_y=0;
	
	obj->crosshair_draw.on=FALSE;
	obj->crosshair_draw.alt_tick=0;

	obj->melee.strike_bone_tag=model_null_tag;
	obj->melee.strike_pose_name[0]=0x0;
	obj->melee.object_strike_bone_tag=model_null_tag;
	obj->melee.object_strike_pose_name[0]=0x0;
	obj->melee.radius=0;
	obj->melee.distance=0;
	obj->melee.damage=0;
	obj->melee.force=0;
	
	obj->count=0;
	obj->item_count=0;

	obj->turn.ang_add.y=0;
	obj->turn.fix_ang_add.y=0;
	obj->turn.ang_to.y=0;
	obj->turn.walk_speed=obj->turn.run_speed=obj->turn.crawl_speed=obj->turn.air_speed=1;
	obj->turn.walk_motion_speed=obj->turn.run_motion_speed=obj->turn.crawl_motion_speed=obj->turn.air_motion_speed=1;
	obj->turn.key_speed=1.8f;
	obj->turn.top_down_ang_offset=0.0f;

	obj->air_mode=am_ground;
    obj->liquid_mode=lm_out;

	obj->fall.dist=0;
	obj->fall.damage_minimum_height=map_enlarge*20;
	obj->fall.damage_factor=0.0f;
    obj->fall.change=FALSE;
	
	obj->last_move_animation_event=-1;
	obj->last_turn_animation_event=-1;

	obj->auto_walk.mode=aw_none;

	obj->fs_effect.tint.on=FALSE;
	obj->fs_effect.fade.on=FALSE;
	
	object_clear_contact(&obj->contact);
	object_clear_touch(&obj->touch);
	object_clear_hit(&obj->hit);
	object_clear_pickup(&obj->pickup);
	object_clear_watch(&obj->watch);

	object_clear_remote(&obj->remote);

	object_grow_clear(obj);
	
	obj->score.kill=obj->score.death=obj->score.suicide=obj->score.goal=obj->score.score=0;
	
    obj->spawning=TRUE;
	
	return(obj);
}

/* =======================================================

      Remote UID for Player
      
======================================================= */

void object_player_set_remote_uid(int remote_uid)
{
	obj_type			*obj;
	
	obj=object_find_uid(server.player_obj_uid);
	obj->remote.uid=remote_uid;
}

int object_player_get_remote_uid(void)
{
	obj_type			*obj;
	
	obj=object_find_uid(server.player_obj_uid);
	return(obj->remote.uid);
}

/* =======================================================

      Scripts for Objects
      
======================================================= */

bool object_start_script(obj_type *obj,char *name,char *params,char *err_str)
{
	obj->attach.thing_type=thing_type_object;
	obj->attach.thing_uid=obj->uid;

	scripts_clear_attach_data(&obj->attach);

	if (!scripts_add_console(&obj->attach,"Objects",name,params,err_str)) {
		obj->hidden=TRUE;			// hide objects if scripts fail to compile
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Reset Object to Original Spawn Spot
      
======================================================= */

void object_reset_prepare(obj_type *obj)
{
	memmove(&obj->org_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&obj->org_ang,&obj->ang,sizeof(d3ang));
}

void object_reset(obj_type *obj)
{
	memmove(&obj->pnt,&obj->org_pnt,sizeof(d3pnt));	
	memmove(&obj->ang,&obj->org_ang,sizeof(d3ang));
	obj->motion.ang.y=obj->turn.ang_to.y=obj->ang.y;
	
	object_set_spawn_mesh(obj);
}

/* =======================================================

      Attaching Crosshairs
      
======================================================= */

void object_attach_click_crosshair_up(obj_type *obj)
{
	obj->click.crosshair_up_idx=crosshair_find(obj->click.crosshair_up_name);
}

void object_attach_click_crosshair_down(obj_type *obj)
{
	obj->click.crosshair_down_idx=crosshair_find(obj->click.crosshair_down_name);
}

/* =======================================================

      Start Objects
      
======================================================= */

int object_start(spot_type *spot,bool player,int bind,int reserve_uid,char *err_str)
{
	int					n;
	bool				ok;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;

		// create object
		
	obj=object_create(bind,reserve_uid);
	if (obj==NULL) {
		strcpy(err_str,"Out of memory");
		return(-1);
	}
		
			// player default setup
		
	if (player) {
		strcpy(obj->name,setup.network.name);
		obj->team_idx=setup.network.team_idx;
		obj->spawn_spot_name[0]=0x0;
		
		obj->player=TRUE;
		obj->hidden=FALSE;

		server.player_obj_uid=obj->uid;
	}

		// regular object setup

	else {
		strcpy(obj->name,spot->attach_name);
		strcpy(obj->type,spot->attach_type);
		
			// special check for bots

		obj->bot=(strcasecmp(obj->type,"bot")==0);

			// if there's an editor display model, then
			// default model to it
			
		if (spot->display_model[0]!=0x0) {
			obj->draw.on=TRUE;
			strcpy(obj->draw.name,spot->display_model);
		}

			// attach object to spot

		object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
		obj->turn.ang_to.y=spot->ang.y;
		
		object_reset_prepare(obj);
	}

		// if networked player, run rules
		// and send choosen team to other clients
	
	if (((player) || (obj->bot)) && (net_setup.client.joined)) {
		game_obj_rule_uid=obj->uid;
		scripts_post_event_console(&js.game_attach,sd_event_rule,sd_event_rule_join,0);
		game_obj_rule_uid=-1;

		net_client_send_set_team(obj->remote.uid,obj->team_idx);
	}
		
		// start script

	if (player) {
		ok=object_start_script(obj,"Player",NULL,err_str);
	}
	else {
		ok=object_start_script(obj,spot->attach_script,spot->attach_params,err_str);
	}

	if (!ok) return(-1);

		// load object model

	model_load_and_init(&obj->draw);

		// start weapons

	weap=server.weapons;

    for (n=0;n!=server.count.weapon;n++) {
		if (weap->obj_uid==obj->uid) weapon_start(weap);
		weap++;
    }

		// start projectiles

	proj_setup=server.proj_setups;

	for (n=0;n!=server.count.proj_setup;n++) {
		if (proj_setup->obj_uid==obj->uid) proj_setup_start(proj_setup);
		proj_setup++;
    }

		// setup held weapon

	if (obj->held_weapon.current_uid!=-1) {
		weap=weapon_find_uid(obj->held_weapon.current_uid);
		weapon_set(obj,weap);
	}

	return(obj->uid);
}

/* =======================================================

      Dispose Objects
      
======================================================= */

void object_dispose_single(int idx)
{
	int					n;
	obj_type			*obj,*ptr;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;

	obj=&server.objs[idx];

		// dispose projectiles

	n=0;
	
	while (n<server.count.proj_setup) {
		proj_setup=&server.proj_setups[n];
		
 		if (proj_setup->obj_uid==obj->uid) {
			proj_setup_dispose(n);
		}
		else {
			n++;
		}
		
		if (server.count.proj_setup==0) break;
	}

		// dispose weapons

	weap=server.weapons;
	
	n=0;
	
	while (n<server.count.weapon) {
		weap=&server.weapons[n];

		if (weap->obj_uid==obj->uid) {
			weapon_dispose(n);
		}
		else {
			n++;
		}

		if (server.count.weapon==0) break;
	}

		// dispose object

	scripts_dispose(obj->attach.script_uid);
	models_dispose(obj->draw.uid);

		// is the list completely empty?

	if (server.count.obj==1) {
		free(server.objs);
		server.objs=NULL;
		server.count.obj=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(obj_type*)malloc(sizeof(obj_type)*(server.count.obj-1));

	if (ptr==NULL) {
		if (idx<(server.count.obj-1)) {
			memmove(&server.objs[idx],&server.objs[idx+1],(sizeof(obj_type)*((server.count.obj-idx)-1)));
		}
	}
	else {

		if (idx>0) {
			memmove(ptr,server.objs,(sizeof(obj_type)*idx));
		}
		if (idx<(server.count.obj-1)) {
			memmove(&ptr[idx],&server.objs[idx+1],(sizeof(obj_type)*((server.count.obj-idx)-1)));
		}

		free(server.objs);
		server.objs=ptr;
	}
	
	server.count.obj--;
}

void object_dispose_2(int bind)
{
	int				i;
	obj_type		*obj;
	
		// delete bound objects
	
	i=0;
	
	while (i<server.count.obj) {
		obj=&server.objs[i];
		
		if (obj->bind!=bind) {
			i++;
			continue;
		}
		
		object_dispose_single(i);

		if (server.count.obj==0) break;
	}
}

/* =======================================================

      Spot Spawning
      
======================================================= */

void spot_start_attach(void)
{
	int					n;
	char				err_str[256];
	bool				multiplayer;
	spot_type			*spot;
	
	multiplayer=(net_setup.host.hosting) || (net_setup.client.joined);
	
		// check if a spot was attached by a
		// script.  If it was, and the skill levels
		// and spawn type are OK, spawn this object into the map
		
	for (n=0;n!=map.nspot;n++) {
	
		spot=&map.spots[n];
		if (!spot->attach) continue;
		if (spot->skill>server.skill) continue;
		if ((spot->spawn==spawn_single_player_only) && (multiplayer)) continue;
		if ((spot->spawn==spawn_multiplayer_only) && (!multiplayer)) continue;
		 
		object_start(spot,FALSE,bt_map,-1,err_str);
	}
}

void spot_add_multiplayer_bots(void)
{
	int				n,spot_idx;
	char			err_str[256];
	spot_type		spot;

		// only spawn on hosts

	if (!net_setup.host.hosting) return;

		// spawn

	for (n=0;n!=setup.network.bot.count;n++) {
	
		if (hud.bot_names.names[n][0]!=0x0) {
			strcpy(spot.attach_name,hud.bot_names.names[n]);
		}
		else {
			sprintf(spot.attach_name,"Bot %d",(n+1));
		}
		
		strcpy(spot.attach_type,"Bot");
		strcpy(spot.attach_script,"Bot");
		spot.attach_params[0]=0x0;
		
		spot_idx=map_find_random_spot(&map,"Spawn","Spawn");
		if (spot_idx==-1) break;
		
		memmove(&spot.pnt,&map.spots[spot_idx].pnt,sizeof(d3pnt));
		memmove(&spot.ang,&map.spots[spot_idx].ang,sizeof(d3ang));
		
		object_start(&spot,FALSE,bt_map,-1,err_str);
	}
}

/* =======================================================

      Script Object Spawn/Remove

	  Spawning can make changes in the obj list, throwing
	  off any obj pointers and causing a crash.  Because of
	  this, we need to make sure all script spawns are
	  cached and then done after everything else
      
======================================================= */

void object_script_spawn_start(void)
{
	ndelayed_obj_spawn=0;
}

void object_script_spawn_finish(void)
{
	int						n,idx,uid;
	char					err_str[256],obj_err_str[256];
	spot_type				spot;
	obj_type				*obj;
	delayed_obj_spawn_type	*spawn;

		// do all the adds

	spawn=delayed_obj_spawns;

	for (n=0;n!=ndelayed_obj_spawn;n++) {

		if (!spawn->dispose) {

				// create fake spot

			bzero(&spot,sizeof(spot_type));

			strcpy(spot.attach_name,spawn->name);
			strcpy(spot.attach_type,spawn->type);
			strcpy(spot.attach_script,spawn->script);
			strcpy(spot.attach_params,spawn->params);

			memmove(&spot.pnt,&spawn->pnt,sizeof(d3pnt));
			memmove(&spot.ang,&spawn->ang,sizeof(d3ang));

				// start object

			uid=object_start(&spot,FALSE,bt_map,spawn->uid,obj_err_str);
			if (uid==-1) {
				sprintf(err_str,"Object Spawn Failed: %s",obj_err_str);
				console_add_error(err_str);
			}

				// hide object

			if (spawn->hide) {
				obj=object_find_uid(uid);

				obj->hidden=TRUE;
				obj->contact.object_on=FALSE;
				obj->contact.projectile_on=FALSE;
				obj->contact.force_on=FALSE;
			}

		}
		
		spawn++;
	}

		// do all the removes

	spawn=delayed_obj_spawns;

	for (n=0;n!=ndelayed_obj_spawn;n++) {

		if (spawn->dispose) {

			idx=object_find_index_uid(spawn->uid);
			if (idx==-1) {
				sprintf(err_str,"No object exists with ID: %d",spawn->uid);
				console_add_error(err_str);
			}
			else {
				object_dispose_single(idx);
			}

		}
		
		spawn++;
	}

}

int object_script_spawn(char *name,char *type,char *script,char *params,d3pnt *pnt,d3ang *ang,bool hide)
{
	delayed_obj_spawn_type	*spawn;

		// room on list?

	if ((ndelayed_obj_spawn>=max_delayed_obj_spawn) || (delayed_obj_spawns==NULL)) {
		JS_ReportError(js.cx,"Not enough memory to spawn object");
		return(-1);
	}

		// add to list

	spawn=&delayed_obj_spawns[ndelayed_obj_spawn];
	ndelayed_obj_spawn++;

	spawn->uid=object_reserve_uid();
	spawn->dispose=FALSE;

	strcpy(spawn->name,name);
	strcpy(spawn->type,type);
	strcpy(spawn->script,script);
	strcpy(spawn->params,params);
	memmove(&spawn->pnt,pnt,sizeof(d3pnt));
	memmove(&spawn->ang,ang,sizeof(d3ang));
	spawn->hide=hide;

		// return uid

	return(spawn->uid);
}

bool object_script_remove(int uid)
{
	delayed_obj_spawn_type	*spawn;

		// can not dispose player object

	if (uid==server.player_obj_uid) {
		JS_ReportError(js.cx,"Can not dispose player object");
		return(FALSE);
	}

		// add to list

	spawn=&delayed_obj_spawns[ndelayed_obj_spawn];
	ndelayed_obj_spawn++;

	spawn->uid=uid;
	spawn->dispose=TRUE;

	return(TRUE);
}
