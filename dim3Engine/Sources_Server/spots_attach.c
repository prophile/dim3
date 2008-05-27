/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Spot Routines

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

extern map_type					map;
extern server_type				server;

// supergumba -- might delete this whole file

/* =======================================================

      Start Attaches
      
======================================================= */

/* supergumba -- delete me

void spot_start_object(spot_type *spot)
{
	obj_type		*obj;
	char			err_str[256];
	
		// create object
		
	obj=object_create(bt_map);
	if (obj==NULL) return;
		
	strcpy(obj->name,spot->attach_name);
	strcpy(obj->type,spot->attach_type);
	
		// if there's an editor display model, then
		// default model to it
		
	if (spot->display_model[0]!=0x0) {
		obj->draw.on=TRUE;
		strcpy(obj->draw.name,spot->display_model);
	}

		// attach object to spot
		
	object_set_position(obj,spot->pos.x,spot->pos.z,spot->pos.y,spot->ang.y,0);
	obj->turn.ang_to.y=spot->ang.y;
	
	object_reset_prepare(obj);
		
		// start scripts
		
	object_start_script(obj,spot->attach_script,spot->attach_params,bt_map,err_str);
}
*/

void spot_start_attach(void)
{
	int					i;
	char				err_str[256];
	spot_type			*spot;
	
		// check if a spot was attached by a
		// script.  If it was, and the skill levels
		// are OK, spawn this object into the map
		
	spot=map.spots;
	
	for (i=0;i!=map.nspot;i++) {
		if ((spot->attach) && (spot->skill<=server.skill)) object_start(spot,FALSE,bt_map,err_str);
		spot++;
	}
}

