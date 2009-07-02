/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.recoil object

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

JSBool js_get_weap_recoil_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_recoil_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_recoil_go_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_recoil_class={"weap_recoil_class",0,
							script_add_property,JS_PropertyStub,
                            JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};


script_js_property	weap_recoil_props[]={
							{"minX",				js_weap_recoil_get_minX,			js_weap_recoil_set_minX},
							{"maxX",				js_weap_recoil_get_maxX,			js_weap_recoil_set_maxX},
							{"resetX",				js_weap_recoil_get_resetX,			js_weap_recoil_set_resetX},
							{"minY",				js_weap_recoil_get_minY,			js_weap_recoil_set_minY},
							{"maxY",				js_weap_recoil_get_maxY,			js_weap_recoil_set_maxY},
							{"resetY",				js_weap_recoil_get_resetY,			js_weap_recoil_set_resetY},
							{"minZ",				js_weap_recoil_get_minZ,			js_weap_recoil_set_minZ},
							{"maxZ",				js_weap_recoil_get_maxZ,			js_weap_recoil_set_maxZ},
							{"resetZ",				js_weap_recoil_get_resetZ,			js_weap_recoil_set_resetZ},
							{"resetLook",			js_weap_recoil_get_resetLook,		js_weap_recoil_set_resetLook},
							{"resetTurn",			js_weap_recoil_get_resetTurn,		js_weap_recoil_set_resetTurn},
							{0}};
							
script_js_function	weap_recoil_functions[]={
							{"go",					js_weap_recoil_go_func,				0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_recoil_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"recoil",&weap_recoil_class,weap_recoil_props,weap_recoil_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_recoil_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_recoil_prop_min_x:
            *vp=script_float_to_value(weap->recoil.min_ang.x);
			break;
		case weap_recoil_prop_max_x:
            *vp=script_float_to_value(weap->recoil.max_ang.x);
			break;
		case weap_recoil_prop_reset_x:
            *vp=script_float_to_value(weap->recoil.reset_ang.x);
			break;
		case weap_recoil_prop_min_y:
            *vp=script_float_to_value(weap->recoil.min_ang.y);
			break;
		case weap_recoil_prop_max_y:
            *vp=script_float_to_value(weap->recoil.max_ang.y);
			break;
		case weap_recoil_prop_reset_y:
            *vp=script_float_to_value(weap->recoil.reset_ang.y);
			break;
		case weap_recoil_prop_min_z:
            *vp=script_float_to_value(weap->recoil.min_ang.z);
			break;
		case weap_recoil_prop_max_z:
            *vp=script_float_to_value(weap->recoil.max_ang.z);
			break;
		case weap_recoil_prop_reset_z:
            *vp=script_float_to_value(weap->recoil.reset_ang.z);
			break;
		case weap_recoil_prop_reset_look:
            *vp=script_float_to_value(weap->recoil.look_reset);
			break;
		case weap_recoil_prop_reset_turn:
            *vp=script_float_to_value(weap->recoil.turn_reset);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_recoil_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_recoil_prop_min_x:
			weap->recoil.min_ang.x=script_value_to_float(*vp);
			break;
		case weap_recoil_prop_max_x:
			weap->recoil.max_ang.x=script_value_to_float(*vp);
			break;
		case weap_recoil_prop_reset_x:
			weap->recoil.reset_ang.x=fabsf(script_value_to_float(*vp));
			break;
		case weap_recoil_prop_min_y:
			weap->recoil.min_ang.y=script_value_to_float(*vp);
			break;
		case weap_recoil_prop_max_y:
			weap->recoil.max_ang.y=script_value_to_float(*vp);
			break;
		case weap_recoil_prop_reset_y:
			weap->recoil.reset_ang.y=fabsf(script_value_to_float(*vp));
			break;
		case weap_recoil_prop_min_z:
			weap->recoil.min_ang.z=script_value_to_float(*vp);
			break;
		case weap_recoil_prop_max_z:
			weap->recoil.max_ang.z=script_value_to_float(*vp);
			break;
		case weap_recoil_prop_reset_z:
			weap->recoil.reset_ang.z=fabsf(script_value_to_float(*vp));
			break;
		case weap_recoil_prop_reset_look:
			weap->recoil.look_reset=fabsf(script_value_to_float(*vp));
			break;
		case weap_recoil_prop_reset_turn:
			weap->recoil.turn_reset=fabsf(script_value_to_float(*vp));
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Recoil Functions
      
======================================================= */

JSBool js_weap_recoil_go_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	weapon_recoil(obj,weap);

	return(JS_TRUE);
}

