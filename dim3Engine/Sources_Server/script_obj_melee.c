/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.melee object

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

extern js_type			js;

JSBool js_get_obj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_melee_class={"obj_melee_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_melee_property,js_set_obj_melee_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_melee_props[]={
							{"strikeBoneTag",			obj_melee_prop_strike_bone_tag,		FALSE},
							{"strikePoseName",			obj_melee_prop_strike_pose_name,	FALSE},
							{"radius",					obj_melee_prop_radius,				FALSE},
							{"distance",				obj_melee_prop_distance,			FALSE},
							{"damage",					obj_melee_prop_damage,				FALSE},
							{"force",					obj_melee_prop_force,				FALSE},
							{"lifeTick",				obj_melee_prop_life_tick,			FALSE},
							{0}};

script_js_function	obj_melee_functions[]={
							{"spawnFromObjectBone",		js_obj_melee_spawn_from_object_bone_func,		0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_melee_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"melee",&obj_melee_class,obj_melee_props,obj_melee_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				str[32];
    obj_type			*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_melee_prop_strike_bone_tag:
			model_tag_to_text(obj->melee.object_strike_bone_tag,str);
			*vp=script_string_to_value(str);
			break;
		case obj_melee_prop_strike_pose_name:
			*vp=script_string_to_value(obj->melee.object_strike_pose_name);
			break;
		case obj_melee_prop_radius:
			*vp=INT_TO_JSVAL(obj->melee.radius);
			break;
		case obj_melee_prop_distance:
			*vp=INT_TO_JSVAL(obj->melee.distance);
			break;
		case obj_melee_prop_damage:
			*vp=INT_TO_JSVAL(obj->melee.damage);
			break;
		case obj_melee_prop_force:
			*vp=INT_TO_JSVAL(obj->melee.force);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				str[32];
    obj_type			*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_melee_prop_strike_bone_tag:
			script_value_to_string(*vp,str,32);
			obj->melee.object_strike_bone_tag=text_to_model_tag(str);
			break;
 		case obj_melee_prop_strike_pose_name:
			script_value_to_string(*vp,obj->melee.object_strike_pose_name,name_str_len);
			break;
		case obj_melee_prop_radius:
			obj->melee.radius=JSVAL_TO_INT(*vp);
			break;
		case obj_melee_prop_distance:
			obj->melee.distance=JSVAL_TO_INT(*vp);
			break;
		case obj_melee_prop_damage:
			obj->melee.damage=JSVAL_TO_INT(*vp);
			break;
		case obj_melee_prop_force:
			obj->melee.force=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSBool js_obj_melee_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				err_str[256];
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);

	if (!melee_script_spawn_object_model(js.time.current_tick,obj,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}
