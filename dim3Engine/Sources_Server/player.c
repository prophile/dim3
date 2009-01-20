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

#include "network.h"
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

extern void mesh_triggers(obj_type *obj,int old_mesh_idx,int mesh_idx);

/* =======================================================

      Attach/Detach Player Object To Spot
      
======================================================= */

bool player_attach_object(void)
{
	int					spot_idx,x,y,z,mesh_idx;
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

		x=spot->pnt.x;
		y=spot->pnt.y;
		z=spot->pnt.z;

		ang_y=spot->ang.y;

			// if editor override then try to start at editor
			// cursor position

		if ((setup.editor_override.on) && (!map.settings.editor_link_always_start)) {
			x=setup.editor_override.pt.x;
			y=setup.editor_override.pt.y-obj->size.eye_offset;
			z=setup.editor_override.pt.z;
			ang_y=setup.editor_override.ang.y;
		}

			// spawn player to spot
		
		spot=&map.spots[spot_idx];
		
		object_set_position(obj,x,y,z,ang_y,0);
		obj->turn.ang_to.y=ang_y;
		
			// spot messages

		mesh_idx=map_mesh_find(&map,&obj->pnt);
		if (mesh_idx!=-1) mesh_triggers(obj,-1,mesh_idx);

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

