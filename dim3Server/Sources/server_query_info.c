/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Server Query Info

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

extern int						server_player_count,up_time;
extern server_player_type		server_players[net_max_remote_count];

extern network_setup_type		net_setup;

extern pthread_mutex_t			server_player_lock;

char team_colors[][16]=net_team_color_list_def;

/* =======================================================

      Describe Utilities
      
======================================================= */

void server_query_describe_ip(char *str)
{
	char			*ip;

	str[0]=0x0;
	ip=net_setup.host.ip_resolve;
	
		// local?
		
	if (strcmp(ip,"127.0.0.1")==0) {
		strcpy(str,"This is a local address that can only be hit on this machine");
		return;
	}
	
		// lan or wan?
		
	if (network_ip_local(ip)) {
		strcpy(str,"This is a LAN address that can only be hit on the local network");
	}
	else {
		strcpy(str,"This is a WAN address that can be hit from the internet");
	}
}

void server_query_describe_up_time(char *str)
{
	int				k,sec,min,hour;
	
	k=time_get()-up_time;
	
	sec=k/1000;
	min=sec/60;
	sec=sec%60;
	hour=min/60;
	min=min%60;
	
	sprintf(str,"%.2d:%.2d.%.2d",hour,min,sec);
}	

/* =======================================================

      HTML Server Info

======================================================= */

char* server_query_html_info(void)
{
	int					n;
	char				*data;
	char				str[256];
	server_player_type	*player;

		// memory for html

	data=(char*)malloc(1024*64);
	if (data==NULL) return(NULL);

	*data=0x0;

		// outside table

	strcat(data,"<table width=\"800\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">");

		// title

	strcat(data,"<tr height=\"30\">");
	strcat(data,"<td align=\"center\" valign=\"middle\"><b>dim3 Server</b></td>");
	strcat(data,"</tr>");

		// controls

	strcat(data,"<tr height=\"30\">");
	strcat(data,"<td align=\"center\" valign=\"middle\">");

	strcat(data,"<a href=\"?password=");
	strcat(data,net_setup.host.password);
	strcat(data,"\">Refresh Info</a>");

	strcat(data,"&nbsp;&nbsp;&nbsp;");

	strcat(data,"<a href=\"?password=");
	strcat(data,net_setup.host.password);
	strcat(data,"&action=quit\">Shutdown Server</a>");

	strcat(data,"</td></tr>");

		// info

	strcat(data,"<tr><td><table width=\"800\" border=\"1\" cellspacing=\"0\" cellpadding=\"3\">");

	strcat(data,"<tr><td align=\"center\" valign=\"middle\" colspan=\"2\"><b>Info</b></td></tr>");
	strcat(data,"<tr height=\"2\"></tr>");

	strcat(data,"<tr>");
	strcat(data,"<td align=\"right\">Version:</td>");
	strcat(data,"<td>");
	strcat(data,dim3_version);
	strcat(data,"</td>");
	strcat(data,"</tr>");
	
	strcat(data,"<tr>");
	strcat(data,"<td align=\"right\">Up Time:</td>");
	strcat(data,"<td>");
	server_query_describe_up_time(str);
	strcat(data,str);
	strcat(data,"</td>");
	strcat(data,"</tr>");

	strcat(data,"<tr>");
	strcat(data,"<td align=\"right\">Given IP/Name:</td>");
	strcat(data,"<td>");
	strcat(data,net_setup.host.ip_name);
	strcat(data,"</td>");
	strcat(data,"</tr>");

	strcat(data,"<tr>");
	strcat(data,"<td align=\"right\">Resolved IP:</td>");
	strcat(data,"<td>");
	strcat(data,net_setup.host.ip_resolve);
	strcat(data,"</td>");
	strcat(data,"</tr>");

	strcat(data,"<tr>");
	strcat(data,"<td align=\"right\">IP Type:</td>");
	strcat(data,"<td>");
	server_query_describe_ip(str);
	strcat(data,str);
	strcat(data,"</td>");
	strcat(data,"</tr>");

	strcat(data,"</td></tr></table>");
	strcat(data,"<br />");

		// games

	strcat(data,"<tr><td><table width=\"800\" border=\"1\" cellspacing=\"0\" cellpadding=\"3\">");

	strcat(data,"<tr><td align=\"center\" valign=\"middle\" colspan=\"4\">");
	strcat(data,"<b>Games</b>");
	strcat(data,"</td></tr>");
	strcat(data,"<tr height=\"2\"></tr>");

	for (n=0;n!=net_setup.ngame;n++) {
		strcat(data,"<tr>");
		strcat(data,"<td width=\"30\" align=\"right\">");
		if (net_setup.host.game_idx==n) {
			strcat(data,"<b>>></b>");
		}
		else {
			strcat(data,"&nbsp;");
		}
		strcat(data,"</td>");
		strcat(data,"<td>");
		strcat(data,net_setup.games[n].script_name);
		strcat(data,"</td>");
		strcat(data,"<td>");
		strcat(data,net_setup.games[n].map_name);
		strcat(data,"</td>");
		strcat(data,"<td width=\"100\">");
		
		if (net_setup.host.game_idx==n) {
			strcat(data,"[Running]");
		}
		else {
			strcat(data,"<a href=\"?password=");
			strcat(data,net_setup.host.password);
			strcat(data,"&action=game&index=");
			sprintf(str,"%.2d",n);
			strcat(data,str);
			strcat(data,"\">Start</a>");
		}
		
		strcat(data,"</td>");
		strcat(data,"</tr>");
	}

	strcat(data,"</td></tr></table>");
	strcat(data,"<br />");

		// players

	pthread_mutex_lock(&server_player_lock);		// need to lock player list

	strcat(data,"<tr><td><table width=\"800\" border=\"1\" cellspacing=\"0\" cellpadding=\"3\">");

	strcat(data,"<tr><td align=\"center\" valign=\"middle\" colspan=\"4\">");
	strcat(data,"<b>Players</b><br />");
	sprintf(str,"%d of %d",server_player_count,net_max_remote_count);
	strcat(data,str);
	strcat(data,"</td></tr>");
	strcat(data,"<tr height=\"2\"></tr>");

	player=server_players;

	for (n=0;n!=server_player_count;n++) {

		strcat(data,"<tr>");
		strcat(data,"<td>");
		strcat(data,player->name);
		strcat(data,"</td>");
		strcat(data,"<td>");
		strcat(data,team_colors[player->team_idx]);
		strcat(data,"</td>");
		strcat(data,"<td align=\"center\">");
		sprintf(str,"%d",player->score);
		strcat(data,str);
		strcat(data,"</td>");
		strcat(data,"<td width=\"100\">");
		strcat(data,"[Kill Link]");
		strcat(data,"</td>");
		strcat(data,"</tr>");

		player++;
	}

	strcat(data,"</td></tr></table>");
	strcat(data,"<br />");

	pthread_mutex_unlock(&server_player_lock);

		// end table

	strcat(data,"</table>");

	return(data);
}

