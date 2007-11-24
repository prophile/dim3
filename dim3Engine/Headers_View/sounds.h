/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Sound Functions Header

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

extern bool al_initialize(char *err_str);
extern void al_shutdown(void);
extern void al_set_volume(float sound_volume);

extern void al_load_wav_file(ALbyte *file,ALenum *format,void **data,ALsizei *size,ALsizei *freq, ALboolean *loop);
extern void al_free_wav_file(ALenum format,ALvoid *data,ALsizei size,ALsizei freq);

extern int al_open_buffer(char *name,char *path,int min_dist,int max_dist);
extern void al_close_buffer(int buffer_idx);
extern void al_close_all_buffers(void);
extern int al_find_buffer(char *name);
extern int al_get_buffer_max_dist(int buffer_idx);

extern void al_set_listener(int x,int y,int z,float ang_y);
extern int al_distance_to_listener(int x,int y,int z);

extern int al_play_source(int buffer_idx,int x,int y,int z,float pitch,bool loop,bool ambient,bool global,bool player);
extern void al_stop_source(int source_idx);

extern bool al_music_initialize(char *err_str);
extern void al_music_shutdown(void);
extern bool al_music_play(char *name,char *path);
extern void al_music_stop(void);
extern bool al_music_playing(void);
extern bool al_music_playing_is_name(char *name);
extern void al_music_set_volume(float music_volume);
extern void al_music_set_state(bool music_on);
extern bool al_music_fade_in(int tick,char *name,char *path,int msec);
extern void al_music_fade_out(int tick,int msec);
extern bool al_music_fade_out_fade_in(int tick,char *name,char *path,int fade_out_msec,int fade_in_msec);
extern void al_music_run(int tick);

extern void al_stop_all_sources(void);
extern void al_stop_all_looping_sources(void);

extern bool al_is_source_playing(int source_idx);
extern bool al_is_source_looping(int source_idx);
	
extern void al_ambient_list_clear(void);
extern void al_ambient_list_add(int buffer_idx,int x,int y,int z,float pitch);
extern void al_ambients_run(void);
extern int al_get_next_source_ambient(int source_idx);
