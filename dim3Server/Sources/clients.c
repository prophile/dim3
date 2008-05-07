/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Network Client Code

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
	#include "dim3server.h"
#endif

#ifdef D3_OS_MAC
	#include <ifaddrs.h>
#endif

extern int					server_player_count;
extern network_setup_type	net_setup;

/* =======================================================

      Client Networking Message Handlers
      
======================================================= */

void client_handle_info(int sock)
{
	network_reply_info		info;
	
	info.player_count=htons((short)server_player_count);
	info.player_max_count=htons(net_max_remote_count);
	strcpy(info.host_name,net_setup.host.name);
	strcpy(info.host_ip_resolve,net_setup.host.ip_resolve);
	strcpy(info.game_name,net_setup.games[net_setup.host.game_idx].script_name);
	strcpy(info.map_name,net_setup.games[net_setup.host.game_idx].map_name);

	network_send_packet(sock,net_action_reply_info,net_queue_mode_normal,net_remote_uid_host,(unsigned char*)&info,sizeof(network_reply_info));
}

int client_handle_join(int sock,network_request_join *request_join)
{
	int							remote_uid;
	network_reply_join			reply_join;
	network_request_remote_add	remote_add;

		// if correct version, add player to host

	if (strncmp(request_join->vers,dim3_version,name_str_len)==0) {
		remote_uid=server_player_join(sock,request_join->name,reply_join.deny_reason);
	}
	else {
		remote_uid=-1;
		sprintf(reply_join.deny_reason,"Client version differs from Server (%s)",dim3_version);
	}

		// construct the reply
	
	strcpy(reply_join.game_name,net_setup.games[net_setup.host.game_idx].script_name);
	strcpy(reply_join.map_name,net_setup.games[net_setup.host.game_idx].map_name);
	reply_join.join_uid=htons((short)remote_uid);
	
	if (remote_uid!=-1) {
		reply_join.remote_count=htons((short)server_player_create_remote_add_list(remote_uid,reply_join.remotes));
	}
	else {
		reply_join.remote_count=0;
	}
	
		// send reply
	
	network_send_packet(sock,net_action_reply_join,net_queue_mode_normal,net_remote_uid_host,(unsigned char*)&reply_join,sizeof(network_reply_join));
	
		// send all other players on host the new player for remote add
		
	if (remote_uid!=-1) {
		strncpy(remote_add.name,request_join->name,name_str_len);
		remote_add.name[name_str_len-1]=0x0;
		remote_add.team_idx=htons((short)net_team_none);
		remote_add.score=0;
		remote_add.pos_rn=remote_add.pos_x=remote_add.pos_y=remote_add.pos_z=0;
		server_player_send_others_packet(remote_uid,net_action_request_remote_add,net_queue_mode_normal,(unsigned char*)&remote_add,sizeof(network_request_remote_add),FALSE);
	}

	return(remote_uid);
}

void client_handle_leave(int remote_uid)
{
		// leave the host
		
	server_player_leave(remote_uid);
	
		// now send all other players on host the remote remove
		
	server_player_send_others_packet(remote_uid,net_action_request_remote_remove,net_queue_mode_normal,NULL,0,FALSE);
}

/* =======================================================

      Client Networking Message Thread
      
======================================================= */

void* client_thread(void *arg)
{
	d3socket						sock;
	int								client_remote_uid,action,queue_mode,from_remote_uid,len,
									net_error_count;
	unsigned char					data[net_max_msg_size];
	
		// get sock from argument
		
	sock=(d3socket)arg;
	
		// no attached player until join request
		
	client_remote_uid=-1;
	
	net_error_count=0;
	
		// wait for messages
		
	while (TRUE) {
	
			// any messages?
			
		if (!network_receive_ready(sock)) {
			usleep(server_no_data_u_wait);
			continue;
		}
		
			// read the message
			// if error reach past error count, then assume socket has closed on other
			// end or some other fatal error and kick off user
			
		if (!network_receive_packet(sock,&action,&queue_mode,&from_remote_uid,data,&len)) {
			net_error_count++;
			if (net_error_count==server_max_network_error_reject) {
				if (client_remote_uid!=-1) client_handle_leave(client_remote_uid);
				client_remote_uid=-1;
				break;
			}
			continue;
		}

		net_error_count=0;
		
			// deal with message type
			
		switch (action) {
		
			case net_action_request_info:
				client_handle_info(sock);
				break;
				
			case net_action_request_join:
				client_remote_uid=client_handle_join(sock,(network_request_join*)data);
				break;

			case net_action_request_ready:
				server_player_ready(client_remote_uid,TRUE);
				break;
				
			case net_action_request_team:
				server_player_update_team(client_remote_uid,(network_request_team*)data);
				server_player_send_others_packet(client_remote_uid,action,net_queue_mode_normal,data,len,FALSE);
				break;
				
			case net_action_request_leave:
				client_handle_leave(client_remote_uid);
				client_remote_uid=-1;
				break;
				
			case net_action_request_remote_update:
				server_player_update(client_remote_uid,(network_request_remote_update*)data);
				server_player_send_others_packet(client_remote_uid,action,queue_mode,data,len,TRUE);
				break;
				
			case net_action_request_remote_death:
				server_player_death(client_remote_uid);
				server_player_send_others_packet(client_remote_uid,action,queue_mode,data,len,FALSE);
				break;
				
			case net_action_request_remote_telefrag:
				server_player_telefrag(client_remote_uid);
				server_player_send_others_packet(client_remote_uid,action,queue_mode,data,len,FALSE);
				break;
				
			case net_action_request_remote_chat:
				server_player_message(client_remote_uid,(network_request_remote_chat*)data);
				server_player_send_others_packet(client_remote_uid,action,queue_mode,data,len,FALSE);
				break;
				
			case net_action_request_remote_sound:
				server_player_send_others_packet(client_remote_uid,action,queue_mode,data,len,FALSE);
				break;

			case net_action_request_projectile_add:
			case net_action_request_hitscan_add:
			case net_action_request_melee_add:
				server_player_send_others_packet(client_remote_uid,action,queue_mode,data,len,FALSE);
				break;

			case net_action_request_latency_ping:
				network_send_packet(sock,net_action_reply_latency_ping,net_queue_mode_normal,net_remote_uid_host,NULL,0);
				break;
				
		}
		
			// if no player attached, close socket and exit thread
			
		if (client_remote_uid==-1) break;
	}
	
	network_close_socket(&sock);
	
	pthread_exit(NULL);
	return(NULL);
}


