/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Join Host

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
#include "network.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;

extern bool					join_thread_quit;
extern d3socket				client_socket;

network_setup_type			net_setup;

/* =======================================================

      Find Game From Name
      
======================================================= */

int net_client_find_game(char *game_name)
{
	int			n;

	for (n=0;n!=net_setup.ngame;n++) {
		if (strcasecmp(net_setup.games[n].name,game_name)==0) return(n);
	}

	return(-1);
}

/* =======================================================

      Ping Host
      
======================================================= */

bool net_client_ping_host(char *ip,char *status,char *host_name,char *proj_name,char *game_name,char *map_name,int *player_count,int *player_max_count,int *ping_msec)
{
	int						msec,count;
	char					err_str[256];
	bool					connect_ok;
	network_reply_info		reply_info;
	
	status[0]=0x0;
	game_name[0]=0x0;
	map_name[0]=0x0;
	*player_count=0;
	*player_max_count=0;
	*ping_msec=-1;
	
		// create client socket
		
	client_socket=net_open_socket();
	if (client_socket==D3_NULL_SOCKET) {
		strcpy(status,"Unreachable");
		return(FALSE);
	}
	
		// connect to host
		
	if (!net_connect_start(client_socket,ip,net_port_host,err_str)) {
		strcpy(status,"Unreachable");
		net_close_socket(&client_socket);
		return(FALSE);
	}
	
		// check if connection completed
		// do it in a non-blocking manner so the join
		// UI can exit if needed
		
	count=client_timeout_wait_seconds;
	connect_ok=FALSE;
	
	count*=1000;
	
	while ((count>0) && (!join_thread_quit)) {
	
		if (net_connect_check(client_socket)) {
			connect_ok=TRUE;
			break;
		}
	
		usleep(1000);
		count--;
	}

	net_connect_end(client_socket);
	
		// connection failed?
		
	if (!connect_ok) {
		strcpy(status,"Unreachable");
		net_close_socket(&client_socket);
		return(FALSE);
	}
	
		// query for info
		
	msec=time_get();
	
	if (!net_queue_block_single_message(client_socket,net_action_request_info,net_remote_uid_none,NULL,0,net_action_reply_info,(unsigned char*)&reply_info,sizeof(network_reply_info))) {
		strcpy(status,"Unreachable");
		net_close_socket(&client_socket);
		return(FALSE);
	}

	msec=time_get()-msec;
	if (msec<1) msec=1;
	
		// close socket
		
	net_close_socket(&client_socket);
	
		// ok status message
		
	strcpy(status,"OK");
	
	strncpy(host_name,reply_info.host_name,name_str_len);
	host_name[name_str_len-1]=0x0;
	strncpy(proj_name,reply_info.proj_name,name_str_len);
	proj_name[name_str_len-1]=0x0;
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

      Join and Exit Host
      
======================================================= */

bool net_client_join_host_start(char *ip,char *name,int *remote_uid,char *game_name,char *map_name,int *tick_offset,char *deny_reason,network_request_add_objects *net_add_objects)
{
	char					err_str[256];
	network_request_join	request_join;
	network_reply_join		reply_join;
	
		// create client socket
		
	client_socket=net_open_socket();
	if (client_socket==D3_NULL_SOCKET) {
		strcpy(err_str,"Networking: Unable to open socket");
		return(FALSE);
	}

		// connect
		
	if (!net_connect_block(client_socket,ip,net_port_host,client_timeout_wait_seconds,err_str)) {
		strcpy(deny_reason,"Host Unreachable (No connection)");
		net_close_socket(&client_socket);
		return(FALSE);
	}
	
		// send join request
		
	strcpy(request_join.name,name);
	strcpy(request_join.vers,dim3_version);
	
	if (!net_queue_block_single_message(client_socket,net_action_request_join,net_remote_uid_none,(unsigned char*)&request_join,sizeof(network_request_join),net_action_reply_join,(unsigned char*)&reply_join,sizeof(network_reply_join))) {
		strcpy(deny_reason,"Host Unreachable (Bad send/receive)");
		net_close_socket(&client_socket);
		return(FALSE);
	}
	
		// check for denial
		
	if (reply_join.join_uid==-1) {
		strcpy(deny_reason,reply_join.deny_reason);
		net_close_socket(&client_socket);
		return(FALSE);
	}
	
		// finish setup
		
	*remote_uid=(int)ntohs(reply_join.join_uid);

	strcpy(game_name,reply_join.game_name);
	strcpy(map_name,reply_join.map_name);
	
	*tick_offset=ntohl(reply_join.map_tick);

		// additional objects

	net_add_objects->count=(int)ntohs((short)reply_join.add_objects.count);
	if (net_add_objects->count!=0) memmove(net_add_objects->objects,reply_join.add_objects.objects,(sizeof(network_request_object_add)*net_add_objects->count));
	
	return(TRUE);
}

void net_client_join_host_end(void)
{
	net_close_socket(&client_socket);
}
