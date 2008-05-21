/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Host

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

#include "network.h"
#include "remotes.h"
#include "interfaces.h"
#include "video.h"

#define host_button_host_id				0
#define host_button_cancel_id			1
#define host_game_type_id				2
#define host_table_id					3
#define host_status_id					4

extern void intro_open(void);
extern bool net_host_game_start(char *err_str);
extern void net_host_game_end(void);
extern bool game_start(int skill,int remote_count,network_request_remote_add *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

char						*net_host_file_list;
char						net_game_types[network_setup_max_game+1][32];

/* =======================================================

      Join Operations
      
======================================================= */

void host_open(void)
{
	int							x,y,high,list_wid,list_high,control_y_add,separate_y_add,
								n,nfile,sz;
	char						path[1024],path2[1024];
	char						*c;
	element_column_type			cols[1];
	file_path_directory_type	*map_pick_fpd;
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","host",FALSE,FALSE);
	
		// controls
							
	element_clear();
	
		// buttons
		
	high=gl_text_get_char_height(FALSE);
		
	x=setup.screen.x_scale-10;
	y=setup.screen.y_scale-30;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_cancel","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_cancel_selected","png");
	element_button_add(path,path2,host_button_cancel_id,x,y,-1,-1,element_pos_right,element_pos_center);

	x=element_get_x_position(host_button_cancel_id)-10;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_host","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_host_selected","png");
	element_button_add(path,path2,host_button_host_id,x,y,-1,-1,element_pos_right,element_pos_center);
	element_enable(host_button_host_id,FALSE);

		// game type

	for (n=0;n!=net_setup.ngame;n++) {
		strcpy(net_game_types[n],net_setup.games[n].name);
	}
	net_game_types[net_setup.ngame][0]=0x0;

	y=high+40;
	control_y_add=element_get_control_high();
	separate_y_add=element_get_separator_high();
	
	element_combo_add("Game Type",(char*)net_game_types,0,host_game_type_id,85,y,TRUE);
	y+=separate_y_add;

		// hosts table
		
	list_wid=setup.screen.x_scale-30;
	list_high=setup.screen.y_scale-(high+125);

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,host_table_id,1,15,y,list_wid,list_high,element_table_bitmap_none);
	
		// fill table with maps

	map_pick_fpd=file_paths_read_directory(&setup.file_path_setup,"Maps","xml",FALSE);

	nfile=map_pick_fpd->nfile;

	if (nfile>0) {
		sz=(nfile+1)*128;
		
		net_host_file_list=valloc(sz);
		bzero(net_host_file_list,sz);

		c=net_host_file_list;
		
		for (n=0;n!=nfile;n++) {
			strcpy(c,map_pick_fpd->files[n].file_name);
			c+=128;
		}
		
		element_set_table_data(host_table_id,net_host_file_list);
	}
	else {
		net_host_file_list=NULL;
	}

	file_paths_close_directory(map_pick_fpd);
	
		// status
		
	y=setup.screen.y_scale-24;
	element_text_add("",host_status_id,15,y,tx_left,TRUE,FALSE,FALSE);
	
	server.state=gs_host;
}

void host_close(void)
{
	if (net_host_file_list!=NULL) free(net_host_file_list);
	gui_shutdown();
}

/* =======================================================

      Host Game
      
======================================================= */

void host_game_setup(void)
{
	int				idx;
	
	idx=element_get_value(host_game_type_id);
	strcpy(net_setup.host.game_name,net_setup.games[idx].name);
	
	idx=element_get_value(host_table_id);
	strcpy(net_setup.host.map_name,(net_host_file_list+(idx*128)));
}

void host_game(void)
{
	int						remote_uid;
	char					err_str[256];
	network_request_join	request_join;
	
		// start hosting
		
	if (!net_host_game_start(err_str)) {
		host_close();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}
	
		// attempt to connect to local server

	strcpy(request_join.name,setup.network.name);
	strcpy(request_join.vers,dim3_version);

	remote_uid=net_host_client_handle_local_join(&request_join,err_str);
	if (remote_uid==-1) {
		host_close();
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}

		// connected

	net_setup.host.hosting=TRUE;
	net_setup.client.joined=TRUE;
	net_setup.client.remote_uid=remote_uid;
	net_setup.client.latency=0;
	strcpy(net_setup.client.joined_ip,"127.0.0.1");

		// setup game from host
		
	strcpy(net_setup.client.game_name,net_setup.host.game_name);
	
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,net_setup.host.map_name);
	
		// start game
	
	host_close();
	
	if (!game_start(skill_medium,0,NULL,err_str)) {
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}
	
		// start the map
		
	if (!map_start(FALSE,err_str)) {
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}

		// start local client network queue
		
	if (!net_client_start_message_queue_local(err_str)) {
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}

		// mark as ready to receive data from host
		
	net_client_send_ready(remote_uid);

		// game is running
	
	server.state=gs_running;
}

/* =======================================================

      Host Input
      
======================================================= */

void host_click(void)
{
	int			id;
	bool		enable;
	
		// is element being clicked
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run selection

	switch (id) {
	
		case host_button_host_id:
			host_game_setup();
			host_close();
			host_game();
			break;
			
		case host_button_cancel_id:
			host_close();
			intro_open();
			break;

		case host_table_id:
			enable=(element_get_value(host_table_id)!=-1);
			element_enable(host_button_host_id,enable);
			break;
	}
}
	
/* =======================================================

      Run Host
      
======================================================= */

void host_run(void)
{
	gui_draw(1.0f,TRUE);
	host_click();
}

