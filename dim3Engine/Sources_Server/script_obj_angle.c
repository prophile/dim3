/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.angle object

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

JSBool js_get_obj_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_angle_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_angle_rotate_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_angle_class={"obj_angle_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_angle_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_angle_props[]={
							{"x",					obj_angle_prop_x,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"y",					obj_angle_prop_y,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"z",					obj_angle_prop_z,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

JSFunctionSpec	obj_angle_functions[]={
							{"rotate",				js_obj_angle_rotate_func,			1},
							{"rotateTo",			js_obj_angle_rotate_to_func,		1},
							{0}};

extern void object_setup_motion(obj_type *obj,float ang,float speed);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_angle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"angle",&obj_angle_class,obj_angle_props,obj_angle_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_angle_prop_x:
            *vp=script_float_to_value(obj->ang.x);
			break;
		case obj_angle_prop_y:
            *vp=script_float_to_value(obj->ang.y);
			break;
		case obj_angle_prop_z:
            *vp=script_float_to_value(obj->ang.z);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Rotational Functions
      
======================================================= */

JSBool js_obj_angle_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			ang_add;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	ang_add=script_value_to_float(argv[0]);
	obj->ang.y=angle_add(obj->ang.y,ang_add);

	return(JS_TRUE);
}

JSBool js_obj_angle_rotate_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->ang.y=script_value_to_float(argv[0]);

	return(JS_TRUE);
}






