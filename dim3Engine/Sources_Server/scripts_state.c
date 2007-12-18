/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Save/Load State

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

#include "scripts.h"

extern server_type		server;
extern js_type			js;

/* =======================================================

      Setup Script States for Save
      
======================================================= */

inline void script_state_save_single(attach_type *attach)
{
	char			err_str[256];

	scripts_post_event(attach,sd_event_state,sd_event_state_save,0,err_str);
}

void script_state_save(void)
{
	int				n;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;

	script_state_save_single(&js.game_attach);
	script_state_save_single(&js.course_attach);
	
	obj=server.objs;
	for (n=0;n!=server.count.obj;n++) {
		script_state_save_single(&obj->attach);
		obj++;
	}
	
	weap=server.weapons;
	for (n=0;n!=server.count.weapon;n++) {
		script_state_save_single(&weap->attach);
		weap++;
	}
	
	proj_setup=server.proj_setups;
	for (n=0;n!=server.count.proj_setup;n++) {
		script_state_save_single(&proj_setup->attach);
		proj_setup++;
	}
}

/* =======================================================

      Setup Script States after Load
      
======================================================= */

inline void script_state_load_single(attach_type *attach)
{
	char			err_str[256];

	scripts_post_event(attach,sd_event_state,sd_event_state_load,0,err_str);
}

void script_state_load(void)
{
	int				n;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;
		
	script_state_load_single(&js.game_attach);
	script_state_load_single(&js.course_attach);
	
	obj=server.objs;
	for (n=0;n!=server.count.obj;n++) {
		script_state_load_single(&obj->attach);
		obj++;
	}
	
	weap=server.weapons;
	for (n=0;n!=server.count.weapon;n++) {
		script_state_load_single(&weap->attach);
		weap++;
	}
	
	proj_setup=server.proj_setups;
	for (n=0;n!=server.count.proj_setup;n++) {
		script_state_load_single(&proj_setup->attach);
		proj_setup++;
	}
}


