/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Remote Player Objects

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
#include "models.h"
#include "physics.h"
#include "sounds.h"
#include "interfaces.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern hud_type				hud;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern char					setup_team_color_list[][32];
extern bool					game_loop_quit;

float						team_color_tint[net_team_count][3]=net_team_color_tint_def;

extern int game_time_get(void);
extern void chat_add_message(int tick,char *name,char *str,d3col *col);
extern bool game_start(int skill,int remote_count,network_request_remote_add *remotes,char *err_str);
extern void game_end(void);
extern bool map_start(bool skip_media,char *err_str);
extern void map_end(void);
extern void mesh_triggers(obj_type *obj,int old_mesh_idx,int mesh_idx);

/* =======================================================

      Add and Remove Remote Object
      
======================================================= */

bool remote_add(network_request_remote_add *add,bool send_event)
{
	int					n,uid;
	char				err_str[256];
	obj_type			*obj,*player_obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
    
		// create new object
		
	obj=object_create(bt_remote,-1);
    if (obj==NULL) return(FALSE);
	
		// setup remote
		
	strcpy(obj->name,add->name);
	strcpy(obj->type,"Remote");

	obj->team_idx=(signed short)ntohs(add->team_idx);
	obj->player=TRUE;
	
	obj->pnt.x=ntohl(add->pnt_x);
	obj->pnt.y=ntohl(add->pnt_y);
	obj->pnt.z=ntohl(add->pnt_z);

	obj->remote.on=TRUE;
	obj->remote.uid=(signed short)ntohs(add->uid);
	obj->remote.last_update=game_time_get();
	obj->remote.talking=FALSE;
	
	obj->bind=bt_remote;
	
		// start remote scripts
		// remember uid as object list might change

	uid=obj->uid;

	object_start_script(obj,"Player",NULL,err_str);

	obj=object_find_uid(uid);

		// load models
		
	model_load_and_init(&obj->draw);

	weap=server.weapons;
		
	for (n=0;n!=server.count.weapon;n++) {
		if (weap->obj_uid==obj->uid) weapon_start(weap);
		weap++;
	}
				
	proj_setup=server.proj_setups;
	
	for (n=0;n!=server.count.proj_setup;n++) {
		if (proj_setup->obj_uid==obj->uid) proj_setup_start(proj_setup);
		proj_setup++;
	}
	
		// initial score
		
	obj->score.score=(signed short)ntohs(add->score);
	
		// start remotes hidden
		
	obj->hidden=TRUE;

		// send event to player

	if (send_event) {
		player_obj=object_find_uid(server.player_obj_uid);
		scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_join,obj->uid);
	}
	
	return(TRUE);
}

void remote_team(int remote_uid,network_request_team *team,bool send_event)
{
	obj_type			*obj,*player_obj;
	
		// change team

	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;

	obj->team_idx=(signed short)ntohs(team->team_idx);

		// send event to player

	if (send_event) {
		player_obj=object_find_uid(server.player_obj_uid);
		scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_team,obj->uid);
	}
}

void remote_remove(int remote_uid,bool send_event)
{
	int					idx;
	obj_type			*player_obj;
	
		// find remote index
		
	idx=object_find_index_remote_uid(remote_uid);
	if (idx==-1) return;

		// send event to player
		// do it before dispose so player can
		// read the object if it wants to

	if (send_event) {
		player_obj=object_find_uid(server.player_obj_uid);
		scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_leave,server.objs[idx].uid);
	}
	
		// remove the obj
		
	object_dispose_single(idx);
}

/* =======================================================

      Remote Team Colors
      
======================================================= */

void remote_get_tint(obj_type *obj,d3col *tint)
{
	int			team_color_idx;

	team_color_idx=obj->team_idx;

	if ((!obj->remote.on) || (team_color_idx==-1)) {
		tint->r=tint->g=tint->b=1.0f;
		return;
	}

	tint->r=team_color_tint[team_color_idx][0];
	tint->g=team_color_tint[team_color_idx][1];
	tint->b=team_color_tint[team_color_idx][2];
}

void remote_get_ui_color(obj_type *obj,d3col *col)
{
	int			team_color_idx;

	team_color_idx=obj->team_idx;

	if ((!obj->remote.on) || (team_color_idx==-1)) {
		col->r=col->g=col->b=0.7f;
		return;
	}

	col->r=team_color_tint[team_color_idx][0];
	col->g=team_color_tint[team_color_idx][1];
	col->b=team_color_tint[team_color_idx][2];
}

/* =======================================================

      Remote Timeouts
      
======================================================= */

bool remote_timed_out(obj_type *obj)
{
	return((game_time_get()-obj->remote.last_update)>client_communication_timeout_msec);
}

bool remote_slow(obj_type *obj)
{
	return((game_time_get()-obj->remote.last_update)>client_communication_slow_msec);
}

/* =======================================================

      Predict Remote Objects
      
======================================================= */

void remote_update_current_mesh(obj_type *obj)
{
	int				old_mesh_idx;
	
		// reset current mesh
		
	old_mesh_idx=obj->mesh.cur_mesh_idx;
	obj->mesh.cur_mesh_idx=map_mesh_find(&map,&obj->pnt);
	
		// handle any triggers
		
	if (old_mesh_idx!=obj->mesh.cur_mesh_idx) mesh_triggers(obj,old_mesh_idx,obj->mesh.cur_mesh_idx);
}

void remote_predict_move(obj_type *obj)
{
		// stop predicting after timeout
		
	if (remote_timed_out(obj)) return;
	
		// if slow, then reduce movement
		
	if (remote_slow(obj)) {
		obj->motion.vct.x*=client_predict_slow_reduction;
		obj->motion.vct.y*=client_predict_slow_reduction;
		obj->motion.vct.z*=client_predict_slow_reduction;
		obj->turn.fix_ang_add.y*=client_predict_slow_reduction;
	}
	
		// predict rotations
	
    obj->ang.y=angle_add(obj->ang.y,obj->turn.fix_ang_add.y);
	
		// predict movements
		
	object_move_remote(obj);
	remote_update_current_mesh(obj);
}

/* =======================================================

      Remote Host Resets and Exits
      
======================================================= */

void remote_host_reset(void)
{
	int							remote_uid,remote_count;
	char						game_name[name_str_len],map_name[name_str_len],
								deny_reason[64],err_str[256];
	network_request_remote_add	remotes[host_max_remote_count];
	
		// quit current game
		
	map_end();
	game_end();
		
		// attempt to join to new game

	if (!net_client_join_host_start(net_setup.client.joined_ip,setup.network.name,&remote_uid,game_name,map_name,deny_reason,&remote_count,remotes)) {
		error_open("Unable to rejoin server after game reset","Network Game Canceled");
		return;
	}
	
		// mark remote and joined
		
	net_setup.client.joined=TRUE;
	net_setup.client.remote_uid=remote_uid;
	net_setup.client.latency=0;

		// setup game play type

	net_setup.game_idx=net_client_find_game(game_name);
	if (net_setup.game_idx==-1) {
		net_client_send_leave_host(net_setup.client.remote_uid);
		net_client_end_message_queue();
		net_client_join_host_end();
		sprintf(err_str,"Could not find game type: %s",game_name);
		error_open(err_str,"Network Game Canceled");
		return;	
	}

	map.info.name[0]=0x0;
	strcpy(map.info.host_name,map_name);
	
		// start the new game
	
	if (!game_start(skill_medium,remote_count,remotes,err_str)) {
		net_client_send_leave_host(net_setup.client.remote_uid);
		net_client_end_message_queue();
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;	
	}
	
		// start the map
		
	if (!map_start(FALSE,err_str)) {
		net_client_send_leave_host(net_setup.client.remote_uid);
		net_client_end_message_queue();
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;	
	}
		
		// game is running
	
	server.state=gs_running;
}

void remote_host_exit(void)
{
		// quit current game
		
	map_end();
	game_end();

		// set error page

	error_open("The network server has been shutdown or stopped responding","Network Game Canceled");
}

/* =======================================================

      Remote Updates
      
======================================================= */

void remote_update(int remote_uid,network_request_remote_update *update)
{
	int							n,flags,map_spawn_idx,
								animation_mode,animate_idx,animate_next_idx;
	d3pnt						org_pnt;
	obj_type					*obj;
	model_draw					*draw;
	model_draw_animation		*animation;
	network_request_animation	*net_animation;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;
	
		// check for vehicles
		
	map_spawn_idx=(signed short)ntohs(update->vehicle_map_spawn_idx);
	if (map_spawn_idx!=-1) {
		
			// hide original object
			
		obj->hidden=TRUE;
		obj->contact.object_on=FALSE;
		obj->contact.projectile_on=FALSE;
		obj->contact.force_on=FALSE;
		
			// switch to vehicle
			
		obj=object_find_spawn_idx(map_spawn_idx);
		if (obj==NULL) return;
	}
	
		// update position

	memmove(&org_pnt,&obj->pnt,sizeof(d3pnt));
	
	obj->pnt.x=ntohl(update->pnt_x);
	obj->pnt.y=ntohl(update->pnt_y);
	obj->pnt.z=ntohl(update->pnt_z);
				
	obj->ang.x=ntohf(update->fp_ang_x);
	obj->ang.y=ntohf(update->fp_ang_y);
	obj->ang.z=ntohf(update->fp_ang_z);

		// update predicition values
		
	obj->motion.vct.x=ntohf(update->fp_move_vct_x);
	obj->motion.vct.y=ntohf(update->fp_move_vct_y);
	obj->motion.vct.z=ntohf(update->fp_move_vct_z);

	obj->turn.fix_ang_add.x=obj->turn.fix_ang_add.z=0.0f;
	obj->turn.fix_ang_add.y=ntohf(update->fp_turn_ang_add_y);
	
		// update animations
		// only change animations if mode, animation, or next
		// animation are changing
		
	draw=&obj->draw;
	animation=draw->animations;
	net_animation=update->animation;

	for (n=0;n!=max_model_blend_animation;n++) {
	
		animation_mode=(signed short)ntohs(net_animation->model_mode);
		animate_idx=(signed short)ntohs(net_animation->model_animate_idx);
		animate_next_idx=(signed short)ntohs(net_animation->model_animate_next_idx);
		
		if ((animation_mode!=animation->mode) || (animation->animate_idx!=animate_idx) || (animation->animate_next_idx!=animate_next_idx)) {
			animation->tick=ntohl(net_animation->model_tick)+game_time_get();
			animation->mode=animation_mode;
			animation->animate_idx=animate_idx;
			animation->animate_next_idx=animate_next_idx;
			animation->pose_move_idx=(signed short)ntohs(net_animation->model_pose_move_idx);
			animation->smooth_animate_idx=(signed short)ntohs(net_animation->model_smooth_animate_idx);
			animation->smooth_pose_move_idx=(signed short)ntohs(net_animation->model_smooth_pose_move_idx);
		}
		
		animation++;
		net_animation++;
	}			
		
	draw->mesh_mask=ntohl(update->model_mesh_mask);
	memmove(&draw->cur_texture_frame,update->model_cur_texture_frame,max_model_texture);
	
		// update flags
	
	flags=ntohl(update->flags);
		
	obj->hidden=((flags&net_update_flag_hidden)!=0);
	obj->contact.object_on=!((flags&net_update_flag_no_contact_object)!=0);
	obj->contact.projectile_on=!((flags&net_update_flag_no_contact_projectile)!=0);
	obj->contact.force_on=!((flags&net_update_flag_no_contact_force)!=0);
	obj->remote.talking=((flags&net_update_flag_talking)!=0);
	
		// update status
		
	obj->score.score=(signed short)ntohs(update->score);
	obj->status.health=(signed short)ntohs(update->health);
	
		// last update tick
		
	obj->remote.last_update=game_time_get();
	
		// update current mesh and handle
		// triggers
		
	remote_update_current_mesh(obj);
}

void remote_death(int remote_uid,network_request_remote_death *death)
{
	int					kill_remote_uid,telefrag_remote_uid;
	bool				telefrag;
	obj_type			*obj,*kill_obj,*telefrag_obj;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;
	
		// set health to obj
		// this stops dead remotes from picking things up
		
	obj->status.health=0;

		// normal death
		
	telefrag=(ntohs(death->telefrag)!=0);

	if (!telefrag) {
		
			// get killing remote uid
			
		kill_remote_uid=(signed short)ntohs(death->kill_remote_uid);
		
			// find kill object
			
		obj->damage_obj_uid=-1;
		
		if (kill_remote_uid!=-1) {
			if (kill_remote_uid==net_setup.client.remote_uid) {
				obj->damage_obj_uid=server.player_obj_uid;			// killed by player
			}
			else {
				kill_obj=object_find_remote_uid(kill_remote_uid);
				if (kill_obj!=NULL) obj->damage_obj_uid=kill_obj->uid;
			}
		}
	}

		// telefrag death

	else {
		telefrag_remote_uid=(signed short)ntohs(death->kill_remote_uid);

		if (telefrag_remote_uid==net_setup.client.remote_uid) {
			telefrag_obj=object_find_uid(server.player_obj_uid);
			object_telefrag(telefrag_obj,obj);
		}
	}
		
		// change the score
		
	object_score_death(obj);
	
		// send alert to all objects with watches on

	object_watch_death_alert(obj);
}

/* =======================================================

      Remote Chats and Sounds
      
======================================================= */

void remote_chat(int remote_uid,network_request_remote_chat *chat)
{
	d3col				col;
	obj_type			*obj;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;
	
		// update chat

	remote_get_ui_color(obj,&col);
	chat_add_message(game_time_get(),obj->name,chat->str,&col);
}

void remote_sound(int remote_uid,network_request_remote_sound *sound)
{
	int					x,y,z,buffer_idx;
	float				pitch;
	obj_type			*obj;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;
	
	x=ntohl(sound->pnt_x);
	y=ntohl(sound->pnt_y);
	z=ntohl(sound->pnt_z);
	
	pitch=ntohf(sound->fp_pitch);
	
	buffer_idx=al_find_buffer(sound->name);
	if (buffer_idx!=-1) al_play_source(buffer_idx,x,y,z,pitch,FALSE,FALSE,FALSE,FALSE);
}

/* =======================================================

      Remote Projectiles, Hit Scans, and Melees
      
======================================================= */

void remote_projectile_add(int remote_uid,network_request_remote_fire *proj_add)
{
	d3pnt				pt;
	d3ang				ang;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;
	
	weap=weapon_find_name(obj,proj_add->weap_name);
	if (weap==NULL) return;
	
	proj_setup=find_proj_setups(weap,proj_add->proj_setup_name);
	if (proj_setup==NULL) return;

	pt.x=ntohl(proj_add->pt_x);
	pt.y=ntohl(proj_add->pt_y);
	pt.z=ntohl(proj_add->pt_z);
	
	ang.x=ntohf(proj_add->fp_ang_x);
	ang.y=ntohf(proj_add->fp_ang_y);
	ang.z=ntohf(proj_add->fp_ang_z);

	weapon_add_projectile(game_time_get(),obj,weap,proj_setup,&pt,&ang);
}

void remote_hit_scan_add(int remote_uid,network_request_remote_fire *hitscan_add)
{
	d3pnt				pt;
	d3ang				ang;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;
	
	weap=weapon_find_name(obj,hitscan_add->weap_name);
	if (weap==NULL) return;
	
	proj_setup=find_proj_setups(weap,hitscan_add->proj_setup_name);
	if (proj_setup==NULL) return;
				
	pt.x=ntohl(hitscan_add->pt_x);
	pt.y=ntohl(hitscan_add->pt_y);
	pt.z=ntohl(hitscan_add->pt_z);
	
	ang.x=ntohf(hitscan_add->fp_ang_x);
	ang.y=ntohf(hitscan_add->fp_ang_y);
	ang.z=ntohf(hitscan_add->fp_ang_z);
	
	projectile_hitscan(game_time_get(),obj,weap,proj_setup,&pt,&ang);
}

void remote_melee_add(int remote_uid,network_request_remote_fire *rem_melee)
{
	d3pnt				pt;
	d3ang				ang;
	obj_type			*obj;
	weapon_type			*weap;
	melee_type			melee;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;
	
	weap=NULL;
	if (rem_melee->weap_name[0]!=0x0) weap=weapon_find_name(obj,rem_melee->weap_name);
	
	pt.x=ntohl(rem_melee->pt_x);
	pt.y=ntohl(rem_melee->pt_y);
	pt.z=ntohl(rem_melee->pt_z);
	
	ang.x=ntohf(rem_melee->fp_ang_x);
	ang.y=ntohf(rem_melee->fp_ang_y);
	ang.z=ntohf(rem_melee->fp_ang_z);
	
	melee.radius=(int)ntohs(rem_melee->radius);
	melee.distance=(int)ntohs(rem_melee->distance);
	melee.damage=(int)ntohs(rem_melee->damage);
	melee.force=(int)ntohs(rem_melee->force);
	
	melee_add(obj,weap,&pt,&ang,&melee,obj->uid);
}

void remote_fire(int remote_uid,network_request_remote_fire *fire)
{
	int				fire_type;

	fire_type=(int)ntohs(fire->fire_type);

	switch (fire_type) {

		case net_remote_fire_type_projectile:
			remote_projectile_add(remote_uid,fire);
			break;

		case net_remote_fire_type_hit_scan:
			remote_hit_scan_add(remote_uid,fire);
			break;

		case net_remote_fire_type_melee:
			remote_melee_add(remote_uid,fire);
			break;
	}
}

/* =======================================================

      Remote Pickup
      
======================================================= */

void remote_pickup(int remote_uid,network_request_remote_pickup *pickup)
{
	int					n,idx;
	d3pnt				org_pnt;
	obj_type			*obj;
	weapon_type			*weap;
	
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;

		// make sure pickup is at proper point

	memmove(&org_pnt,&obj->pnt,sizeof(d3pnt));
	
	obj->pnt.x=ntohl(pickup->pt_x);
	obj->pnt.y=ntohl(pickup->pt_y);
	obj->pnt.z=ntohl(pickup->pt_z);

		// setup health, ammo as they
		// could effect pickups

	obj->status.health=(signed short)ntohs(pickup->health);

	idx=0;
	weap=server.weapons;
		
	for (n=0;n!=server.count.weapon;n++) {

		if (weap->obj_uid==obj->uid) {
			weap->ammo.count=(signed short)ntohs(pickup->ammos[idx].ammo_count);
			weap->ammo.clip_count=(signed short)ntohs(pickup->ammos[idx].clip_count);
			weap->alt_ammo.count=(signed short)ntohs(pickup->ammos[idx].alt_ammo_count);
			weap->alt_ammo.clip_count=(signed short)ntohs(pickup->ammos[idx].alt_clip_count);

			idx++;
			if (idx==net_max_weapon_per_remote) break;
		}

		weap++;
	}

		// run pickup

	item_pickup_check(obj);

		// restore point

	memmove(&obj->pnt,&org_pnt,sizeof(d3pnt));
}

/* =======================================================

      Remote Networking Receives
      
======================================================= */

bool remote_network_get_updates(int tick)
{
	int										action,from_remote_uid,count;
	unsigned char							data[net_max_msg_size];

	count=0;
	
	while (count<client_message_per_loop_count) {
	
			// check for messages

		if (!net_client_check_message_queue(&action,&from_remote_uid,data)) return(TRUE);
		
			// run message
		
		switch (action) {
		
			case net_action_request_host_reset:
				remote_host_reset();
				break;
				
			case net_action_request_team:
				remote_team(from_remote_uid,(network_request_team*)data,TRUE);
				break;

			case net_action_request_remote_add:
				remote_add((network_request_remote_add*)data,TRUE);
				break;
				
			case net_action_request_remote_remove:
				remote_remove(from_remote_uid,TRUE);
				break;
			
			case net_action_request_remote_update:
				remote_update(from_remote_uid,(network_request_remote_update*)data);
				break;
				
			case net_action_request_remote_death:
				remote_death(from_remote_uid,(network_request_remote_death*)data);
				break;
				
			case net_action_request_remote_chat:
				remote_chat(from_remote_uid,(network_request_remote_chat*)data);
				break;
				
			case net_action_request_remote_sound:
				remote_sound(from_remote_uid,(network_request_remote_sound*)data);
				break;

			case net_action_request_remote_fire:
				remote_fire(from_remote_uid,(network_request_remote_fire*)data);
				break;

			case net_action_request_remote_pickup:
				remote_pickup(from_remote_uid,(network_request_remote_pickup*)data);
				break;

			case net_action_reply_latency_ping:
				net_setup.client.latency=(tick-net_setup.client.latency_ping_tick)>>1;		// latency is half of round trip as client-client
				break;

			case net_action_request_host_exit:
				remote_host_exit();
				return(FALSE);				// break out of all the loops and exit client game
				
		}
	}
	
	return(TRUE);
}

/* =======================================================

      Remote Networking Updates
      
======================================================= */

void remote_network_send_updates(int tick)
{
	obj_type			*obj;
    
	obj=object_find_uid(server.player_obj_uid);
	net_client_send_remote_update(tick,net_setup.client.remote_uid,obj,hud.chat.type_on);
}

/* =======================================================

      Remote Networking Latency Pings
      
======================================================= */

void remote_network_send_latency_ping(int tick)
{
	net_setup.client.latency_ping_tick=tick;
	net_client_send_latency_ping(net_setup.client.remote_uid);
}

