/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Periodic View Processes

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

#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "sounds.h"

extern server_type			server;
extern view_type			view;
extern setup_type			setup;

extern void run_objects_animations(void);
extern void segment_moves_run(void);
extern void ambient_add_objects(void);
extern void ambient_add_map_sounds(void);
extern void map_set_ambient(char *name,float pitch);
extern void map_clear_ambient(void);

/* =======================================================

      Run View Processes
      
======================================================= */

void view_run(int tick)
{
	obj_type		*obj;
	
		// run-tick objects
		
	if (tick>=view.time.run_tick) {
		while (tick>=view.time.run_tick) {
			view.time.run_tick+=10;
		}
		
		obj=object_find_uid(server.player_obj_uid);
		
			// check clicable objects
			
		obj->click.current_click_obj_uid=object_find_uid_click_object(obj);

			// listener position
			
		al_set_listener(obj->pos.x,obj->pos.y,obj->pos.z,obj->view_ang.y);
	}
	
		// ambients and music
			
	if (tick>=view.time.ambient_tick) {
		while (tick>=view.time.ambient_tick) {
			view.time.ambient_tick+=50;
		}
		
		al_ambient_list_clear();
		ambient_add_map_sounds();
		ambient_add_objects();
		
		al_ambients_run();
		al_music_run(tick);
	}
}

