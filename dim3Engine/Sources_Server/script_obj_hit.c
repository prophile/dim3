/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.hit object

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
#include "weapons.h"
#include "projectiles.h"

extern server_type		server;
extern js_type			js;

JSBool js_get_obj_hit_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_hit_class={"obj_hit_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_hit_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_hit_props[]={
							{"objectId",			obj_hit_prop_object_id,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectName",			obj_hit_prop_object_name,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectIsPlayer",		obj_hit_prop_object_is_player,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"weaponName",			obj_hit_prop_weapon_name,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"projectileName",		obj_hit_prop_projectile_name,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"hitBoxName",			obj_hit_prop_hit_box_name,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"damage",				obj_hit_prop_damage,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_hit_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"hit",&obj_hit_class,obj_hit_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_hit_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj,*hit_obj;
	weapon_type			*hit_weap;
	proj_type			*hit_proj;
	proj_setup_type		*hit_proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_hit_prop_object_id:
			*vp=INT_TO_JSVAL(obj->hit.obj_uid);
			break;

		case obj_hit_prop_object_name:
			hit_obj=object_find_uid(obj->hit.obj_uid);
			if (hit_obj==NULL) {
				*vp=JSVAL_NULL;
				break;
			}
			*vp=script_string_to_value(hit_obj->name);
			break;

		case obj_hit_prop_object_is_player:
			*vp=BOOLEAN_TO_JSVAL(obj->hit.obj_uid==server.player_obj_uid);
			break;

		case obj_hit_prop_weapon_name:
			hit_weap=weapon_find_uid(obj->hit.weap_uid);
			if (hit_weap==NULL) {
				*vp=JSVAL_NULL;
				break;
			}
			*vp=script_string_to_value(hit_weap->name);
			break;

		case obj_hit_prop_projectile_name:
			hit_proj=projectile_find_uid(obj->hit.proj_uid);
			if (hit_proj==NULL) {
				*vp=JSVAL_NULL;
				break;
			}
			hit_proj_setup=proj_setups_find_uid(hit_proj->proj_setup_uid);
			*vp=script_string_to_value(hit_proj_setup->name);
			break;

		case obj_hit_prop_hit_box_name:
			if (obj->hit.hit_box_name[0]==0x0) {
				*vp=JSVAL_NULL;
				break;
			}
			*vp=script_string_to_value(obj->hit.hit_box_name);
			break;

		case obj_hit_prop_damage:
			*vp=INT_TO_JSVAL(obj->hit.damage);
			break;
			
	}
	
	return(JS_TRUE);
}

