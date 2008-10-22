/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Music

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

#include "sounds.h"

int							audio_music_buffer_idx,
							audio_music_fade_mode,audio_music_fade_start_tick,audio_music_fade_msec,
							audio_music_fade_next_msec,audio_music_original_volume;
char						audio_music_fade_next_name[name_str_len],audio_music_fade_next_path[1024];
bool						audio_music_state_on,audio_music_paused;

extern int					audio_buffer_count,audio_global_music_volume,
							audio_music_buffer_idx,audio_music_stream_pos;
extern bool					audio_music_playing;
extern audio_buffer_type	audio_buffers[audio_max_buffer];

/* =======================================================

      Music Initialize and Shutdown
      
======================================================= */

bool al_music_initialize(char *err_str)
{
	audio_music_state_on=TRUE;

	audio_music_playing=FALSE;
	audio_music_paused=FALSE;

	audio_music_buffer_idx=-1;

	audio_global_music_volume=600;
	
	return(TRUE);
}

void al_music_shutdown(void)
{
	if (audio_music_buffer_idx!=-1) al_close_buffer(audio_music_buffer_idx);
}

/* =======================================================

      Start and Stop Music
      
======================================================= */

bool al_music_play(char *name,char *path)
{
	bool				load;
	audio_buffer_type  *buffer;

	SDL_LockAudio();
	
		// start with no fade
		
	audio_music_fade_mode=music_fade_mode_none;
	
		// if same music, don't reload
		
	load=TRUE;
	
	if (audio_music_buffer_idx!=-1) {
		buffer=&audio_buffers[audio_music_buffer_idx];
		load=(strcmp(buffer->name,name)!=0);
	}
	
		// open music
		
	if (load) {
		audio_music_buffer_idx=al_open_buffer(name,path,0,0);

		if (audio_music_buffer_idx==-1) {
			SDL_UnlockAudio();
			return(FALSE);
		}
	}
	
	buffer=&audio_buffers[audio_music_buffer_idx];
	
		// play

	audio_music_stream_pos=0;
	audio_music_paused=FALSE;
		
	if (audio_music_state_on) audio_music_playing=TRUE;

	SDL_UnlockAudio();

	return(TRUE);
}

void al_music_stop(void)
{
	SDL_LockAudio();

	audio_music_stream_pos=0;
	audio_music_playing=FALSE;

	SDL_UnlockAudio();
}

void al_music_pause(void)
{
	if (audio_music_playing) {
		audio_music_paused=TRUE;
		audio_music_playing=FALSE;
	}
}

void al_music_resume(void)
{
	if (audio_music_paused) {
		audio_music_paused=FALSE;
		audio_music_playing=TRUE;
	}
}

bool al_music_playing(void)
{
	return(audio_music_playing);
}

bool al_music_playing_is_name(char *name)
{
	audio_buffer_type  *buffer;
	
	if (!audio_music_playing) return(FALSE);
	
	buffer=&audio_buffers[audio_music_buffer_idx];
	return(strcmp(buffer->name,name)==0);
}

/* =======================================================

      Set Music Volume and State
      
======================================================= */

void al_music_set_volume(float music_volume)
{
	audio_global_music_volume=(int)(1024.0f*music_volume);
}

void al_music_set_state(bool music_on)
{
	if (audio_music_state_on==music_on) return;
	
	audio_music_state_on=music_on;
	if (music_on) {
		al_music_resume();
	}
	else {
		al_music_pause();
	}
}

/* =======================================================

      Run Music
      
======================================================= */

bool al_music_fade_in(int tick,char *name,char *path,int msec)
{
		// start music

	if (!al_music_play(name,path)) return(FALSE);

		// if no msec, then just play music

	if (msec<=0) {
		audio_music_fade_mode=music_fade_mode_none;
		audio_music_original_volume=audio_global_music_volume;
		audio_global_music_volume=0;
		return(TRUE);
	}
	
		// start fade in

	audio_music_fade_mode=music_fade_mode_in;
	audio_music_fade_start_tick=tick;
	audio_music_fade_msec=msec;

	return(TRUE);
}

void al_music_fade_out(int tick,int msec)
{
		// if no music playing, no fade out

	if (!audio_music_playing) {
		audio_music_fade_mode=music_fade_mode_none;
		return;
	}
	
		// if no msec, then just stop music

	if (msec<=0) {
		al_music_stop();
		audio_global_music_volume=audio_music_original_volume;
		audio_music_fade_mode=music_fade_mode_none;
		return;
	}

		// start fade out

	audio_music_fade_mode=music_fade_mode_out;
	audio_music_fade_start_tick=tick;
	audio_music_fade_msec=msec;
}

bool al_music_fade_out_fade_in(int tick,char *name,char *path,int fade_out_msec,int fade_in_msec)
{
		// if no fade out or no music playing, go directly to fade in

	if ((fade_out_msec<=0) || (!audio_music_playing)) {
		return(al_music_fade_in(tick,name,path,fade_in_msec));
	}

		// setup next music for fade in

	strcpy(audio_music_fade_next_name,name);
	strcpy(audio_music_fade_next_path,path);
	audio_music_fade_next_msec=fade_in_msec;

		// start fade

	al_music_fade_out(tick,fade_out_msec);

	audio_music_fade_mode=music_fade_mode_out_fade_in;		// switch to fade out/fade in mode

	return(TRUE);
}

void al_music_run(int tick)
{
	int				dif;
	
		// is there a fade on?
		
	if (audio_music_fade_mode==music_fade_mode_none) return;
	
		// is music even playing?
		
	if (!audio_music_playing) {
		audio_music_fade_mode=music_fade_mode_none;
		audio_global_music_volume=audio_music_original_volume;
		return;
	}
	
		// time to stop fade?
		
	dif=tick-audio_music_fade_start_tick;
	if (dif>=audio_music_fade_msec) {

		switch (audio_music_fade_mode) {

			case music_fade_mode_in:
				audio_global_music_volume=audio_music_original_volume;
				audio_music_fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out:
				al_music_stop();
				audio_global_music_volume=audio_music_original_volume;
				audio_music_fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out_fade_in:
				al_music_fade_in(tick,audio_music_fade_next_name,audio_music_fade_next_path,audio_music_fade_next_msec);
				break;

		}

		return;
	}
	
		// set the fade volume
		
	switch (audio_music_fade_mode) {
		
		case music_fade_mode_in:
			audio_global_music_volume=(int)((float)audio_music_original_volume*(((float)dif)/(float)audio_music_fade_msec));
			break;

		case music_fade_mode_out:
		case music_fade_mode_out_fade_in:
			audio_global_music_volume=(int)((float)audio_music_original_volume*((float)(audio_music_fade_msec-dif)/(float)audio_music_fade_msec));
			break;

	}

}
