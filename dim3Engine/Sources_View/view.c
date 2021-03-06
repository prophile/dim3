/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View MainLine

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "lights.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "xmls.h"
#include "video.h"
#include "sounds.h"
#include "inputs.h"

view_type					view;
render_info_type			render_info;

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;
extern network_setup_type	net_setup;

extern void game_file_initialize(void);
extern void menu_input(void);
extern void file_input(void);
extern void debug_input(void);
extern void view_run(int tick);
extern void view_draw(int tick);
extern int game_time_get(void);
extern void chat_clear_messages(void);
extern bool fog_solid_on(void);
extern bool shadow_initialize(char *err_str);
extern void shadow_shutdown(void);

/* =======================================================

      View Memory
      
======================================================= */

bool view_memory_allocate(void)
{
		// initialize pointers
		
	view.images=NULL;
	view.shaders=NULL;
	view.rain_draws=NULL;
	
		// view pointers

	view.images=(view_image_type*)malloc(max_view_image*sizeof(view_image_type));
	if (view.images==NULL) return(FALSE);
	
	view.shaders=(view_shader_type*)malloc(max_view_shader*sizeof(view_shader_type));
	if (view.shaders==NULL) return(FALSE);

	view.rain_draws=(rain_draw_type*)malloc(max_rain_density*sizeof(rain_draw_type));
	if (view.rain_draws==NULL) return(FALSE);
	
		// clear pointers

	bzero(view.images,(max_view_image*sizeof(view_image_type)));
	bzero(view.shaders,(max_view_shader*sizeof(view_shader_type)));
	bzero(view.rain_draws,(max_rain_density*sizeof(rain_draw_type)));
			
	return(TRUE);
}

void view_memory_release(void)
{
	if (view.images!=NULL) free(view.images);
	if (view.shaders!=NULL) free(view.shaders);
	if (view.rain_draws!=NULL) free(view.rain_draws);
}

/* =======================================================

      Screen Size List
      
======================================================= */

void view_create_screen_size_list(void)
{
	int				n,k,i,nscreen_size;
	float			ratio;
	bool			hit;
	SDL_Rect		**modes;
		
	modes=SDL_ListModes(NULL,SDL_OPENGL|SDL_FULLSCREEN);
	
		// if no modes, then 640x480 is the only mode
		
	if ((modes==(SDL_Rect**)0) || (modes==(SDL_Rect**)-1)) {
		render_info.screen_sizes[0].wid=640;
		render_info.screen_sizes[0].high=480;
		render_info.nscreen_size=1;
		return;
	}
	
		// get mode count
		
	nscreen_size=0;
	while (modes[nscreen_size]!=0) {
		nscreen_size++;
	}
	
		// create screen list
		
	k=0;
	
	for (n=(nscreen_size-1);n>=0;n--) {

			// knock out any less than 640x480 or when height >= width

		if (modes[n]->w<640) continue;
		if (modes[n]->h<480) continue;
		if (modes[n]->h>=modes[n]->w) continue;

			// is this screen already in list?

		hit=FALSE;

		for (i=0;i!=k;i++) {
			if ((render_info.screen_sizes[i].wid==modes[n]->w) && (render_info.screen_sizes[i].high==modes[n]->h)) {
				hit=TRUE;
				break;
			}
		}

			// add to list if ratio is equal or
			// better than 4:3 (0.75) and not less
			// then 1:85:1 (0.54)

		if (!hit) {
			ratio=(float)modes[n]->h/(float)modes[n]->w;
			if ((ratio>=0.54) && (ratio<=0.75f)) {
				render_info.screen_sizes[k].wid=modes[n]->w;
				render_info.screen_sizes[k].high=modes[n]->h;

				k++;
				if (k>=max_screen_size) break;
			}
		}
	}
	
	render_info.nscreen_size=k;
}

int view_search_screen_size_list(int wid,int high)
{
	int				n;
	
	for (n=0;n!=render_info.nscreen_size;n++) {
		if ((render_info.screen_sizes[n].wid==wid) && (render_info.screen_sizes[n].high==high)) return(n);
	}
	
	return(-1);
}

/* =======================================================

      View Initialize and Shutdown Display
      
======================================================= */

bool view_initialize_display(char *err_str)
{
		// is screen size legal?
		
	if (view_search_screen_size_list(setup.screen_wid,setup.screen_high)==-1) {
		setup.screen_wid=640;
		setup.screen_high=480;
	}
	
		// start openGL
		
	if (!gl_initialize(setup.screen_wid,setup.screen_high,setup.lock_fps_refresh,setup.fsaa_mode,FALSE,err_str)) {
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

		// fix some OpenGL settings if not supported by card

	if (!gl_check_fsaa_ok()) setup.fsaa_mode=fsaa_mode_none;
	if (!gl_check_texture_compress_ok()) setup.texture_compression=FALSE;
	if (!gl_check_texture_anisotropic_filter_ok()) setup.anisotropic_mode=anisotropic_mode_none;
	
		// start the shaders
		
	read_settings_shader();
	if (!gl_shader_initialize(err_str)) {
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}

		// initialize text
	
	gl_text_initialize();
	
		// back renderer
		
	if (!gl_back_render_initialize(err_str)) {
		gl_text_shutdown();
		gl_shutdown();
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

		// shadows

	if (!shadow_initialize(err_str)) {
		gl_back_render_shutdown();
		gl_text_shutdown();
		gl_shutdown();
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

		// vertex objects
	
	view_create_vertex_objects();

	return(TRUE);
}

void view_shutdown_display(void)
{
	view_dispose_vertex_objects();
	shadow_shutdown();
	gl_shader_shutdown();
	gl_back_render_shutdown();
	gl_text_shutdown();
	gl_shutdown();
}

bool view_reset_display(char *err_str)
{
	view_shutdown_display();
	return(view_initialize_display(err_str));
}

/* =======================================================

      View Initialize and Shutdown Main
      
======================================================= */

bool view_initialize(char *err_str)
{
	int				rate;
	
		// clear view structure
		
	memset(&view,0x0,sizeof(view_type));

		// allocate memory
		
	if (!view_memory_allocate()) {
		view_memory_release();
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

		// start SDL

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_NOPARACHUTE)==-1) {
		view_memory_release();
		sprintf(err_str,"SDL: Could not init (Error: %s)\n",SDL_GetError());
		return(FALSE);
	}
	
		// create screen sizes
		
	view_create_screen_size_list();
	
		// gl initialize

	if (!view_initialize_display(err_str)) return(FALSE);

		// sound initialize
		
	if (!al_initialize(err_str)) {
		view_shutdown_display();
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

	al_set_volume(setup.sound_volume);
	al_music_set_volume(setup.music_volume);
	al_music_set_state(setup.music_on);
	
		// read in the sounds
		
	read_settings_sound();
	
		// connect the input
		
	read_settings_action();
	
	input_initialize(gl_in_window_mode());
	setup_to_input();
	
		// draw timing
		
	view.time.input_tick=game_time_get();
	view.time.draw_tick=game_time_get();
	
	rate=render_info.monitor_refresh_rate;
	if (!setup.lock_fps_refresh) rate=max_fps;
	
	if (rate>max_fps) rate=max_fps;
	view.time.draw_time=1000/rate;
	
	return(TRUE);
}

void view_shutdown(void)
{
		// shutdown view

	input_shutdown();
	al_shutdown();
	view_shutdown_display();

		// shutdown SDL
		
	SDL_Quit();
	
		// release memory

	view_memory_release();
}

/* =======================================================

      View Game Start and Stop
      
======================================================= */

void view_game_start(void)
{
		// cameras and globals
		
    camera_initialize();
	game_file_initialize();
	
		// load images for hud bitmaps, radar, particles,
		// rings, halos, marks, crosshairs and remote icons
	
	view_images_load();

		// precalculate particles

	particle_precalculate_all();
	
		// clear chat messages
		
	chat_clear_messages();
}

void view_game_stop(void)
{
		// stop any music
	
	al_music_stop();
	
		// free images for hud bitmaps, radar, particles,
		// rings, halos, marks, crosshairs and remote icons
	
	view_images_free();
}

/* =======================================================

      View Loop Input
      
======================================================= */

void view_loop_input(int tick)
{
		// timing
		
	if (tick<view.time.input_tick) return;
	view.time.input_tick=tick+input_tick_rate;
	
		// pump input
		
	input_event_pump();

		// player input
	
	player_get_input(tick);
	
		// system input
	
	console_input();
	menu_input();
	file_input();
	debug_input();
}

/* =======================================================

      View Draw for Play Loop
      
======================================================= */

void view_loop_draw(int tick)
{
	if (tick<view.time.draw_tick) return;
	view.time.draw_tick=tick+view.time.draw_time;
	
		// texture setup
	
	map_setup_animated_textures(&map,tick);
	map_mesh_poly_run_shifts(&map,tick);

		// start frame
		
	if (!fog_solid_on()) {
		gl_frame_start(NULL);
	}
	else {
		gl_frame_start(&map.fog.col);		// is obscuring fog on, then background = fog color
	}

		// draw frame
		
	view_draw(tick);
	hud_draw(tick);
	radar_draw(tick);
	network_draw(tick);
	
	gl_frame_end();

	view.fps.count++;
}

/* =======================================================

      View Draw for Screen Captures
      
======================================================= */

void view_capture_draw(char *path)
{
	int			tick;

	tick=game_time_get();

	gl_frame_start(NULL);
	view_draw(tick);
	
	gl_screen_shot(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz,TRUE,path);
}

/* =======================================================

      View Pause Draw
      
======================================================= */

void view_pause_draw(void)
{
	d3col		col;

	gl_frame_start(NULL);

	gl_2D_view_screen();

	col.r=col.g=col.b=1.0f;
			
	gl_text_start(hud.font.text_size_small);
	gl_text_draw(2,(setup.screen.y_sz-2),"[paused]",tx_left,FALSE,&col,1.0f);
	gl_text_draw((setup.screen.x_sz-2),(setup.screen.y_sz-2),"[click to resume]",tx_right,FALSE,&col,1.0f);
	gl_text_end();
	
	gl_frame_end();
}

/* =======================================================

      View Loop
      
======================================================= */

void view_loop(int tick)
{
		// run the input
		
	view_loop_input(tick);
	
		// early exit from input changing state
		
	if ((server.state!=gs_running) && (server.state!=gs_score_limit)) return;
		
		// run the view
		
	view_run(tick);
	view_loop_draw(tick);
}
