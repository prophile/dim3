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

int							al_music_idx=-1,
							listener_x=0,listener_y=0,listener_z=0;
float						listener_ang_y=0;

extern int					al_buffer_count,al_source_count;
extern al_buffer_type		al_buffers[al_max_buffer];
extern al_source_type		al_sources[al_max_source];

/* =======================================================

      Set Listener
      
======================================================= */

void al_set_listener(int x,int y,int z,float ang_y)
{
	double		d;
	ALfloat		alf[6];
	
		// any changes in listener?
		
	if ((listener_x==x) && (listener_y==y) && (listener_z==z) && (listener_ang_y==ang_y)) return;
	
		// mark changes
		
	listener_x=x;
	listener_y=y;
	listener_z=z;
	
	listener_ang_y=ang_y;

		// setup openal listener
		
	d=(double)(ang_y*ANG_to_RAD);
	
	alf[0]=(ALfloat)cos(d);
	alf[1]=0;
	alf[2]=(ALfloat)sin(d);
	alf[3]=0;
	alf[4]=1;
	alf[5]=0;
	alListenerfv(AL_ORIENTATION,alf);
	
	alListener3f(AL_POSITION,(ALfloat)x,(ALfloat)y,(ALfloat)z);
}

/* =======================================================

      Distance To Listener
      
======================================================= */

int al_distance_to_listener(int x,int y,int z)
{
	return(distance_get(x,y,z,listener_x,listener_y,listener_z));
}

/* =======================================================

      Find Free Source
      
======================================================= */

int al_find_free_source(int x,int y,int z,bool player)
{
	int					n,d,cur_dist,cur_far_idx;
	al_source_type		*source;
	ALint				playing;
	
		// distance for new sound
		
	cur_far_idx=-1;

	if (player) {
		cur_dist=0;		// if player, replace with any sound not also from the player
	}
	else {
		cur_dist=al_distance_to_listener(x,y,z);
	}
	
		// find free sources
	
	source=al_sources;
	
	for (n=0;n!=al_source_count;n++) {
	
			// if source isn't playing, choose it
			
		alGetSourcei(source->al_id,AL_SOURCE_STATE,&playing);
		if (playing!=AL_PLAYING) return(n);
		
			// otherwise remember farthest sound
			// that's farther than new position and not
			// an ambient or from the player
			
		if ((!source->ambient) && (!source->player)) {
			d=al_distance_to_listener(source->pnt.x,source->pnt.y,source->pnt.z);
			if (d>=cur_dist) {
				cur_dist=d;
				cur_far_idx=n;
			}
		}
		
		source++;
	}
	
		// no free source found, did we find a
		// farther source we can replace?
		
	if (cur_far_idx==-1) return(-1);

		// stop and replace
	
	alSourceStop(al_sources[cur_far_idx].al_id);
	return(cur_far_idx);
}

/* =======================================================

      Play Sound
      
======================================================= */

int al_play_source(int buffer_idx,int x,int y,int z,float pitch,bool loop,bool ambient,bool global,bool player)
{
	int				source_idx,al_id;
	al_source_type  *source;
	al_buffer_type  *buffer;
	
	buffer=&al_buffers[buffer_idx];
	
		// find free source or farthest replacable source
		
	source_idx=al_find_free_source(x,y,z,player);
	if (source_idx==-1) return(-1);
	
		// setup source
	
	source=&al_sources[source_idx];
		
	source->buffer_idx=buffer_idx;
	source->ambient=ambient;
	
	source->pnt.x=x;
	source->pnt.y=y;
	source->pnt.z=z;

	source->player=player;
	
	al_id=source->al_id;
	
		// setup source to play
	
	alSourcei(al_id,AL_BUFFER,buffer->al_id);

		// position
		
	if (!global) {
		alSourcei(al_id,AL_SOURCE_RELATIVE,AL_FALSE);
		
		alSourcef(al_id,AL_REFERENCE_DISTANCE,buffer->min_dist);
		alSourcef(al_id,AL_MAX_DISTANCE,buffer->max_dist);
		alSourcef(al_id,AL_ROLLOFF_FACTOR,1.0f);
		
		alSource3f(al_id,AL_POSITION,(ALfloat)x,(ALfloat)y,(ALfloat)z);
	}
	else {
		alSourcei(al_id,AL_SOURCE_RELATIVE,AL_TRUE);
		alSource3f(al_id,AL_POSITION,0,0,0);
	}
	
		// pitch and looping
		
	alSourcef(al_id,AL_PITCH,pitch);
	alSourcei(al_id,AL_LOOPING,loop?AL_TRUE:AL_FALSE);
	
		// play
		
	alSourcePlay(al_id);
	
	return(source_idx);
}

void al_stop_source(int source_idx)
{
	alSourceStop(al_sources[source_idx].al_id);
}

/* =======================================================

      Stop All Sounds
      
======================================================= */

void al_stop_all_sources(void)
{
	int			n;
	
	for (n=0;n!=al_source_count;n++) {
		al_stop_source(n);
	}
}

void al_stop_all_looping_sources(void)
{
	int			n;
	ALint		looping;
	
	for (n=0;n!=al_source_count;n++) {
		alGetSourcei(al_sources[n].al_id,AL_LOOPING,&looping);
		if (looping==AL_TRUE) al_stop_source(n);
	}
}

/* =======================================================

      Sound Information
      
======================================================= */

bool al_is_source_playing(int source_idx)
{
	ALint			playing;
	
	alGetSourcei(al_sources[source_idx].al_id,AL_SOURCE_STATE,&playing);
	return(playing==AL_PLAYING);
}

bool al_is_source_looping(int source_idx)
{
	ALint			looping;
	
	alGetSourcei(al_sources[source_idx].al_id,AL_LOOPING,&looping);
	return(looping==AL_TRUE);
}

/* =======================================================

      Ambients
      
======================================================= */

int al_get_next_source_ambient(int source_idx)
{
	while (TRUE) {
		source_idx++;
		if (source_idx>=al_source_count) return(-1);
		if (al_is_source_playing(source_idx)) {
			if (al_sources[source_idx].ambient) return(source_idx);
		}
	}

	return(-1);
}

