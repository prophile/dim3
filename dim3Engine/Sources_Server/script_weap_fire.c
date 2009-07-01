/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.fire object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_fire_get_method(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_fire_get_lastFireTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);


JSBool js_get_weap_fire_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_fire_past_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_fire_reset_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_fire_cancel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_fire_class={"weap_fire_class",0,
							script_add_property,JS_PropertyStub,
							js_get_weap_fire_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	weap_fire_props[]={
							{"method",				js_weap_fire_get_method,				NULL},
							{"lastFireTick",		js_weap_fire_get_lastFireTick,			NULL},
							{0}};
							
script_js_function	weap_fire_functions[]={
							{"pastLastFire",		js_weap_fire_past_last_fire_func,		1},
							{"resetLastFire",		js_weap_fire_reset_last_fire_func,		0},
							{"cancel",				js_weap_fire_cancel_func,				0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_fire_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"fire",&weap_fire_class,weap_fire_props,weap_fire_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_fire_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_fire_prop_method:
			*vp=INT_TO_JSVAL(weap->fire.method);
			break;
			
		case weap_fire_prop_last_fire_tick:
			if (!weap->dual.in_dual) {
				*vp=INT_TO_JSVAL(weap->fire.last_fire_tick);
			}
			else {
				*vp=INT_TO_JSVAL(weap->fire.last_fire_dual_tick);
			}
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_weap_fire_get_method(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->fire.method);
	
	return(JS_TRUE);
}

JSBool js_weap_fire_get_lastFireTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	if (!weap->dual.in_dual) {
		*vp=INT_TO_JSVAL(weap->fire.last_fire_tick);
	}
	else {
		*vp=INT_TO_JSVAL(weap->fire.last_fire_dual_tick);
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_weap_fire_past_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				last_fire_tick;
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	if (!weap->dual.in_dual) {
		last_fire_tick=weap->fire.last_fire_tick;
	}
	else {
		last_fire_tick=weap->fire.last_fire_dual_tick;
	}

	if (js.time.current_tick>(last_fire_tick+JSVAL_TO_INT(argv[0]))) {
		*rval=JSVAL_TRUE;
	}
	else {
		*rval=JSVAL_FALSE;
	}
    
	return(JS_TRUE);
}

JSBool js_weap_fire_reset_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	if (!weap->dual.in_dual) {
		weap->fire.last_fire_tick=js.time.current_tick;
	}
	else {
		weap->fire.last_fire_dual_tick=js.time.current_tick;
	}
    
	return(JS_TRUE);
}

JSBool js_weap_fire_cancel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->fire.cancel=TRUE;
    
	return(JS_TRUE);
}

