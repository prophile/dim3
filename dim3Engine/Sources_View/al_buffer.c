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

extern int					audio_buffer_count;
extern audio_buffer_type	audio_buffers[audio_max_buffer];

/* =======================================================

      Load and Close OpenAL Buffers
      
======================================================= */

int al_open_buffer(char *name,char *path,int min_dist,int max_dist)
{
	int					len;
	unsigned char		*data;
	audio_buffer_type  *buffer;
	SDL_AudioSpec		aspec;
	SDL_AudioCVT		acvt;

		// any more buffers?

	if (audio_buffer_count>=audio_max_buffer) return(-1);

		// get next buffer

	buffer=&audio_buffers[audio_buffer_count];

		// load wave data

	if (SDL_LoadWAV(path,&aspec,(Uint8**)&data,(Uint32*)&len)==NULL) return(-1);

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

	buffer->data=(short*)acvt.buf;
	buffer->len=(int)(((double)acvt.len)*acvt.len_ratio);
	buffer->sample_len=buffer->len>>1;						// size of 16 bit samples
	buffer->f_sample_len=(float)buffer->sample_len;
	
	buffer->min_dist=(float)min_dist;
	buffer->max_dist=(float)max_dist;

	buffer->loaded=TRUE;

		// goto next buffer

	audio_buffer_count++;
	return(audio_buffer_count-1);
}

void al_close_buffer(int idx)
{
	audio_buffer_type  *buffer;

	buffer=&audio_buffers[idx];
	if (buffer->loaded) SDL_FreeWAV((Uint8*)buffer->data);

	buffer->loaded=FALSE;
}

void al_close_all_buffers(void)
{
	int		n;
	
	for (n=0;n!=audio_max_buffer;n++) {
		al_close_buffer(n);
	}
}

/* =======================================================

      Find a Buffer
      
======================================================= */

int al_find_buffer(char *name)
{
	int					n;
	audio_buffer_type	*buffer;
    
	buffer=audio_buffers;
	
	for (n=0;n!=audio_buffer_count;n++) {
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
	return((int)audio_buffers[buffer_idx].max_dist);
}


