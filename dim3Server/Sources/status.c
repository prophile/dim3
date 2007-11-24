/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Status Window

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

#define STATUS_INFO_Y			0
#define STATUS_GAME_Y			8
#define STATUS_PLAYER_Y			0
#define STATUS_MESSAGE_Y		26

#define STATUS_MAX_GAMES		16

#define STATUS_MAX_MESSAGE		12
#define STATUS_MESSAGE_SIZE		77

extern network_setup_type		net_setup;

float							status_col[STATUS_MAX_MESSAGE][3];
char							status_msg[STATUS_MAX_MESSAGE][STATUS_MESSAGE_SIZE];
pthread_mutex_t					status_lock;

/* =======================================================

      Status Page Initialize
      
======================================================= */

void status_initialize(void)
{
	int			n;

	for (n=0;n!=STATUS_MAX_MESSAGE;n++) {
		status_msg[n][0]=0x0;
		status_col[n][0]=status_col[n][1]=status_col[n][2]=0.0f;
	}

	pthread_mutex_init(&status_lock,NULL);
}

void status_shutdown(void)
{
	pthread_mutex_destroy(&status_lock);
}

/* =======================================================

      Determine IP Type
      
======================================================= */

void status_determine_ip_type(char *str,char *str2)
{
	char			*ip;

		// determine IP type

	str[0]=0x0;
	str2[0]=0x0;

	ip=net_setup.host.ip_resolve;
	
		// local?
		
	if (strcmp(ip,"127.0.0.1")==0) {
		strcpy(str,"This is a local address that");
		strcpy(str2,"can only be hit on this machine");
		return;
	}
	
		// lan or wan?
		
	if (network_ip_local(ip)) {
		strcpy(str,"This is a LAN address that can");
		strcpy(str2,"only be hit on the local network");
	}
	else {
		strcpy(str,"This is a WAN address that can");
		strcpy(str2,"can be hit from the internet");
	}
}

/* =======================================================

      Draw Status Page
      
======================================================= */

void status_draw_sections(void)
{
	ui_draw_section(0,STATUS_INFO_Y,42,8,"SERVER");
	ui_draw_section(0,STATUS_GAME_Y,42,(STATUS_MAX_GAMES+2),"GAMES");
	ui_draw_section(42,STATUS_PLAYER_Y,37,(net_max_remote_count+2),"PLAYERS");
	ui_draw_section(0,(STATUS_MESSAGE_Y),79,(STATUS_MAX_MESSAGE+2),"MESSAGES");
}

void status_draw_info(void)
{
	char			str[64],str2[64];

		// version and IPs

	ui_draw_text(1,1,9," Version:",NULL,FALSE);
	ui_draw_text(10,1,29,dim3_version,NULL,FALSE);

	ui_draw_text(1,2,9,"    Name:",NULL,FALSE);
	ui_draw_text(10,2,29,net_setup.host.name,NULL,FALSE);

	ui_draw_text(1,3,9,"      IP:",NULL,FALSE);
	ui_draw_text(10,3,29,net_setup.host.ip_name,NULL,FALSE);

	ui_draw_text(1,4,9,"Resolved:",NULL,FALSE);
	ui_draw_text(10,4,29,net_setup.host.ip_resolve,NULL,FALSE);

	status_determine_ip_type(str,str2);
	ui_draw_text(1,5,9,"    Type:",NULL,FALSE);
	ui_draw_text(10,5,29,str,NULL,FALSE);
	ui_draw_text(10,6,29,str2,NULL,FALSE);
}

void status_draw_games(void)
{
	int					n,y;
	bool				invert;

	y=STATUS_GAME_Y+1;
	
	for (n=0;n!=STATUS_MAX_GAMES;n++) {

		if (n<net_setup.ngame) {
			invert=(net_setup.host.game_idx==n);

			ui_draw_text(1,y,20,net_setup.games[n].script_name,NULL,invert);
			ui_draw_text(21,y,20,net_setup.games[n].map_name,NULL,invert);
		}
		else {
			ui_draw_text(1,y,40,NULL,NULL,FALSE);
		}

		y++;
	}
}

void status_draw_players(void)
{
	int							n,cnt,y;
	char						str[32];
	server_player_info_type		*player_info,
								player_infos[net_max_remote_count];
	
		// get list from player routines
		// so it can be locked from changes

	cnt=server_player_info_list(player_infos);

		// draw list

	player_info=player_infos;
	y=STATUS_PLAYER_Y+1;

	for (n=0;n!=net_max_remote_count;n++) {

		if (n<cnt) {
			ui_draw_text(43,y,29,player_info->name,player_info->col,FALSE);
			sprintf(str,"%.5d",player_info->score);
			ui_draw_text(73,y,5,str,player_info->col,FALSE);
		}
		else {
			ui_draw_text(43,y,35,NULL,NULL,FALSE);
		}

		player_info++;
		y++;
	}
}

void status_draw_messages(void)
{
	int			n,y;

	y=STATUS_MESSAGE_Y+1;

	for (n=0;n!=STATUS_MAX_MESSAGE;n++) {
		ui_draw_text(1,y,STATUS_MESSAGE_SIZE,status_msg[n],status_col[n],FALSE);
		y++;
	}
}

void status_draw(void)
{
	status_draw_sections();
	status_draw_info();
	status_draw_games();
	status_draw_players();
	status_draw_messages();
}

/* =======================================================

      Status Changes
      
======================================================= */

void status_player_message(char *name,char *str,float *col)
{
#ifndef D3_SERVER_HEADLESS

	int			sz;
	
		// lock drawing
		
	pthread_mutex_lock(&status_lock);

		// move other messages up

	sz=(STATUS_MAX_MESSAGE-1)*STATUS_MESSAGE_SIZE;
	memmove((char*)status_msg[0],(char*)status_msg[1],sz);

	sz=(STATUS_MAX_MESSAGE-1)*(3*sizeof(float));
	memmove((char*)status_col[0],(char*)status_col[1],sz);

		// put message on bottom

	if (name==NULL) {
		strncpy(status_msg[STATUS_MAX_MESSAGE-1],str,STATUS_MESSAGE_SIZE);
	}
	else {
		snprintf(status_msg[STATUS_MAX_MESSAGE-1],STATUS_MESSAGE_SIZE,"[%s] %s",name,str);
	}

	status_msg[STATUS_MAX_MESSAGE-1][STATUS_MESSAGE_SIZE-1]=0x0;

	if (col==NULL) {
		status_col[STATUS_MAX_MESSAGE-1][0]=status_col[STATUS_MAX_MESSAGE-1][1]=status_col[STATUS_MAX_MESSAGE-1][2]=0.0f;
	}
	else {
		memmove(status_col[STATUS_MAX_MESSAGE-1],col,(3*sizeof(float)));
	}
	
		// redraw messages

	status_draw_messages();
	
		// unlock drawing
		
	pthread_mutex_unlock(&status_lock);

#endif
}

void status_player_update(void)
{
#ifndef D3_SERVER_HEADLESS
	pthread_mutex_lock(&status_lock);
	
	status_draw_players();

	pthread_mutex_unlock(&status_lock);
#endif
}

/* =======================================================

      Get Click Area
      
======================================================= */

int status_get_game_idx(int x,int y)
{
	int			idx;

	if ((y<(STATUS_GAME_Y+1)) || (y>(STATUS_GAME_Y+11))) return(-1);
	if ((x<1) || (x>40)) return(-1);

	idx=y-(STATUS_GAME_Y+1);
	if ((idx<0) || (idx>net_setup.ngame)) return(-1);

	return(idx);
}

/* =======================================================

      Status Commands
      
======================================================= */

void status_change_game(int game_idx)
{
	net_setup.host.game_idx=game_idx;
	server_player_send_all_packet(net_action_request_host_reset,net_queue_mode_normal,NULL,0,FALSE);		// force all players to reset
}

void status_click(int x,int y)
{
	int			game_idx;

		// check game clicking first

	game_idx=status_get_game_idx(x,y);
	if (game_idx!=-1) {
		status_change_game(game_idx);
		status_draw_games();
		return;
	}

		// todo -- player killing


}

