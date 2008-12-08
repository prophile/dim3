/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Playing

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

extern int					audio_buffer_count,audio_global_sound_volume;
extern float				audio_listener_ang_y;
extern d3pnt				audio_listener_pnt;
extern audio_buffer_type	audio_buffers[audio_max_buffer];
extern audio_play_type		audio_plays[audio_max_play];

/* =======================================================

      Set Listener
      
======================================================= */

void al_set_listener(d3pnt *pnt,float ang_y)
{
	SDL_LockAudio();

		// position

	audio_listener_pnt.x=pnt->x;
	audio_listener_pnt.y=pnt->y;
	audio_listener_pnt.z=pnt->z;

		// add 90 degrees to put ears on side of head

	audio_listener_ang_y=angle_add(ang_y,-90.0f);

	SDL_UnlockAudio();
}

/* =======================================================

      Distance To Listener
      
======================================================= */

int al_distance_to_listener(d3pnt *pnt)
{
	return(distance_get(pnt->x,pnt->y,pnt->z,audio_listener_pnt.x,audio_listener_pnt.y,audio_listener_pnt.z));
}

/* =======================================================

      Play Sound
      
======================================================= */

int al_play_source(int buffer_idx,d3pnt *pnt,float pitch,bool loop,bool ambient,bool no_position,bool no_cancel)
{
	int					n,idx,non_no_cancel_idx;
	audio_play_type		*play;

	SDL_LockAudio();

		// find an open play structure
		// also find first non no_cancel
		// source in case we need to replace one

	idx=non_no_cancel_idx=-1;
	play=audio_plays;

	for (n=0;n!=audio_max_play;n++) {
		if (!play->used) {
			idx=n;
			break;
		}
		if ((non_no_cancel_idx==-1) && (!play->no_cancel)) non_no_cancel_idx=n;
		play++;
	}

		// if possible, replace another sound

	if (idx==-1) idx=non_no_cancel_idx;

		// no place, can't place sound

	if (idx==-1) {
		SDL_UnlockAudio();
		return(-1);
	}

		// setup play structure

	play=&audio_plays[idx];

	play->buffer_idx=buffer_idx;
	play->stream_pos=0.0f;

	play->pitch=pitch;

	if (pnt!=NULL) {
		play->pnt.x=pnt->x;
		play->pnt.y=pnt->y;
		play->pnt.z=pnt->z;
	}
	else {
		play->pnt.x=play->pnt.y=play->pnt.z=0;
	}

	play->loop=loop;
	play->ambient=ambient;
	play->no_position=no_position;
	play->no_cancel=no_cancel;

	play->used=TRUE;

	SDL_UnlockAudio();

	return(idx);
}

void al_stop_source(int play_idx)
{
	SDL_LockAudio();

	audio_plays[play_idx].used=FALSE;

	SDL_UnlockAudio();
}

/* =======================================================

      Stop All Sounds
      
======================================================= */

void al_stop_all_sources(void)
{
	int					n;
	audio_play_type		*play;

	SDL_LockAudio();

	play=audio_plays;

	for (n=0;n!=audio_max_play;n++) {
		play->used=FALSE;
		play++;
	}

	SDL_UnlockAudio();
}

void al_stop_all_looping_sources(void)
{
	int					n;
	audio_play_type		*play;

	SDL_LockAudio();

	play=audio_plays;

	for (n=0;n!=audio_max_play;n++) {
		if (play->loop) play->used=FALSE;
		play++;
	}

	SDL_UnlockAudio();
}

