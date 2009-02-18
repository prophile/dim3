/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Start/Stop

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

#include "network.h"
#include "objects.h"
#include "projectiles.h"
#include "effects.h"
#include "consoles.h"
#include "interfaces.h"
#include "scripts.h"

extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern bool server_game_start(char *game_script_name,int skill,network_reply_join_remotes *remotes,char *err_str);
extern void server_game_stop(void);
extern void view_game_start(void);
extern void view_game_stop(void);
extern void net_host_game_end(void);

/* =======================================================

      Game Start and End
      
======================================================= */

bool game_start(int skill,network_reply_join_remotes *remotes,char *err_str)
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
	
	if (!server_game_start("Game",skill,remotes,err_str)) {
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
	
		// close any network joins or hosting
	
	if (net_setup.client.joined) {
		if (!net_setup.host.hosting) {
			net_client_send_leave_host(object_player_get_remote_uid());
			net_client_end_message_queue();
			net_client_join_host_end();
		}
		else {
			net_host_game_end();
			net_client_end_message_queue_local();
		}
	}
	
		// stop view
		
	view_game_stop();

		// stop server
		
	server_game_stop();
	
		// game closed
		
	server.game_open=FALSE;
}

/* =======================================================

      Game Reset
      
======================================================= */

void game_reset_single_object(obj_type *obj,bool reposition)
{
	spot_type		*spot;

	obj->score.kill=obj->score.death=obj->score.suicide=obj->score.goal=obj->score.score=0;
	obj->spawning=TRUE;
	
	obj->input_freeze=FALSE;
	obj->death_trigger=FALSE;
	
	object_stop(obj);
	
	if (reposition) {
		spot=script_find_network_spot(obj);
		if (spot!=NULL) object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
	}
	
	object_spawn(obj);
}

void game_reset(void)
{
	int				n;
	obj_type		*obj;
	
		// reset projectiles, effects, etc
		
	projectile_start();
	effect_start();
	
		// if a remote player, then just reset yourself and
		// hide all other players until and update
		
	if (!net_setup.host.hosting) {
	
		obj=server.objs;

		for (n=0;n!=server.count.obj;n++) {
			if (obj->uid==server.player_obj_uid) {
				game_reset_single_object(obj,FALSE);
			}
			else {
				if ((obj->player) || (obj->remote.on) || (obj->bot)) {
					obj->hidden=TRUE;
				}
			}
			
			obj++;
		}
	
		return;
	}
	
		// force all players and bots to respawn

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if ((obj->player) || (obj->bot)) game_reset_single_object(obj,TRUE);
		obj++;
	}
	
		// force all remotes to update
		
	obj=object_find_uid(server.player_obj_uid);
	net_host_player_send_others_packet(obj->remote.uid,net_action_request_game_reset,NULL,0);
}



