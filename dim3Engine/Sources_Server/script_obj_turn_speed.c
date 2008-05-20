/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.turnSpeed object

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

JSBool js_get_obj_turn_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_turn_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_turn_speed_class={"obj_turn_speed_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_turn_speed_property,js_set_obj_turn_speed_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_turn_speed_props[]={
							{"facingWalk",				obj_turn_speed_prop_facing_walk,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"motionWalk",				obj_turn_speed_prop_motion_walk,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"facingRun",				obj_turn_speed_prop_facing_run,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"motionRun",				obj_turn_speed_prop_motion_run,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"facingCrawl",				obj_turn_speed_prop_facing_crawl,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"motionCrawl",				obj_turn_speed_prop_motion_crawl,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"facingAir",				obj_turn_speed_prop_facing_air,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"motionAir",				obj_turn_speed_prop_motion_air,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"key",						obj_turn_speed_prop_key,				JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_turn_speed_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"turnSpeed",&obj_turn_speed_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_turn_speed_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_turn_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
    
		case obj_turn_speed_prop_facing_walk:
            *vp=script_float_to_value(obj->turn.walk_speed);
            break;
		case obj_turn_speed_prop_motion_walk:
            *vp=script_float_to_value(obj->turn.walk_motion_speed);
            break;
		case obj_turn_speed_prop_facing_run:
            *vp=script_float_to_value(obj->turn.run_speed);
            break;
		case obj_turn_speed_prop_motion_run:
            *vp=script_float_to_value(obj->turn.run_motion_speed);
            break;
		case obj_turn_speed_prop_facing_crawl:
            *vp=script_float_to_value(obj->turn.crawl_speed);
            break;
		case obj_turn_speed_prop_motion_crawl:
            *vp=script_float_to_value(obj->turn.crawl_motion_speed);
            break;
		case obj_turn_speed_prop_facing_air:
            *vp=script_float_to_value(obj->turn.air_speed);
            break;
		case obj_turn_speed_prop_motion_air:
            *vp=script_float_to_value(obj->turn.air_motion_speed);
            break;
		case obj_turn_speed_prop_key:
            *vp=script_float_to_value(obj->turn.key_speed);
            break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_turn_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_turn_speed_prop_facing_walk:
            obj->turn.walk_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_motion_walk:
            obj->turn.walk_motion_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_facing_run:
            obj->turn.run_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_motion_run:
            obj->turn.run_motion_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_facing_crawl:
            obj->turn.crawl_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_motion_crawl:
            obj->turn.crawl_motion_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_facing_air:
            obj->turn.air_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_motion_air:
            obj->turn.air_motion_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_turn_speed_prop_key:
            obj->turn.key_speed=fabsf(script_value_to_float(*vp));
            break;

	}
	
	return(JS_TRUE);
}

