/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Send Message to Host

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "interfaces.h"

extern map_type					map;
extern server_type				server;
extern js_type					js;
extern setup_type				setup;

extern d3socket					client_socket;

extern network_setup_type		net_setup;

/* =======================================================

      Host Messages
      
======================================================= */

void net_client_send_ready(int remote_uid)
{
	network_send_packet(client_socket,net_action_request_ready,net_queue_mode_normal,remote_uid,NULL,0);
}

void net_client_send_set_team(int remote_uid,int team_idx)
{
	network_request_team	team;
	
	team.team_idx=htons((short)team_idx);
	network_send_packet(client_socket,net_action_request_team,net_queue_mode_normal,remote_uid,(unsigned char*)&team,sizeof(network_request_team));
}

void net_client_send_leave_host(int remote_uid)
{
	network_send_packet(client_socket,net_action_request_leave,net_queue_mode_normal,remote_uid,NULL,0);
}

void net_client_send_latency_ping(int remote_uid)
{
	network_send_packet(client_socket,net_action_request_latency_ping,net_queue_mode_normal,remote_uid,NULL,0);
}	

/* =======================================================

      Object Update Messages
      
======================================================= */

void net_client_send_remote_update(int tick,int remote_uid,obj_type *obj,bool chat_on)
{
	int								n,flags;
	model_draw						*draw;
	model_draw_animation			*animation;
	network_request_animation		*net_animation;
	network_request_remote_update	update;
	
		// no updates if pipe full
		
	if (!network_send_ready(client_socket)) return;
	
		// create flags
		
	flags=0;
	
	if (obj->hidden) flags|=net_update_flag_hidden;
	if (!obj->contact.on) flags|=net_update_flag_no_contact;
	if (chat_on) flags|=net_update_flag_talking;
	
	update.flags=htonl(flags);
	
		// status

	update.score=htons((short)obj->score.score);
	update.health=htons((short)obj->status.health);
	
		// position
		
	update.pos_rn=htons((short)obj->pos.rn);
	update.pos_x=htonl(obj->pos.x);
	update.pos_y=htonl(obj->pos.y);
	update.pos_z=htonl(obj->pos.z);
	
	update.fp_ang_x=htonf(obj->ang.x);
	update.fp_ang_y=htonf(obj->ang.y);
	update.fp_ang_z=htonf(obj->ang.z);

	update.fp_move_vct_x=htonf(obj->motion.vct.x);
	update.fp_move_vct_y=htonf(obj->motion.vct.y);
	update.fp_move_vct_z=htonf(obj->motion.vct.z);
	
	update.fp_turn_ang_add_y=htonf(obj->turn.fix_ang_add.y);
	
		// vehicles
		
	if (obj->vehicle.attach_obj_uid==-1) {
		update.vehicle_map_spawn_idx=-1;
	}
	else {
		update.vehicle_map_spawn_idx=obj->spawn_idx;
	}
	
		// model animations

	draw=&obj->draw;
	
	for (n=0;n!=max_model_blend_animation;n++) {
		animation=&draw->animations[n];
		net_animation=&update.animation[n];

		net_animation->model_tick=htonl(animation->tick-tick);
		net_animation->model_mode=htons((short)animation->mode);
		net_animation->model_animate_idx=htons((short)animation->animate_idx);
		net_animation->model_animate_next_idx=htons((short)animation->animate_next_idx);
		net_animation->model_pose_move_idx=htons((short)animation->pose_move_idx);
		net_animation->model_smooth_animate_idx=htons((short)animation->smooth_animate_idx);
		net_animation->model_smooth_pose_move_idx=htons((short)animation->smooth_pose_move_idx);
	}
	
	update.model_mesh_mask=htons((short)draw->mesh_mask);
	memmove(update.model_cur_texture_frame,draw->cur_texture_frame,max_model_texture);

		// send update
		
	network_send_packet(client_socket,net_action_request_remote_update,net_queue_mode_replace,remote_uid,(unsigned char*)&update,sizeof(network_request_remote_update));
}

void net_client_send_death(int remote_uid,int kill_uid)
{
	int								send_kill_uid;
	obj_type						*obj;
	network_request_remote_death	net_death;
	
	send_kill_uid=-1;
	
	if (kill_uid!=-1) {
		if (kill_uid==server.player_obj_uid) {
			send_kill_uid=net_setup.client.remote_uid;
		}
		else {
			obj=object_find_uid(kill_uid);
			if (obj!=NULL) {
				if (obj->remote.on) {
					send_kill_uid=obj->remote.uid;
				}
			}
		}
	}
	
	net_death.kill_remote_uid=htons((short)send_kill_uid);

	network_send_packet(client_socket,net_action_request_remote_death,net_queue_mode_normal,remote_uid,(unsigned char*)&net_death,sizeof(network_request_remote_death));
}

void net_client_send_telefrag(int remote_uid,int telefrag_uid)
{
	int								send_telefrag_uid;
	obj_type						*obj;
	network_request_remote_telefrag	net_telefrag;

	if (telefrag_uid==server.player_obj_uid) {
		send_telefrag_uid=net_setup.client.remote_uid;
	}
	else {
		obj=object_find_uid(telefrag_uid);		// only remote objects can telefrag each other, so no other checks necessary
		send_telefrag_uid=obj->remote.uid;
	}
	
	net_telefrag.telefrag_remote_uid=htons((short)send_telefrag_uid);

	network_send_packet(client_socket,net_action_request_remote_telefrag,net_queue_mode_normal,remote_uid,(unsigned char*)&net_telefrag,sizeof(network_request_remote_telefrag));
}

/* =======================================================

      Chats and Sound Messages
	        
======================================================= */

void net_client_send_chat(int remote_uid,char *str)
{
	network_request_remote_chat		chat;
	
	strcpy(chat.str,str);
	network_send_packet(client_socket,net_action_request_remote_chat,net_queue_mode_normal,remote_uid,(unsigned char*)&chat,sizeof(network_request_remote_chat));
}

void net_client_send_sound(int remote_uid,int x,int y,int z,float pitch,char *name)
{
	network_request_remote_sound	sound;
	
	sound.pos_x=htonl(x);
	sound.pos_y=htonl(y);
	sound.pos_z=htonl(z);
	
	sound.fp_pitch=htonf(pitch);
	
	strcpy(sound.name,name);

	network_send_packet(client_socket,net_action_request_remote_sound,net_queue_mode_normal,remote_uid,(unsigned char*)&sound,sizeof(network_request_remote_sound));
}

/* =======================================================

      Weapon Messages
	        
======================================================= */

void net_client_send_projectile_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_projectile_add	proj_add;
	
	proj_add.pt_x=htonl(pt->x);
	proj_add.pt_y=htonl(pt->y);
	proj_add.pt_z=htonl(pt->z);
	
	proj_add.fp_ang_x=htonf(ang->x);
	proj_add.fp_ang_y=htonf(ang->y);
	proj_add.fp_ang_z=htonf(ang->z);
	
	strcpy(proj_add.weap_name,weap_name);
	strcpy(proj_add.proj_setup_name,proj_setup_name);

	network_send_packet(client_socket,net_action_request_projectile_add,net_queue_mode_normal,remote_uid,(unsigned char*)&proj_add,sizeof(network_request_projectile_add));
}

void net_client_send_hitscan_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_hitscan_add	hitscan_add;
	
	hitscan_add.pt_x=htonl(pt->x);
	hitscan_add.pt_y=htonl(pt->y);
	hitscan_add.pt_z=htonl(pt->z);
	
	hitscan_add.fp_ang_x=htonf(ang->x);
	hitscan_add.fp_ang_y=htonf(ang->y);
	hitscan_add.fp_ang_z=htonf(ang->z);
	
	strcpy(hitscan_add.weap_name,weap_name);
	strcpy(hitscan_add.proj_setup_name,proj_setup_name);

	network_send_packet(client_socket,net_action_request_hitscan_add,net_queue_mode_normal,remote_uid,(unsigned char*)&hitscan_add,sizeof(network_request_hitscan_add));
}

void net_client_send_melee_add(int remote_uid,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang)
{
	network_request_melee_add	melee_add;
	
	melee_add.pt_x=htonl(pt->x);
	melee_add.pt_y=htonl(pt->y);
	melee_add.pt_z=htonl(pt->z);
	
	melee_add.fp_ang_x=htonf(ang->x);
	melee_add.fp_ang_y=htonf(ang->y);
	melee_add.fp_ang_z=htonf(ang->z);
	
	strcpy(melee_add.weap_name,weap_name);
	
	melee_add.radius=htons((short)radius);
	melee_add.distance=htons((short)distance);
	melee_add.damage=htons((short)damage);
	melee_add.force=htons((short)force);

	network_send_packet(client_socket,net_action_request_melee_add,net_queue_mode_normal,remote_uid,(unsigned char*)&melee_add,sizeof(network_request_melee_add));
}

