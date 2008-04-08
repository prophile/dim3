/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Host

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

#define host_button_host_id				0
#define host_button_cancel_id			1
#define host_game_type_id				2
#define host_table_id					3
#define host_status_id					4

extern void intro_open(void);
extern bool game_start(int skill,int remote_count,network_request_remote_add *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

char						net_game_types[][32]={"Death Match","Team Death Match","CTF",""};	// supergumba -- not static

/* =======================================================

      Join Operations
      
======================================================= */

void host_open(void)
{
	int							x,y,high,list_wid,list_high,control_y_add,
								n,nfile,sz;
	char						path[1024],path2[1024];
	char						*c,*row_data;
	element_column_type			cols[1];
	file_path_directory_type	*map_pick_fpd;
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","host");
	
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

	y=high+30;
	control_y_add=element_get_control_high();
	
	element_combo_add("Game Type",(char*)net_game_types,0,host_game_type_id,85,y,TRUE);
	y+=control_y_add;

		// hosts table
		
	list_wid=setup.screen.x_scale-30;
	list_high=setup.screen.y_scale-((high+30)+85);

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,host_table_id,1,15,y,list_wid,list_high);
	
		// fill table with maps

	map_pick_fpd=file_paths_read_directory(&setup.file_path_setup,"Maps","xml",FALSE);

	nfile=map_pick_fpd->nfile;

	if (nfile>0) {
		sz=(nfile+1)*128;
		
		row_data=valloc(sz);
		bzero(row_data,sz);

		c=row_data;
		
		for (n=0;n!=nfile;n++) {
			strcpy(c,map_pick_fpd->files[n].file_name);
			c+=128;
		}
		
		element_set_table_data(host_table_id,row_data);
		free(row_data);
	}

	file_paths_close_directory(map_pick_fpd);
	
		// status
		
	y=setup.screen.y_scale-24;
	element_text_add("",host_status_id,15,y,tx_left,TRUE,FALSE,FALSE);
	
	server.state=gs_host;
}

void host_close(void)
{
	gui_shutdown();
}

/* =======================================================

      Host Game
      
======================================================= */

/* supergumba
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
	strcpy(net_setup.client.joined_ip,join_list[idx].ip);
							
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
	
	if (!game_start(skill_medium,remote_count,remotes,err_str)) {
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
*/

void host_cancel(void)
{
	gui_shutdown();
	intro_open();
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
		//	join_game();
			break;
			
		case host_button_cancel_id:
			host_cancel();
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

