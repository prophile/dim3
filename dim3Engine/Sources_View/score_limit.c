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

#include "scripts.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"

extern bool					game_loop_quit;

extern server_type			server;
extern map_type				map;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

bool						score_limit_start_trigger;

/* =======================================================

      Score Limit Open/Close
      
======================================================= */

void score_limit_open(void)
{
	gui_initialize(NULL,NULL,TRUE);
	
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
	score_limit_start_trigger=TRUE;
}

/* =======================================================

      Run Score Limit
      
======================================================= */

void score_limit_run(int tick)
{
		// draw score limit
		
	gl_frame_start(NULL);
	gui_draw_background(1.0f);
	network_draw(tick);
	gl_frame_end();
	
		// esc cancels menu
		// we are not in GUI so we need to pump events here

	input_event_pump();
	
	if (input_action_get_state_single(nc_menu)) {
		score_limit_close();
		return;
	}
}

