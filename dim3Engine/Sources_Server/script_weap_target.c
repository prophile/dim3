/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.target object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

JSBool js_get_weap_target_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_target_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_target_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_target_start_opponent_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_target_end_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_target_class={"weap_target_class",0,
							script_add_property,JS_PropertyStub,
                            js_get_weap_target_property,js_set_weap_target_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};
                            
JSPropertySpec	weap_target_props[]={
							{"on",					weap_target_prop_on,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"distance",			weap_target_prop_distance,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectId",			weap_target_prop_objectId,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	weap_target_functions[]={
							{"start",				js_weap_target_start_func,				1},
							{"startOpponent",		js_weap_target_start_opponent_func,		0},
							{"end",					js_weap_target_end_func,				0},
							{0}};

extern void script_add_weap_target_color_object(JSObject *parent_obj);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_target_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=script_create_child_object(parent_obj,"target",&weap_target_class,weap_target_props,weap_target_functions);

		// target color objects
		
	script_add_weap_target_color_object(j_obj);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_target_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_target_prop_on:
			*vp=BOOLEAN_TO_JSVAL(weap->target.on);
			break;
		case weap_target_prop_distance:
			*vp=INT_TO_JSVAL(weap->target.distance);
			break;
		case weap_target_prop_objectId:
			*vp=INT_TO_JSVAL(weap->target.obj_uid);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_target_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_target_prop_distance:
			weap->target.distance=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Target Functions
      
======================================================= */

JSBool js_weap_target_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				target_type[name_str_len];
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],target_type,name_str_len);

	*rval=BOOLEAN_TO_JSVAL(weapon_target_start(obj,weap,target_type));

	return(JS_TRUE);
}

JSBool js_weap_target_start_opponent_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	*rval=BOOLEAN_TO_JSVAL(weapon_target_start(obj,weap,NULL));

	return(JS_TRUE);
}

JSBool js_weap_target_end_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	*rval=BOOLEAN_TO_JSVAL(weapon_target_end(obj,weap));

	return(JS_TRUE);
}

