/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Network Definitions Header

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
// host defines
//

#define host_no_data_u_wait							25
#define host_max_network_error_reject				10

#define host_max_remote_count						24

#define host_client_timeout_msec_rate				10000


//
// client defines
//

#define client_timeout_wait_seconds					5
#define client_message_per_loop_count				5

#define client_communication_update_msec_rate		50
#define client_communication_group_synch_msec_rate	10000
#define client_communication_latency_ping_msec_rate	5000

#define client_communication_timeout_msec			1000

//
// host player list
//

typedef struct		{
						d3socket					sock;
						int							remote_uid,team_idx,score;
						char						name[name_str_len];
						bool						ready;
						d3pnt						pnt;
					} net_host_player_type;

//
// setup structures
//

#define network_setup_max_game						32

typedef struct		{
						char						name[name_str_len],
													ip_name[256],ip_resolve[64],
													proj_name[name_str_len],
													game_name2[name_str_len],
													map_name[name_str_len];
						bool						hosting;
					} network_setup_host_type;

typedef struct		{
						int							remote_uid,
													latency,latency_ping_tick;
						char						joined_ip[32],game_name2[name_str_len];
						bool						joined;
					} network_setup_client_type;

typedef struct		{
						char						name[name_str_len];
						bool						use_teams;
					} network_setup_game_type;

typedef struct		{
						int							ngame,game_idx;
						network_setup_host_type		host;
						network_setup_client_type	client;
						network_setup_game_type		games[network_setup_max_game];
					} network_setup_type;

//
// special remote unqiue IDs
//

#define net_remote_uid_host							0
#define net_remote_uid_none							1
#define net_remote_uid_client_start					1000

//
// messages actions
//

#define net_action_none								0
#define net_action_request_info						1
#define net_action_reply_info						2
#define net_action_request_join						3
#define net_action_reply_join						4
#define net_action_request_host_reset				5
#define net_action_request_ready					6
#define net_action_request_team						7
#define net_action_request_leave					8
#define net_action_request_remote_add				9
#define net_action_request_remote_remove			10
#define net_action_request_remote_update			11
#define net_action_request_remote_death				12
#define net_action_request_remote_telefrag			13
#define net_action_request_remote_chat				14
#define net_action_request_remote_sound				15
#define net_action_request_remote_fire				16
#define net_action_request_remote_pickup			17
#define net_action_request_latency_ping				18
#define net_action_reply_latency_ping				19
#define net_action_request_host_exit				20
#define net_action_request_group_synch				21
#define net_action_reply_group_synch				22

//
// remote fire types
//

#define net_remote_fire_type_projectile				0
#define net_remote_fire_type_hit_scan				1
#define net_remote_fire_type_melee					2

//
// network maximums
//

#define net_max_weapon_per_remote					24

//
// team definitions
//

#define net_team_count								6

#define net_team_none								0
#define net_team_red								1
#define net_team_blue								2
#define net_team_green								3
#define net_team_yellow								4
#define net_team_purple								5

#define net_team_color_tint_def						{{0.7f,0.7f,0.7f},{1.0f,0.25f,0.25f},{0.25f,0.25f,1.0f},{0.25f,1.0f,0.25f},{1.0f,1.0f,0.25f},{1.0f,0.25f,1.0f}}
#define net_team_color_server_tint_def				{{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{0.0f,0.0f,1.0f},{0.0f,1.0f,0.0f},{1.0f,1.0f,0.0f},{1.0f,0.0f,1.0f}}
#define net_team_color_list_def						{"None","Red","Blue","Green","Yellow","Purple",""}

//
// flags
//

#define net_update_flag_hidden						0x00000001
#define net_update_flag_no_contact_object			0x00000002
#define net_update_flag_no_contact_projectile		0x00000004
#define net_update_flag_no_contact_force			0x00000008
#define net_update_flag_talking						0x00000010

#define net_group_synch_flag_on						0x00000001
#define net_group_synch_flag_freeze					0x00000002
#define net_group_synch_flag_main_move				0x00000004

//
// message data types
//

typedef struct		{
						int							pnt_x,pnt_y,pnt_z;
						short						uid,score,team_idx;
						char						name[name_str_len];
					} network_request_remote_add;

typedef struct		{
						short						player_count,player_max_count;
						char						host_name[name_str_len],host_ip_resolve[16],
													proj_name[name_str_len],
													game_name[name_str_len],
													map_name[name_str_len];
					} network_reply_info;
					
typedef struct		{
						short						team_idx;
						char						name[name_str_len],vers[name_str_len];
					} network_request_join;

typedef struct		{
						int							map_tick;
						short						join_uid,remote_count;
						char						game_name[name_str_len],map_name[name_str_len],
													deny_reason[64];
						network_request_remote_add	remotes[host_max_remote_count];
					} network_reply_join;
	
typedef struct		{
						short						team_idx;
					} network_request_team;
				
typedef struct		{
						int							model_tick;
						short						model_mode,
													model_animate_idx,model_animate_next_idx,model_pose_move_idx,
													model_smooth_animate_idx,model_smooth_pose_move_idx;
					} network_request_animation;			// used as part of network_request_update

typedef struct		{
						int							flags,pnt_x,pnt_y,pnt_z,
													fp_ang_x,fp_ang_y,fp_ang_z,
													fp_predict_move_x,fp_predict_move_y,fp_predict_move_z,
													fp_predict_turn_y,model_mesh_mask;
						short						vehicle_map_spawn_idx,
													score,health;
						unsigned char				model_cur_texture_frame[max_model_texture];
						network_request_animation	animation[max_model_blend_animation];
					} network_request_remote_update;

typedef struct		{
						short						kill_remote_uid,telefrag;
					} network_request_remote_death;
					
typedef struct		{
						char						str[64];
					} network_request_remote_chat;
					
typedef struct		{
						int							pnt_x,pnt_y,pnt_z,fp_pitch;
						char						name[name_str_len];
					} network_request_remote_sound;

typedef struct		{
						int							pt_x,pt_y,pt_z,
													fp_ang_x,fp_ang_y,fp_ang_z;
						short						fire_type,radius,distance,damage,force;
						char						weap_name[name_str_len],proj_setup_name[name_str_len];
					} network_request_remote_fire;

typedef struct		{
						short						hidden,ammo_count,clip_count,
													alt_ammo_count,alt_clip_count;
					} network_request_remote_ammo;

typedef struct		{
						int							pt_x,pt_y,pt_z;
						short						health;
						network_request_remote_ammo	ammos[net_max_weapon_per_remote];
					} network_request_remote_pickup;

typedef struct		{
						int							flags,
													mov_add_x,mov_add_y,mov_add_z,
													cuml_mov_add_x,cuml_mov_add_y,cuml_mov_add_z,
													fp_rot_add_x,fp_rot_add_y,fp_rot_add_z,
													fp_cuml_rot_add_x,fp_cuml_rot_add_y,fp_cuml_rot_add_z;
						short						group_idx,movement_idx,movement_move_idx,
													count,user_id;
					} network_reply_group_synch;
