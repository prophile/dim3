/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Setup Routines

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



void audio_callback(void *userdata,Uint8 *stream,int len)
{


}



bool al_initialize(char *err_str)
{
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

	return(TRUE);
}

void al_shutdown(void)
{
	SDL_CloseAudio();
}

void al_set_volume(float sound_volume)
{
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