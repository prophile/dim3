/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Host Player Lists

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

extern char team_colors[][16];

int						net_host_player_count,server_next_remote_uid;
float					team_color_server_tint[net_team_count][3]=net_team_color_server_tint_def;
net_host_player_type	net_host_players[host_max_remote_count];

pthread_mutex_t			net_host_player_lock;

extern void net_client_push_queue_local(int action,int queue_mode,int remote_uid,unsigned char *data,int len);

/* =======================================================

      Initialize Players
      
======================================================= */

void net_host_player_initialize(void)
{
	net_host_player_count=0;
	server_next_remote_uid=net_remote_uid_client_start;
	
	pthread_mutex_init(&net_host_player_lock,NULL);
}

void net_host_player_shutdown(void)
{
	pthread_mutex_destroy(&net_host_player_lock);
}

/* =======================================================

      Find Players
      
======================================================= */

int net_host_player_find(int remote_uid)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->remote_uid==remote_uid) return(n);
		player++;
	}
	
	return(-1);
}

bool net_host_player_find_name(int remote_uid,char *name)
{
	int				idx;
	bool			hit;
	
	hit=FALSE;

	pthread_mutex_lock(&net_host_player_lock);

	idx=net_host_player_find(remote_uid);
	
	if (idx!=-1) {
		strcpy(name,net_host_players[idx].name);
		hit=TRUE;
	}
	
	pthread_mutex_unlock(&net_host_player_lock);
	
	return(hit);
}

bool net_host_player_find_name_team(int remote_uid,char *name,float *col)
{
	int				idx;
	bool			hit;
	
	hit=FALSE;

	pthread_mutex_lock(&net_host_player_lock);

	idx=net_host_player_find(remote_uid);
	
	if (idx!=-1) {
		strcpy(name,net_host_players[idx].name);
		memmove(col,team_color_server_tint[net_host_players[idx].team_idx],(3*sizeof(float)));
		hit=TRUE;
	}
	
	pthread_mutex_unlock(&net_host_player_lock);
	
	return(hit);
}

/* =======================================================

      Join and Leave Players
      
======================================================= */

int net_host_player_join(d3socket sock,char *name,char *deny_reason)
{
	int						n,remote_uid;
	net_host_player_type	*player;

		// lock all player operations
		
	pthread_mutex_lock(&net_host_player_lock);
	
		// host full?
		
	if (net_host_player_count==host_max_remote_count) {
		strcpy(deny_reason,"Server is full");
		pthread_mutex_unlock(&net_host_player_lock);
		return(-1);
	}

		// id in use?

	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (strcasecmp(player->name,name)==0) {
			sprintf(deny_reason,"Player name '%s' already in use",name);
			pthread_mutex_unlock(&net_host_player_lock);
			return(-1);
		}
		player++;
	}
	
		// create new player
		
	player=&net_host_players[net_host_player_count];
	
	player->remote_uid=server_next_remote_uid;
	player->sock=sock;
	player->score=0;
	strcpy(player->name,name);
	player->team_idx=net_team_none;
	
	player->pnt.x=player->pnt.y=player->pnt.z=0;
	
	player->ready=FALSE;
	
	server_next_remote_uid++;
	net_host_player_count++;
	
	remote_uid=player->remote_uid;
	
	pthread_mutex_unlock(&net_host_player_lock);
	
	return(remote_uid);
}

void net_host_player_ready(int remote_uid,bool ready)
{
	int				idx;
	
		// lock all player operations
		
	pthread_mutex_lock(&net_host_player_lock);
		
		// set player ready state
		
	idx=net_host_player_find(remote_uid);
	if (idx!=-1) net_host_players[idx].ready=ready;
	
		// unlock player operation
		
	pthread_mutex_unlock(&net_host_player_lock);
}

void net_host_player_leave(int remote_uid)
{
	int				idx;
	char			name[name_str_len];
	
		// lock all player operations
		
	pthread_mutex_lock(&net_host_player_lock);
		
		// find player
		
	idx=net_host_player_find(remote_uid);
	if (idx==-1) {
		pthread_mutex_unlock(&net_host_player_lock);
		return;
	}

		// remember name

	strcpy(name,net_host_players[idx].name);

		// exit player
	
	if (idx<(net_host_player_count-1)) {
		memmove(&net_host_players[idx],&net_host_players[idx+1],(sizeof(net_host_player_type)*((net_host_player_count-idx)-1)));
	}
	
	net_host_player_count--;
	
	pthread_mutex_unlock(&net_host_player_lock);
}

/* =======================================================

      Count Teams
      
======================================================= */

int net_host_player_count_team(int team_idx)
{
	int						n,count;
	net_host_player_type	*player;
	
	count=0;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->team_idx==team_idx) count++;
		player++;
	}
	
	return(count);
}

/* =======================================================

      Update Player Information
      
======================================================= */

void net_host_player_update_team(int remote_uid,network_request_team *team)
{
	int							idx;
	net_host_player_type		*player;
	
	pthread_mutex_lock(&net_host_player_lock);

	idx=net_host_player_find(remote_uid);
	if (idx==-1) {
		pthread_mutex_unlock(&net_host_player_lock);
		return;
	}

		// update team

	player=&net_host_players[idx];
	player->team_idx=(signed short)ntohs(team->team_idx);

	pthread_mutex_unlock(&net_host_player_lock);
}

void net_host_player_update(int remote_uid,network_request_remote_update *update)
{
	int							idx,score;
	bool						score_update;
	net_host_player_type		*player;

	score=(signed short)ntohs(update->score);
	
		// update score
		
	pthread_mutex_lock(&net_host_player_lock);

	idx=net_host_player_find(remote_uid);
	if (idx==-1) {
		pthread_mutex_unlock(&net_host_player_lock);
		return;
	}

	player=&net_host_players[idx];
	
		// position
		
	player->pnt.x=ntohl(update->pnt_x);
	player->pnt.y=ntohl(update->pnt_y);
	player->pnt.z=ntohl(update->pnt_z);
		
		// score
	
	score_update=(score!=player->score);
	player->score=score;
	
	pthread_mutex_unlock(&net_host_player_lock);
}

/* =======================================================

      Build Remote List for Join Replies
      
======================================================= */

int net_host_player_create_remote_add_list(int player_remote_uid,network_request_remote_add *remotes)
{
	int						n,cnt;
	net_host_player_type	*player;

	pthread_mutex_lock(&net_host_player_lock);
	
		// set up the remotes
		
	cnt=0;
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		
		if (player->remote_uid!=player_remote_uid) {
			remotes[cnt].uid=htons((short)player->remote_uid);
			strcpy(remotes[cnt].name,player->name);
			remotes[cnt].team_idx=htons((short)player->team_idx);
			remotes[cnt].score=htons((short)player->score);
			remotes[cnt].pnt_x=htonl(player->pnt.x);
			remotes[cnt].pnt_y=htonl(player->pnt.y);
			remotes[cnt].pnt_z=htonl(player->pnt.z);
			cnt++;
		}
		
		player++;
	}
	
	pthread_mutex_unlock(&net_host_player_lock);

	return(cnt);
}

/* =======================================================

      Send Packet to Other or All Players
      
======================================================= */

// supergumba -- need to implement skip_flooded here

void net_host_player_send_others_packet(int player_remote_uid,int action,int queue_mode,unsigned char *data,int len,bool skip_flooded)
{
	int						n,nsock;
	d3socket				socks[host_max_remote_count];
	net_host_player_type	*player;
	
		// we build the list of other player's sockets
		// inside the lock to deal with changing player list
		// we then send outside the lock so we don't hold
		// the lock too long
		
	pthread_mutex_lock(&net_host_player_lock);
	
	nsock=0;
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if ((player->remote_uid!=player_remote_uid) && (player->ready)) socks[nsock++]=player->sock;
		player++;
	}
	
	pthread_mutex_unlock(&net_host_player_lock);
	
		// send to all other players
	
	for (n=0;n!=nsock;n++) {
	
			// local (hosting) player
			
		if (socks[n]==(d3socket)NULL) {
			net_client_push_queue_local(action,queue_mode,player_remote_uid,data,len);
		}
		
			// remote joined players
			
		else {
			if (network_send_ready(socks[n])) network_send_packet(socks[n],action,queue_mode,player_remote_uid,data,len);
		}
	}
}

void net_host_player_send_all_packet(int action,int queue_mode,unsigned char *data,int len,bool skip_flooded)
{
	net_host_player_send_others_packet(-1,action,queue_mode,data,len,skip_flooded);
}
