/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Network Host Main Code

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

#include <signal.h>

file_path_setup_type		file_path_setup;
network_setup_type			net_setup;

int							up_time;
bool						quit;

/* =======================================================

      Error Message
      
======================================================= */

void server_error(char *err_str)
{
#ifdef D3_OS_MAC
	unsigned char		p_str[256];
	
	CopyCStringToPascal(err_str,p_str);
	StandardAlert(0,"\pFatal Error",p_str,NULL,NULL);
#endif

#ifdef D3_OS_LINUX
	printf("Fatal Error: %s\n",err_str);
#endif

#ifdef D3_OS_WINDOWS
	MessageBox(NULL,err_str,"Fatal Error",MB_OK);
#endif
}

/* =======================================================

      Run Modes
      
======================================================= */

void run_headless(void)
{
	char			err_str[256],err_str_2[256];

#ifdef D3_OS_MAC
	EventRecord		evt;
#endif

		// start query/command thread

	if (!server_query_initialize(err_str)) {
		sprintf(err_str_2,"Could not initialize query interface\n%s\n",err_str);
		server_error(err_str_2);
		return;
	}

		// if on a mac, need to pump UI
		// to stop bounce

	while (!quit) {

#ifdef D3_OS_MAC
		GetNextEvent(0xFFFF,&evt);
#endif

		usleep(250000);
	}

		// end query/command thread

	server_query_shutdown();
}

void run_with_ui(void)
{
	status_initialize();

	ui_start();
	status_draw();

	ui_pump();
	ui_end();

	status_shutdown();
}

/* =======================================================

      Defualt Games
      
======================================================= */

bool setup_default_game(char *err_str)
{
		// user first game as default start

	if (net_setup.ngame==0) {
		strcpy(err_str,"There are no game definitions, server can not start");
		return(FALSE);
	}

	net_setup.host.game_idx=0;

	return(TRUE);
}

/* =======================================================

      Main Code
      
======================================================= */

int main(int argc,char* argv[])
{
	char			err_str[256],err_str_2[256];
	
	time_start();
	up_time=time_get();

		// setup paths

	file_paths_setup(&file_path_setup,TRUE);

		// read network.xml

	if (!network_xml_read(&file_path_setup,&net_setup)) {
		strcpy(err_str,"Could not read network.xml\n");
		server_error(err_str);
		return(0);
	}
	
		// default games
		
	if (!setup_default_game(err_str)) {
		server_error(err_str);
		return(0);
	}
	
		// initialize players
		
	server_player_initialize();

		// start networking

	network_initialize();
	
		// resolve names to IPs
		
	if (net_setup.host.name[0]==0x0) network_get_host_name(net_setup.host.name);
	network_get_host_ip(net_setup.host.ip_name,net_setup.host.ip_resolve);

		// start hosting

	if (!host_initialize(err_str)) {
		network_shutdown();
		server_player_shutdown();
		snprintf(err_str_2,256,"Could not initialize host interface\n%s\n",err_str);
		err_str_2[255]=0x0;
		server_error(err_str_2);
		return(0);
	}
	
	if (!broadcast_initialize(err_str)) {
		host_shutdown();
		network_shutdown();
		server_player_shutdown();
		snprintf(err_str_2,256,"Could not initialize broadcast listener interface\n%s\n",err_str);
		err_str_2[255]=0x0;
		server_error(err_str_2);
		return(0);
	}

		// run server

#ifdef D3_SERVER_HEADLESS
	run_headless();
#else
	run_with_ui();
#endif

		// inform all player of server shutdown

	server_player_send_all_packet(net_action_request_host_exit,net_queue_mode_normal,NULL,0,FALSE);

		// shutdown server

	broadcast_shutdown();
	host_shutdown();
	
	network_shutdown();
	
	server_player_shutdown();

	return(0);
}

