/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Player Routines

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
	#include "dim3engine.h"
#endif

#include "client.h"
#include "scripts.h"
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "cameras.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern float				team_color_tint[net_team_count][3];
extern char					setup_team_color_list[][32];

int							game_obj_rule_uid=-1;

extern void portal_triggers(obj_type *obj,int old_rn,int rn);

/* =======================================================

      Setup Player Object
      
======================================================= */

bool player_start_object(char *err_str)
{
	obj_type			*obj;
    
		// create player object
		
	server.player_obj_uid=-1;

	obj=object_create(bt_game);
    if (obj==NULL) {
		strcpy(err_str,"Player: Not enough memory to create player object");
		return(FALSE);
	}
	
		// player default setup
		
	strcpy(obj->name,setup.network.name);
	obj->team_idx=setup.network.team_idx;
	obj->spawn_spot_name[0]=0x0;

		// if joined then run game join rule
		// and send choosen team to other clients
	
	if (net_setup.client.joined) {
		game_obj_rule_uid=obj->uid;
		scripts_post_event_console(&js.game_attach,sd_event_rule,sd_event_rule_join,0);
		game_obj_rule_uid=-1;

		network_client_set_team(net_setup.client.remote_uid,obj->team_idx);
	}
	
		// default player settings
		
	obj->player=TRUE;
	obj->hidden=FALSE;
	
	server.player_obj_uid=obj->uid;
	
		// create player script
		
	return(object_start_script(obj,"Player",NULL,bt_game,err_str));
}

/* =======================================================

      Attach/Detach Player Object To Spot
      
======================================================= */

bool player_attach_object(void)
{
	int					spot_idx,x,y,z,rn;
	float				ang_y;
	spot_type			*spot;
	obj_type			*obj;
	
	obj=object_find_uid(server.player_obj_uid);

	if (!map.info.in_load) {
	
			// get spawn spot

		if (!net_setup.client.joined) {
			spot_idx=map_find_random_spot(&map,map.info.player_start_name,map.info.player_start_type);
		}
		else {
			if (obj->spawn_spot_name[0]==0x0) {
				spot_idx=map_find_random_spot(&map,NULL,"Spawn");
			}
			else {
				spot_idx=map_find_random_spot(&map,obj->spawn_spot_name,"Spawn");
			}
		}

		if (spot_idx==-1) return(FALSE);

			// get spawn spot position
		
		spot=&map.spots[spot_idx];

		x=spot->pos.x;
		y=spot->pos.y;
		z=spot->pos.z;

		ang_y=spot->ang.y;

			// if editor override then try to start at editor
			// cursor position

		if ((setup.editor_override.on) && (!map.settings.editor_link_always_start)) {
			rn=map_find_portal(&map,setup.editor_override.pt.x,setup.editor_override.pt.y,setup.editor_override.pt.z);
			if (rn!=-1) {
				x=setup.editor_override.pt.x;
				y=setup.editor_override.pt.y-obj->size.eye_offset;
				z=setup.editor_override.pt.z;
				ang_y=setup.editor_override.ang.y;
			}
		}

			// spawn player to spot
		
		spot=&map.spots[spot_idx];
		
		object_set_position(obj,x,z,y,ang_y,0);
		obj->turn.ang_to.y=ang_y;
		
			// spot messages
			
		portal_triggers(obj,-1,obj->pos.rn);

			// check for telefrag

		object_telefrag_check(obj);
	}
	
		// other settings
		
 	obj->air_mode=am_ground;
    obj->liquid_mode=lm_out;
	obj->fall.dist=0;
    obj->fall.change=FALSE;
  
	if (!map.info.in_load) {
		scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_stop,0);
	}
	
	return(TRUE);
}

void player_detach_object(void)
{
	obj_type			*obj;
	
	obj=object_find_uid(server.player_obj_uid);
	
		// need to leave vehicle?
		
	if (obj->vehicle.attach_obj_uid!=-1) {
		object_exit_vehicle(obj,TRUE,NULL);
	}
}

/* =======================================================

      Player Team Colors
      
======================================================= */

void player_get_ui_color(d3col *col)
{
	int					team_idx;
	obj_type			*obj;
	
	obj=object_find_uid(server.player_obj_uid);
	
	team_idx=obj->team_idx;

	if (team_idx==-1) {
		col->r=col->g=col->b=0.7f;
		return;
	}

	col->r=team_color_tint[team_idx][0];
	col->g=team_color_tint[team_idx][1];
	col->b=team_color_tint[team_idx][2];
}

