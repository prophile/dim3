/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.weaponFire object

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

extern js_type			js;

JSBool js_get_obj_weapon_fire_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_weapon_fire_class={"obj_weapon_fire_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_weapon_fire_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_weapon_fire_props[]={
							{"name",				obj_weapon_fire_prop_name,			TRUE},
							{"method",				obj_weapon_fire_prop_method,		TRUE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_weapon_fire_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"weaponFire",&obj_weapon_fire_class,obj_weapon_fire_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_weapon_fire_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_weapon_fire_prop_name:
			*vp=script_string_to_value(obj->weapon_fire.name);
			break;
		case obj_weapon_fire_prop_method:
			*vp=INT_TO_JSVAL(obj->weapon_fire.method);
			break;
			
	}
	
	return(JS_TRUE);
}

