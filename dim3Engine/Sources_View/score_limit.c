/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Score Limit

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

#define SCORE_LIMIT_SECOND_PAUSE		15

#include "objects.h"
#include "remotes.h"
#include "scripts.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"
#include "network.h"

extern bool					game_loop_quit;

extern server_type			server;
extern map_type				map;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							score_limit_start_tick;
bool						score_limit_start_trigger;

extern void game_reset(void);
extern void remote_host_exit(void);

/* =======================================================

      Score Limit Open/Close
      
======================================================= */

void score_limit_open(void)
{
	gui_initialize(NULL,NULL,TRUE);
	
	score_limit_start_tick=time_get();
	
	server.state=gs_score_limit;
}

void score_limit_close(void)
{
	gui_shutdown();
	
	server.state=gs_running;
}

/* =======================================================

      Score Limit Triggers
      
======================================================= */

void score_limit_trigger_clear(void)
{
	score_limit_start_trigger=FALSE;
}

void score_limit_trigger_check(void)
{
	if (score_limit_start_trigger) score_limit_open();
}	

void score_limit_trigger_set(void)
{
	obj_type			*obj;
	
	score_limit_start_trigger=TRUE;
	
		// if on the host, trigger all remotes to reach game limit
		
	if (net_setup.host.hosting) {
		obj=object_find_uid(server.player_obj_uid);
		net_host_player_send_others_packet(obj->remote.uid,net_action_request_game_score_limit,NULL,0);
	}
}

void score_limit_trigger_set_check_scores(void)
{
	int				n,limit,team_score[net_team_count];
	obj_type		*obj;

		// get score limit
		
	limit=setup.network.score_limit;
	if (limit==0) return;

		// team checks?
		
	if (net_setup.games[net_setup.game_idx].use_teams) {
		
		for (n=0;n!=net_team_count;n++) {
			team_score[n]=0;
		}

		obj=server.objs;

		for (n=0;n!=server.count.obj;n++) {
			if ((obj->player) || (obj->remote.on) || (obj->bot)) team_score[obj->team_idx]+=obj->score.score;
			obj++;
		}
		
		for (n=0;n!=net_team_count;n++) {
			if (team_score[n]>=limit) {
				score_limit_trigger_set();
				return;
			}
		}
		
		return;
	}
	
		// regular player checks

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if ((obj->player) || (obj->remote.on) || (obj->bot)) {
			if (obj->score.score>=limit) {
				score_limit_trigger_set();
				return;
			}
		}

		obj++;
	}
}

/* =======================================================

      Run Score Limit
      
======================================================= */

void score_limit_run(void)
{
	int			tick;
	
		// we are paused so use regular ticks
		
	tick=time_get();
	
		// draw score limit

	gl_frame_start(NULL);
	gui_draw_background(1.0f);
	network_draw(tick);
	gl_frame_end();
	
		// pump events since we aren't calling
		// the regular gui routines
		
	input_event_pump();
	
		// if we are hosting, we can exit early
		// and send the reset message to clients
		
	if (net_setup.host.hosting) {
		
			// check for time exit or escape exit
			
		if ((tick>(score_limit_start_tick+(SCORE_LIMIT_SECOND_PAUSE*1000))) || (input_action_get_state_single(nc_menu))) {
			game_reset();
			score_limit_close();
		}
		
		return;
	}
	
		// clients can't exit, but if they don't
		// get an update in the time + 10 seconds,
		// they auto-quit the game
		
	if (tick>(score_limit_start_tick+((SCORE_LIMIT_SECOND_PAUSE+10)*1000))) {
		score_limit_close();
		remote_host_exit();
	}
}

