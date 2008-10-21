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

int							al_music_buffer_idx,
							al_music_fade_mode,al_music_fade_start_tick,al_music_fade_msec,
							al_music_fade_next_msec;
float						al_music_current_volume;
char						al_music_fade_next_name[name_str_len],al_music_fade_next_path[1024];
bool						al_music_state_on,al_music_ready;
ALuint						al_music_source_id;

extern int					al_buffer_count;
extern al_buffer_type		al_buffers[al_max_buffer];
extern al_source_type		al_sources[al_max_source];

#ifdef SDL_SOUND

bool						audio_music_paused;

extern int					audio_global_music_volume,
							audio_music_buffer_idx,audio_music_stream_pos;
extern bool					audio_music_playing;

bool al_music_initialize(char *err_str)
{
	al_music_state_on=TRUE;

	audio_music_playing=FALSE;
	audio_music_paused=FALSE;

	al_music_buffer_idx=-1;

	audio_global_music_volume=600;
	
	return(TRUE);
}

void al_music_shutdown(void)
{
	if (al_music_buffer_idx!=-1) al_close_buffer(al_music_buffer_idx);
}

bool al_music_play(char *name,char *path)
{
	bool			load;
	al_buffer_type  *buffer;

	SDL_LockAudio();
	
		// start with no fade
		
	al_music_fade_mode=music_fade_mode_none;
	
		// if same music, don't reload
		
	load=TRUE;
	
	if (al_music_buffer_idx!=-1) {
		buffer=&al_buffers[al_music_buffer_idx];
		load=(strcmp(buffer->name,name)!=0);
	}
	
		// open music
		
	if (load) {
		al_music_buffer_idx=al_open_buffer(name,path,0,0);

		if (al_music_buffer_idx==-1) {
			SDL_UnlockAudio();
			return(FALSE);
		}
	}
	
	buffer=&al_buffers[al_music_buffer_idx];
	
		// play

	audio_music_stream_pos=0;
	audio_music_paused=FALSE;
		
	if (al_music_state_on) audio_music_playing=TRUE;

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
	al_buffer_type  *buffer;
	
	if (!audio_music_playing) return(FALSE);
	
	buffer=&al_buffers[al_music_buffer_idx];
	return(strcmp(buffer->name,name)==0);
}

void al_music_set_volume(float music_volume)
{
	audio_global_music_volume=(int)(1024.0f*music_volume);
}

void al_music_set_state(bool music_on)
{
	if (al_music_state_on==music_on) return;
	
	al_music_state_on=music_on;
	if (music_on) {
		al_music_resume();
	}
	else {
		al_music_pause();
	}
}

bool al_music_fade_in(int tick,char *name,char *path,int msec)
{
	return(TRUE);
}

void al_music_fade_out(int tick,int msec)
{
}

bool al_music_fade_out_fade_in(int tick,char *name,char *path,int fade_out_msec,int fade_in_msec)
{
	return(TRUE);
}

void al_music_run(int tick)
{
}

#else

/* =======================================================

      Music Initialize and Shutdown
      
======================================================= */

bool al_music_initialize(char *err_str)
{
	int				err;
	
	alGetError();
	alGenSources(1,&al_music_source_id);
	err=alGetError();
	
	if (err!=AL_NO_ERROR) {
		sprintf(err_str,"OpenAL: Could not create source (Error: 0x%.4X)",err);
		return(FALSE);
	}
	
	al_music_buffer_idx=-1;
	
	al_music_state_on=TRUE;
	al_music_ready=FALSE;

	al_music_current_volume=0.6f;
	
	return(TRUE);
}

void al_music_shutdown(void)
{
	alSourceStop(al_music_source_id);
	if (al_music_buffer_idx!=-1) al_close_buffer(al_music_buffer_idx);
	alDeleteSources(1,&al_music_source_id);
}

/* =======================================================

      Start and Stop Music
      
======================================================= */

bool al_music_play(char *name,char *path)
{
	bool			load;
	al_buffer_type  *buffer;
	
		// stop any other music
		
	al_music_fade_mode=music_fade_mode_none;
	alSourceStop(al_music_source_id);
	
		// if same music, don't reload
		
	load=TRUE;
	
	if (al_music_buffer_idx!=-1) {
		buffer=&al_buffers[al_music_buffer_idx];
		load=(strcmp(buffer->name,name)!=0);
	}
	
		// open music
		
	if (load) {
		al_music_buffer_idx=al_open_buffer(name,path,0,0);
		if (al_music_buffer_idx==-1) return(FALSE);
	}
	
	buffer=&al_buffers[al_music_buffer_idx];
	
		// setup source to play
	
	alSourcei(al_music_source_id,AL_BUFFER,buffer->al_id);
	alSourcei(al_music_source_id,AL_SOURCE_RELATIVE,AL_TRUE);
	alSource3f(al_music_source_id,AL_POSITION,0,0,0);
	alSourcef(al_music_source_id,AL_PITCH,1);
	alSourcei(al_music_source_id,AL_LOOPING,AL_TRUE);
	
		// play
		
	al_music_ready=TRUE;
	if (al_music_state_on) alSourcePlay(al_music_source_id);

	return(TRUE);
}

void al_music_stop(void)
{
		// stop music
		
	alSourceStop(al_music_source_id);
	
		// was it fading?
		
	if (al_music_fade_mode!=music_fade_mode_none) {
		al_music_fade_mode=music_fade_mode_none;
		alSourcef(al_music_source_id,AL_GAIN,al_music_current_volume);
	}
	
		// nothing to resume
		
	al_music_ready=FALSE;
}

void al_music_pause(void)
{
	if (al_music_ready) alSourceStop(al_music_source_id);
}

void al_music_resume(void)
{
	if (al_music_state_on) alSourcePlay(al_music_source_id);
}

bool al_music_playing(void)
{
	ALint			playing;
	
	alGetSourcei(al_music_source_id,AL_SOURCE_STATE,&playing);
	return(playing==AL_PLAYING);
}

bool al_music_playing_is_name(char *name)
{
	al_buffer_type  *buffer;
	
	if (!al_music_playing()) return(FALSE);
	if (al_music_buffer_idx==-1) return(FALSE);
	
	buffer=&al_buffers[al_music_buffer_idx];
	return(strcmp(buffer->name,name)==0);
}

/* =======================================================

      Set Music Volume and State
      
======================================================= */

void al_music_set_volume(float music_volume)
{
	alSourcef(al_music_source_id,AL_GAIN,music_volume);
	al_music_current_volume=music_volume;
}

void al_music_set_state(bool music_on)
{
	if (al_music_state_on==music_on) return;
	
	al_music_state_on=music_on;
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
		al_music_fade_mode=music_fade_mode_none;
		alSourcef(al_music_source_id,AL_GAIN,al_music_current_volume);
		return(TRUE);
	}
	
		// start fade in

	al_music_fade_mode=music_fade_mode_in;
	al_music_fade_start_tick=tick;
	al_music_fade_msec=msec;

	return(TRUE);
}

void al_music_fade_out(int tick,int msec)
{
		// if no music playing, no fade out

	if (!al_music_playing()) {
		al_music_fade_mode=music_fade_mode_none;
		return;
	}
	
		// if no msec, then just stop music

	if (msec<=0) {
		al_music_fade_mode=music_fade_mode_none;
		alSourceStop(al_music_source_id);
		alSourcef(al_music_source_id,AL_GAIN,al_music_current_volume);
		return;
	}

		// start fade out

	al_music_fade_mode=music_fade_mode_out;
	al_music_fade_start_tick=tick;
	al_music_fade_msec=msec;
}

bool al_music_fade_out_fade_in(int tick,char *name,char *path,int fade_out_msec,int fade_in_msec)
{
		// if no fade out or no music playing, go directly to fade in

	if ((fade_out_msec<=0) || (!al_music_playing())) {
		return(al_music_fade_in(tick,name,path,fade_in_msec));
	}

		// setup next music for fade in

	strcpy(al_music_fade_next_name,name);
	strcpy(al_music_fade_next_path,path);
	al_music_fade_next_msec=fade_in_msec;

		// start fade

	al_music_fade_out(tick,fade_out_msec);

	al_music_fade_mode=music_fade_mode_out_fade_in;		// switch to fade out/fade in mode

	return(TRUE);
}

void al_music_run(int tick)
{
	int				dif;
	float			vol;
	
		// is there a fade on?
		
	if (al_music_fade_mode==music_fade_mode_none) return;
	
		// is music even playing?
		
	if (!al_music_playing()) {
		al_music_fade_mode=music_fade_mode_none;
		alSourcef(al_music_source_id,AL_GAIN,al_music_current_volume);
		return;
	}
	
		// time to stop fade?
		
	dif=tick-al_music_fade_start_tick;
	if (dif>=al_music_fade_msec) {

		switch (al_music_fade_mode) {

			case music_fade_mode_in:
				alSourcef(al_music_source_id,AL_GAIN,al_music_current_volume);
				al_music_fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out:
				alSourceStop(al_music_source_id);
				alSourcef(al_music_source_id,AL_GAIN,al_music_current_volume);
				al_music_fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out_fade_in:
				al_music_fade_in(tick,al_music_fade_next_name,al_music_fade_next_path,al_music_fade_next_msec);
				break;

		}

		return;
	}
	
		// set the fade volume
		
	switch (al_music_fade_mode) {
		
		case music_fade_mode_in:
			vol=al_music_current_volume*(((float)dif)/(float)al_music_fade_msec);
			alSourcef(al_music_source_id,AL_GAIN,vol);
			break;

		case music_fade_mode_out:
		case music_fade_mode_out_fade_in:
			vol=al_music_current_volume*((float)(al_music_fade_msec-dif)/(float)al_music_fade_msec);
			alSourcef(al_music_source_id,AL_GAIN,vol);
			break;

	}
	
}

#endif