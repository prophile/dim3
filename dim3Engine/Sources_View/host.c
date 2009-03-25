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
#include "objects.h"
#include "remotes.h"
#include "interfaces.h"
#include "xmls.h"
#include "video.h"

#define host_pane_game					0
#define host_pane_options				1

#define host_tab_id						0

#define host_button_host_id				10
#define host_button_cancel_id			11
#define host_game_type_id				12
#define host_table_id					13
#define host_status_id					14

#define host_game_bot_count_id			20
#define host_game_bot_skill_id			21

#define host_game_score_limit_id		22

extern void intro_open(void);
extern bool net_host_game_start(char *err_str);
extern void net_host_game_end(void);
extern bool game_start(int skill,network_reply_join_remotes *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);

extern map_type				map;
extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							host_tab_value,host_map_idx;
char						*net_host_file_list;
char						net_game_types[network_setup_max_game+1][32],
							bot_count_list[max_multiplayer_bot+2][32],
							bot_skill_list[6][32]={"Very Easy","Easy","Normal","Hard","Very Hard",""};

/* =======================================================

      Host Map Table
      
======================================================= */

void host_fill_map_table(char *game_type)
{
	int							n,nfile,sz;
	char						*c,info_name[name_str_len];
	file_path_directory_type	*map_pick_fpd;

		// need to make sure map paths are correct

	map_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_compression);

		// load in all maps with the correct game type

	map_pick_fpd=file_paths_read_directory_data(&setup.file_path_setup,"Maps","xml");

	nfile=map_pick_fpd->nfile;

	if (nfile>0) {
		sz=(nfile+1)*128;
		
		net_host_file_list=malloc(sz);
		bzero(net_host_file_list,sz);

		c=net_host_file_list;
		
		for (n=0;n!=nfile;n++) {
			if (map_check_game_type(game_type,map_pick_fpd->files[n].file_name,info_name)) {
				sprintf(c,"Bitmaps/Icons_Map;%s;%s",map_pick_fpd->files[n].file_name,info_name);
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

/* =======================================================

      Set and Get Last Host Map
      
======================================================= */

void host_set_last_map(void)
{
	int				idx;
	char			*list,*c;
	char			str[256];
	
	idx=0;
	list=net_host_file_list;
	
	while (TRUE) {
		if (*list==0x0) break;
		
		c=list;
		list+=128;
		
		c=strchr(c,';');
		if (c!=NULL) {
			strcpy(str,(c+1));
			c=strchr(str,';');
			if (c!=NULL) *c=0x0;
		}
		
		if (strcasecmp(setup.network.last_map,str)==0) {
			host_map_idx=idx;
			return;
		}
		
		idx++;
	}
	
	host_map_idx=-1;
}

void host_get_last_map(void)
{
	char			*c;
	char			str[256];
	
	if (host_map_idx==-1) {
		setup.network.last_map[0]=0x0;
		return;
	}
	
	c=net_host_file_list+(host_map_idx*128);
	
	c=strchr(c,';');
	if (c!=NULL) {
		strcpy(str,(c+1));
		c=strchr(str,';');
		if (c!=NULL) *c=0x0;
	}
	
	strcpy(setup.network.last_map,str);
}

/* =======================================================

      Host Panes
      
======================================================= */

void host_game_pane(void)
{
	int						n,x,y,wid,high;
	element_column_type		cols[1];
	
		// game type

	for (n=0;n!=net_setup.ngame;n++) {
		strcpy(net_game_types[n],net_setup.games[n].name);
	}
	net_game_types[net_setup.ngame][0]=0x0;

	x=(int)(((float)hud.scale_x)*0.15f);
	y=(int)(((float)hud.scale_y)*0.17f);
	
	element_combo_add("Game Type",(char*)net_game_types,setup.network.game_type,host_game_type_id,x,y,TRUE);
	y+=element_get_padding();

		// hosts table
		
	x=(int)(((float)hud.scale_x)*0.03f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.83f)-y;

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,host_table_id,1,x,y,wid,high,element_table_bitmap_data);
	
		// fill table with maps

	host_fill_map_table(net_setup.games[setup.network.game_type].name);
	host_set_last_map();

	element_set_value(host_table_id,host_map_idx);
	element_make_selection_visible(host_table_id);
}

void host_options_pane(void)
{
	int				n,x,y;
	char			str[32];

	x=(int)(((float)hud.scale_x)*0.15f);
	y=(int)(((float)hud.scale_y)*0.17f);
	
	for (n=0;n!=(max_multiplayer_bot+1);n++) {
		if (n==0) {
			strcpy(bot_count_list[n],"None");
		}
		else {
			sprintf(bot_count_list[n],"%d",n);
		}
	}
	bot_count_list[max_multiplayer_bot+1][0]=0x0;
	
	element_combo_add("Bot Count",(char*)bot_count_list,setup.network.bot.count,host_game_bot_count_id,x,y,TRUE);
	y+=element_get_control_high();

	element_combo_add("Bot Skill",(char*)bot_skill_list,setup.network.bot.skill,host_game_bot_skill_id,x,y,TRUE);
	y+=element_get_control_high();

	sprintf(str,"%d",setup.network.score_limit);
	element_text_field_add("Score Limit:",str,32,host_game_score_limit_id,x,y,TRUE);
}

void host_create_pane(void)
{
	int			x,y,wid,high,yadd,padding,
				tab_list_wid,tab_pane_high,pane;
	char		str[256],str2[256],
				tab_list[][32]={"Host Game","Options"};
							
	element_clear();
	
		// tabs
		
	padding=element_get_padding();;
	
	wid=hud.scale_x;
	yadd=(int)(((float)hud.scale_y)*0.015f);
	high=(int)(((float)hud.scale_y)*0.065f);
	tab_list_wid=(int)(((float)hud.scale_x)*0.85f);
	tab_pane_high=(int)(((float)hud.scale_y)*0.82f);
	
	element_tab_add((char*)tab_list,host_tab_value,host_tab_id,2,0,(padding+yadd),wid,high,tab_list_wid,tab_pane_high);
	
		// status
		// start with IP information

	if (net_setup.host.name[0]==0x0) net_get_host_name(net_setup.host.name);
	net_get_host_ip(net_setup.host.ip_name,net_setup.host.ip_resolve);

	if (strcmp(net_setup.host.ip_resolve,"127.0.0.1")==0) {
		strcpy(str,"Local Machine");
	}
	else {
		if (net_ip_local(net_setup.host.ip_resolve)) {
			strcpy(str,"Local Network");
		}
		else {
			strcpy(str,"Internet");
		}
	}

	sprintf(str2,"; %s; %s:%d-%d",net_setup.host.name,net_setup.host.ip_resolve,net_port_host,net_port_host_query);
	strcat(str,str2);

	padding=element_get_padding();
	high=(int)(((float)hud.scale_x)*0.05f);
		
	y=hud.scale_y-((padding+(high/2))-(element_get_control_high()/2));
	element_text_add(str,host_status_id,15,y,hud.font.text_size_small,tx_left,FALSE,FALSE);
	
		// buttons
		
	wid=(int)(((float)hud.scale_x)*0.1f);

	x=hud.scale_x-padding;
	y=hud.scale_y-padding;
	
	element_button_text_add("Host",host_button_host_id,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(host_button_host_id)-padding;

	element_button_text_add("Cancel",host_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);
	
		// specific pane controls
		
	pane=element_get_value(host_tab_id);
		
	switch (pane) {
		case host_pane_game:
			host_game_pane();
			break;
		case host_pane_options:
			host_options_pane();
			break;
	}
	
		// enable host button
		
	element_enable(host_button_host_id,(host_map_idx!=-1));
}

/* =======================================================

      Open/Close Host Game
      
======================================================= */

void host_open(void)
{
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);

		// start with first tab
		
	host_tab_value=0;
	host_map_idx=-1;

	host_create_pane();

		// in host thread
	
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
	char			*c;
	
		// game type
		
	net_setup.game_idx=setup.network.game_type;
	
		// use graphic name to get to original map name
		
	net_setup.host.map_name[0]=0x0;
				
	c=net_host_file_list+(host_map_idx*128);
	
	c=strchr(c,';');
	if (c!=NULL) {
		strcpy(net_setup.host.map_name,(c+1));
		c=strchr(net_setup.host.map_name,';');
		if (c!=NULL) *c=0x0;
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
	net_setup.client.latency=0;
	strcpy(net_setup.client.joined_ip,"127.0.0.1");

		// setup game from host
		
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,net_setup.host.map_name);
	
		// start game
	
	if (!game_start(skill_medium,NULL,err_str)) {
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}
	
		// start the map
		
	if (!map_start(TRUE,err_str)) {
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}
	
	object_player_set_remote_uid(remote_uid);

		// add bots to player list

	net_host_player_add_bots_to_list();

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

void host_handle_click(int id)
{
	int			idx;
	char		str[32];
	
		// run selection

	switch (id) {

			// tab
			
		case host_tab_id:
			host_tab_value=element_get_value(host_tab_id);
			host_create_pane();
			return;

			// controls

		case host_game_type_id:
			idx=element_get_value(host_game_type_id);
			if (idx!=setup.network.game_type) {
				setup.network.game_type=idx;
				host_fill_map_table(net_setup.games[idx].name);
				host_set_last_map();
				element_set_value(host_table_id,host_map_idx);
				element_make_selection_visible(host_table_id);
				element_enable(host_button_host_id,(host_map_idx!=-1));
			}
			break;

		case host_table_id:
			host_map_idx=element_get_value(host_table_id);
			element_enable(host_button_host_id,(host_map_idx!=-1));
			host_get_last_map();
			break;

		case host_game_bot_count_id:
			setup.network.bot.count=element_get_value(host_game_bot_count_id);
			break;

		case host_game_bot_skill_id:
			setup.network.bot.skill=element_get_value(host_game_bot_skill_id);
			break;

		case host_game_score_limit_id:
			element_get_value_string(host_game_score_limit_id,str);
			setup.network.score_limit=atoi(str);
			if (setup.network.score_limit<0) setup.network.score_limit=0;
			break;

			// buttons

		case host_button_host_id:
			setup_xml_write();
			host_game_setup();
			host_close();
			host_game();
			break;
			
		case host_button_cancel_id:
			setup_xml_write();
			host_close();
			intro_open();
			break;
	}
}


void host_keyboard(void)
{
	int			id;

	id=gui_keyboard();
	if (id!=-1) host_handle_click(id);
}

void host_click(void)
{
	int			id;
	
	id=gui_click();
	if (id==-1) return;
	
	hud_click();

	host_handle_click(id);
}

/* =======================================================

      Run Host
      
======================================================= */

void host_run(void)
{
	gui_draw(1.0f,TRUE);
	host_click();
	host_keyboard();
}

