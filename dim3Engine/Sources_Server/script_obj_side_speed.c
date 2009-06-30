/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.sideSpeed object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

JSBool js_get_obj_side_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_side_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_side_speed_class={"obj_side_speed_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_side_speed_property,js_set_obj_side_speed_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_side_speed_props[]={
							{"walk",					obj_side_speed_prop_walk,				FALSE},
							{"run",						obj_side_speed_prop_run,				FALSE},
							{"crawl",					obj_side_speed_prop_crawl,				FALSE},
							{"air",						obj_side_speed_prop_air,				FALSE},
							{"acceleration",			obj_side_speed_prop_acceleration,		FALSE},
							{"deceleration",			obj_side_speed_prop_deceleration,		FALSE},
							{"accelerationAir",			obj_side_speed_prop_acceleration_air,	FALSE},
							{"decelerationAir",			obj_side_speed_prop_deceleration_air,	FALSE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_side_speed_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"sideSpeed",&obj_side_speed_class,obj_side_speed_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_side_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
    
		case obj_side_speed_prop_walk:
            *vp=script_float_to_value(obj->side_move.max_walk_speed);
            break;
		case obj_side_speed_prop_run:
            *vp=script_float_to_value(obj->side_move.max_run_speed);
            break;
		case obj_side_speed_prop_crawl:
            *vp=script_float_to_value(obj->side_move.max_crawl_speed);
            break;
		case obj_side_speed_prop_air:
            *vp=script_float_to_value(obj->side_move.max_air_speed);
            break;
		case obj_side_speed_prop_acceleration:
            *vp=script_float_to_value(obj->side_move.accelerate);
            break;
		case obj_side_speed_prop_deceleration:
            *vp=script_float_to_value(obj->side_move.decelerate);
            break;
		case obj_side_speed_prop_acceleration_air:
            *vp=script_float_to_value(obj->side_move.air_accelerate);
            break;
		case obj_side_speed_prop_deceleration_air:
            *vp=script_float_to_value(obj->side_move.air_decelerate);
            break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_side_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_side_speed_prop_walk:
            obj->side_move.max_walk_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_side_speed_prop_run:
            obj->side_move.max_run_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_side_speed_prop_crawl:
            obj->side_move.max_crawl_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_side_speed_prop_air:
            obj->side_move.max_air_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_side_speed_prop_acceleration:
            obj->side_move.accelerate=fabsf(script_value_to_float(*vp));
            break;
		case obj_side_speed_prop_deceleration:
            obj->side_move.decelerate=fabsf(script_value_to_float(*vp));
            break;
		case obj_side_speed_prop_acceleration_air:
            obj->side_move.air_accelerate=fabsf(script_value_to_float(*vp));
            break;
		case obj_side_speed_prop_deceleration_air:
            obj->side_move.air_decelerate=fabsf(script_value_to_float(*vp));
            break;

	}
	
	return(JS_TRUE);
}

