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
// d3Angle/d3Point/d3Size property IDs
//

#define d3_angle_prop_x								0
#define d3_angle_prop_y								1
#define d3_angle_prop_z								2

#define d3_point_prop_x								0
#define d3_point_prop_y								1
#define d3_point_prop_z								2

#define d3_size_prop_x								0
#define d3_size_prop_y								1
#define d3_size_prop_z								2

//
// game property IDs
//

#define game_setting_prop_type						0
#define game_setting_prop_multiplayer				1
#define game_setting_prop_skill						2

#define game_score_prop_kill						0
#define game_score_prop_death						1
#define game_score_prop_suicide						2
#define game_score_prop_goal						3

#define game_join_prop_name							0
#define game_join_prop_team							1

//
// map property IDs
//

#define map_info_prop_file							0
#define map_info_prop_title							1
#define map_info_prop_author						2

#define map_setting_prop_scale						0
#define map_setting_prop_gravity					1
#define map_setting_prop_resistance					2
#define map_setting_prop_multiplayer				3
#define map_setting_prop_multiplayer_type			4
#define map_setting_prop_bot_skill					5

#define map_light_color_prop_red					0
#define map_light_color_prop_green					1
#define map_light_color_prop_blue					2
#define map_light_color_prop_drop_off_factor		3

#define map_spot_prop_count							0

#define map_fog_prop_on								0
#define map_fog_prop_count							1
#define map_fog_prop_outer_radius					2
#define map_fog_prop_inner_radius					3
#define map_fog_prop_high							4
#define map_fog_prop_drop							5
#define map_fog_prop_texture_index					6
#define map_fog_prop_speed							7
#define map_fog_prop_txt_x_fact						8
#define map_fog_prop_txt_y_fact						9
#define map_fog_prop_alpha							10
#define map_fog_prop_use_solid_color				11

#define map_fog_color_prop_red						0
#define map_fog_color_prop_green					1
#define map_fog_color_prop_blue						2

//
// camera property IDs
//
 
#define camera_setting_prop_type					0
#define camera_setting_prop_attach_object_id		1

#define camera_chase_prop_size						0
#define camera_chase_prop_distance					1
#define camera_chase_prop_track_speed				2

#define camera_position_prop_x						0
#define camera_position_prop_y						1
#define camera_position_prop_z						2

#define camera_angle_prop_x							0
#define camera_angle_prop_y							1
#define camera_angle_prop_z							2

#define camera_chase_angle_prop_x					0
#define camera_chase_angle_prop_y					1
#define camera_chase_angle_prop_z					2

#define camera_chase_offset_prop_x					0
#define camera_chase_offset_prop_y					1
#define camera_chase_offset_prop_z					2

#define camera_chase_slop_prop_x					0
#define camera_chase_slop_prop_y					1
#define camera_chase_slop_prop_z					2

#define camera_static_position_prop_follow			0
#define camera_static_position_prop_walk_turn_speed	1

#define camera_plane_prop_projection_type			0
#define camera_plane_prop_fov						1
#define camera_plane_prop_aspect_ratio				2
#define camera_plane_prop_left						3
#define camera_plane_prop_right						4
#define camera_plane_prop_top						5
#define camera_plane_prop_bottom					6
#define camera_plane_prop_near						7
#define camera_plane_prop_far						8
#define camera_plane_prop_near_offset				9

//
// interface property IDs
//

#define interface_screen_prop_width					0
#define interface_screen_prop_height				1

#define interface_radar_prop_on						0
#define interface_radar_prop_x						1
#define interface_radar_prop_y						2
#define interface_radar_prop_display_radius			3
#define interface_radar_prop_view_radius			4

//
// model property IDs
//

#define model_prop_on								0
#define model_prop_name								1
#define model_prop_lit								2
#define model_prop_bounce							3
#define model_prop_alpha							4
#define model_prop_resize							5
#define model_prop_face_forward						6

#define model_offset_prop_x							0
#define model_offset_prop_z							1
#define model_offset_prop_y							2

#define model_rotate_prop_x							0
#define model_rotate_prop_z							1
#define model_rotate_prop_y							2

#define model_spin_prop_x							0
#define model_spin_prop_z							1
#define model_spin_prop_y							2

#define model_light_prop_index						0
#define model_light_prop_on							1
#define model_light_prop_type						2
#define model_light_prop_intensity					3

#define model_light_color_prop_red					0
#define model_light_color_prop_green				1
#define model_light_color_prop_blue					2

#define model_hilite_color_prop_red					0
#define model_hilite_color_prop_green				1
#define model_hilite_color_prop_blue				2

#define model_halo_prop_index						0
#define model_halo_prop_on							1
#define model_halo_prop_name						2
#define model_halo_prop_min_distance				3
#define model_halo_prop_max_distance				4
#define model_halo_prop_min_size					5
#define model_halo_prop_max_size					6
#define model_halo_prop_min_alpha					7
#define model_halo_prop_max_alpha					8
#define model_halo_prop_no_clip_object				9
#define model_halo_prop_no_clip_self				10

#define model_shadow_prop_on						0
#define model_shadow_prop_mode						1
#define model_shadow_prop_always_in_air				2
#define model_shadow_prop_static_in_air				3
#define model_shadow_prop_static_face_angle			4
#define model_shadow_prop_static_size				5
#define model_shadow_prop_static_alpha				6

#define model_animation_prop_index					0
#define model_animation_prop_cur_animation_name		1

//
// object property IDs
//

#define obj_setting_prop_id							0
#define obj_setting_prop_name						1
#define obj_setting_prop_team						2
#define obj_setting_prop_hidden						3
#define obj_setting_prop_suspend					4
#define obj_setting_prop_fly						5
#define obj_setting_prop_find						6
#define obj_setting_prop_contact					7
#define obj_setting_prop_contact_object				8
#define obj_setting_prop_contact_projectile			9
#define obj_setting_prop_contact_force				10
#define obj_setting_prop_hit_box					11
#define obj_setting_prop_damage						12
#define obj_setting_prop_crushable					13
#define obj_setting_prop_invincible					14
#define obj_setting_prop_clickable					15
#define obj_setting_prop_pickup						16
#define obj_setting_prop_ignore_pickup_items		17
#define obj_setting_prop_ignore_mouse				18
#define obj_setting_prop_turn_only_when_moving		19
#define obj_setting_prop_restrict_player_turning	20
#define obj_setting_prop_quick_reverse				21
#define obj_setting_prop_side_step					22
#define obj_setting_prop_jump						23
#define obj_setting_prop_duck						24
#define obj_setting_prop_crawl						25
#define obj_setting_prop_single_speed				26
#define obj_setting_prop_bump_up					27
#define obj_setting_prop_slope_gravity				28
#define obj_setting_prop_pushable					29
#define obj_setting_prop_open_doors					30
#define obj_setting_prop_input_mode					31

#define obj_position_prop_x							0
#define obj_position_prop_y							1
#define obj_position_prop_z							2

#define obj_angle_prop_x							0
#define obj_angle_prop_y							1
#define obj_angle_prop_z							2

#define obj_lock_prop_x								0
#define obj_lock_prop_y								1
#define obj_lock_prop_z								2

#define obj_status_prop_speed						0
#define obj_status_prop_moving						1
#define obj_status_prop_running						2
#define obj_status_prop_backward					3
#define obj_status_prop_sliding						4
#define obj_status_prop_stand						5
#define obj_status_prop_air							6
#define obj_status_prop_liquid						7
#define obj_status_prop_stand_on_object_id			8
#define obj_status_prop_stand_under_object_id		9

#define obj_motion_vector_prop_x					0
#define obj_motion_vector_prop_y					1
#define obj_motion_vector_prop_z					2

#define obj_motion_angle_prop_x						0
#define obj_motion_angle_prop_y						1
#define obj_motion_angle_prop_z						2

#define obj_forward_speed_prop_walk					0
#define obj_forward_speed_prop_run					1
#define obj_forward_speed_prop_crawl				2
#define obj_forward_speed_prop_air					3
#define obj_forward_speed_prop_acceleration			4
#define obj_forward_speed_prop_deceleration			5
#define obj_forward_speed_prop_acceleration_air		6
#define obj_forward_speed_prop_deceleration_air		7

#define obj_side_speed_prop_walk					0
#define obj_side_speed_prop_run						1
#define obj_side_speed_prop_crawl					2
#define obj_side_speed_prop_air						3
#define obj_side_speed_prop_acceleration			4
#define obj_side_speed_prop_deceleration			5
#define obj_side_speed_prop_acceleration_air		6
#define obj_side_speed_prop_deceleration_air		7

#define obj_vert_speed_prop_normal					0
#define obj_vert_speed_prop_acceleration			1
#define obj_vert_speed_prop_deceleration			2
#define obj_vert_speed_prop_fly_slop				3

#define obj_turn_speed_prop_facing_walk				0
#define obj_turn_speed_prop_motion_walk				1
#define obj_turn_speed_prop_facing_run				2
#define obj_turn_speed_prop_motion_run				3
#define obj_turn_speed_prop_facing_crawl			4
#define obj_turn_speed_prop_motion_crawl			5
#define obj_turn_speed_prop_facing_air				6
#define obj_turn_speed_prop_motion_air				7
#define obj_turn_speed_prop_key						8
#define obj_turn_speed_prop_top_down_ang_offset		9

#define obj_object_speed_prop_jump_height			0
#define obj_object_speed_prop_duck_add				1
#define obj_object_speed_prop_duck_change			2
#define obj_object_speed_prop_bump_height			3
#define obj_object_speed_prop_bump_smooth_factor	4
#define obj_object_speed_prop_bounce_factor			5
#define obj_object_speed_prop_no_slide				6

#define obj_thrust_prop_speed						0
#define obj_thrust_prop_max_speed					1
#define obj_thrust_prop_drag						2

#define obj_look_prop_up_angle						0
#define obj_look_prop_down_angle					1
#define obj_look_prop_effect_weapons				2

#define obj_sight_prop_side_field_angle				0
#define obj_sight_prop_look_field_angle				1
#define obj_sight_prop_side_field_division			2
#define obj_sight_prop_look_field_division			3
#define obj_sight_prop_distance						4

#define obj_health_prop_maximum						0
#define obj_health_prop_start						1
#define obj_health_prop_current						2
#define obj_health_prop_recover_tick				3
#define obj_health_prop_recover_amount				4
#define obj_health_prop_fall_damage_minimum_height	5
#define obj_health_prop_fall_damage_factor			6

#define obj_size_prop_x								0
#define obj_size_prop_z								1
#define obj_size_prop_y								2
#define obj_size_prop_eye_offset					3
#define obj_size_prop_weight						4
#define obj_size_prop_click_distance				5

#define obj_rigid_body_prop_on						0
#define obj_rigid_body_prop_max_drop_y				1
#define obj_rigid_body_prop_reset_factor_y			2
#define obj_rigid_body_prop_smooth_factor_y			3
#define obj_rigid_body_prop_max_angle_x				4
#define obj_rigid_body_prop_reset_factor_x			5
#define obj_rigid_body_prop_smooth_factor_x			6
#define obj_rigid_body_prop_max_angle_z				7
#define obj_rigid_body_prop_reset_factor_z			8
#define obj_rigid_body_prop_smooth_factor_z			9

#define obj_vehicle_prop_on							0
#define obj_vehicle_prop_has_occupant				1

#define obj_radar_prop_on							0
#define obj_radar_prop_icon							1
#define obj_radar_prop_motion_only					2
#define obj_radar_prop_always_visible				3

#define obj_score_prop_kill							0
#define obj_score_prop_death						1
#define obj_score_prop_suicide						2
#define obj_score_prop_goal							3
#define obj_score_prop_score						4

#define obj_touch_prop_object_id					0
#define obj_touch_prop_object_name					1
#define obj_touch_prop_object_is_player				2
#define obj_touch_prop_stand						3

#define obj_touch_position_prop_x					0
#define obj_touch_position_prop_y					1
#define obj_touch_position_prop_z					2

#define obj_touch_angle_prop_x						0
#define obj_touch_angle_prop_y						1
#define obj_touch_angle_prop_z						2

#define obj_hit_prop_object_id						0
#define obj_hit_prop_object_name					1
#define obj_hit_prop_object_is_player				2
#define obj_hit_prop_weapon_name					3
#define obj_hit_prop_projectile_name				4
#define obj_hit_prop_hit_box_name					5
#define obj_hit_prop_damage							6

#define obj_hit_position_prop_x						0
#define obj_hit_position_prop_y						1
#define obj_hit_position_prop_z						2

#define obj_hit_angle_prop_x						0
#define obj_hit_angle_prop_y						1
#define obj_hit_angle_prop_z						2

#define obj_click_prop_crosshair_up					0
#define obj_click_prop_crosshair_down				1
#define obj_click_prop_object_id					2
#define obj_click_prop_object_name					3

#define obj_pickup_prop_object_id					0
#define obj_pickup_prop_object_name					1
#define obj_pickup_prop_object_is_player			2
#define obj_pickup_prop_item_id						3
#define obj_pickup_prop_item_name					4

#define obj_watch_prop_object_id					0
#define obj_watch_prop_object_name					1
#define obj_watch_prop_object_is_player				2
#define obj_watch_prop_object_is_remote				3
#define obj_watch_prop_object_is_bot				4
#define obj_watch_prop_object_is_player_remote_bot	5
#define obj_watch_prop_object_team					6
#define obj_watch_prop_base_team					7
#define obj_watch_prop_sound_name					8

#define obj_weapon_fire_prop_name					0
#define obj_weapon_fire_prop_method					1

#define obj_melee_prop_strike_bone_tag				0
#define obj_melee_prop_strike_pose_name				1
#define obj_melee_prop_radius						2
#define obj_melee_prop_distance						3
#define obj_melee_prop_damage						4
#define obj_melee_prop_force						5
#define obj_melee_prop_life_tick					6

//
// weapon property IDs
//

#define weap_setting_prop_name						0
#define weap_setting_prop_fail_in_liquid			1
#define weap_setting_prop_parent_object_id			2

#define weap_hand_prop_raise_tick					0
#define weap_hand_prop_lower_tick					1
#define weap_hand_prop_select_shift					2
#define weap_hand_prop_bob_speed					3
#define weap_hand_prop_bob_angle					4

#define weap_hand_position_prop_x					0
#define weap_hand_position_prop_y					1
#define weap_hand_position_prop_z					2

#define weap_hand_angle_prop_x						0
#define weap_hand_angle_prop_y						1
#define weap_hand_angle_prop_z						2

#define weap_projectile_prop_fire_bone_tag			0
#define weap_projectile_prop_barrel_bone_tag		1
#define weap_projectile_prop_fire_pose_name			2
#define weap_projectile_prop_object_fire_bone_tag	3
#define weap_projectile_prop_object_fire_pose_name	4
#define weap_projectile_prop_repeat_on				5
#define weap_projectile_prop_repeat_tick			6

#define weap_crosshair_prop_on						0
#define weap_crosshair_prop_type					1
#define weap_crosshair_prop_name					2
#define weap_crosshair_prop_min_size				3
#define weap_crosshair_prop_max_size				4
#define weap_crosshair_prop_distance				5

#define weap_crosshair_color_prop_red				0
#define weap_crosshair_color_prop_green				1
#define weap_crosshair_color_prop_blue				2

#define weap_crosshair_empty_color_prop_red			0
#define weap_crosshair_empty_color_prop_green		1
#define weap_crosshair_empty_color_prop_blue		2

#define weap_crosshair_pickup_color_prop_red		0
#define weap_crosshair_pickup_color_prop_green		1
#define weap_crosshair_pickup_color_prop_blue		2

#define weap_kickback_prop_size						0
#define weap_kickback_prop_tick						1		// depreciated

#define weap_recoil_prop_min_x						0
#define weap_recoil_prop_max_x						1
#define weap_recoil_prop_reset_x					2
#define weap_recoil_prop_min_y						3
#define weap_recoil_prop_max_y						4
#define weap_recoil_prop_reset_y					5
#define weap_recoil_prop_min_z						6
#define weap_recoil_prop_max_z						7
#define weap_recoil_prop_reset_z					8
#define weap_recoil_prop_reset_look					9
#define weap_recoil_prop_reset_turn					10

#define weap_ammo_prop_ammo							0
#define weap_ammo_prop_clip							1
#define weap_ammo_prop_count						2
#define weap_ammo_prop_init_count					3
#define weap_ammo_prop_max_count					4
#define weap_ammo_prop_clip_count					5
#define weap_ammo_prop_init_clip_count				6
#define weap_ammo_prop_max_clip_count				7
#define weap_ammo_prop_last_reload_tick				8

#define weap_alt_ammo_prop_clip						0
#define weap_alt_ammo_prop_count					1
#define weap_alt_ammo_prop_init_count				2
#define weap_alt_ammo_prop_max_count				3
#define weap_alt_ammo_prop_clip_count				4
#define weap_alt_ammo_prop_init_clip_count			5
#define weap_alt_ammo_prop_max_clip_count			6
#define weap_alt_ammo_prop_last_reload_tick			7

#define weap_melee_prop_strike_bone_tag				0
#define weap_melee_prop_strike_pose_name			1
#define weap_melee_prop_object_strike_bone_tag		2
#define weap_melee_prop_object_strike_pose_name		3
#define weap_melee_prop_radius						4
#define weap_melee_prop_distance					5
#define weap_melee_prop_damage						6
#define weap_melee_prop_force						7
#define weap_melee_prop_fall_off					8

#define weap_fire_prop_method						0
#define weap_fire_prop_last_fire_tick				1

#define weap_dual_prop_on							0
#define weap_dual_prop_active						1
#define weap_dual_prop_hand_offset					2

#define weap_target_prop_on							0
#define weap_target_prop_distance					1
#define weap_target_prop_objectId					2

#define weap_target_color_prop_red					0
#define weap_target_color_prop_green				1
#define weap_target_color_prop_blue					2

#define weap_zoom_prop_on							0
#define weap_zoom_prop_active						1
#define weap_zoom_prop_fov_minimum					2
#define weap_zoom_prop_fov_maximum					3
#define weap_zoom_prop_fov_steps					4
#define weap_zoom_prop_turn_factor					5
#define weap_zoom_prop_crawl_turn_factor			6
#define weap_zoom_prop_look_factor					7
#define weap_zoom_prop_mask_name					8

//
// projectile property IDs
//

#define proj_setting_prop_name						0
#define proj_setting_prop_hitscan					1
#define proj_setting_prop_reset_angle				2
#define proj_setting_prop_parent_object_id			3

#define proj_position_prop_x						0
#define proj_position_prop_y						1
#define proj_position_prop_z						2

#define proj_origin_prop_x							0
#define proj_origin_prop_y							1
#define proj_origin_prop_z							2

#define proj_motion_vector_prop_x					0
#define proj_motion_vector_prop_y					1
#define proj_motion_vector_prop_z					2

#define proj_motion_angle_prop_x					0
#define proj_motion_angle_prop_y					1
#define proj_motion_angle_prop_z					2

#define proj_speed_prop_speed						0
#define proj_speed_prop_deceleration				1
#define proj_speed_prop_deceleration_wait			2
#define proj_speed_prop_deceleration_min_speed		3
#define proj_speed_prop_max_hitscan_distance		4

#define proj_hit_prop_type							0
#define proj_hit_prop_name							1
#define proj_hit_prop_is_player						2
#define proj_hit_prop_start_tick					3
#define proj_hit_prop_material_name					4
#define proj_hit_prop_eject_vector					5
#define proj_hit_prop_reflect_vector				6

#define proj_action_prop_damage						0
#define proj_action_prop_collision					1
#define proj_action_prop_auto_hit_tick				2
#define proj_action_prop_auto_bounce				3
#define proj_action_prop_auto_bounce_min_move		4
#define proj_action_prop_auto_bounce_reduce			5
#define proj_action_prop_auto_reflect				6
#define proj_action_prop_target_object_id			7

#define proj_push_prop_on							0
#define proj_push_prop_force						1

#define proj_size_prop_x							0
#define proj_size_prop_z							1
#define proj_size_prop_y							2
#define proj_size_prop_weight						3

#define proj_mark_prop_on							0
#define proj_mark_prop_name							1
#define proj_mark_prop_size							2
#define proj_mark_prop_alpha						3

#define proj_melee_prop_strike_bone_tag				0
#define proj_melee_prop_strike_pose_name			1
#define proj_melee_prop_radius						2
#define proj_melee_prop_distance					3
#define proj_melee_prop_damage						4
#define proj_melee_prop_force						5
#define proj_melee_prop_fall_off					6

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
#define sd_team_green									1143
#define sd_team_yellow									1144
#define sd_team_purple									1145

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
#define sd_event_interface								2033
#define sd_event_state									2034
#define sd_event_rule									2035
#define sd_event_remote									2036

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

#define sd_event_remote_join							3079
#define sd_event_remote_leave							3080
#define sd_event_remote_team							3081
#define sd_event_remote_death							3082
#define sd_event_remote_suicide							3083
#define sd_event_remote_telefrag						3084

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
						JSObject			*global_prototype,*game_prototype,*course_prototype,
											*obj_prototype,*weap_prototype,*proj_setup_prototype;
						
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


