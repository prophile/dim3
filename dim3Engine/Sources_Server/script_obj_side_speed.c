/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.sideSpeed object

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

JSBool js_obj_side_speed_get_walk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_get_run(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_get_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_get_air(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_get_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_get_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_get_accelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_get_decelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_walk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_run(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_air(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_accelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_side_speed_set_decelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_side_speed_class={"obj_side_speed_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_side_speed_props[]={
							{"walk",					js_obj_side_speed_get_walk,				js_obj_side_speed_set_walk},
							{"run",						js_obj_side_speed_get_run,				js_obj_side_speed_set_run},
							{"crawl",					js_obj_side_speed_get_crawl,			js_obj_side_speed_set_crawl},
							{"air",						js_obj_side_speed_get_air,				js_obj_side_speed_set_air},
							{"acceleration",			js_obj_side_speed_get_acceleration,		js_obj_side_speed_set_acceleration},
							{"deceleration",			js_obj_side_speed_get_deceleration,		js_obj_side_speed_set_deceleration},
							{"accelerationAir",			js_obj_side_speed_get_accelerationAir,	js_obj_side_speed_set_accelerationAir},
							{"decelerationAir",			js_obj_side_speed_get_decelerationAir,	js_obj_side_speed_set_decelerationAir},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_side_speed_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"sideSpeed",&obj_side_speed_class,obj_side_speed_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_side_speed_get_walk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_walk_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_get_run(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_run_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_get_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_crawl_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_get_air(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_air_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_get_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.accelerate);
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_get_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.decelerate);
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_get_accelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.air_accelerate);
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_get_decelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.air_decelerate);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_side_speed_set_walk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_walk_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_set_run(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_run_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_set_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_crawl_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_set_air(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_air_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_set_acceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.accelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_set_deceleration(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.decelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_set_accelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.air_accelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_side_speed_set_decelerationAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.air_decelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

