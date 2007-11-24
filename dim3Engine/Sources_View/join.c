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

#include "client.h"
#include "remotes.h"
#include "interfaces.h"
#include "video.h"

#define join_button_join_id				0
#define join_button_cancel_id			1
#define join_table_id					2
#define join_status_id					3

#define join_tab_id						10

#define join_tab_value_local			0
#define join_tab_value_network			1

extern void intro_open(void);
extern bool game_start(int remote_count,network_request_remote_add *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					client_timeout_values[];
extern char					setup_team_color_list[][32];

char						*join_table_data;
bool						join_thread_quit;
pthread_t					join_thread_local,join_thread_local_accept,join_thread_network;
pthread_mutex_t				join_thread_lock_local;

int							join_tab_value,join_start_tick_local,
							join_count_local,join_count_network;
bool						join_done_local,join_done_network;
join_server_info			join_list_local[max_setup_network_host],
							join_list_network[max_setup_network_host];
			
/* =======================================================

      Build Join List
      
======================================================= */

char* join_create_list(int count,join_server_info *info)
{
	int					n,sz;
	char				*c,*row_data;
	
		// create row data
		
	sz=(count+1)*128;
	
	row_data=valloc(sz);
	bzero(row_data,sz);
	
		// build table
	
	c=row_data;
	
	for (n=0;n!=count;n++) {
		snprintf(c,128,"%s\t%s\t%s\t%d/%d\t%dms",info->name,info->game_name,info->map_name,info->player_count,info->player_max_count,info->ping_msec);
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
	int					n,msec,action,queue_mode,from_remote_uid,len;
	unsigned char		data[net_max_msg_size];
	char				*row_data;
	network_reply_info	*reply_info;
	d3socket			sock;
	join_server_info	*info;

	msec=time_get()-join_start_tick_local;
	
		// read reply and add to list
		
	pthread_mutex_lock(&join_thread_lock_local);
	
	sock=(d3socket)arg;
	
	network_socket_blocking(sock,TRUE);
		
		// wait for data
		
	n=0;
		
	while (n!=100) {
		if (network_receive_ready(sock)) break;
		usleep(10000);
		n++;
	}
	
		// read data
		
	if (network_receive_packet(sock,&action,&queue_mode,&from_remote_uid,data,&len)) {
	
			// only add to list if reply is right type
			
		if (action==net_action_reply_info) {
		
			reply_info=(network_reply_info*)data;
				
			info=&join_list_local[join_count_local++];
		
			strcpy(info->name,reply_info->host_name);
			strcpy(info->ip,reply_info->host_ip_resolve);
			strcpy(info->game_name,reply_info->game_name);
			strcpy(info->map_name,reply_info->map_name);
			info->player_count=reply_info->player_count;
			info->player_max_count=reply_info->player_max_count;
			info->ping_msec=msec;

				// rebuild list

			if (join_tab_value==join_tab_value_local) {
				row_data=join_create_list(join_count_local,join_list_local);
				element_set_table_data(join_table_id,row_data);
				free(row_data);
			}
		}
	}
	
	network_close_socket(&sock);

	pthread_mutex_unlock(&join_thread_lock_local);
	
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
			
		pthread_create(&join_thread_local_accept,NULL,join_ping_thread_local_accept_read,(void*)sock);
	}

	pthread_exit(NULL);

	return(0);
}

void join_ping_thread_local_broadcast(void)
{
	int			count;
	char		ip_name[256],ip_resolve[256],err_str[256];
	d3socket	broadcast_reply_sock;
	
	join_start_tick_local=time_get();
	
	network_get_host_ip(ip_name,ip_resolve);
	
		// this lock is used to keep multiple
		// threaded accepts from constantly over-writing
		// the list
		
	pthread_mutex_init(&join_thread_lock_local,NULL);
	
		// spawn another thread to wait for replies
		// we do this on another thread so we can
		// cancel it under a certain time limit
		// by closing the socket

	broadcast_reply_sock=network_open_socket();
	network_socket_blocking(broadcast_reply_sock,TRUE);

	if (!network_bind(broadcast_reply_sock,ip_resolve,net_port_host_broadcast_reply,err_str)) return;

	if (listen(broadcast_reply_sock,32)!=0) {
		network_close_socket(&broadcast_reply_sock);
		return;
	}

		// run thread and wait for accepts to start
		
	pthread_create(&join_thread_local_accept,NULL,join_ping_thread_local_accept,(void*)broadcast_reply_sock);
	
		// send broadcast to all network nodes
		// asking for connections from hosts
		
	join_start_tick_local=time_get();
		
	if (!network_udp_send_broadcast(ip_resolve,net_port_host_broadcast)) return;
		
	count=client_timeout_values[setup.network.timeout_mode];
	count*=1000;
	
	while ((count>0) && (!join_thread_quit)) {
		usleep(1000);
		count--;
	}
	
		// close the socket to cancel the accept and
		// join the thread to wait for end
		
	network_close_socket(&broadcast_reply_sock);
	pthread_join(join_thread_local_accept,NULL);
	
	pthread_mutex_destroy(&join_thread_lock_local);
}

void* join_ping_thread_local(void *arg)
{
		// main thread, called this way so "mark as done"
		// doesn't have to be repeated
		
	join_ping_thread_local_broadcast();

		// mark this thread as done
		
	join_done_local=TRUE;
	if (join_tab_value==join_tab_value_local) element_table_busy(join_table_id,FALSE);
	
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
								game_name[name_str_len],map_name[name_str_len];
	char						*row_data;
	setup_network_hosts_type	*host;
	join_server_info			*info;
	
		// run ping until finished or
		// join page ends

	idx=0;

	while ((!join_thread_quit) && (idx<setup.network.nhost)) {

			// ping host

		host=&setup.network.hosts[idx];

		if (network_client_ping_host(host->ip,status,host_name,game_name,map_name,&player_count,&player_max_count,&ping_msec)) {

				// setup info
				
			info=&join_list_network[join_count_network++];
		
			strcpy(info->name,host_name);
			strcpy(info->ip,host->ip);
			strcpy(info->game_name,game_name);
			strcpy(info->map_name,map_name);
			info->player_count=player_count;
			info->player_max_count=player_max_count;
			info->ping_msec=ping_msec;

				// rebuild list

			if (join_tab_value==join_tab_value_network) {
				row_data=join_create_list(join_count_network,join_list_network);
				element_set_table_data(join_table_id,row_data);
				free(row_data);
			}
		}
		
		idx++;
	}
	
	join_done_network=TRUE;
	if (join_tab_value==join_tab_value_network) element_table_busy(join_table_id,FALSE);
	
	pthread_exit(NULL);
	
	return(0);
}

/* =======================================================

      Join Ping Threads Mainline
      
======================================================= */

void join_ping_thread_start(void)
{
	join_thread_quit=FALSE;
	
	join_done_local=FALSE;
	join_done_network=FALSE;
	
	pthread_create(&join_thread_local,NULL,join_ping_thread_local,NULL);
	if (setup.network.nhost!=0) pthread_create(&join_thread_network,NULL,join_ping_thread_network,NULL);
}

void join_ping_thread_end(void)
{
	join_thread_quit=TRUE;
	
	pthread_join(join_thread_local,NULL);
	if (setup.network.nhost!=0) pthread_join(join_thread_network,NULL);
}

/* =======================================================

      Join Operations
      
======================================================= */

void join_open(void)
{
	int					n,x,y,high,list_wid,list_high;
	char				path[1024],path2[1024],
						tab_path[2][1024],tab_path2[2][1024],
						tab_list[2][32]={"tab_local_servers","tab_network_servers"},
						tab_selected_list[2][32]={"tab_local_servers_selected","tab_network_servers_selected"};
	element_column_type	cols[5];
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","join");
	
		// make the join list
		
	join_count_local=0;
	join_count_network=0;
	
		// controls
							
	element_clear();
	
		// tabs
		
	for (n=0;n!=2;n++) {
		file_paths_data(&setup.file_path_setup,tab_path[n],"Bitmaps/UI_Elements",tab_list[n],"png");
		file_paths_data(&setup.file_path_setup,tab_path2[n],"Bitmaps/UI_Elements",tab_selected_list[n],"png");
	}
	
	join_tab_value=join_tab_value_local;
	
	x=setup.screen.x_scale/2;
	if (x>128) x=128;
	high=x/4;
	y=41-high;
	
	element_tab_add((char*)tab_path,(char*)tab_path2,join_tab_value,join_tab_id,2,0,y,setup.screen.x_scale,high,(setup.screen.y_scale-(60+y)));
	
		// controls
		
	high=gl_text_get_char_height(FALSE);
		
	x=setup.screen.x_scale-10;
	y=setup.screen.y_scale-30;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_cancel","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_cancel_selected","png");
	element_button_add(path,path2,join_button_cancel_id,x,y,-1,-1,element_pos_right,element_pos_center);

	x=element_get_x_position(join_button_cancel_id)-10;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_join","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_join_selected","png");
	element_button_add(path,path2,join_button_join_id,x,y,-1,-1,element_pos_right,element_pos_center);
	element_enable(join_button_join_id,FALSE);
	
		// hosts table
		
	list_wid=setup.screen.x_scale-30;
	list_high=setup.screen.y_scale-(high+105);
	
	y=high+30;

	strcpy(cols[0].name,"Name");
	cols[0].percent_size=0.30f;
	strcpy(cols[1].name,"Game");
	cols[1].percent_size=0.25f;
	strcpy(cols[2].name,"Map");
	cols[2].percent_size=0.25f;
	strcpy(cols[3].name,"Players");
	cols[3].percent_size=0.10f;
	strcpy(cols[4].name,"Ping");
	cols[4].percent_size=0.10f;

	element_table_add(cols,NULL,join_table_id,5,15,y,list_wid,list_high);
	element_table_busy(join_table_id,TRUE);
	
		// status
		
	y=setup.screen.y_scale-24;
	element_text_add("",join_status_id,15,y,tx_left,TRUE,FALSE,FALSE);
	
	server.state=gs_join;

		// start ping thread

	join_ping_thread_start();
}

void join_close(void)
{
	join_ping_thread_end();
	gui_shutdown();
}

/* =======================================================

      Tabs and Enabling
	        
======================================================= */

void join_switch_tab(void)
{
	char			*row_data;
	bool			done;
	
		// rebuild list

	if (join_tab_value==join_tab_value_local) {
		row_data=join_create_list(join_count_local,join_list_local);
		done=join_done_local;
	}
	else {
		row_data=join_create_list(join_count_network,join_list_network);
		done=join_done_network;
	}
	
	element_set_table_data(join_table_id,row_data);
	free(row_data);
	
	element_table_busy(join_table_id,!done);
}

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
	int							idx,remote_uid,remote_count;
	char						game_name[name_str_len],map_name[name_str_len],
								deny_reason[64],err_str[256];
	network_request_remote_add	remotes[net_max_remote_count];
	
		// end pinging thread
		
	join_ping_thread_end();
		
		// get game to join
		
	join_activity_start();
	
	idx=element_get_value(join_table_id);
	
	if (join_tab_value==join_tab_value_local) {
		strcpy(net_setup.client.joined_ip,join_list_local[idx].ip);
	}
	else {
		strcpy(net_setup.client.joined_ip,join_list_network[idx].ip);
	}
							
		// status
		
	element_text_change(join_status_id,"Joining ...");
	gui_draw(1.0f,FALSE);
							
		// attempt to join

	if (!network_client_join_host(net_setup.client.joined_ip,setup.network.name,&remote_uid,game_name,map_name,deny_reason,&remote_count,remotes)) {
		gui_shutdown();
		sprintf(err_str,"Unable to Join Game: %s",deny_reason);
		error_open(err_str,"Network Game Canceled");
		return;
	}

		// mark remote and joined
		
	net_setup.client.joined=TRUE;
	net_setup.client.remote_uid=remote_uid;
	net_setup.client.latency=0;

	strcpy(net_setup.client.game_name,game_name);
	
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,map_name);
	
		// start game
	
	gui_shutdown();
	
	if (!game_start(remote_count,remotes,err_str)) {
		network_client_leave_host(net_setup.client.remote_uid);
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
		// start the map
		
	if (!map_start(FALSE,err_str)) {
		network_client_leave_host(net_setup.client.remote_uid);
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
		// start client network thread
		
	if (!network_client_ready_host(remote_uid,err_str)) {
		network_client_leave_host(net_setup.client.remote_uid);
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
		// game is running
	
	server.state=gs_running;
}

void join_cancel(void)
{
	join_ping_thread_end();
	gui_shutdown();
	intro_open();
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
	
		case join_tab_id:
			join_tab_value=element_get_value(join_tab_id);
			join_switch_tab();
			break;
	
		case join_button_join_id:
			join_game();
			break;
			
		case join_button_cancel_id:
			join_cancel();
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
	gui_draw(1.0f,TRUE);
	join_click();
}

