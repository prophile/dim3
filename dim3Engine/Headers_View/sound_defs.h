/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Sound Definitions Header

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

//
// audio setup
//

#define audio_frequency							44100
#define audio_format							AUDIO_S16LSB
#define audio_channels							2
#define audio_sample_size						8192

//
// sound maximums
//
	
#define al_max_source							32
#define al_max_buffer							512
#define al_max_ambient							8

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
						int						index,len;
						float					min_dist,max_dist;
						char					name[name_str_len];
						bool					loaded;
						unsigned char			*data;
						unsigned int			al_id;			// supergumba -- can delete a lot of this
					} al_buffer_type;
	
typedef struct		{
						int						buffer_idx;
						bool					ambient,player;
						unsigned int			al_id;
						d3pnt					pnt;
					} al_source_type;

typedef struct		{
                        int						buffer_idx,dist;
                        float					pitch;
						bool					hit;
						d3pnt					pnt;
					} al_ambient_type;
