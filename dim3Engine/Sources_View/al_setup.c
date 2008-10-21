/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Setup Routines

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

#include "sounds.h"

int						al_buffer_count,al_source_count;

ALCcontext				*al_context;
ALCdevice				*al_device;

al_buffer_type			al_buffers[al_max_buffer];
al_source_type			al_sources[al_max_source];

#ifdef D3_OS_MAC
	ALC_API ALvoid alcMacOSXRenderingQuality(const ALint value);
	ALC_API ALvoid alMacOSXRenderChannelCount(const ALint value);
#endif

#ifdef SDL_SOUND

int						audio_global_sound_volume,audio_global_music_volume,
						audio_music_buffer_idx,audio_music_stream_pos;
float					audio_listener_ang_y;
bool					audio_music_playing;
d3pnt					audio_listener_pnt;
audio_play_type			audio_plays[audio_max_play];


void audio_callback(void *userdata,Uint8 *stream,int len)
{
	int					n,k,stream_len,dist,vol,
						left_channel,right_channel;
	short				*s_stream,data;
	float				ang;
	double				d_ang;
	bool				has_play;
	audio_play_type		*play;
	al_buffer_type		*buffer;

		// calculate pitch and left/right channel multiplications
		// here.  We use a x/1024 factor to save on having to do
		// any int -> float conversions

	has_play=FALSE;

	for (n=0;n!=audio_max_play;n++) {

		play=&audio_plays[n];

			// start with sound being skipped, only
			// use if a sound exists and it's within the
			// maximum distance

		play->skip=TRUE;

		if (!play->used) continue;

			// mark to see if we have any sounds at all

		has_play=TRUE;

			// is there positioning?

		if (play->no_position) {
			play->skip=FALSE;
			play->left_fact=play->right_fact=1024;
			continue;
		}

			// are we within the maximum distance?

		buffer=&al_buffers[play->buffer_idx];

		dist=distance_get(play->pnt.x,play->pnt.y,play->pnt.z,audio_listener_pnt.x,audio_listener_pnt.y,audio_listener_pnt.z);
		if (dist>=buffer->max_dist) continue;

			// calculate the volume

		play->skip=FALSE;

		if (dist<=buffer->min_dist) {
			vol=1024;
		}
		else {
			vol=1024-(int)(1024.0f*((float)(dist-buffer->min_dist)/(float)(buffer->max_dist-buffer->min_dist)));
		}

			// get position to source
			// need to add in player's Y angle

		ang=angle_find(audio_listener_pnt.x,audio_listener_pnt.z,play->pnt.x,play->pnt.z);
		ang=angle_add(ang,-audio_listener_ang_y);

			// calculate left/right channels

		if (ang<180.0f) {
			d_ang=(double)((ang/2.0f)*ANG_to_RAD);
		}
		else {
			d_ang=(double)(((350.0f-ang)/2.0f)*ANG_to_RAD);
		}
		
		play->left_fact=(int)(((float)vol)*fabs(cos(d_ang)));
		play->right_fact=(int)(((float)vol)*fabs(sin(d_ang)));
	}

		// if no plays or music, skip audio mix

	if ((!has_play) && (!audio_music_playing)) return;

		// mix the audio

	s_stream=(short*)stream;
	stream_len=len>>2;					// we are in stereo and 16 bit samples, so divide len by 4

	for (k=0;k<stream_len;k++) {

		left_channel=0;
		right_channel=0;

			// mix plays

		play=audio_plays;

		for (n=0;n!=audio_max_play;n++) {

			if (!play->skip) {

					// get stream data

				buffer=&al_buffers[play->buffer_idx];
				data=(int)(*(buffer->data+play->stream_pos));

					// create the channels

				left_channel+=((data*play->left_fact)>>10);
				right_channel+=((data*play->right_fact)>>10);
				
					// move onto next position in stream
					// or loop or stop sound

				play->stream_pos++;
				if (play->stream_pos>=buffer->sample_len) {

					if (play->loop) {
						play->stream_pos=0;
					}
					else {
						play->used=FALSE;
						play->skip=TRUE;
					}

				}

			}

			play++;
		}

			// global volume adjustment

		left_channel=((left_channel*audio_global_sound_volume)>>10);
		right_channel=((right_channel*audio_global_sound_volume)>>10);
		
			// add in the music

		if (audio_music_playing) {
			buffer=&al_buffers[audio_music_buffer_idx];

			data=(int)(*(buffer->data+audio_music_stream_pos));
			vol=(data*audio_global_music_volume)>>10;

			left_channel+=vol;
			right_channel+=vol;

			audio_music_stream_pos++;
			if (audio_music_stream_pos>=buffer->sample_len) audio_music_stream_pos=0;
		}

			// fix any overflow

		if (left_channel<-32768) left_channel=-32768;
		if (left_channel>32768) left_channel=32768;
		if (right_channel<-32768) right_channel=-32768;
		if (right_channel>32768) right_channel=32768;

			// send channels to stream

		*s_stream++=(short)left_channel;
		*s_stream++=(short)right_channel;
	}
}



bool al_initialize(char *err_str)
{
	int				n;
	SDL_AudioSpec	aspec,ospec;

	SDL_InitSubSystem(SDL_INIT_AUDIO);		// supergumba -- move to SDL_Init

		// initialize SDL audio

	aspec.freq=audio_frequency;
	aspec.format=audio_format;
	aspec.channels=audio_channels;
	aspec.samples=audio_sample_size;
	aspec.callback=audio_callback;
	aspec.userdata=NULL;

	if (SDL_OpenAudio(&aspec,&ospec)==-1) {
		sprintf(err_str,"Audio: Could not initialize: %s",SDL_GetError());
		return(FALSE);
	}

		// make sure we got the setup we need

	if ((ospec.freq!=audio_frequency) || (ospec.format!=audio_format) || (ospec.channels!=audio_channels)) {
		SDL_CloseAudio();
		sprintf(err_str,"Audio: Unable to get desired audio format");
		return(FALSE);

	}

		// no buffers loaded

	al_buffer_count=0;

		// no sounds playing

	for (n=0;n!=audio_max_play;n++) {
		audio_plays[n].used=FALSE;
	}

		// default global volumes
		// sound volumes are factors of x/1024

	audio_global_sound_volume=512;
	audio_global_music_volume=512;

		// start loop

	SDL_PauseAudio(0);

	return(TRUE);
}

void al_shutdown(void)
{
	SDL_PauseAudio(1);
	SDL_CloseAudio();
}

void al_set_volume(float sound_volume)
{
	audio_global_sound_volume=(int)(1024.0f*sound_volume);
}

#else


/* =======================================================

      Initialize and Shutdown OpenAL
      
======================================================= */

bool al_initialize(char *err_str)
{
	int				n,err;
	
		// create device and context
		
 	al_device=alcOpenDevice(NULL);
	if (al_device==NULL) {
		strcpy(err_str,"OpenAL: Could not open device");
		return(FALSE);
	}
	
	al_context=alcCreateContext(al_device,0);
	if (al_context==NULL) {
		alcCloseDevice(al_device);
		strcpy(err_str,"OpenAL: Could not create context");
		return(FALSE);
	}

	alcMakeContextCurrent(al_context);
	
		// initialize music
		
	if (!al_music_initialize(err_str)) return(FALSE);
	
		// create the sample sources
		
	al_source_count=0;

	for (n=0;n!=al_max_source;n++) {

		alGetError();
		alGenSources(1,&al_sources[n].al_id);
		err=alGetError();

		if (err!=AL_NO_ERROR) break;			// reached max number of sources for this implementation

		al_source_count++;
	}
	
		// some initial setup
	
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alListenerf(AL_GAIN,1);

#ifdef D3_OS_MAC
	alEnable(ALC_MAC_OSX_CONVERT_DATA_UPON_LOADING);
	alcMacOSXRenderingQuality(ALC_MAC_OSX_SPATIAL_RENDERING_QUALITY_LOW);
	alMacOSXRenderChannelCount(ALC_MAC_OSX_RENDER_CHANNEL_COUNT_STEREO);
#endif

	alDopplerFactor(0);
		
		// no buffers loaded
	
	al_buffer_count=0;

	return(TRUE);
}

void al_shutdown(void)
{
	int			n;
	ALuint		al_id[al_max_source];
	
		// shutdown music
		
	al_music_shutdown();
	
		// close all buffers
		
	al_close_all_buffers();
	
		// close sources
		
	for (n=0;n!=al_source_count;n++) {
		al_id[n]=al_sources[n].al_id;
	}

	alDeleteSources(al_source_count,al_id);
	
		// close context and device

	alcMakeContextCurrent(NULL);
	alcDestroyContext(al_context);
	alcCloseDevice(al_device);
}

/* =======================================================

      Set OpenAL Volumes
      
======================================================= */

void al_set_volume(float sound_volume)
{
	int			n;
	
	for (n=0;n!=al_source_count;n++) {
		alSourcef(al_sources[n].al_id,AL_GAIN,sound_volume);
	}
}

#endif