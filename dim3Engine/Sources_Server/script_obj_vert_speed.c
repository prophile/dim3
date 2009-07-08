/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.vertSpeed object

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

JSBool js_obj_vert_speed_get_normal(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_get_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_get_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_get_flySlop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_set_normal(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_set_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_set_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_set_flySlop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_vert_speed_class={"obj_vert_speed_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_vert_speed_props[]={
							{"normal",					js_obj_vert_speed_get_normal,			js_obj_vert_speed_set_normal},
							{"acceleration",			js_obj_vert_speed_get_acceleration,		js_obj_vert_speed_set_acceleration},
							{"deceleration",			js_obj_vert_speed_get_deceleration,		js_obj_vert_speed_set_deceleration},
							{"flySlop",					js_obj_vert_speed_get_flySlop,			js_obj_vert_speed_set_flySlop},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_vert_speed_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"verticalSpeed",&obj_vert_speed_class,obj_vert_speed_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_vert_speed_get_normal(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.max_walk_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_vert_speed_get_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.accelerate);
	
	return(JS_TRUE);
}

JSBool js_obj_vert_speed_get_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.decelerate);
	
	return(JS_TRUE);
}

JSBool js_obj_vert_speed_get_flySlop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.slop);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_vert_speed_set_normal(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.max_walk_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_vert_speed_set_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.accelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}


JSBool js_obj_vert_speed_set_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.decelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}


JSBool js_obj_vert_speed_set_flySlop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.slop=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}


