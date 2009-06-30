/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.motionAngle object

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

JSBool js_get_obj_motion_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_motion_angle_turn_to_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_angle_turn_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_motion_angle_class={"obj_motion_angle_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_motion_angle_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_motion_angle_props[]={
							{"x",					obj_motion_angle_prop_x,			TRUE},
							{"y",					obj_motion_angle_prop_y,			TRUE},
							{"z",					obj_motion_angle_prop_z,			TRUE},
							{0}};

script_js_function	obj_motion_angle_functions[]={
							{"turnToAngle",			js_obj_motion_angle_turn_to_angle_func,	2},
							{"turnStop",			js_obj_motion_angle_turn_stop_func,		0},
							{0}};
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_motion_angle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"motionAngle",&obj_motion_angle_class,obj_motion_angle_props,obj_motion_angle_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_motion_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_motion_angle_prop_x:
            *vp=script_float_to_value(obj->motion.ang.x);
			break;
		case obj_motion_angle_prop_y:
            *vp=script_float_to_value(obj->motion.ang.y);
			break;
		case obj_motion_angle_prop_z:
            *vp=script_float_to_value(obj->motion.ang.z);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Turn Functions
      
======================================================= */

JSBool js_obj_motion_angle_turn_to_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			turn_speed;
	bool			cwise;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->turn.ang_to.y=script_value_to_float(argv[0]);
	if (obj->turn.ang_to.y<0.0f) obj->turn.ang_to.y=360.0f+obj->turn.ang_to.y;
	if (obj->turn.ang_to.y>=360.0f) obj->turn.ang_to.y=obj->turn.ang_to.y-360.0f;

	turn_speed=object_get_turn_speed(obj);

	switch (JSVAL_TO_INT(argv[1])) {
	
		case -1:
			obj->turn.ang_add.y=-turn_speed;
			break;
		case 1:
			obj->turn.ang_add.y=turn_speed;
			break;
		default:
			angle_dif(obj->turn.ang_to.y,obj->ang.y,&cwise);
			if (cwise) {
				obj->turn.ang_add.y=-turn_speed;
			}
			else {
				obj->turn.ang_add.y=turn_speed;
			}
			break;
	
	}

	return(JS_TRUE);
}

JSBool js_obj_motion_angle_turn_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	object_turn_stop(obj);

	return(JS_TRUE);
}
