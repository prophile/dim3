/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: JavaScript Definitions Header

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
// Javascript Engine
//

#ifdef D3_OS_MAC
	#define XP_UNIX
	#include <js/jsapi.h>
	#include <js/jsconfig.h>
#endif

#ifdef D3_OS_LINUX
	#define XP_UNIX
	#include "jsapi.h"
	#include "jsconfig.h"
#endif

#ifdef D3_OS_WINDOWS
	#define XP_WIN
	#include "jsapi.h"
	#include "jsconfig.h"
#endif

//
// Script Defines
//

#define max_script_cache							1024
#define max_scripts									1024
#define max_timers									1024
#define max_moves									256
#define max_globals									1024

#define js_script_memory_size						(20*1024*1024)

//
// Thing types
//

#define thing_type_game								0
#define thing_type_course							1
#define thing_type_object							2
#define thing_type_weapon							3
#define thing_type_projectile_setup					4
#define thing_type_projectile						5

//
// Timer types
//

#define timer_mode_single							0
#define timer_mode_repeat							1
#define timer_mode_chain							2
#define timer_mode_dispose							3

//
// hit constants
//

#define sd_proj_hit_type_none							1000
#define sd_proj_hit_type_object							1001
#define sd_proj_hit_type_wall							1002
#define sd_proj_hit_type_floor							1003
#define sd_proj_hit_type_ceiling						1004
#define sd_proj_hit_type_projectile						1005
#define sd_proj_hit_type_melee							1006
#define sd_proj_hit_type_auto							1007

//
// crosshair constants
//

#define sd_weap_crosshair_type_center					1010
#define sd_weap_crosshair_type_bone_tracking			1011
#define sd_weap_crosshair_type_bone_tracking_resizing	1012
#define sd_weap_crosshair_type_barrel_tracking			1013
#define sd_weap_crosshair_type_barrel_tracking_resizing	1014

//
// camera constants
//

#define sd_camera_type_fpp								1020
#define sd_camera_type_chase							1021
#define sd_camera_type_static							1022
#define sd_camera_type_chase_static						1023

//
// input constants
//

#define sd_input_mode_fpp								1030
#define sd_input_mode_side_scroll						1031
#define sd_input_mode_top_down							1032
#define sd_input_mode_fly								1033
#define sd_input_mode_thrust							1034

//
// light constants
//

#define sd_light_type_normal							1040
#define sd_light_type_blink								1041
#define sd_light_type_glow								1042
#define sd_light_type_pulse								1043
#define sd_light_type_flicker							1044
#define sd_light_type_failing							1045

#define sd_light_direction_all							1050
#define sd_light_direction_neg_x						1051
#define sd_light_direction_pos_x						1052
#define sd_light_direction_neg_y						1053
#define sd_light_direction_pos_y						1054
#define sd_light_direction_neg_z						1055
#define sd_light_direction_pos_z						1056

//
// model constants
//

#define sd_model_lit_flat								1060
#define sd_model_lit_hilite								1061
#define sd_model_lit_hilite_diffuse						1062
#define sd_model_lit_vertex								1063

#define sd_model_shadow_mode_normal						1070
#define sd_model_shadow_mode_static						1071

//
// message constants
//

#define sd_message_to_player							1080
#define sd_message_to_object							1081
#define sd_message_to_course							1082
#define sd_message_to_game								1083

//
// ducking constants
//

#define sd_stand_standing								1090
#define sd_stand_ducking								1091
#define sd_stand_standing_up							1092
#define sd_stand_ducking_down							1093

//
// air constants
//

#define sd_air_up										1100
#define sd_air_down										1101
#define sd_air_falling									1102
#define sd_air_ground									1103

//
// liquid constants
//

#define sd_liquid_out									1110
#define sd_liquid_in									1111
#define sd_liquid_under									1112

//
// projection constants
//

#define sd_projection_type_fov							1130
#define sd_projection_type_frustum						1131

//
// team constants
// (these must be equal to defines in networkutility.h)
//

#define sd_team_none									1140
#define sd_team_red										1141
#define sd_team_blue									1142

//
// skill constants
//

#define sd_skill_easy									1150
#define sd_skill_medium									1151
#define sd_skill_hard									1152

//
// main events constants
//

#define sd_event_construct								2000
#define sd_event_map									2001
#define sd_event_spawn									2002
#define sd_event_die									2003
#define sd_event_telefrag								2004
#define sd_event_jump									2005
#define sd_event_fall									2006
#define sd_event_land									2007
#define sd_event_collide								2008
#define sd_event_liquid									2009
#define sd_event_animation_object						2010
#define sd_event_animation_weapon						2011
#define sd_event_timer									2012
#define sd_event_wait									2013
#define sd_event_message								2014
#define sd_event_touch									2015
#define sd_event_hit									2016
#define sd_event_click									2017
#define sd_event_damage									2018
#define sd_event_crush									2019
#define sd_event_health									2020
#define sd_event_pickup									2021
#define sd_event_add_custom								2022
#define sd_event_path									2023
#define sd_event_watch									2024
#define sd_event_weapon_fire							2025
#define sd_event_weapon_select							2026
#define sd_event_projectile								2027
#define sd_event_melee									2028
#define sd_event_vehicle								2029
#define sd_event_move									2030
#define sd_event_menu									2031
#define sd_event_chooser								2032
#define sd_event_console								2033
#define sd_event_interface								2034
#define sd_event_state									2035
#define sd_event_rule									2036
#define sd_event_remote									2037

//
// sub events constants
//

#define sd_event_map_open								3000
#define sd_event_map_close								3001

#define sd_event_liquid_in								3002
#define sd_event_liquid_out								3003
#define sd_event_liquid_submerge						3004
#define sd_event_liquid_surface							3005

#define sd_event_animation_object_walk					3006
#define sd_event_animation_object_run					3007	
#define sd_event_animation_object_crawl					3008

#define sd_event_animation_object_walk_back				3009
#define sd_event_animation_object_run_back				3010
#define sd_event_animation_object_crawl_back			3011

#define sd_event_animation_object_walk_left				3012
#define sd_event_animation_object_run_left				3013
#define sd_event_animation_object_crawl_left			3014
#define sd_event_animation_object_walk_left_forward		3015
#define sd_event_animation_object_run_left_forward		3016
#define sd_event_animation_object_crawl_left_forward	3017
#define sd_event_animation_object_walk_left_back		3018
#define sd_event_animation_object_run_left_back			3019
#define sd_event_animation_object_crawl_left_back		3020

#define sd_event_animation_object_walk_right			3021
#define sd_event_animation_object_run_right				3022
#define sd_event_animation_object_crawl_right			3023
#define sd_event_animation_object_walk_right_forward	3024
#define sd_event_animation_object_run_right_forward		3025
#define sd_event_animation_object_crawl_right_forward	3026
#define sd_event_animation_object_walk_right_back		3027
#define sd_event_animation_object_run_right_back		3028
#define sd_event_animation_object_crawl_right_back		3029

#define sd_event_animation_object_turn_left				3030
#define sd_event_animation_object_turn_right			3031
#define sd_event_animation_object_turn_stop				3032

#define sd_event_animation_object_stop					3033
#define sd_event_animation_object_fall					3034
#define sd_event_animation_object_land					3035
#define sd_event_animation_object_jump					3036
#define sd_event_animation_object_duck_down				3037
#define sd_event_animation_object_ducking				3038
#define sd_event_animation_object_stand_up				3039
#define sd_event_animation_object_standing				3040

#define sd_event_message_from_script					3041
#define sd_event_message_from_course					3042
#define sd_event_message_from_key_down					3043
#define sd_event_message_from_key_up					3044

#define sd_event_animation_weapon_raise					3045
#define sd_event_animation_weapon_lower					3046
#define sd_event_animation_weapon_held					3047

#define sd_event_path_node								3048
#define sd_event_path_done								3049

#define sd_event_watch_object_near						3050
#define sd_event_watch_object_far						3051
#define sd_event_watch_object_death						3052
#define sd_event_watch_object_enter_base				3053
#define sd_event_watch_object_exit_base					3054
#define sd_event_watch_object_sound						3055

#define sd_event_weapon_fire_single						3058
#define sd_event_weapon_fire_up							3059
#define sd_event_weapon_fire_down						3060
#define sd_event_weapon_fire_repeat						3061
#define sd_event_weapon_fire_clip_change				3062
#define sd_event_weapon_fire_zoom_enter					3063
#define sd_event_weapon_fire_zoom_exit					3064

#define sd_event_projectile_bounce						3065
#define sd_event_projectile_reflect						3066

#define sd_event_melee_hit								3067

#define sd_event_vehicle_enter							3068
#define sd_event_vehicle_exit							3069

#define sd_event_move_done								3070
#define sd_event_move_loop								3071

#define sd_event_interface_title_done					3072
#define sd_event_interface_story_done					3073
#define sd_event_interface_movie_done					3074

#define sd_event_state_load								3075
#define sd_event_state_save								3076

#define sd_event_rule_join								3077
#define sd_event_rule_score								3078

#define sd_event_remote_join							3080
#define sd_event_remote_leave							3081
#define sd_event_remote_team							3082
#define sd_event_remote_death							3083
#define sd_event_remote_suicide							3084
#define sd_event_remote_telefrag						3085

//
// object setup structures
//

typedef struct		{
						const char			*name;
						JSPropertyOp		getter,setter;
					} script_js_property;

typedef struct		{
						const char			*name;
						JSNative			call;
						int					nargs;
					} script_js_function;

//
// define structure
//
 
typedef struct		{
						int					value_int;
						char				value_str[64],name[64];
					} script_define_type;

//
// global structures
//
 
typedef struct		{
						int					type,script_uid;
						char				name[name_str_len];
						d3_jsval_data_type	data;
					} global_type;

//
// script structures
//
 
typedef struct		{
						int					mode,count,freq,user_id;
						char				chain_func_name[64];
						attach_type			attach;
					} timer_type;
					
typedef struct		{
						int					uid,data_len;
						char				name[file_str_len],params[param_str_len];
						char				*data;
						bool				used;
						jsval				event_func;
						JSObject			*global,*obj;
					} script_type;

//
// count and flag structures
//

typedef struct		{
						int					timer,global;
					} script_count_type;
					
typedef struct		{
						int					timer_tick,current_tick;
					} script_time_type;
		
//
// main js engine structure
//

typedef struct		{
						JSRuntime			*rt;
						JSContext			*cx;
						
						int					script_current_uid;
						char				last_error_str[256];
						bool				add_property_lock;
						attach_type			attach;
						
						attach_type			game_attach,course_attach;
						
						script_count_type	count;
						script_time_type	time;
						
						script_type			*scripts;
						timer_type			*timers;
						global_type			*globals;
						
					} js_type;


