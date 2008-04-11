/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Titles

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
#include "interfaces.h"
#include "sounds.h"
#include "inputs.h"

#define title_fade_mode_in		0
#define title_fade_mode_out		1
#define title_fade_mode_none	2

extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;
extern js_type				js;

int							title_fade_tick,title_fade_mode,title_event_id;
char						title_dir[name_str_len],title_name[name_str_len],title_sound_name[name_str_len];
bool						title_start_trigger,title_show_view;

extern void intro_open(void);

/* =======================================================

      Title Operations
      
======================================================= */

void title_open(void)
{
	int			buffer_idx;

		// titles
		
	gui_initialize(title_dir,title_name,title_show_view,FALSE);
	
		// sound

	if (title_sound_name[0]!=0x0) {
		buffer_idx=al_find_buffer(title_sound_name);
		al_play_source(buffer_idx,0,0,0,1.0f,FALSE,FALSE,TRUE,FALSE);
	}

		// fades
		
	title_fade_tick=time_get();
	title_fade_mode=title_fade_mode_in;

		// running title
		
	server.state=gs_title;
}

void title_set_open(char *dir,char *name,char *sound_name,bool show_view)
{
	strcpy(title_dir,dir);
	strcpy(title_name,name);
	strcpy(title_sound_name,sound_name);
	title_show_view=show_view;
	title_event_id=-1;
	
	title_open();
}

void title_close(void)
{
	gui_shutdown();

	if (!server.map_open) {			// if no map opened, then return to intro
		intro_open();
		return;
	}

	if (title_event_id!=-1) scripts_post_event_console(&js.game_attach,sd_event_interface,sd_event_interface_title_done,title_event_id);
	
	server.state=gs_running;
}

/* =======================================================

      Title Triggers
      
======================================================= */

void title_trigger_clear(void)
{
	title_start_trigger=FALSE;
}

void title_trigger_check(void)
{
	if (title_start_trigger) title_open();
}	

void title_trigger_set(char *dir,char *name,char *sound_name,int event_id)
{
	strcpy(title_dir,dir);
	strcpy(title_name,name);
	strcpy(title_sound_name,sound_name);
	title_show_view=FALSE;
	title_event_id=event_id;
	
	title_start_trigger=TRUE;
}

/* =======================================================

      Run Title Page
      
======================================================= */

void title_run(void)
{
	int				tick;
	float			alpha;

		// get the fade

	alpha=1.0f;

	switch (title_fade_mode) {

		case title_fade_mode_in:
			tick=time_get()-title_fade_tick;
			if (tick>hud.fade.title_msec) {
				title_fade_mode=title_fade_mode_none;
				break;
			}
			alpha=((float)tick)/(float)hud.fade.title_msec;
			break;

		case title_fade_mode_out:
			tick=time_get()-title_fade_tick;
			if (tick>hud.fade.title_msec) {
				title_close();
				return;
			}
			alpha=1.0f-(((float)tick)/(float)hud.fade.title_msec);
			break;
	}

		// run the GUI

	gui_draw(alpha,FALSE);

	if ((title_fade_mode==title_fade_mode_none) && (input_gui_get_mouse_left_button_down())) {
		title_fade_tick=time_get();
		title_fade_mode=title_fade_mode_out;
	}
}


