/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Setup XML Definitions Header

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
// setup sizes
//

#define max_setup_action				128
#define max_setup_action_attach			4

#define max_setup_network_host			32

//
// setup values
//

// anistropic mode, texture quality mode, mipmap mode all defined in bitmaputility.h
// light mode defined in maputility.h

#define fsaa_mode_none					0
#define fsaa_mode_low					1
#define fsaa_mode_medium				2
#define fsaa_mode_high					3

#define setup_fsaa_mode_list_def		{"None","Low","Medium","High",""}

#define shadow_mode_none				0
#define shadow_mode_low					1
#define shadow_mode_high				2

#define setup_shadow_mode_list_def		{"None","Low","High",""}

#define timeout_mode_short				0
#define timeout_mode_medium				1
#define timeout_mode_long				2

#define setup_timeout_mode_list_def		{"Short","Medium","Long",""}
#define setup_timeout_mode_values		{3,5,10}

#define setup_joystick_mode_list_def	{"Not Used","Turn Only","Turning and Moving","Turning and Looking",""}

#define joystick_mode_not_used			0
#define joystick_mode_turn_only			1
#define joystick_mode_turn_move			2
#define joystick_mode_turn_look			3

//
// setup paths
//

typedef struct		{
						char						dim3_name[256],dim3_base_path[1024],dim3_data_path[1024];
					} setup_path_type;
					
//
// setup actions
//

typedef struct		{
						char						name[32],attach[max_setup_action_attach][32];
					} setup_action_type;
					
typedef struct		{
						int							naction;
						setup_action_type			actions[max_setup_action];
					} setup_action_list_type;
					
//
// setup screen
//

typedef struct		{
						int							x_sz,y_sz;
					} setup_screen_type;

//
// setup mouse/joystick axis
//

typedef struct		{
						float						speed,speed_min,speed_max,
													acceleration,acceleration_min,acceleration_max;
					} setup_axis_type;

//
// setup network
//

typedef struct		{
						char						ip[256];
					} setup_network_hosts_type;

typedef struct		{
						int							nhost,team_idx,timeout_mode;
						char						name[name_str_len];
						bool						show_names;
						setup_network_hosts_type	hosts[max_setup_network_host];
					} setup_network_type;

//
// setup editor overrides
//

typedef struct		{
						char						map[256];
						bool						on;
						d3pnt						pt;
						d3ang						ang;
					} setup_editor_override_type;

//
// setup struct
//
					
typedef struct		{
						int							screen_wid,screen_high,
													anisotropic_mode,texture_quality_mode,
													mipmap_mode,fsaa_mode,shadow_mode,
													joystick_mode;
						float						gamma,sound_volume,music_volume;
						bool						lock_fps_refresh,mipmap_card_generated,texture_compression,
													diffuse_lighting,ray_trace_lighting,
													bump_mapping,specular_mapping,glow_mapping,
													halo,mark,fog,
													always_run,toggle_run,invert_look,mouse_smooth,
													music_on,debug_console,window,window_editor;
						setup_path_type				path;
						setup_screen_type			screen;
						setup_axis_type				mouse_x,mouse_y,joystick_x,joystick_y;
						setup_action_list_type		action_list;
						setup_network_type			network;
						file_path_setup_type		file_path_setup;
						setup_editor_override_type	editor_override;
					} setup_type;

//
// fatal error struct
//

typedef struct		{
						char						title[256],descript[256];
						bool						is_error;
					} fatal_error_type;
