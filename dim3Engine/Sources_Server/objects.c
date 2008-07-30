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

extern int					current_map_spawn_idx;

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern js_type				js;

int							game_obj_rule_uid=-1;

/* =======================================================

      Initialize Object List
      
======================================================= */

void object_initialize_list(void)
{
	server.count.obj=0;
	server.uid.obj=0;
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
	memset(watch->obj_flags,0x0,max_object);
}

void object_clear_draw(model_draw *draw)
{
	int				k;
	
		// model draw
		
	draw->on=FALSE;
	draw->uid=-1;
	draw->name[0]=0x0;
	draw->lit_type=ml_flat;
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
		draw->lights[k].intensity=0;
		draw->lights[k].col.r=draw->lights[k].col.g=draw->lights[k].col.b=1;
	}

	draw->hilite.r=draw->hilite.g=draw->hilite.b=1;
	
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

      Object Radius
      
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

void object_set_position(obj_type *obj,int x,int z,int y,float ang_y,float ymove)
{
	d3pos				*pos;
	d3ang				*ang;
	obj_motion			*motion;
	
	pos=&obj->pos;
	
	pos->x=x;
	pos->z=z;
	pos->y=y;
	
	map_find_portal_by_pos(&map,pos);
	
	ang=&obj->ang;
	ang->x=0;
	ang->y=ang_y;
	ang->z=0;
	
	ang=&obj->view_ang;
	ang->x=0;
	ang->y=0;
	ang->z=0;
	
	motion=&obj->motion;
	
	motion->last_y_change=0;
	motion->ang.y=ang_y;
	motion->vct.y=ymove;
	
	obj->force.gravity=gravity_start_power;
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
}   

/* =======================================================

      Create a New Object
      
======================================================= */

obj_type* object_create(int bind)
{
	obj_type		*obj;
	
	if (server.count.obj>=max_object) return(NULL);
	
	obj=&server.objs[server.count.obj];
	server.count.obj++;
	
	obj->uid=server.uid.obj;
	server.uid.obj++;
	
	obj->bind=bind;
	
	obj->spawn_idx=current_map_spawn_idx;
	current_map_spawn_idx++;
	
	obj->hidden=FALSE;
	obj->player=FALSE;
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
    obj->bump.high=map_enlarge*3;
	obj->bump.smooth_factor=0.1f;
	obj->bump.smooth_offset=0;

	obj->slope_gravity=TRUE;
	
	obj->bounce.mesh_idx=-1;
	obj->bounce.factor=0.0f;
	
	obj->turn.ignore_mouse=FALSE;
	obj->turn.only_when_moving=FALSE;
	obj->turn.restrict_player_turning=FALSE;
	
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

	obj->air_mode=am_ground;
    obj->liquid_mode=lm_out;

	obj->fall.dist=0;
	obj->fall.damage_minimum_height=map_enlarge*20;
	obj->fall.damage_factor=0.0f;
    obj->fall.change=FALSE;
	
	obj->last_move_animation_event=-1;
	obj->last_turn_animation_event=-1;

	obj->auto_walk.mode=aw_none;
	obj->auto_walk.node_slop=map_enlarge*5;
	
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

      Scripts for Objects
      
======================================================= */

bool object_start_script(obj_type *obj,char *name,char *params,char *err_str)
{
	obj->attach.thing_type=thing_type_object;
	obj->attach.thing_uid=obj->uid;

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

int object_start(spot_type *spot,bool player,int bind,char *err_str)
{
	int					n;
	bool				ok;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
		// create object
		
	obj=object_create(bind);
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

			// if there's an editor display model, then
			// default model to it
			
		if (spot->display_model[0]!=0x0) {
			obj->draw.on=TRUE;
			strcpy(obj->draw.name,spot->display_model);
		}

			// attach object to spot

		object_set_position(obj,spot->pnt.x,spot->pnt.z,spot->pnt.y,spot->ang.y,0);
		obj->turn.ang_to.y=spot->ang.y;
		
		object_reset_prepare(obj);
	}

		// if networked player, run rules
		// and send choosen team to other clients
	
	if ((player) && (net_setup.client.joined)) {
		game_obj_rule_uid=obj->uid;
		scripts_post_event_console(&js.game_attach,sd_event_rule,sd_event_rule_join,0);
		game_obj_rule_uid=-1;

		net_client_send_set_team(net_setup.client.remote_uid,obj->team_idx);
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
	obj_type			*obj;
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

		// remove from list

	if (idx<(server.count.obj-1)) {
		memmove(&server.objs[idx],&server.objs[idx+1],(sizeof(obj_type)*((server.count.obj-idx)-1)));
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
	int					i;
	char				err_str[256];
	spot_type			*spot;
	
		// check if a spot was attached by a
		// script.  If it was, and the skill levels
		// are OK, spawn this object into the map
		
	spot=map.spots;
	
	for (i=0;i!=map.nspot;i++) {
		if ((spot->attach) && (spot->skill<=server.skill)) object_start(spot,FALSE,bt_map,err_str);
		spot++;
	}
}

/* =======================================================

      Script Object Spawn/Remove
      
======================================================= */

int object_script_spawn(char *name,char *type,char *script,char *params,d3pnt *pnt,d3ang *ang)
{
	int					uid;
	char				err_str[256];
	spot_type			spot;

		// create fake spot

	bzero(&spot,sizeof(spot_type));

	strcpy(spot.attach_name,name);
	strcpy(spot.attach_type,type);
	strcpy(spot.attach_script,script);
	strcpy(spot.attach_params,params);

	memmove(&spot.pnt,pnt,sizeof(d3pnt));
	memmove(&spot.ang,ang,sizeof(d3ang));

		// start object

	uid=object_start(&spot,FALSE,bt_map,err_str);
	if (uid==-1) {
		JS_ReportError(js.cx,"Object Spawn Failed: %s",err_str);
		return(FALSE);
	}

	return(uid);
}

bool object_script_remove(int uid)
{
	int				idx;

		// can not dispose player object

	if (uid==server.player_obj_uid) {
		JS_ReportError(js.cx,"Can not dispose player object");
		return(FALSE);
	}

		// dispose

	idx=object_find_index_uid(uid);
	if (idx==-1) {
		JS_ReportError(js.cx,"No object exists with ID: %d",uid);
		return(FALSE);
	}
		
	object_dispose_single(idx);

	return(TRUE);
}
