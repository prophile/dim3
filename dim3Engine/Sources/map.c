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
extern bool gl_check_shader_ok(void);
extern void map_movements_initialize(void);
extern void segment_normal_smooth_init(void);
extern void fade_screen_start(int tick);

/* =======================================================

      Map Based Media and Music
      
======================================================= */

void map_media_start(map_media_type *media)
{
	if (setup.editor_override.on) return;			// no media when launched from editor

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

      Map Shader Errors to Console
      
======================================================= */

void map_shader_errors_write_console(void)
{
	int					n;
	texture_type		*texture;

	texture=map.textures;
	
	for (n=0;n!=max_map_texture;n++) {
		if ((texture->shader.on) && (texture->shader.init_error[0]!=0x0)) console_add_error(texture->shader.init_error);
		texture++;
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
	int				n,tick;
	char			txt[256];
	bool			load_shaders;
	obj_type		*obj;

	game_time_pause_start();
	
		// start progress
		
	progress_initialize("Opening");
	progress_draw(5);
	
	current_map_spawn_idx=0;
	strcpy(current_map_name,map.info.name);		// remember for close
	
		// load the map
		
	sprintf(txt,"Opening %s",map.info.name);
	console_add_system(txt);
	
	map_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.texture_quality_mode,setup.mipmap_mode,setup.mipmap_card_generated,setup.texture_compression);
	
	load_shaders=gl_check_shader_ok();


	/* supergumba -- testing! */
/*
	if (!map_auto_generate_test(&map,load_shaders)) {
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s",map.info.name);
		return(FALSE);
	}
*/

	if (!map_open(&map,map.info.name,TRUE,TRUE,load_shaders)) {
		progress_shutdown();
		sprintf(err_str,"Could not open map: %s",map.info.name);
		return(FALSE);
	}

		// deal with shader errors or shaders turned off

	if (!load_shaders) {
		for (n=0;n!=max_map_texture;n++) {
			map.textures[n].shader.on=FALSE;
		}
	}
	else {
		map_shader_errors_write_console();
	}

		// prepare map surfaces
	
	progress_draw(15);
	map_prepare(&map,setup.curve_mode);

	segment_normal_smooth_init();
	
		// create group lists
	
	progress_draw(17);
	if (!map_group_create_segment_list(&map)) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

		// segment, vertex, and light lists for portals

	progress_draw(20);

	if (!map_portal_create_segment_lists(&map)) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}
	
	progress_draw(23);

	if (!map_portal_create_vertex_lists(&map,setup.high_quality_lighting)) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	progress_draw(26);

	if (!map_portal_create_light_spots(&map)) {
		progress_shutdown();
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	progress_draw(30);
	
		// start map ambients
		
	map_start_ambient();
	if (map.ambient.sound_name[0]!=0x0) map_set_ambient(map.ambient.sound_name,map.ambient.sound_pitch);

		// clear projectile, effects, and marks
		
	projectile_start();
	effect_start();
	particle_map_initialize();
	decal_clear();
	script_move_clear_all();
	
		// reset rain
		
	map.rain.reset=TRUE;
	
	progress_draw(35);

		// clear spot attachements
		
	map_spot_clear_attach(&map);
 
        // run the course script

	js.course_attach.thing_type=thing_type_course;
	js.course_attach.thing_uid=-1;
	
	if (!scripts_add(&js.course_attach,"Courses",map.info.name,NULL,bt_map,err_str)) {
		progress_shutdown();
		return(FALSE);
	}
	
		// create the attached objects
		
	spot_start_attach();
	progress_draw(40);
   
        // start map bound weapons and projectiles
		
	weapon_start_bind(bt_map);
	progress_draw(45);
	
	proj_setup_start_bind(bt_map);
	progress_draw(50);

	weapon_reset_ammo_bind(bt_map);
	weapon_set_default_bind(bt_map);
	progress_draw(55);

		// create scenery

	scenery_create();
	progress_draw(60);
	
		// load models

	models_load(bt_map,load_shaders);
	progress_draw(70);
	
		// start scenery

	scenery_start();
	progress_draw(80);
	
		// attach player to map

	if (!player_attach_object()) {
		progress_shutdown();
		sprintf(err_str,"Could not find spot: %s-%s",map.info.player_start_name,map.info.player_start_type);
		return(FALSE);
	}

	player_clear_input();
	
	progress_draw(90);
	
		// connect camera to player
		
	obj=object_find_uid(server.player_obj_uid);
	camera_connect(obj);
	
	progress_draw(93);
	
		// map start event
		
	scripts_post_event_console(&js.game_attach,sd_event_map,sd_event_map_open,0);
	scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_map_open,0);
	scripts_post_event_console(&obj->attach,sd_event_map,sd_event_map_open,0);
	
	progress_draw(97);

		// initialize movements and lookups
		
	map_movements_initialize();
	map_lookups_setup();
	
	progress_draw(100);
	
	progress_shutdown();
	
		// map open
		
	server.map_open=TRUE;
	
		// flag off all map changes
		
	map_clear_changes();
	map_portal_clear_segment_list_changes(&map);
	
		// restart all the timing
		
	tick=game_time_get();
	map.start_game_tick=tick;
	server.time.run_tick=tick;
	view.time.run_tick=view.time.ambient_tick=tick;
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
	obj_type		*obj;

	game_time_pause_start();
	
		// detach player
		
	player_detach_object();
		
		// setup progress
		
	progress_initialize("Closing");
	progress_draw(5);
	
	console_add_system("Closing Map");
	
	obj=object_find_uid(server.player_obj_uid);
	
		// map close event
		
	scripts_post_event_console(&js.game_attach,sd_event_map,sd_event_map_close,0);
	scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_map_close,0);
	scripts_post_event_console(&obj->attach,sd_event_map,sd_event_map_close,0);
	
	progress_draw(10);
	
		// stop sounds
			
	map_end_ambient();
	al_stop_all_sources();

		// end all projectiles
	
	projectile_dispose_all();
	
	progress_draw(15);
    
        // end script
		
	timers_dispose(bt_map);
	progress_draw(20);
	
	scripts_dispose(bt_map);
	progress_draw(30);

		// free map-bound models

	models_dispose(bt_map);
	progress_draw(50);
	
		// free map bound items
		
	proj_setup_dispose(bt_map);
	progress_draw(65);
	
	weapon_dispose(bt_map);
	progress_draw(75);
	
	object_dispose_2(bt_map);
	progress_draw(90);
	
		// free group, portal segment, vertex and light lists
		
	map_group_dispose_segment_list(&map);
	map_portal_dispose_segment_lists(&map);
	map_portal_dispose_vertex_lists(&map);
	map_portal_dispose_light_spots(&map);
	
	progress_draw(95);
	
		// close map
	
	map_close(&map);
	
	progress_draw(100);
	
	progress_shutdown();
	
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
