/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Intro

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

#include "interfaces.h"
#include "sounds.h"

#define intro_button_new_id				0
#define intro_button_load_id			1
#define intro_button_join_id			4
#define intro_button_credit_id			5
#define intro_button_setup_id			6
#define intro_button_quit_id			7

extern bool game_start(int remote_count,network_request_remote_add *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);
extern void file_open(void);
extern void setup_open(bool in_game);
extern void title_set_open(char *dir,char *name,char *sound_name);
extern void join_open(void);

extern bool					game_loop_quit;

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

bitmap_type					intro_bitmap;

/* =======================================================

      Intro Operations
      
======================================================= */

void intro_open(void)
{
	bool		start_music;
	char		path[1024],path2[1024],wave_path[1024];
	
		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","main");
	
	if (hud.intro.button_new.on) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_new_game","png");
		file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_new_game_selected","png");
		element_button_add(path,path2,intro_button_new_id,hud.intro.button_new.x,hud.intro.button_new.y,hud.intro.button_new.wid,hud.intro.button_new.high,element_pos_left,element_pos_top);
	}
	
	if (hud.intro.button_load.on) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_load_game","png");
		file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_load_game_selected","png");
		element_button_add(path,path2,intro_button_load_id,hud.intro.button_load.x,hud.intro.button_load.y,hud.intro.button_load.wid,hud.intro.button_load.high,element_pos_left,element_pos_top);
	}
	
	if (hud.intro.button_join.on) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_join_game","png");
		file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_join_game_selected","png");
		element_button_add(path,path2,intro_button_join_id,hud.intro.button_join.x,hud.intro.button_join.y,hud.intro.button_join.wid,hud.intro.button_join.high,element_pos_left,element_pos_top);
	}
	
	if (hud.intro.button_credit.on) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_credit_game","png");
		file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_credit_game_selected","png");
		element_button_add(path,path2,intro_button_credit_id,hud.intro.button_credit.x,hud.intro.button_credit.y,hud.intro.button_credit.wid,hud.intro.button_credit.high,element_pos_left,element_pos_top);
	}
	
	if (hud.intro.button_setup.on) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_setup_game","png");
		file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_setup_game_selected","png");
		element_button_add(path,path2,intro_button_setup_id,hud.intro.button_setup.x,hud.intro.button_setup.y,hud.intro.button_setup.wid,hud.intro.button_setup.high,element_pos_left,element_pos_top);
	}
	
	if (hud.intro.button_quit.on) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_quit_game","png");
		file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_quit_game_selected","png");
		element_button_add(path,path2,intro_button_quit_id,hud.intro.button_quit.x,hud.intro.button_quit.y,hud.intro.button_quit.wid,hud.intro.button_quit.high,element_pos_left,element_pos_top);
	}
	
	server.state=gs_intro;
	
		// intro music
		
	if ((hud.intro_music[0]!=0x0) && (setup.music_on)) {
	
			// previously playing music?
			
		start_music=TRUE;
		
		if (al_music_playing()) {
			if (al_music_playing_is_name(hud.intro_music)) {		// same music, keep playing
				start_music=FALSE;
			}
			else {
				start_music=TRUE;
				al_music_stop();
			}
		}
		
		if (start_music) {
			file_paths_data(&setup.file_path_setup,wave_path,"Music",hud.intro_music,"wav");
			al_music_play(hud.intro_music,wave_path);
		}
	}
}

void intro_close(bool in_game,bool stop_music)
{
		// stop music
		
	if (stop_music) {
		if (al_music_playing()) al_music_stop();
	}
	
		// shutdown UI
		
	gui_shutdown();
	
	server.state=gs_running;
}

/* =======================================================

      Intro Input
      
======================================================= */

void intro_click(void)
{
	int				id;
	char			err_str[256];
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run click
		
	switch (id) {
	
		case intro_button_new_id:
			intro_close(TRUE,TRUE);
			net_setup.client.joined=FALSE;
			if (!game_start(0,NULL,err_str)) {
				error_open(err_str,"Game Start Canceled");
				break;
			}
			if (!map_start(FALSE,err_str)) {
				error_open(err_str,"Game Start Canceled");
				break;
			}
			break;
			
		case intro_button_load_id:
			intro_close(TRUE,FALSE);
			net_setup.client.joined=FALSE;
			file_open();
			break;

		case intro_button_join_id:
			intro_close(TRUE,FALSE);
			join_open();
			break;
			
		case intro_button_credit_id:
			intro_close(FALSE,FALSE);
			title_set_open("Bitmaps/Backgrounds","credit","");
			break;

		case intro_button_setup_id:
			intro_close(FALSE,FALSE);
			setup_open(FALSE);
			break;

		case intro_button_quit_id:
			intro_close(FALSE,FALSE);
			game_loop_quit=TRUE;
			break;
	}
}

/* =======================================================

      Run Intro Page
      
======================================================= */

void intro_run(void)
{
	gui_draw(1.0f,TRUE);
	intro_click();
}


