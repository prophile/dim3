/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Server Player Code

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

#ifdef D3_PCH
	#include "dim3server.h"
#endif

extern char team_colors[][16];

int						server_player_count,server_next_remote_uid;
float					team_color_server_tint[net_team_count][3]=net_team_color_server_tint_def;
server_player_type		server_players[net_max_remote_count];

pthread_mutex_t			server_player_lock;

/* =======================================================

      Initialize Players
      
======================================================= */

void server_player_initialize(void)
{
	server_player_count=0;
	server_next_remote_uid=net_remote_uid_client_start;
	
	pthread_mutex_init(&server_player_lock,NULL);
}

void server_player_shutdown(void)
{
	pthread_mutex_destroy(&server_player_lock);
}

/* =======================================================

      Find Players
      
======================================================= */

int server_player_find(int remote_uid)
{
	int					n;
	server_player_type	*player;
	
	player=server_players;
	
	for (n=0;n!=server_player_count;n++) {
		if (player->remote_uid==remote_uid) return(n);
		player++;
	}
	
	return(-1);
}

bool server_player_find_name(int remote_uid,char *name)
{
	int				idx;
	bool			hit;
	
	hit=FALSE;

	pthread_mutex_lock(&server_player_lock);

	idx=server_player_find(remote_uid);
	
	if (idx!=-1) {
		strcpy(name,server_players[idx].name);
		hit=TRUE;
	}
	
	pthread_mutex_unlock(&server_player_lock);
	
	return(hit);
}

bool server_player_find_name_team(int remote_uid,char *name,float *col)
{
	int				idx;
	bool			hit;
	
	hit=FALSE;

	pthread_mutex_lock(&server_player_lock);

	idx=server_player_find(remote_uid);
	
	if (idx!=-1) {
		strcpy(name,server_players[idx].name);
		memmove(col,team_color_server_tint[server_players[idx].team_idx],(3*sizeof(float)));
		hit=TRUE;
	}
	
	pthread_mutex_unlock(&server_player_lock);
	
	return(hit);
}

/* =======================================================

      Join and Leave Players
      
======================================================= */

int server_player_info_list(server_player_info_type *infos)
{
	int						n,cnt;
	server_player_type		*player;
	server_player_info_type	*info;
	
	pthread_mutex_lock(&server_player_lock);

	player=server_players;
	info=infos;

	for (n=0;n!=server_player_count;n++) {

		strcpy(info->name,player->name);
		memmove(info->col,team_color_server_tint[player->team_idx],(3*sizeof(float)));
		info->score=player->score;

		player++;
		info++;
	}

	cnt=server_player_count;

	pthread_mutex_unlock(&server_player_lock);

	return(cnt);
}

/* =======================================================

      Join and Leave Players
      
======================================================= */

int server_player_join(d3socket sock,char *name,char *deny_reason)
{
	int					n,remote_uid;
	char				str[256];
	server_player_type	*player;

		// lock all player operations
		
	pthread_mutex_lock(&server_player_lock);
	
		// host full?
		
	if (server_player_count==net_max_remote_count) {
		strcpy(deny_reason,"Server is full");
		pthread_mutex_unlock(&server_player_lock);
		return(-1);
	}

		// id in use?

	player=server_players;
	
	for (n=0;n!=server_player_count;n++) {
		if (strcasecmp(player->name,name)==0) {
			strcpy(deny_reason,"Player name already in use");
			pthread_mutex_unlock(&server_player_lock);
			return(-1);
		}
		player++;
	}
	
		// create new player
		
	player=&server_players[server_player_count];
	
	player->remote_uid=server_next_remote_uid;
	player->sock=sock;
	player->score=0;
	strcpy(player->name,name);
	player->team_idx=net_team_none;
	
	player->pos.rn=0;
	player->pos.x=player->pos.y=player->pos.z=0;
	
	player->ready=FALSE;
	
	server_next_remote_uid++;
	server_player_count++;
	
	remote_uid=player->remote_uid;
	
	pthread_mutex_unlock(&server_player_lock);

		// write to status page

	sprintf(str,"%s Joined",name);
	status_player_message(NULL,str,NULL);

	status_player_update();
	
	return(remote_uid);
}

void server_player_ready(int remote_uid,bool ready)
{
	int				idx;
	
		// lock all player operations
		
	pthread_mutex_lock(&server_player_lock);
		
		// set player ready state
		
	idx=server_player_find(remote_uid);
	if (idx!=-1) server_players[idx].ready=ready;
	
		// unlock player operation
		
	pthread_mutex_unlock(&server_player_lock);
}

void server_player_leave(int remote_uid)
{
	int				idx;
	char			name[name_str_len],str[256];
	
		// lock all player operations
		
	pthread_mutex_lock(&server_player_lock);
		
		// find player
		
	idx=server_player_find(remote_uid);
	if (idx==-1) {
		pthread_mutex_unlock(&server_player_lock);
		return;
	}

		// remember name

	strcpy(name,server_players[idx].name);

		// exit player
	
	if (idx<(server_player_count-1)) {
		memmove(&server_players[idx],&server_players[idx+1],(sizeof(server_player_type)*((server_player_count-idx)-1)));
	}
	
	server_player_count--;
	
	pthread_mutex_unlock(&server_player_lock);

		// write to status page

	sprintf(str,"%s Left",name);
	status_player_message(NULL,str,NULL);

	status_player_update();
}

/* =======================================================

      Count Teams
      
======================================================= */

int server_player_count_team(int team_idx)
{
	int					n,count;
	server_player_type	*player;
	
	count=0;
	
	player=server_players;
	
	for (n=0;n!=server_player_count;n++) {
		if (player->team_idx==team_idx) count++;
		player++;
	}
	
	return(count);
}

/* =======================================================

      Update Player Information
      
======================================================= */

void server_player_update_team(int remote_uid,network_request_team *team)
{
	int						idx;
	server_player_type		*player;
	
	pthread_mutex_lock(&server_player_lock);

	idx=server_player_find(remote_uid);
	if (idx==-1) {
		pthread_mutex_unlock(&server_player_lock);
		return;
	}

		// update team

	player=&server_players[idx];
	player->team_idx=(signed short)ntohs(team->team_idx);

	pthread_mutex_unlock(&server_player_lock);
	
	status_player_update();
}

void server_player_death(int remote_uid)
{
	char					name[name_str_len],str[256];
	
	server_player_find_name(remote_uid,name);

	sprintf(str,"%s Died",name);
	status_player_message(NULL,str,NULL);

	status_player_update();
}

void server_player_telefrag(int remote_uid)
{
	char					name[name_str_len],str[256];
	
	server_player_find_name(remote_uid,name);

	sprintf(str,"%s Telefragged",name);
	status_player_message(NULL,str,NULL);

	status_player_update();
}

void server_player_update(int remote_uid,network_request_remote_update *update)
{
	int						idx,score;
	bool					score_update;
	server_player_type		*player;

	score=(signed short)ntohs(update->score);
	
		// update score
		
	pthread_mutex_lock(&server_player_lock);

	idx=server_player_find(remote_uid);
	if (idx==-1) {
		pthread_mutex_unlock(&server_player_lock);
		return;
	}

	player=&server_players[idx];
	
		// position
		
	player->pos.rn=(int)ntohs(update->pos_rn);
	player->pos.x=ntohl(update->pos_x);
	player->pos.y=ntohl(update->pos_y);
	player->pos.z=ntohl(update->pos_z);
		
		// score
	
	score_update=(score!=player->score);
	player->score=score;
	
	pthread_mutex_unlock(&server_player_lock);

		// update player score

	if (score_update) status_player_update();
}

void server_player_message(int remote_uid,network_request_remote_message *message)
{
	char				name[name_str_len];
	float				col[3];

	server_player_find_name_team(remote_uid,name,col);
	status_player_message(name,message->str,col);
}

/* =======================================================

      Build Remote List for Join Replies
      
======================================================= */

int server_player_create_remote_add_list(int player_remote_uid,network_request_remote_add *remotes)
{
	int						n,cnt;
	server_player_type		*player;

	pthread_mutex_lock(&server_player_lock);
	
		// set up the remotes
		
	cnt=0;
	player=server_players;
	
	for (n=0;n!=server_player_count;n++) {
		
		if (player->remote_uid!=player_remote_uid) {
			remotes[cnt].uid=player->remote_uid;
			strcpy(remotes[cnt].name,player->name);
			remotes[cnt].team_idx=player->team_idx;
			remotes[cnt].score=player->score;
			remotes[cnt].pos_rn=htons((short)player->pos.rn);
			remotes[cnt].pos_x=htonl(player->pos.x);
			remotes[cnt].pos_y=htonl(player->pos.y);
			remotes[cnt].pos_z=htonl(player->pos.z);
			cnt++;
		}
		
		player++;
	}
	
	pthread_mutex_unlock(&server_player_lock);

	return(cnt);
}

/* =======================================================

      Send Packet to Other or All Players
      
======================================================= */

void server_player_send_others_packet(int player_remote_uid,int action,int queue_mode,unsigned char *data,int len,bool skip_flooded)
{
	int						n,nsock;
	d3socket				socks[net_max_remote_count];
	server_player_type		*player;
	
		// we build the list of other player's sockets
		// inside the lock to deal with changing player list
		// we then send outside the lock so we don't hold
		// the lock too long
		
	pthread_mutex_lock(&server_player_lock);
	
	nsock=0;
	player=server_players;
	
	for (n=0;n!=server_player_count;n++) {
		if ((player->remote_uid!=player_remote_uid) && (player->ready)) socks[nsock++]=player->sock;
		player++;
	}
	
	pthread_mutex_unlock(&server_player_lock);
	
		// send to all other players
	
	for (n=0;n!=nsock;n++) {
		if (network_send_ready(socks[n])) network_send_packet(socks[n],action,queue_mode,player_remote_uid,data,len);
	}
}

void server_player_send_all_packet(int action,int queue_mode,unsigned char *data,int len,bool skip_flooded)
{
	server_player_send_others_packet(-1,action,queue_mode,data,len,skip_flooded);
}
