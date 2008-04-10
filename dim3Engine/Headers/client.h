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
	
// messages

extern bool network_client_check_message(int *action,int *from_remote_uid,unsigned char *data);

// host pinging and joining

extern bool network_client_ping_host(char *ip,char *status,char *host_name,char *proj_name,char *game_name,char *map_name,int *player_count,int *player_max_count,int *ping_msec);
extern bool network_client_join_host(char *ip,char *name,int *remote_uid,char *game_name,char *map_name,char *deny_reason,int *remote_count,network_request_remote_add *remotes);
extern bool network_client_ready_host(int remote_uid,char *err_str);
extern void network_client_set_team(int remote_uid,int team_idx);
extern void network_client_leave_host(int remote_uid);
extern void network_client_latency_ping_host(int remote_uid);

// messages

extern void network_client_send_remote_update(int tick,int remote_uid,obj_type *obj,bool chat_on);
extern void network_client_send_death(int remote_uid,int kill_uid);
extern void network_client_send_telefrag(int remote_uid,int telefrag_uid);
extern void network_client_send_message(int remote_uid,char *str);
extern void network_client_send_sound(int remote_uid,int x,int y,int z,float pitch,char *name);
extern void network_client_send_projectile_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void network_client_send_hitscan_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void network_client_send_melee_add(int remote_uid,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang);
