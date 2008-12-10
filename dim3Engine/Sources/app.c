/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: App Start/Stop

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

#include "consoles.h"
#include "interfaces.h"
#include "effects.h"
#include "video.h"
#include "inputs.h"
#include "xmls.h"

extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void game_time_initialize(void);
extern bool server_initialize(char *err_str);
extern void server_shutdown(void);
extern void server_loop(int tick);
extern bool view_initialize(char *err_str);
extern void view_shutdown(void);
extern void view_loop(int tick);
extern bool game_start(int skill,int remote_count,network_request_remote_add *remotes,char *err_str);
extern void game_end(void);
extern bool map_start(bool skip_media,char *err_str);
extern void map_end(void);
extern void intro_open(void);

/* =======================================================

      App Start and End
      
======================================================= */

bool app_start(char *err_str)
{
		// initialize timing

	game_time_initialize();
	
		// math utility initialization
		
	fast_trig_setup();
	
		// OS network initialization
		
	network_initialize();
	
		// start console
		
	console_initialize();
	console_add_system("Starting App");
	
		// read setup preferences
		
	setup_xml_read();
	
		// client network defaults
		
	net_setup.host.hosting=FALSE;
	net_setup.client.joined=FALSE;
	
		// initialize server
		
	if (!server_initialize(err_str)) return(FALSE);

		// initialize view
	
	if (!view_initialize(err_str)) {
		server_shutdown();
		return(FALSE);
	}
	
		// if no editor launch, start in intro
		
	if (!setup.editor_override.on) {
		server.state=gs_intro;
		intro_open();
		return(TRUE);
	}
	
		// launch directly into map
		
	server.state=gs_running;
		
	if (!game_start(skill_medium,0,NULL,err_str)) {
		view_shutdown();
		server_shutdown();
		return(FALSE);
	}
	
	if (!map_start(TRUE,err_str)) {
		game_end();
		view_shutdown();
		server_shutdown();
		return(FALSE);
	}

	input_clear();
	
	return(TRUE);
}

void app_end(void)
{
		
	if (server.map_open) map_end();
	if (server.game_open) game_end();
	
		// shut down app
		
	console_add_system("Closing App");
	
		// shutdown view
		
	view_shutdown();
	
		// shutdown server
		
	server_shutdown();
	
		// OS network shutdown
		
	network_shutdown();
}

