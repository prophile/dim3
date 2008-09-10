/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server MainLine

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
    server.projs=NULL;
    server.effects=NULL;
    server.decals=NULL;
	server.models=NULL;
	server.particles=NULL;
	server.rings=NULL;
	server.halos=NULL;
	server.marks=NULL;
	server.crosshairs=NULL;
	
	js.scripts=NULL;
	js.timers=NULL;
	js.globals=NULL;
	
	hud.bitmaps=NULL;
	hud.texts=NULL;
	hud.bars=NULL;
	hud.menus=NULL;
	hud.choosers=NULL;
	hud.chat.lines=NULL;
	
		// server pointers
		
	server.objs=(obj_type*)valloc(max_object*sizeof(obj_type));
	if (server.objs==NULL) return(FALSE);
	
	server.weapons=(weapon_type*)valloc(max_weapon*sizeof(weapon_type));
	if (server.weapons==NULL) return(FALSE);
	
	server.proj_setups=(proj_setup_type*)valloc(max_proj_setup*sizeof(proj_setup_type));
	if (server.proj_setups==NULL) return(FALSE);
	
	server.projs=(proj_type*)valloc(max_projectile*sizeof(proj_type));
	if (server.projs==NULL) return(FALSE);

	server.effects=(effect_type*)valloc(max_effect*sizeof(effect_type));
	if (server.effects==NULL) return(FALSE);
	
	server.decals=(decal_type*)valloc(max_decal*sizeof(decal_type));
	if (server.decals==NULL) return(FALSE);
	
	server.models=(model_type*)valloc(max_model*sizeof(model_type));
	if (server.models==NULL) return(FALSE);
	
	server.particles=(particle_type*)valloc(max_particle*sizeof(particle_type));
	if (server.particles==NULL) return(FALSE);
	
	server.rings=(ring_type*)valloc(max_ring*sizeof(ring_type));
	if (server.rings==NULL) return(FALSE);

	server.halos=(halo_type*)valloc(max_halo*sizeof(halo_type));
	if (server.halos==NULL) return(FALSE);
	
	server.marks=(mark_type*)valloc(max_mark*sizeof(mark_type));
	if (server.marks==NULL) return(FALSE);
	
	server.crosshairs=(crosshair_type*)valloc(max_crosshair*sizeof(crosshair_type));
	if (server.crosshairs==NULL) return(FALSE);
	
		// js engine pointers
		
	js.scripts=(script_type*)valloc(max_scripts*sizeof(script_type));
	if (js.scripts==NULL) return(FALSE);
	
	js.timers=(timer_type*)valloc(max_timers*sizeof(timer_type));
	if (js.timers==NULL) return(FALSE);
	
	js.globals=(global_type*)valloc(max_globals*sizeof(global_type));
	if (js.globals==NULL) return(FALSE);
	
		// hud pointers

	hud.bitmaps=(hud_bitmap_type*)valloc(max_hud_bitmap*sizeof(hud_bitmap_type));
	if (hud.bitmaps==NULL) return(FALSE);
	
	hud.texts=(hud_text_type*)valloc(max_hud_text*sizeof(hud_text_type));
	if (hud.texts==NULL) return(FALSE);
	
	hud.bars=(hud_bar_type*)valloc(max_hud_bar*sizeof(hud_bar_type));
	if (hud.bars==NULL) return(FALSE);

	hud.menus=(menu_type*)valloc(max_menu*sizeof(menu_type));
	if (hud.menus==NULL) return(FALSE);
	
	hud.choosers=(chooser_type*)valloc(max_chooser*sizeof(chooser_type));
	if (hud.choosers==NULL) return(FALSE);
	
	hud.chat.lines=(hud_chat_line_type*)valloc(max_chat_lines*sizeof(hud_chat_line_type));
	if (hud.chat.lines==NULL) return(FALSE);
	
		// zero memory
		
	bzero(server.objs,(max_object*sizeof(obj_type)));
	bzero(server.weapons,(max_weapon*sizeof(weapon_type)));
	bzero(server.proj_setups,(max_proj_setup*sizeof(proj_setup_type)));
	bzero(server.projs,(max_projectile*sizeof(proj_type)));
	bzero(server.effects,(max_effect*sizeof(effect_type)));
	bzero(server.decals,(max_decal*sizeof(decal_type)));
	bzero(server.models,(max_model*sizeof(model_type)));
	bzero(server.particles,(max_particle*sizeof(particle_type)));
	bzero(server.rings,(max_ring*sizeof(ring_type)));
	bzero(server.halos,(max_halo*sizeof(halo_type)));
	bzero(server.marks,(max_mark*sizeof(mark_type)));
	bzero(server.crosshairs,(max_crosshair*sizeof(crosshair_type)));
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
		// server pointers
		
	if (server.objs!=NULL) free(server.objs);
	if (server.weapons!=NULL) free(server.weapons);
	if (server.proj_setups!=NULL) free(server.proj_setups);
	if (server.projs!=NULL) free(server.projs);
	if (server.effects!=NULL) free(server.effects);
	if (server.decals!=NULL) free(server.decals);
	if (server.models!=NULL) free(server.models);
	if (server.particles!=NULL) free(server.particles);
	if (server.rings!=NULL) free(server.rings);
	if (server.halos!=NULL) free(server.halos);
	if (server.marks!=NULL) free(server.marks);
	if (server.crosshairs!=NULL) free(server.crosshairs);
	
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
	
		// start player object
	
	server.player_obj_uid=object_start(NULL,TRUE,bt_game,err_str);
	if (server.player_obj_uid==-1) {
		scripts_dispose(js.game_attach.script_uid);
		return(FALSE);
	}
			
		// force player to auto-spawn
		// spawing of player needs to happen before map_start events
		
	object_spawn(object_find_uid(server.player_obj_uid));
	
		// put in remotes
		
	if (remote_count!=0) {
	
		remote=remotes;
		
		for (n=0;n!=remote_count;n++) {
			remote_add(remote->uid,remote);
			remote++;
		}
	}
	
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
}

/* =======================================================

      Server Loop
      
======================================================= */

void server_loop(int tick)
{
	scripts_run(tick);
	server_run(tick);
}
