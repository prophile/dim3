/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.thrust object

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

extern js_type			js;

JSBool js_obj_thrust_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_get_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_get_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_set_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_set_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_set_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_thrust_class={"obj_thrust_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_thrust_props[]={
							{"speed",					js_obj_thrust_get_speed,			js_obj_thrust_set_speed},
							{"maxSpeed",				js_obj_thrust_get_maxSpeed,			js_obj_thrust_set_maxSpeed},
							{"drag",					js_obj_thrust_get_drag,				js_obj_thrust_set_drag},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_thrust_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"thrust",&obj_thrust_class,obj_thrust_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_thrust_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->thrust.speed);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_get_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_get_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->thrust.drag);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_thrust_set_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->thrust.speed=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_set_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->thrust.max_speed=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_set_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->thrust.drag=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}


