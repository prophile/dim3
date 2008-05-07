/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server Private Headers

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
// server mode
//

	// define this to put it in headless mode, otherwise it operates with a simple
	// cross-platform display/click interface

// #define D3_SERVER_HEADLESS							1

//
// players
//

typedef struct		{
						d3socket					sock;
						int							remote_uid,team_idx,score;
						char						name[name_str_len];
						bool						ready;
						d3pos						pos;
					} server_player_type;

typedef struct		{
						int							score;
						float						col[3];
						char						name[name_str_len];
					} server_player_info_type;

//
// server defines
//

#define server_no_data_u_wait						25
#define server_max_network_error_reject				10

//
// host functions
//

extern bool host_initialize(char *err_str);
extern void host_shutdown(void);
extern void* host_thread(void *arg);

extern bool broadcast_initialize(char *err_str);
extern void broadcast_shutdown(void);
extern void* broadcast_thread(void *arg);

extern void* client_thread(void *arg);

extern void* broadcast_thread(void *arg);

//
// server query functions
//

extern void* server_query_listen_thread(void *arg);
extern void* server_query_client_thread(void *arg);

extern bool server_query_initialize(char *err_str);
extern void server_query_shutdown(void);

extern char* server_query_html_info(void);

//
// player functions
//

extern void server_player_initialize(void);
extern void server_player_shutdown(void);

extern int server_player_find(int remote_uid);
extern int server_player_info_list(server_player_info_type *infos);

extern int server_player_join(d3socket sock,char *name,char *deny_reason);
extern void server_player_ready(int remote_uid,bool ready);
extern void server_player_leave(int remote_uid);
extern int server_player_create_remote_add_list(int player_remote_uid,network_request_remote_add *remotes);
extern void server_player_send_others_packet(int player_remote_uid,int action,int queue_mode,unsigned char *data,int len,bool skip_flooded);
extern void server_player_send_all_packet(int action,int queue_mode,unsigned char *data,int len,bool skip_flooded);

extern void server_player_update_team(int remote_uid,network_request_team *team);
extern void server_player_update(int remote_uid,network_request_remote_update *update);
extern void server_player_death(int remote_uid);
extern void server_player_telefrag(int remote_uid);
extern void server_player_message(int remote_uid,network_request_remote_chat *chat);

//
// status functions
//

extern void status_initialize(void);
extern void status_shutdown(void);
extern void status_draw(void);
extern void status_click(int x,int y);

extern void status_player_message(char *name,char *str,float *col);
extern void status_player_update(void);

//
// ui functions
//

extern void ui_start(void);
extern void ui_end(void);
extern void ui_pump(void);
extern void ui_draw_text(int x,int y,int wid,char *str,float *col,bool invert);
extern void ui_draw_section(int x,int y,int wid,int high,char *title);
