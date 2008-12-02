/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Functions Header

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
// hosting
//

extern bool net_host_initialize(char *err_str);
extern void net_host_shutdown(void);
extern void* net_host_thread(void *arg);

extern bool net_host_broadcast_initialize(char *err_str);
extern void net_host_broadcast_shutdown(void);
extern void* net_host_broadcast_thread(void *arg);

extern void* net_host_client_handler_thread(void *arg);

//
// host local joins
//

extern int net_client_find_game(char *game_name);
extern int net_host_client_handle_local_join(network_request_join *request_join,char *err_str);

//
// host player lists
//

extern void net_host_player_initialize(void);
extern void net_host_player_shutdown(void);

extern int net_host_player_find(int remote_uid);

extern int net_host_player_join(d3socket sock,char *name,char *deny_reason);
extern void net_host_player_ready(int remote_uid,bool ready);
extern void net_host_player_leave(int remote_uid);
extern int net_host_player_create_remote_add_list(int player_remote_uid,network_request_remote_add *remotes);
extern void net_host_player_send_others_packet(int player_remote_uid,int action,int queue_mode,unsigned char *data,int len,bool skip_flooded);
extern void net_host_player_send_all_packet(int action,int queue_mode,unsigned char *data,int len,bool skip_flooded);

extern void net_host_player_update_team(int remote_uid,network_request_team *team);
extern void net_host_player_update(int remote_uid,network_request_remote_update *update);

//
// client host pinging and joining
//

extern bool net_client_ping_host(char *ip,char *status,char *host_name,char *proj_name,char *game_name,char *map_name,int *player_count,int *player_max_count,int *ping_msec);
extern bool net_client_join_host_start(char *ip,char *name,int *remote_uid,char *game_name,char *map_name,int *tick_offset,char *deny_reason,int *remote_count,network_request_remote_add *remotes);
extern void net_client_join_host_end(void);

//
// client receiving messages
//

extern bool net_client_start_message_queue(char *err_str);
extern void net_client_end_message_queue(void);
extern bool net_client_start_message_queue_local(char *err_str);
extern void net_client_end_message_queue_local(void);
extern bool net_client_check_message_queue(int *action,int *from_remote_uid,unsigned char *data);

//
// client sending messages
//

extern void net_client_send_ready(int remote_uid);
extern void net_client_send_set_team(int remote_uid,int team_idx);
extern void net_client_send_leave_host(int remote_uid);
extern void net_client_send_latency_ping(int remote_uid);
extern void net_client_send_remote_update(int tick,int remote_uid,obj_type *obj,bool chat_on);
extern void net_client_send_death(int remote_uid,int kill_uid,bool telefrag);
extern void net_client_send_chat(int remote_uid,char *str);
extern void net_client_send_sound(int remote_uid,int x,int y,int z,float pitch,char *name);
extern void net_client_send_projectile_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void net_client_send_hitscan_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void net_client_send_melee_add(int remote_uid,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang);
extern void net_client_setup_pickup(obj_type *obj,network_request_remote_pickup *pickup);
extern void net_client_send_pickup(int remote_uid,network_request_remote_pickup *pickup);
