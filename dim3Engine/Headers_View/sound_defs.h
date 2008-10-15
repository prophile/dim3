/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Sound Definitions Header

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

#define SDL_SOUND			1
// supergumba -- temporary, trying to switch to SDL sound

//
// audio setup
//

#define audio_frequency							44100
#define audio_format							AUDIO_S16LSB
#define audio_channels							2
#define audio_sample_size						2048

//
// sound maximums
//
	
#define al_max_source							32
#define al_max_buffer							512
#define al_max_ambient							8

#define audio_max_play							32

//
// fade directions
//

#define music_fade_mode_none					0
#define music_fade_mode_in						1
#define music_fade_mode_out						2
#define music_fade_mode_out_fade_in				3

//
// sound structures
//
	
typedef struct		{
						int						index,len,sample_len;
						float					min_dist,max_dist;
						char					name[name_str_len];
						bool					loaded;
						short					*data;
						unsigned int			al_id;			// supergumba -- can delete a lot of this
					} al_buffer_type;

typedef struct		{
                        int						buffer_idx,stream_pos,
												left_fact,right_fact,vol_fact;
                        float					pitch;
						bool					used,skip,loop,no_position,no_cancel;
						d3pnt					pnt;
					} audio_play_type;
	
typedef struct		{
						int						buffer_idx;
						bool					ambient,player2;
						unsigned int			al_id;
						d3pnt					pnt;
					} al_source_type;						// supergumba -- can delete

typedef struct		{
                        int						buffer_idx,dist;
                        float					pitch;
						bool					hit;
						d3pnt					pnt;
					} al_ambient_type;
