/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Loop

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

#include "remotes.h"
#include "interfaces.h"
#include "inputs.h"
#include "video.h"

extern bool					game_loop_pause;

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

bool						interface_quit,game_loop_pause_button_down;

extern void game_time_initialize(void);
extern int game_time_calculate(void);
extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern int time_get(void);
extern int game_time_get(void);
extern bool server_initialize(char *err_str);
extern void server_shutdown(void);
extern void server_loop(int tick);
extern bool view_initialize(char *err_str);
extern void view_shutdown(void);
extern void view_loop(int tick);
extern void view_pause_draw(void);
extern void map_clear_changes(void);
extern bool map_need_rebuild(void);
extern bool map_rebuild_changes(char *err_str);
extern void game_end(void);
extern void map_end(void);

/* =======================================================

      Game Loop Interface Quit
      
======================================================= */

void interface_quit_trigger_clear(void)
{
	interface_quit=FALSE;
}

void interface_quit_trigger_set(void)
{
	interface_quit=TRUE;
}

bool interface_quit_trigger_check(void)
{
	return(interface_quit);
}

/* =======================================================

      Main Game Loop Run
      
======================================================= */

void loop_game_run(int tick)
{
		// receive networking updates
		
	if (net_setup.client.joined) {
		if (!remote_network_get_updates(tick)) return;
	}
	
		// start fps
		
	view.fps.time=time_get();
	
		// mark for interface quits
		
	interface_quit_trigger_clear();
	
		// run game

	server_loop(tick);
	view_loop(tick);
	
		// check interface quits
		
	if (interface_quit_trigger_check()) {
		map_end();
		game_end();
		intro_open();
		return;
	}
	
		// calculate fps
		
	view.fps.tick+=(time_get()-view.fps.time);

	if (view.fps.tick>=1000) {						// average fps over 1 second
		view.fps.total=((float)view.fps.count*1000)/(float)view.fps.tick;
		view.fps.tick=view.fps.count=0;
	}
	
		// sending network updates

	if (net_setup.client.joined) {
	
		if (tick>=server.time.network_update_tick) {
			server.time.network_update_tick=tick+client_communication_update_msec_rate;
			remote_network_send_updates(tick);
		}

		if (tick>=server.time.network_latency_ping_tick) {
			server.time.network_latency_ping_tick=tick+client_communication_latency_ping_msec_rate;
			remote_network_send_latency_ping(tick);
		}
	}
}

/* =======================================================

      Pausing
      
======================================================= */

bool loop_pause(void)
{
	bool		unpause;

		// only windowed versions can pause

	if (!gl_in_window_mode()) return(FALSE);

		// current not paused

	if (!game_loop_pause) {
		if (input_app_active()) return(FALSE);

			// pause game

		game_loop_pause=TRUE;
		game_loop_pause_button_down=FALSE;

		input_mouse_pause();
		game_time_pause_start();

		return(TRUE);
	}

		// paused

	input_event_pump();

	view_pause_draw();

		// are we clicking to unpause

	unpause=FALSE;

	if (input_app_active()) {
		if (game_loop_pause_button_down) {
			unpause=!input_gui_get_mouse_left_button_down();
		}
		else {
			game_loop_pause_button_down=input_gui_get_mouse_left_button_down();
		}
	}

	if (!unpause) {
		usleep(10000);
		return(TRUE);
	}

		// unpause game

	game_loop_pause=FALSE;

	input_clear();
	input_mouse_resume();

	game_time_pause_end();

	return(FALSE);
}

/* =======================================================

      Main App Loop Run
      
======================================================= */

bool loop_main(char *err_str)
{
	int				tick,state;

		// paused?

	if (loop_pause()) return(TRUE);
	
		// calculate timing
		
	tick=game_time_calculate();
	
		// clear story, title, and file trigger
		
	story_trigger_clear();
	title_trigger_clear();
	chooser_trigger_clear();
	movie_trigger_clear();
	setup_game_trigger_clear();
	menu_trigger_clear();
	file_trigger_clear();
	map_pick_trigger_clear();
	
		// clear map changes
	
	map_clear_changes();
		
		// run proper game state

	state=server.state;
		
	switch (server.state) {
	
		case gs_running:
			loop_game_run(tick);
			break;
			
		case gs_intro:
			intro_run();
			break;
			
		case gs_join:
			join_run();
			break;

		case gs_host:
			host_run();
			break;
			
		case gs_menu:
			menu_run();
			break;
			
		case gs_chooser:
			chooser_run();
			break;
			
		case gs_setup_game:
			setup_game_run();
			break;
			
		case gs_setup_network:
			setup_network_run();
			break;
			
		case gs_file:
			file_run();
			break;
			
		case gs_story:
			story_run();
			break;
			
		case gs_title:
			title_run();
			break;
			
		case gs_movie:
			movie_run();
			break;
			
		case gs_error:
			error_run();
			break;
			
		case gs_map_pick:
			map_pick_run();
			break;
			
	}
	
		// map changes (never change map if in fatal error)
		
	if (server.state!=gs_error) {
		if (map_need_rebuild()) {
			if (!map_rebuild_changes(err_str)) return(FALSE);			// bad map changes is a fatal error
		}
	}
	
		// check if we've gone into story, title, setup, or file
		
	story_trigger_check();
	title_trigger_check();
	chooser_trigger_check();
	movie_trigger_check();
	setup_game_trigger_check();
	menu_trigger_check();
	file_trigger_check();
	map_pick_trigger_check();
		
		// if we are changing state from game
		// play to interface element, capture screen
		// for background

	if ((server.state!=gs_running) && (state==gs_running)) {
		gui_screenshot_load();
		fprintf(stdout,"got shot!\n");
	}
	
	return(TRUE);
}
