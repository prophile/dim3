/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.status object

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

extern map_type			map;
extern js_type			js;

JSBool js_obj_status_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_moving(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_running(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_backward(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_sliding(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_stand(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_air(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_liquid(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_standOnObjectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_get_standUnderObjectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_freeze_input_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_status_tint_view_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_status_class={"obj_status_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_status_props[]={
							{"speed",				js_obj_status_get_speed,				NULL},
							{"moving",				js_obj_status_get_moving,				NULL},
							{"running",				js_obj_status_get_running,				NULL},
							{"backward",			js_obj_status_get_backward,				NULL},
							{"sliding",				js_obj_status_get_sliding,				NULL},
							{"stand",				js_obj_status_get_stand,				NULL},
							{"air",					js_obj_status_get_air,					NULL},
							{"liquid",				js_obj_status_get_liquid,				NULL},
							{"standOnObjectId",		js_obj_status_get_standOnObjectId,		NULL},
							{"standUnderObjectId",	js_obj_status_get_standUnderObjectId,	NULL},
							{0}};
							
script_js_function	obj_status_functions[]={
							{"freezeInput",			js_obj_status_freeze_input_func,		1},
							{"tintView",			js_obj_status_tint_view_func,			7},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_status_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"status",&obj_status_class,obj_status_props,obj_status_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_status_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.speed);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_moving(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->forward_move.moving);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_running(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->forward_move.running);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_backward(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->forward_move.reverse);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_sliding(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->side_move.moving);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_stand(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->duck.mode+sd_stand_standing);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_air(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->air_mode+sd_air_up);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_liquid(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->liquid_mode+sd_liquid_out);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_standOnObjectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->stand_obj_uid);
	
	return(JS_TRUE);
}

JSBool js_obj_status_get_standUnderObjectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(object_find_uid_by_stood_on_object_uid(obj->uid));
	
	return(JS_TRUE);
}

/* =======================================================

      Input Freeze Functions
      
======================================================= */

JSBool js_obj_status_freeze_input_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_input_freeze(obj,JSVAL_TO_BOOLEAN(argv[0]));

	return(JS_TRUE);
}

/* =======================================================

      Full Screen Effects
      
======================================================= */

JSBool js_obj_status_tint_view_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			r,g,b;
	obj_type		*obj;
	
	r=script_value_to_float(argv[0]);
	g=script_value_to_float(argv[1]);
	b=script_value_to_float(argv[2]);

	obj=object_find_uid(js.attach.thing_uid);
	object_fs_tint_start(js.time.current_tick,obj,r,g,b,script_value_to_float(argv[3]),JSVAL_TO_INT(argv[4]),JSVAL_TO_INT(argv[5]),JSVAL_TO_INT(argv[6]));
    *rval=JSVAL_TRUE;
    
	return(JS_TRUE);
}
