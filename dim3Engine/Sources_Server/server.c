/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server MainLine

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
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "projectiles.h"
#include "interfaces.h"
#include "xmls.h"
#include "video.h"

map_type					map;
server_type					server;
js_type						js;
hud_type					hud;

extern setup_type			setup;

extern void server_run(int tick);

/* =======================================================

      Server Memory
      
======================================================= */

bool server_memory_allocate(void)
{
		// initialize pointers
		
    server.objs=NULL;
	server.weapons=NULL;
	server.proj_setups=NULL;
	server.particles=NULL;
 	server.rings=NULL;
	server.halos=NULL;
	server.marks=NULL;
	server.crosshairs=NULL;
	server.models=NULL;

	server.projs=NULL;
    server.effects=NULL;
    server.decals=NULL;
	
	js.scripts=NULL;
	js.timers=NULL;
	js.globals=NULL;
	
	hud.bitmaps=NULL;
	hud.texts=NULL;
	hud.bars=NULL;
	hud.menus=NULL;
	hud.choosers=NULL;
	hud.chat.lines=NULL;

		// non-dynamic server pointers
		// these are pre-created to speed up
		// the game
	
	server.projs=(proj_type*)malloc(max_projectile*sizeof(proj_type));
	if (server.projs==NULL) return(FALSE);

	server.effects=(effect_type*)malloc(max_effect*sizeof(effect_type));
	if (server.effects==NULL) return(FALSE);
	
	server.decals=(decal_type*)malloc(max_decal*sizeof(decal_type));
	if (server.decals==NULL) return(FALSE);

		// js engine pointers
		
	js.scripts=(script_type*)malloc(max_scripts*sizeof(script_type));
	if (js.scripts==NULL) return(FALSE);
	
	js.timers=(timer_type*)malloc(max_timers*sizeof(timer_type));
	if (js.timers==NULL) return(FALSE);
	
	js.globals=(global_type*)malloc(max_globals*sizeof(global_type));
	if (js.globals==NULL) return(FALSE);
	
		// hud pointers

	hud.bitmaps=(hud_bitmap_type*)malloc(max_hud_bitmap*sizeof(hud_bitmap_type));
	if (hud.bitmaps==NULL) return(FALSE);
	
	hud.texts=(hud_text_type*)malloc(max_hud_text*sizeof(hud_text_type));
	if (hud.texts==NULL) return(FALSE);
	
	hud.bars=(hud_bar_type*)malloc(max_hud_bar*sizeof(hud_bar_type));
	if (hud.bars==NULL) return(FALSE);

	hud.menus=(menu_type*)malloc(max_menu*sizeof(menu_type));
	if (hud.menus==NULL) return(FALSE);
	
	hud.choosers=(chooser_type*)malloc(max_chooser*sizeof(chooser_type));
	if (hud.choosers==NULL) return(FALSE);
	
	hud.chat.lines=(hud_chat_line_type*)malloc(max_chat_lines*sizeof(hud_chat_line_type));
	if (hud.chat.lines==NULL) return(FALSE);
	
		// zero memory
		
	bzero(server.projs,(max_projectile*sizeof(proj_type)));
	bzero(server.effects,(max_effect*sizeof(effect_type)));
	bzero(server.decals,(max_decal*sizeof(decal_type)));
	bzero(js.scripts,(max_scripts*sizeof(script_type)));
	bzero(js.timers,(max_timers*sizeof(timer_type)));
	bzero(js.globals,(max_globals*sizeof(global_type)));
	bzero(hud.bitmaps,(max_hud_bitmap*sizeof(hud_bitmap_type)));
	bzero(hud.texts,(max_hud_text*sizeof(hud_text_type)));
	bzero(hud.bars,(max_hud_bar*sizeof(hud_bar_type)));
	bzero(hud.menus,(max_menu*sizeof(menu_type)));
	bzero(hud.choosers,(max_chooser*sizeof(chooser_type)));
	bzero(hud.chat.lines,(max_chat_lines*sizeof(hud_chat_line_type)));

	return(TRUE);
}

void server_memory_release(void)
{
		// catch all for dynamic server pointers
		
	if (server.objs!=NULL) free(server.objs);
	if (server.weapons!=NULL) free(server.weapons);
	if (server.proj_setups!=NULL) free(server.proj_setups);
	if (server.particles!=NULL) free(server.particles);
	if (server.rings!=NULL) free(server.rings);
	if (server.halos!=NULL) free(server.halos);
	if (server.marks!=NULL) free(server.marks);
	if (server.crosshairs!=NULL) free(server.crosshairs);
	if (server.models!=NULL) free(server.models);

		// non-dynamic server pointers

	if (server.projs!=NULL) free(server.projs);
	if (server.effects!=NULL) free(server.effects);
	if (server.decals!=NULL) free(server.decals);
	
		// js pointers
		
	if (js.scripts!=NULL) free(js.scripts);
	if (js.timers!=NULL) free(js.timers);
	if (js.globals!=NULL) free(js.globals);
	
		// hud pointers
		
	if (hud.bitmaps!=NULL) free(hud.bitmaps);
	if (hud.texts!=NULL) free(hud.texts);
	if (hud.bars!=NULL) free(hud.bars);
	if (hud.menus!=NULL) free(hud.menus);
	if (hud.choosers!=NULL) free(hud.choosers);
	if (hud.chat.lines!=NULL) free(hud.chat.lines);
}

/* =======================================================

      Server Initialize and Shutdown
      
======================================================= */

bool server_initialize(char *err_str)
{
		// clear server, js, hud structures
		
	memset(&server,0x0,sizeof(server_type));
	memset(&js,0x0,sizeof(js_type));
	memset(&hud,0x0,sizeof(hud_type));
	
		// allocate memory
		
	if (!server_memory_allocate()) {
		server_memory_release();
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// start script engine
		
	if (!scripts_engine_initialize(err_str)) {
		server_memory_release();
		return(FALSE);
	}
	
		// initialize project settings
		
	hud_initialize();
	particle_initialize();
	ring_initialize();
	halo_initialize();
	mark_initialize();
	crosshair_initialize();
	
		// load project XML
		
	read_settings_interface();
	read_settings_particle();
	read_settings_ring();
	read_settings_halo();
	read_settings_mark();
	read_settings_crosshair();
	
		// game states
		
	server.game_open=FALSE;
	server.map_open=FALSE;

	server.map_change=FALSE;
	server.skip_media=FALSE;
	
	return(TRUE);
}

void server_shutdown(void)
{
		// shutdown script engine
		
	scripts_engine_shutdown();
	
		// release memory
		
	server_memory_release();
}

/* =======================================================

      Server Game Start and Stop
      
======================================================= */

bool server_game_start(char *game_script_name,int skill,int remote_count,network_request_remote_add *remotes,char *err_str)
{
	int							n;
	network_request_remote_add	*remote;
	
		// initialize lists
		
	model_initialize();
		
	object_initialize_list();
	weapon_initialize_list();
	proj_setup_initialize_list();
	
	scripts_initialize();
	script_globals_initialize();
	timers_initialize();

		// setup skill level

	server.skill=skill;
	
		// run game script

	map.info.name[0]=0x0;
	map.info.player_start_name[0]=0x0;
	map.info.player_start_type[0]=0x0;
	map.info.in_load=FALSE;
	
	server.player_obj_uid=-1;
	
	js.game_attach.thing_type=thing_type_game;
	js.game_attach.thing_uid=-1;

	scripts_clear_attach_data(&js.game_attach);
	
	if (!scripts_add(&js.game_attach,"Game",game_script_name,NULL,err_str)) return(FALSE);
	
		// editor map override?
		
	if (setup.editor_override.on) {
		strcpy(map.info.name,setup.editor_override.map);
	}
	
		// can't start a game without a map
	
	if (map.info.name[0]==0x0) {
		strcpy(err_str,"Game: No start map specified in game script");
		return(FALSE);
	}

		// prepare for any script based spawns

	object_script_spawn_start();
	
		// put in remotes
		
	if (remote_count!=0) {
	
		remote=remotes;
		
		for (n=0;n!=remote_count;n++) {
			remote_add(remote,FALSE);
			remote++;
		}
	}
	
		// start player object
	
	server.player_obj_uid=object_start(NULL,TRUE,bt_game,-1,err_str);
	if (server.player_obj_uid==-1) {
		scripts_dispose(js.game_attach.script_uid);
		return(FALSE);
	}
			
		// force player to auto-spawn
		// spawing of player needs to happen before map_start events
		
	object_spawn(object_find_uid(server.player_obj_uid));

		// finish any script based spawns

	object_script_spawn_finish();
	
	return(TRUE);
}

void server_game_stop(void)
{
		// dispose remote and game
		// bound objects

	object_dispose_2(bt_remote);
	object_dispose_2(bt_game);

		// dispose game script

	scripts_dispose(js.game_attach.script_uid);

		// finish with object list free

	object_free_list();
}

/* =======================================================

      Server Loop
      
======================================================= */

void server_loop(int tick)
{
		// prepare for any script based spawns

	object_script_spawn_start();

		// run all server functions

	scripts_run(tick);
	server_run(tick);

		// finish spawning any script based objects

	object_script_spawn_finish();
}
