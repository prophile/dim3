/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Start/Stop

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

#include "client.h"
#include "consoles.h"
#include "interfaces.h"

extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern bool server_game_start(char *game_script_name,int remote_count,network_request_remote_add *remotes,char *err_str);
extern void server_game_stop(void);
extern void view_game_start(void);
extern void view_game_stop(void);

/* =======================================================

      Game Start and End
      
======================================================= */

bool game_start(int remote_count,network_request_remote_add *remotes,char *err_str)
{
		// pause time
		
	game_time_pause_start();
	
		// reset random numbers
		
	random_reset();
	
		// start progress
	
	console_add_system("Starting Game");
		
	progress_initialize("Starting");
	progress_draw(0);

		// start server
	
	if (!server_game_start("Game",remote_count,remotes,err_str)) {
		return(FALSE);
	}
	
		// start view
		
	view_game_start();

		// game in running state
		
	server.game_open=TRUE;
	server.state=gs_running;
	
	game_time_pause_end();
	
	return(TRUE);
}

void game_end(void)
{
	console_add_system("Closing Game");
	
		// blank progress
		
	progress_initialize("Ending");
	progress_draw(0);
	
		// close any network joins
	
	if (net_setup.client.joined) network_client_leave_host(net_setup.client.remote_uid);
	
		// stop view
		
	view_game_stop();

		// stop server
		
	server_game_stop();
	
		// game closed
		
	server.game_open=FALSE;
}

