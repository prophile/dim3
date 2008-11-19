/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Held Items

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
#include "objects.h"
#include "models.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;

/* =======================================================

      Object Adding Held Item
      
======================================================= */

int object_held_add(obj_type *obj,char *name,char *type,char *script,char *params,char *err_str)
{
	return(object_script_spawn(name,type,script,params,&obj->pnt,&obj->ang,TRUE));
}

/* =======================================================

      Object Dropping Held Item
      
======================================================= */

bool object_held_drop(obj_type *obj,int uid,float y_ang,int y_change,char *err_str)
{
	obj_type		*held_obj;

		// find object

	held_obj=object_find_uid(uid);
	if (held_obj==NULL) {
		strcpy(err_str,"No object with that ID exists");
		return(FALSE);
	}

		// drop object

	object_set_position(held_obj,obj->pnt.x,(obj->pnt.y+y_change),obj->pnt.z,obj->ang.y,y_ang);
	held_obj->turn.ang_to.y=y_ang;

	held_obj->hidden=FALSE;
	held_obj->contact.object_on=TRUE;

	return(TRUE);
}


