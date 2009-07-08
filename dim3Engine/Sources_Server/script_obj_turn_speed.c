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

JSBool js_obj_turn_speed_get_facingWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_motionWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_facingRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_motionRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_facingCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_motionCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_facingAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_motionAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_key(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_get_topDownAngleOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_facingWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_motionWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_facingRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_motionRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_facingCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_motionCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_facingAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_motionAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_key(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_topDownAngleOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_turn_speed_class={"obj_turn_speed_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_turn_speed_props[]={
							{"facingWalk",				js_obj_turn_speed_get_facingWalk,			js_obj_turn_speed_set_facingWalk},
							{"motionWalk",				js_obj_turn_speed_get_motionWalk,			js_obj_turn_speed_set_motionWalk},
							{"facingRun",				js_obj_turn_speed_get_facingRun,			js_obj_turn_speed_set_facingRun},
							{"motionRun",				js_obj_turn_speed_get_motionRun,			js_obj_turn_speed_set_motionRun},
							{"facingCrawl",				js_obj_turn_speed_get_facingCrawl,			js_obj_turn_speed_set_facingCrawl},
							{"motionCrawl",				js_obj_turn_speed_get_motionCrawl,			js_obj_turn_speed_set_motionCrawl},
							{"facingAir",				js_obj_turn_speed_get_facingAir,			js_obj_turn_speed_set_facingAir},
							{"motionAir",				js_obj_turn_speed_get_motionAir,			js_obj_turn_speed_set_motionAir},
							{"key",						js_obj_turn_speed_get_key,					js_obj_turn_speed_set_key},
							{"topDownAngleOffset",		js_obj_turn_speed_get_topDownAngleOffset,	js_obj_turn_speed_set_topDownAngleOffset},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_turn_speed_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"turnSpeed",&obj_turn_speed_class,obj_turn_speed_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_turn_speed_get_facingWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.walk_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_motionWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.walk_motion_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_facingRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.run_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_motionRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.run_motion_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_facingCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.crawl_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_motionCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.crawl_motion_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_facingAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.air_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_motionAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.air_motion_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_key(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.key_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_get_topDownAngleOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.top_down_ang_offset);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_turn_speed_set_facingWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.walk_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_motionWalk(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.walk_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_facingRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.run_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_motionRun(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.run_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_facingCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.crawl_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_motionCrawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.crawl_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_facingAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.air_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_motionAir(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.air_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_key(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.key_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_turn_speed_set_topDownAngleOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.top_down_ang_offset=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

