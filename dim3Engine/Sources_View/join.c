/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Join

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
#include "remotes.h"
#include "interfaces.h"
#include "video.h"

#define join_button_join_id				0
#define join_button_cancel_id			1
#define join_button_rescan_id			2
#define join_table_id					3
#define join_status_id					4

extern void intro_open(void);
extern bool game_start(int skill,network_request_add_objects *net_add_objects,char *err_str);
extern bool map_start(bool skip_media,char *err_str);

extern map_type				map;
extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					client_timeout_values[];
extern char					setup_team_color_list[][32];

char						*join_table_data;
bool						join_local,join_thread_quit;
pthread_t					join_thread,join_thread_accept;
pthread_mutex_t				join_thread_lock;

int							join_count,join_start_tick_local;
bool						join_list_done;
join_server_info			join_list[max_setup_network_host];
			
/* =======================================================

      Build Join List
      
======================================================= */

char* join_create_list(void)
{
	int					n,sz;
	char				*c,*row_data;
	join_server_info	*info;
	
		// create row data
		
	sz=(join_count+1)*128;
	
	row_data=malloc(sz);
	bzero(row_data,sz);
	
		// build table
	
	info=join_list;
	c=row_data;
	
	for (n=0;n!=join_count;n++) {
		snprintf(c,128,"Maps;%s;%s\t%s @ %s\t%d/%d\t%dms",info->map_name,info->name,info->game_name,info->map_name,info->player_count,info->player_max_count,info->ping_msec);
		c[127]=0x0;
		
		c+=128;
		info++;
	}
	
	return(row_data);
}

/* =======================================================

      Join Ping Local Threads
      
======================================================= */

void* join_ping_thread_local_accept_read(void *arg)
{
	int					msec;
	unsigned char		data[net_max_msg_size];
	char				*row_data;
	network_reply_info	*reply_info;
	d3socket			sock;
	join_server_info	*info;

	msec=time_get()-join_start_tick_local;
	
		// read reply and add to list
		
	sock=(d3socket)arg;
	
	net_socket_blocking(sock,TRUE);
		
		// wait for data
	
	if (net_queue_block_single_message(sock,-1,0,NULL,0,net_action_reply_info,data,sizeof(network_reply_info))) {
		
		reply_info=(network_reply_info*)data;
		
			// is it the same dim3 project?

		if (strcasecmp(reply_info->proj_name,net_setup.host.proj_name)==0) {
	
			pthread_mutex_lock(&join_thread_lock);

				// setup info

			info=&join_list[join_count++];

			strcpy(info->name,reply_info->host_name);
			strcpy(info->ip,reply_info->host_ip_resolve);
			strcpy(info->game_name,reply_info->game_name);
			strcpy(info->map_name,reply_info->map_name);
			info->local=TRUE;
			info->player_count=(int)ntohs(reply_info->player_count);
			info->player_max_count=(int)ntohs(reply_info->player_max_count);
			info->ping_msec=msec;

				// rebuild list

			row_data=join_create_list();
			element_set_table_data(join_table_id,row_data);
			free(row_data);
			
			pthread_mutex_unlock(&join_thread_lock);
		}
	}
	
	net_close_socket(&sock);
	
	pthread_exit(NULL);

	return(0);
}

void* join_ping_thread_local_accept(void *arg)
{
	d3socket			broadcast_reply_sock,sock;
	socklen_t			addr_len;
	struct sockaddr		addr;
	
	broadcast_reply_sock=(d3socket)arg;

		// wait for server replies
		
	addr_len=sizeof(struct sockaddr);
	
	while (TRUE) {
		sock=accept(broadcast_reply_sock,&addr,&addr_len);
		if (sock==-1) break;				// accept ending means socket has been closed and host shutting down
		
			// spawn a thread to deal with replies
			
		pthread_create(&join_thread_accept,NULL,join_ping_thread_local_accept_read,(void*)sock);
	}

	pthread_exit(NULL);

	return(0);
}

void* join_ping_thread_local(void *arg)
{
	int			count;
	char		ip_name[256],ip_resolve[256],err_str[256];
	d3socket	broadcast_reply_sock;
	
	join_start_tick_local=time_get();
	
	net_get_host_ip(ip_name,ip_resolve);
	
		// spawn another thread to wait for replies
		// we do this on another thread so we can
		// cancel it under a certain time limit
		// by closing the socket

	broadcast_reply_sock=net_open_socket();
	net_socket_blocking(broadcast_reply_sock,TRUE);

	if (!net_bind(broadcast_reply_sock,ip_resolve,net_port_host_broadcast_reply,err_str)) {
		join_list_done=TRUE;
		pthread_exit(NULL);
		return(0);
	}
	
	if (listen(broadcast_reply_sock,32)!=0) {
		net_close_socket(&broadcast_reply_sock);
		join_list_done=TRUE;
		pthread_exit(NULL);
		return(0);
	}

		// run thread and wait for accepts to start
		
	pthread_create(&join_thread_accept,NULL,join_ping_thread_local_accept,(void*)broadcast_reply_sock);
	
		// send broadcast to all network nodes
		// asking for connections from hosts
		
	join_start_tick_local=time_get();
		
	if (net_udp_send_broadcast(ip_resolve,net_port_host_broadcast)) {
		
		count=client_timeout_wait_seconds;
		count*=1000;
		
		while ((count>0) && (!join_thread_quit)) {
			usleep(1000);
			count--;
		}
		
	}
	
		// close the socket to cancel the accept and
		// join the thread to wait for end
		
	net_close_socket(&broadcast_reply_sock);
	pthread_join(join_thread_accept,NULL);
	
	join_list_done=TRUE;
	pthread_exit(NULL);
	return(0);
}

/* =======================================================

      Join Ping Network Threads
      
======================================================= */

void* join_ping_thread_network(void *arg)
{
	int							idx,player_count,player_max_count,ping_msec;
	char						status[32],host_name[name_str_len],
								proj_name[name_str_len],game_name[name_str_len],map_name[name_str_len];
	char						*row_data;
	setup_network_hosts_type	*host;
	join_server_info			*info;
	
		// run ping until finished or
		// join page ends

	idx=0;

	while ((!join_thread_quit) && (idx<setup.network.nhost)) {

			// ping host

		host=&setup.network.hosts[idx];

		if (net_client_ping_host(host->ip,status,host_name,proj_name,game_name,map_name,&player_count,&player_max_count,&ping_msec)) {

				// is this reply the same dim3 project?
				
			if (strcasecmp(proj_name,net_setup.host.proj_name)==0) {

				pthread_mutex_lock(&join_thread_lock);
				
					// setup info

				info=&join_list[join_count++];
			
				strcpy(info->name,host_name);
				strcpy(info->ip,host->ip);
				strcpy(info->game_name,game_name);
				strcpy(info->map_name,map_name);
				info->local=FALSE;
				info->player_count=player_count;
				info->player_max_count=player_max_count;
				info->ping_msec=ping_msec;

					// rebuild list

				row_data=join_create_list();
				element_set_table_data(join_table_id,row_data);
				free(row_data);
				
				pthread_mutex_unlock(&join_thread_lock);
			}
		}
		
		idx++;
	}
	
	join_list_done=TRUE;
	
	pthread_exit(NULL);
	
	return(0);
}

/* =======================================================

      Join Ping Threads Mainline
      
======================================================= */

void join_ping_thread_start(bool local)
{
		// empty join list
		
	join_count=0;
	join_thread_quit=FALSE;
	
		// table is busy
		
	element_table_busy(join_table_id,TRUE);
	
		// table update locks
		
	pthread_mutex_init(&join_thread_lock,NULL);

		// start pinging hosts
		
	join_list_done=FALSE;
	
	if (local) {
		pthread_create(&join_thread,NULL,join_ping_thread_local,NULL);
	}
	else {
		pthread_create(&join_thread,NULL,join_ping_thread_network,NULL);
	}
}

void join_ping_thread_idle(void)
{
	element_table_busy(join_table_id,(!join_list_done));
	element_enable(join_button_rescan_id,join_list_done);
}

void join_ping_thread_end(void)
{
	join_thread_quit=TRUE;
	
	pthread_join(join_thread,NULL);
	
	pthread_mutex_destroy(&join_thread_lock);
}

/* =======================================================

      Join Operations
      
======================================================= */

void join_open(bool local)
{
	int					x,y,wid,high,padding;
	element_column_type	cols[4];
	
	join_local=local;

		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);
	
		// controls
							
	element_clear();
	
		// title
		
	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.09f);
	
	if (local) {
		element_text_add("Join Local Multiplayer Game",-1,x,y,hud.font.text_size_large,tx_left,FALSE,FALSE);
	}
	else {
		element_text_add("Join Internet Multiplayer Game",-1,x,y,hud.font.text_size_large,tx_left,FALSE,FALSE);
	}
	
		// hosts table
		
	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.12f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.88f)-y;

	strcpy(cols[0].name,"Name");
	cols[0].percent_size=0.45f;
	strcpy(cols[1].name,"Game");
	cols[1].percent_size=0.37f;
	strcpy(cols[2].name,"Players");
	cols[2].percent_size=0.10f;
	strcpy(cols[3].name,"Ping");
	cols[3].percent_size=0.8f;

	element_table_add(cols,NULL,join_table_id,4,x,y,wid,high,element_table_bitmap_data);
	element_table_busy(join_table_id,TRUE);
	
		// status

	padding=element_get_padding();
	high=(int)(((float)hud.scale_x)*0.05f);
		
	y=hud.scale_y-((padding+(high/2))-(element_get_control_high()/2));
	element_text_add("",join_status_id,15,y,hud.font.text_size_small,tx_left,FALSE,FALSE);
	
		// buttons
		
	x=padding;
	y=hud.scale_y-padding;
	
	wid=(int)(((float)hud.scale_x)*0.2f);

	element_button_text_add("Rescan Hosts",join_button_rescan_id,x,y,wid,high,element_pos_left,element_pos_bottom);
	element_enable(join_button_rescan_id,FALSE);
	
	x=hud.scale_x-padding;
	wid=(int)(((float)hud.scale_x)*0.1f);

	element_button_text_add("Join",join_button_join_id,x,y,wid,high,element_pos_right,element_pos_bottom);
	element_enable(join_button_join_id,FALSE);

	x=element_get_x_position(join_button_join_id)-padding;

	element_button_text_add("Cancel",join_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);

		// in join thread
		
	server.state=gs_join;

		// start ping thread

	join_ping_thread_start(local);
}

void join_close(void)
{
	join_ping_thread_end();
	gui_shutdown();
}

/* =======================================================

      Enabling
	        
======================================================= */

void join_activity_start(void)
{
	element_enable(join_button_join_id,FALSE);
	element_enable(join_button_cancel_id,FALSE);
	
	gui_draw(1.0f,FALSE);
}

void join_activity_complete(bool single,char *msg)
{
	element_enable(join_button_join_id,single);
	element_enable(join_button_cancel_id,TRUE);
	
	if (!single) element_set_value(join_table_id,-1);
	if (msg!=NULL) element_text_change(join_status_id,msg);
	
	gui_draw(1.0f,FALSE);
}

/* =======================================================

      Join Game
      
======================================================= */

void join_game(void)
{
	int							idx,remote_uid,tick_offset;
	char						game_name[name_str_len],map_name[name_str_len],
								deny_reason[64],err_str[256];
	network_request_add_objects net_add_objects;
	
		// get game to join
		
	join_activity_start();
	
	idx=element_get_value(join_table_id);
	strcpy(net_setup.client.joined_ip,join_list[idx].ip);
							
		// status
		
	element_text_change(join_status_id,"Joining ...");
	gui_draw(1.0f,FALSE);
							
		// attempt to join

	if (!net_client_join_host_start(net_setup.client.joined_ip,setup.network.name,&remote_uid,game_name,map_name,&tick_offset,deny_reason,&net_add_objects)) {
		join_close();
		sprintf(err_str,"Unable to Join Game: %s",deny_reason);
		error_open(err_str,"Network Game Canceled");
		return;
	}

		// mark remote and joined
		
	net_setup.host.hosting=FALSE;
	
	net_setup.client.joined=TRUE;
	net_setup.client.remote_uid=remote_uid;
	net_setup.client.latency=0;
	
		// setup game type

	net_setup.game_idx=net_client_find_game(game_name);
	if (net_setup.game_idx==-1) {
		net_client_send_leave_host(net_setup.client.remote_uid);
		net_client_join_host_end();
		sprintf(err_str,"Could not find game type: %s",game_name);
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,map_name);
	
		// start game
	
	join_close();
	
	if (!game_start(skill_medium,&net_add_objects,err_str)) {
		net_client_send_leave_host(net_setup.client.remote_uid);
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
		// start the map
		
	if (!map_start(TRUE,err_str)) {
		net_client_send_leave_host(net_setup.client.remote_uid);
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
		// start client network thread
		
	if (!net_client_start_message_queue(err_str)) {
		net_client_send_leave_host(net_setup.client.remote_uid);
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;
	}

		// mark as ready to receive data from host

	net_client_send_ready(remote_uid);

		// request moving group synchs

	net_client_request_group_synch_ping(remote_uid);
	
		// game is running
	
	server.state=gs_running;
}

/* =======================================================

      Join Input
      
======================================================= */

void join_click(void)
{
	int			id;
	bool		enable;
	
		// is element being clicked
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run selection

	switch (id) {
	
		case join_button_join_id:
			join_game();
			break;
			
		case join_button_cancel_id:
			join_close();
			intro_open();
			break;
			
		case join_button_rescan_id:
			element_enable(join_button_rescan_id,FALSE);
			join_ping_thread_end();
			join_ping_thread_start(join_local);
			break;

		case join_table_id:
			enable=(element_get_value(join_table_id)!=-1);
			element_enable(join_button_join_id,enable);
			break;
	}
}
	
/* =======================================================

      Run Join
      
======================================================= */

void join_run(void)
{
		// idle the ping thread to check if pinging
		// is finished
		
	join_ping_thread_idle();
	
		// drawing and clicking
		
	gui_draw(1.0f,TRUE);
	join_click();
}

