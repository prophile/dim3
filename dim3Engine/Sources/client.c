/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: Network Client Code

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
#include "interfaces.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;

extern bool					join_thread_quit;

void* network_client_receive_thread(void *arg);			// forward reference

extern int time_get(void);

d3socket					client_socket;
bool						client_complete,client_msg_handler_active;
pthread_t					client_thread;
network_message_queue		client_queue;

int							client_timeout_values[]=setup_timeout_mode_values;

network_setup_type			net_setup;

/* =======================================================

      Message Handler
      
======================================================= */

bool network_client_start_message_handler(char *err_str)
{
		// create cache

	if (!network_queue_initialize(&client_queue)) {
		strcpy(err_str,"Networking: Out of memory");
		return(FALSE);
	}
	
		// start receive thread
		
	if (pthread_create(&client_thread,NULL,network_client_receive_thread,NULL)==0) {
		client_msg_handler_active=TRUE;
		return(TRUE);
	}
	
		// could not start thread
		
	network_queue_shutdown(&client_queue);
		
	strcpy(err_str,"Networking: Could not start client receive thread");
	return(FALSE);
}

void network_client_end_message_handler(void)
{
		// handler hasn't started yet
	
	if (!client_msg_handler_active) return;
	
		// shutdown message handler
		
	client_complete=FALSE;
	
		// wait for thread to exit
		
	while (TRUE) {
		if (client_complete) break;
		usleep(1000);
	}

		// free queue
		
	network_queue_shutdown(&client_queue);
}

void* network_client_receive_thread(void *arg)
{
	int					action,queue_mode,from_remote_uid,len,
						queue_count;
	unsigned char		data[net_max_msg_size];
	
	client_complete=TRUE;
		
	while (client_complete) {
	
			// is a message ready?
		
		if (!network_receive_ready(client_socket)) {
			usleep(10000);		// no packet, wait 1/100th of a second and try again
			continue;
		}
	
			// read message
			
		if (!network_receive_packet(client_socket,&action,&queue_mode,&from_remote_uid,data,&len)) {
			usleep(10000);		// wait 1/100th of a second between errors
			continue;
		}

			// add to queue
			// if the queue if full, retry a number of times and then
			// give up and ignore the message

		queue_count=0;

		while (client_complete) {
			if (network_queue_add_message(&client_queue,action,queue_mode,from_remote_uid,data,len)) break;

			queue_count++;
			if (queue_count>10) break;		// give up

			usleep(10000);		// queue if full, wait 1/100th of a second and try again
		}
	}
	
	client_complete=TRUE;
	
	pthread_exit(NULL);
	
	return(0);
}

bool network_client_check_message(int *action,int *from_remote_uid,unsigned char *data)
{
	return(network_queue_check_message(&client_queue,action,from_remote_uid,data));
}

/* =======================================================

      Ping Host
      
======================================================= */

bool network_client_ping_host(char *ip,char *status,char *host_name,char *game_name,char *map_name,int *player_count,int *player_max_count,int *ping_msec)
{
	int						action,from_remote_uid,len,msec,count;
	char					err_str[256];
	bool					connect_ok,read_ok;
	network_reply_info		reply_info;
	
	status[0]=0x0;
	game_name[0]=0x0;
	map_name[0]=0x0;
	*player_count=0;
	*player_max_count=0;
	*ping_msec=-1;
	
		// create client socket
		
	client_socket=network_open_socket();
	if (client_socket==D3_NULL_SOCKET) {
		strcpy(status,"Unreachable");
		return(FALSE);
	}
	
		// connect to host
		
	if (!network_connect_start(client_socket,ip,net_port_host,err_str)) {
		strcpy(status,"Unreachable");
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
		// check if connection completed
		// do it in a non-blocking manner so the join
		// UI can exit if needed
		
	count=client_timeout_values[setup.network.timeout_mode];
	connect_ok=FALSE;
	
	count*=1000;
	
	while ((count>0) && (!join_thread_quit)) {
	
		if (network_connect_check(client_socket)) {
			connect_ok=TRUE;
			break;
		}
	
		usleep(1000);
		count--;
	}

	network_connect_end(client_socket);
	
		// connection failed?
		
	if (!connect_ok) {
		strcpy(status,"Unreachable");
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
		// query for info
		
	msec=time_get();

	if (!network_send_packet(client_socket,net_action_request_info,net_queue_mode_normal,net_remote_uid_none,NULL,0)) {
		strcpy(status,"Unreachable");
		network_close_socket(&client_socket);
		return(FALSE);
	}

		// try to read reply for rest of time-out
		
	read_ok=FALSE;
	action=-1;
	
	if (count==0) count=1;
	
	while ((count>0) && (!join_thread_quit)) {
		if (network_receive_packet(client_socket,&action,NULL,&from_remote_uid,(unsigned char*)&reply_info,&len)) {
			read_ok=TRUE;
			break;
		}
		
		usleep(1000);
		count--;
	}
	
	if (!read_ok) {
		strcpy(status,"Unreachable");
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
	if (action!=net_action_reply_info) {
		strcpy(status,"Host Not dim3");
		network_close_socket(&client_socket);
		return(FALSE);
	}

	msec=time_get()-msec;
	if (msec<1) msec=1;
	
		// close socket
		
	network_close_socket(&client_socket);
	
		// ok status message
		
	strcpy(status,"OK");
	
	strncpy(host_name,reply_info.host_name,name_str_len);
	host_name[name_str_len-1]=0x0;
	strncpy(game_name,reply_info.game_name,name_str_len);
	game_name[name_str_len-1]=0x0;
	strncpy(map_name,reply_info.map_name,name_str_len);
	map_name[name_str_len-1]=0x0;

	*player_count=(signed short)ntohs(reply_info.player_count);
	*player_max_count=(signed short)ntohs(reply_info.player_max_count);
	*ping_msec=msec;
	
	return(TRUE);
}

/* =======================================================

      Join/Leave/Ping Host
      
======================================================= */

bool network_client_join_host(char *ip,char *name,int *remote_uid,char *game_name,char *map_name,char *deny_reason,int *remote_count,network_request_remote_add *remotes)
{
	int						action,from_remote_uid,count,len,
							timeout_count;
	char					err_str[256];
	bool					read_ok;
	network_request_join	request_join;
	network_reply_join		reply_join;
	
		// create client socket
		
	client_socket=network_open_socket();
	if (client_socket==D3_NULL_SOCKET) {
		strcpy(err_str,"Networking: Unable to open socket");
		return(FALSE);
	}

		// connect
		
	if (!network_connect_block(client_socket,ip,net_port_host,client_timeout_values[setup.network.timeout_mode],err_str)) {
		strcpy(deny_reason,"Host Unreachable (No connection)");
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
		// send join request
		
	strcpy(request_join.name,name);
	strcpy(request_join.vers,dim3_version);

	if (!network_send_packet(client_socket,net_action_request_join,net_queue_mode_normal,net_remote_uid_none,(unsigned char*)&request_join,sizeof(network_request_join))) {
		strcpy(deny_reason,"Host Unreachable (No send)");
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
		// wait for a reply -- it's possible that non-reply
		// messages could come in (updates from other remotes on
		// the host) -- throw these away until we get the join
		// reply
		
	timeout_count=client_timeout_values[setup.network.timeout_mode];
	timeout_count*=1000;
	
	read_ok=FALSE;
	action=-1;
	
	while (timeout_count>0) {
	
		if (network_receive_packet(client_socket,&action,NULL,&from_remote_uid,(unsigned char*)&reply_join,&len)) {
			if (action==net_action_reply_join) {
				read_ok=TRUE;
				break;
			}
		}
		
		usleep(1000);
		timeout_count--;
	}
	
	if (!read_ok) {
		strcpy(deny_reason,"Host Unreachable (No receive)");
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
	if (action!=net_action_reply_join) {
		strcpy(deny_reason,"Host Not dim3 Server");
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
		// check for denial
		
	if (reply_join.join_uid==-1) {
		strcpy(deny_reason,reply_join.deny_reason);
		network_close_socket(&client_socket);
		return(FALSE);
	}
	
		// message handler isn't active yet
		
	client_msg_handler_active=FALSE;
	
		// finish setup
		
	*remote_uid=(int)ntohs(reply_join.join_uid);

	strcpy(game_name,reply_join.game_name);
	strcpy(map_name,reply_join.map_name);
	
	count=(int)ntohs(reply_join.remote_count);
	*remote_count=count;
	if (count!=0) memmove(remotes,reply_join.remotes,(sizeof(network_request_remote_add)*count));
	
	return(TRUE);
}

bool network_client_ready_host(int remote_uid,char *err_str)
{
		// start client loop
		
	if (!network_client_start_message_handler(err_str)) return(FALSE);

		// mark as ready to receive data from host
		
	network_send_packet(client_socket,net_action_request_ready,net_queue_mode_normal,remote_uid,NULL,0);
	
	return(TRUE);
}

void network_client_set_team(int remote_uid,int team_idx)
{
	network_request_team	team;
	
	team.team_idx=htons((short)team_idx);
	network_send_packet(client_socket,net_action_request_team,net_queue_mode_normal,remote_uid,(unsigned char*)&team,sizeof(network_request_team));
}

void network_client_leave_host(int remote_uid)
{
		// send the leave message
	
	network_send_packet(client_socket,net_action_request_leave,net_queue_mode_normal,remote_uid,NULL,0);
	
		// end message handler
		
	network_client_end_message_handler();

		// shutdown the network
		
	network_close_socket(&client_socket);
}

void network_client_latency_ping_host(int remote_uid)
{
	network_send_packet(client_socket,net_action_request_latency_ping,net_queue_mode_normal,remote_uid,NULL,0);
}	

/* =======================================================

      Host Remote Sends
      
======================================================= */

void network_client_send_remote_update(int tick,int remote_uid,obj_type *obj,bool chat_on)
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

void network_client_send_death(int remote_uid,int kill_uid)
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

void network_client_send_telefrag(int remote_uid,int telefrag_uid)
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

      Host Messages and Sounds
	        
======================================================= */

void network_client_send_message(int remote_uid,char *str)
{
	network_request_remote_message	message;
	
	strcpy(message.str,str);
	network_send_packet(client_socket,net_action_request_remote_message,net_queue_mode_normal,remote_uid,(unsigned char*)&message,sizeof(network_request_remote_message));
}

void network_client_send_sound(int remote_uid,int x,int y,int z,float pitch,char *name)
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

      Host Weapon Sends
	        
======================================================= */

void network_client_send_projectile_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
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

void network_client_send_hitscan_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
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

void network_client_send_melee_add(int remote_uid,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang)
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

