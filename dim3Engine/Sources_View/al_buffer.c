/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Buffer and Sound Loading

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

extern int				al_buffer_count;
extern al_buffer_type   al_buffers[al_max_buffer];

#ifdef SDL_SOUND

/*
#define AUDIO_U8	0x0008	-- Unsigned 8-bit samples --
#define AUDIO_S8	0x8008	-- Signed 8-bit samples --
#define AUDIO_U16LSB	0x0010	-- Unsigned 16-bit samples --
#define AUDIO_S16LSB	0x8010	-- Signed 16-bit samples --
#define AUDIO_U16MSB	0x1010	-- As above, but big-endian byte order --
#define AUDIO_S16MSB	0x9010	-- As above, but big-endian byte order --
#define AUDIO_U16	AUDIO_U16LSB
#define AUDIO_S16	AUDIO_S16LSB

-- Native audio byte ordering --
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define AUDIO_U16SYS	AUDIO_U16LSB
#define AUDIO_S16SYS	AUDIO_S16LSB
#else
#define AUDIO_U16SYS	AUDIO_U16MSB
#define AUDIO_S16SYS	AUDIO_S16MSB
#endif
*/

int al_open_buffer(char *name,char *path,int min_dist,int max_dist)
{
	int				len;
	unsigned char	*data;
	al_buffer_type  *buffer;
	SDL_AudioSpec	aspec;
	SDL_AudioCVT	acvt;

		// any more buffers?

	if (al_buffer_count>=al_max_buffer) return(-1);

		// get next buffer

	buffer=&al_buffers[al_buffer_count];

		// load wave data

	if (SDL_LoadWAV(path,&aspec,&data,&len)==NULL) return(-1);

		// convert data to required format

	if (SDL_BuildAudioCVT(&acvt,aspec.format,aspec.channels,aspec.freq,audio_format,1,audio_frequency)==-1) {
		SDL_FreeWAV(data);
		return(-1);
	}

	acvt.buf=valloc(len*acvt.len_mult);
	acvt.len=len;
	memcpy(acvt.buf,data,len);

	SDL_FreeWAV(data);

	if (SDL_ConvertAudio(&acvt)==-1) return(-1);

		// setup buffer

	strcpy(buffer->name,name);

	buffer->data=acvt.buf;
	buffer->len=(int)(((double)acvt.len)*acvt.len_ratio);
	
	buffer->min_dist=(float)min_dist;
	buffer->max_dist=(float)max_dist;

	buffer->loaded=TRUE;

		// goto next buffer

	al_buffer_count++;
	return(al_buffer_count-1);
}

void al_close_buffer(int idx)
{
	al_buffer_type  *buffer;

	buffer=&al_buffers[idx];
	if (buffer->loaded) SDL_FreeWAV(al_buffers[idx].data);

	buffer->loaded=FALSE;
}

void al_close_all_buffers(void)
{
	int		n;
	
	for (n=0;n!=al_max_buffer;n++) {
		al_close_buffer(n);
	}
}

int al_find_buffer(char *name)
{
	int					n;
	al_buffer_type		*buffer;
    
	buffer=al_buffers;
	
	for (n=0;n!=al_buffer_count;n++) {
		if (buffer->loaded) {
			if (strcmp(buffer->name,name)==0) return(n);
		}
		buffer++;
	}

	return(-1);
}

int al_get_buffer_max_dist(int buffer_idx)
{
	return((int)al_buffers[buffer_idx].max_dist);
}


#else

/* =======================================================

      Load and Close OpenAL Buffers
      
======================================================= */

int al_open_buffer(char *name,char *path,int min_dist,int max_dist)
{
	int				err;
	ALenum			format;
	ALsizei			size,freq;
    ALboolean		loop;
	ALvoid			*data;
	al_buffer_type  *buffer;

		// load the wave
		
	alGetError();
	
    loop=FALSE;
	al_load_wav_file(path,&format,&data,&size,&freq,&loop);

	if ((data==NULL) || (alGetError()!=AL_NO_ERROR)) return(-1);
	
		// initialize the buffer
		
	buffer=&al_buffers[al_buffer_count];
	strcpy(buffer->name,name);
	
	alGenBuffers(1,&buffer->al_id);
	if (alGetError()!=AL_NO_ERROR) {
		al_free_wav_file(format,data,size,freq);
		return(-1);
	}
	
	alBufferData(buffer->al_id,format,data,size,freq);
	err=alGetError();
	
	al_free_wav_file(format,data,size,freq);

	if (alGetError()!=AL_NO_ERROR) {
		alDeleteBuffers(1,&buffer->al_id);
		return(-1);
	}
	
	buffer->min_dist=(float)min_dist;
	buffer->max_dist=(float)max_dist;

	buffer->loaded=TRUE;
	
	al_buffer_count++;
	return(al_buffer_count-1);
}

void al_close_buffer(int idx)
{
	al_buffer_type  *buffer;
	
	buffer=&al_buffers[idx];
	if (buffer->loaded) alDeleteBuffers(1,&buffer->al_id);
	
	buffer->loaded=FALSE;
}

void al_close_all_buffers(void)
{
	int		n;
	
	for (n=0;n!=al_max_buffer;n++) {
		al_close_buffer(n);
	}
}

/* =======================================================

      Find a Buffer
      
======================================================= */

int al_find_buffer(char *name)
{
	int					n;
	al_buffer_type		*buffer;
    
	buffer=al_buffers;
	
	for (n=0;n!=al_buffer_count;n++) {
		if (buffer->loaded) {
			if (strcmp(buffer->name,name)==0) return(n);
		}
		buffer++;
	}

	return(-1);
}
	
/* =======================================================

      Buffer Info
      
======================================================= */

int al_get_buffer_max_dist(int buffer_idx)
{
	return((int)al_buffers[buffer_idx].max_dist);
}

#endif