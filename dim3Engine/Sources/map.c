/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Start/Stop

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
#include "weapons.h"
#include "projectiles.h"
#include "scenery.h"
#include "models.h"
#include "cameras.h"
#include "effects.h"
#include "consoles.h"
#include "interfaces.h"
#include "sounds.h"
#include "inputs.h"
#include "video.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;

int							current_map_spawn_idx;
char						current_map_name[name_str_len];

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern void map_clear_changes(void);
extern void map_start_ambient(void);
extern void map_set_ambient(char *name,float pitch);
extern void map_end_ambient(void);
extern int game_time_get(void);
extern void spot_start_attach(void);
extern void spot_add_multiplayer_bots(void);
extern bool gl_check_shader_ok(void);
extern void map_movements_initialize(void);
extern void fade_screen_start(int tick);
extern void group_move_clear_all(void);
extern void liquid_gl_list_init(void);
extern void draw_sky_init(void);
extern bool view_compile_mesh_gl_list_init(void);
extern void view_compile_mesh_gl_list_free(void);
extern bool render_transparent_create_sort_list(void);
extern void render_transparent_dispose_sort_list(void);

/* =======================================================

      Map Based Media and Music
      
======================================================= */

void map_media_start(map_media_type *media)
{
		// no media when launched from editor

	if (setup.editor_override.on) return;

		// no media if last change was a skip change

	if (server.skip_media) {
		server.skip_media=FALSE;
		return;
	}

		// run the media type

	switch (media->type) {
	
		case mi_story:
			story_trigger_set(media->name,-1);
			story_trigger_check();
			story_trigger_clear();
			break;
			
		case mi_title:
			title_trigger_set("Titles",media->name,media->title_sound_name,-1);
			title_trigger_check();
			title_trigger_clear();
			break;
			
		case mi_movie:
			movie_trigger_set(media->name,-1);
			movie_trigger_check();
			movie_trigger_clear();
			break;
		
	}
}

void map_music_start(map_music_type *music)
{
	char			wave_path[1024];
	
	if (music->name[0]==0x0) return;
	
		// stop old music
		
	if (al_music_playing()) al_music_stop();
	
		// start new music
	
	file_paths_data(&setup.file_path_setup,wave_path,"Music",music->name,"wav");
	
	if (music->fade_msec==0) {
		al_music_play(music->name,wave_path);
	}
	else {
		al_music_fade_in(server.time.run_tick,music->name,wave_path,music->fade_msec);
	}
}

/* =======================================================

      Cache Some Map Lookups
      
======================================================= */

void map_lookups_setup(void)
{
	int					n;
	map_sound_type		*sound;
	map_particle_type	*particle;

	sound=map.sounds;
	
	for (n=0;n!=map.nsound;n++) {
		sound->buffer_idx=al_find_buffer(sound->name);
		sound++;
	}
	
	particle=map.particles;
	
	for (n=0;n!=map.nparticle;n++) {
		particle->particle_idx=particle_find_index(particle->name);
		particle++;
	}
}

/* =======================================================

      Map Start and End
      
======================================================= */

bool map_start(bool skip_media,char *err_str)
{
	int				tick;
	char			txt[256];
	obj_type		*obj;

	game_time_pause_start();
	gui_screenshot_initialize();

		// start progress
		
	progress_initialize("Opening");
	progress_draw(10);
	
	current_map_spawn_idx=0;
	strcpy(current_map_name,map.info.name);		// remember for close
	
		// load the map
		
	sprintf(txt,"Opening %s",map.info.name);
	console_add_system(txt);
	
	map_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_compression);

// supergumba -- auto generator testing

	if (!map_auto_generate_test(&map)) {
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s",map.info.name);
		return(FALSE);
	}
/*
	if (!map_open(&map,map.info.name,TRUE)) {
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s.  If this map is from an older version of dim3, use Editor to upgrade it.",map.info.name);
		return(FALSE);
	}
*/
	gl_shader_attach_map();

		// prepare map surfaces
	
	progress_draw(20);

	map_prepare(&map);

		// map lists

	progress_draw(30);

	if (!render_transparent_create_sort_list()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	if (!map_group_create_unit_list(&map)) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	if (!view_compile_mesh_gl_list_init()) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	liquid_gl_list_init();
	draw_sky_init();

		// start map ambients
		// and clear all proj, effects, decals, etc
		
	progress_draw(40);

	map_start_ambient();
	if (map.ambient.sound_name[0]!=0x0) map_set_ambient(map.ambient.sound_name,map.ambient.sound_pitch);

	projectile_start();
	effect_start();
	particle_map_initialize();

	decal_clear();
	group_move_clear_all();
	
		// reset rain
		
	map.rain.reset=TRUE;

        // run the course script

	progress_draw(50);

	map_spot_clear_attach(&map);
		
	js.course_attach.thing_type=thing_type_course;
	js.course_attach.thing_uid=-1;

	scripts_clear_attach_data(&js.course_attach);
	
	if (!scripts_add(&js.course_attach,"Courses",map.info.name,NULL,err_str)) {
		progress_shutdown();
		return(FALSE);
	}
	
		// prepare for any script based spawns

	object_script_spawn_start();

		// create the attached objects
		// and scenery
		
	progress_draw(70);

	spot_start_attach();
	spot_add_multiplayer_bots();

	scenery_create();
	scenery_start();
	
		// attach player to map

	progress_draw(80);

	if (!player_attach_object(err_str)) {
		progress_shutdown();
		return(FALSE);
	}

	player_clear_input();
	
		// connect camera to player
		
	obj=object_find_uid(server.player_obj_uid);
	camera_connect(obj);
	
		// map start event
		
	progress_draw(90);

	scripts_post_event_console(&js.game_attach,sd_event_map,sd_event_map_open,0);
	scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_map_open,0);
	scripts_post_event_console(&obj->attach,sd_event_map,sd_event_map_open,0);

		// finish any script based spawns

	object_script_spawn_finish();

		// initialize movements and lookups
	
	map_movements_initialize();
	map_lookups_setup();
	gl_back_render_map_start();
	
	progress_draw(100);
	
	progress_shutdown();
	
		// map open
		
	server.map_open=TRUE;
	
		// flag off all map changes
		
	map_clear_changes();
	
		// restart all the timing
		
	tick=game_time_get();
	
	map.start_game_tick=tick;
	server.time.run_tick=tick;
	server.time.network_update_tick=tick;
	server.time.network_group_synch_tick=tick;
	server.time.network_latency_ping_tick=tick;	
	view.time.run_tick=tick;
	js.time.timer_tick=tick;

	view.fps.tick=view.fps.count=0;
	view.fps.total=0;

		// clear all input
		
	input_clear();
	
	game_time_pause_end();
	
		// start any map open media
		
	if (!skip_media) map_media_start(&map.media);
	map_music_start(&map.music);

		// start any map fades

	fade_screen_start(tick);
	
	return(TRUE);
}

void map_end(void)
{
	char			txt[256];
	obj_type		*obj;
	
	game_time_pause_start();
	
		// detach player
		
	player_detach_object();
		
		// setup progress
		
	progress_initialize("Closing");
	progress_draw(5);
	
	sprintf(txt,"Closing %s",map.info.name);
	console_add_system(txt);
	
	obj=object_find_uid(server.player_obj_uid);
	
		// map close event
		
	scripts_post_event_console(&js.game_attach,sd_event_map,sd_event_map_close,0);
	scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_map_close,0);
	scripts_post_event_console(&obj->attach,sd_event_map,sd_event_map_close,0);

		// clear all back buffers

	gl_back_render_map_end();
	
		// stop sounds
			
	progress_draw(15);

	map_end_ambient();
	al_stop_all_sources();

		// end all projectiles
	
	progress_draw(25);

	projectile_dispose_all();

        // end script
		
	progress_draw(35);

	scripts_dispose(js.course_attach.script_uid);

		// free map bound items
		
	progress_draw(45);

	object_dispose_2(bt_map);
	
		// free group, portal segment, vertex and light lists
		
	progress_draw(65);

	view_compile_mesh_gl_list_free();
	render_transparent_dispose_sort_list();
	map_group_dispose_unit_list(&map);
	
		// close map
	
	progress_draw(85);

	map_close(&map);
	
		// finish

	progress_draw(100);
	
	progress_shutdown();

	gui_screenshot_free();
	
		// map closed
		
	server.map_open=FALSE;
	
	game_time_pause_end();
}

/* =======================================================

      In Game Map changes
      
======================================================= */

void map_clear_changes(void)
{
	server.map_change=FALSE;
	server.skip_media=FALSE;
}

bool map_need_rebuild(void)
{
	return(server.map_change);
}

bool map_rebuild_changes(char *err_str)
{
	obj_type		*obj;
	
		// close existing map
		
	if (server.map_open) map_end();
	
		// open new map
		
	if (!map_start(FALSE,err_str)) return(FALSE);
	
		// stop all movement on player object
		// force current weapon into held position
		
	obj=object_find_uid(server.player_obj_uid);
	object_stop(obj);
	
	obj->held_weapon.mode=wm_held;
	
	return(TRUE);
}
