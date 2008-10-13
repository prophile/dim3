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
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							net_game_current_game_type_index;
char						*net_host_file_list;
char						net_game_types[network_setup_max_game+1][32];

/* =======================================================

      Join Operations
      
======================================================= */

void host_fill_map_table(char *game_type)
{
	int							n,nfile,sz;
	char						*c;
	file_path_directory_type	*map_pick_fpd;

		// need to make sure map paths are correct

	map_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_compression);

		// load in all maps with the correct game type

	map_pick_fpd=file_paths_read_directory(&setup.file_path_setup,"Maps","xml",FALSE);

	nfile=map_pick_fpd->nfile;

	if (nfile>0) {
		sz=(nfile+1)*128;
		
		net_host_file_list=valloc(sz);
		bzero(net_host_file_list,sz);

		c=net_host_file_list;
		
		for (n=0;n!=nfile;n++) {
			if (map_check_game_type(game_type,map_pick_fpd->files[n].file_name)) {
				sprintf(c,"Maps;%s;%s",map_pick_fpd->files[n].file_name,map_pick_fpd->files[n].file_name);
				c+=128;
			}
		}
		
		element_set_table_data(host_table_id,net_host_file_list);
	}
	else {
		net_host_file_list=NULL;
	}

	file_paths_close_directory(map_pick_fpd);
}

void host_open(void)
{
	int							n,x,y,wid,high,padding;
	element_column_type			cols[1];
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);
	
		// controls
							
	element_clear();
	
		// title
		
	y=(int)(((float)hud.scale_y)*0.09f);
	element_text_add("Host Multiplayer Game",-1,5,y,tx_left,FALSE,FALSE,FALSE);
	
		// game type

	for (n=0;n!=net_setup.ngame;n++) {
		strcpy(net_game_types[n],net_setup.games[n].name);
	}
	net_game_types[net_setup.ngame][0]=0x0;

	x=(int)(((float)hud.scale_x)*0.15f);
	y=(int)(((float)hud.scale_y)*0.17f);
	
	element_combo_add("Game Type",(char*)net_game_types,0,host_game_type_id,x,y,TRUE);
	y+=element_get_control_high();

		// hosts table
		
	x=(int)(((float)hud.scale_x)*0.03f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.83f)-y;

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,host_table_id,1,x,y,wid,high,element_table_bitmap_data);
	
		// fill table with maps

	net_game_current_game_type_index=0;
	host_fill_map_table(net_setup.games[0].name);
	
		// status
		
	y=hud.scale_y-24;
	element_text_add("",host_status_id,15,y,tx_left,TRUE,FALSE,FALSE);
	
		// buttons
		
	padding=element_get_padding();
	
	wid=(int)(((float)hud.scale_x)*0.1f);
	high=(int)(((float)hud.scale_x)*0.05f);
	
	x=hud.scale_x-padding;
	y=hud.scale_y-padding;
	
	element_button_text_add("Host",host_button_host_id,x,y,wid,high,element_pos_right,element_pos_bottom);
	element_enable(host_button_host_id,FALSE);

	x=element_get_x_position(host_button_host_id)-padding;

	element_button_text_add("Cancel",host_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);

		// in join thread
	
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
	char			*c;
	
	idx=element_get_value(host_game_type_id);
	strcpy(net_setup.host.game_name,net_setup.games[idx].name);
	
	net_setup.host.map_name[0]=0x0;
	
	idx=element_get_value(host_table_id);
	
		// need to remove graphic name
				
	c=net_host_file_list+(idx*128);
	
	c=strchr(c,';');
	if (c!=NULL) {
		c=strchr((c+1),';');
		if (c!=NULL) {
			strcpy(net_setup.host.map_name,(c+1));
		}
	}
}

void host_game(void)
{
	int						remote_uid;
	char					err_str[256];
	network_request_join	request_join;
	
		// start hosting
		
	if (!net_host_game_start(err_str)) {
		error_open(err_str,"Hosting Game Canceled");
		return;
	}
	
		// attempt to connect to local server

	strcpy(request_join.name,setup.network.name);
	strcpy(request_join.vers,dim3_version);

	remote_uid=net_host_client_handle_local_join(&request_join,err_str);
	if (remote_uid==-1) {
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
	int			id,idx;
	bool		enable;
	
		// is element being clicked
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run selection

	switch (id) {

		case host_game_type_id:
			idx=element_get_value(host_game_type_id);
			if (idx!=net_game_current_game_type_index) {
				net_game_current_game_type_index=idx;
				host_fill_map_table(net_setup.games[idx].name);
				element_set_value(host_table_id,-1);
				element_enable(host_button_host_id,FALSE);
			}
			break;

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

