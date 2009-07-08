/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.size object

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

JSBool js_obj_size_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_get_eyeOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_get_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_get_clickDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_set_eyeOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_set_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_set_clickDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_size_grow_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_size_grow_over_time_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_size_grow_over_time_change_size_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_size_grow_over_time_change_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_size_class={"obj_size_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_size_props[]={
							{"x",					js_obj_size_get_x,					js_obj_size_set_x},
							{"y",					js_obj_size_get_y,					js_obj_size_set_y},
							{"z",					js_obj_size_get_z,					js_obj_size_set_z},
							{"eyeOffset",			js_obj_size_get_eyeOffset,			js_obj_size_set_eyeOffset},
							{"weight",				js_obj_size_get_weight,				js_obj_size_set_weight},
							{"clickDistance",		js_obj_size_get_clickDistance,		js_obj_size_set_clickDistance},
							{0}};

script_js_function	obj_size_functions[]={
							{"growTo",						js_obj_size_grow_to_func,						1},
							{"growOverTime",				js_obj_size_grow_over_time_func,				2},
							{"growOverTimeChangeSize",		js_obj_size_grow_over_time_change_size_func,	5},
							{"growOverTimeChangeOffset",	js_obj_size_grow_over_time_change_offset_func,	5},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_size_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"size",&obj_size_class,obj_size_props,obj_size_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_size_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->size.x);
	
	return(JS_TRUE);
}

JSBool js_obj_size_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->size.y);
	
	return(JS_TRUE);
}

JSBool js_obj_size_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->size.z);
	
	return(JS_TRUE);
}

JSBool js_obj_size_get_eyeOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->size.eye_offset);
	
	return(JS_TRUE);
}

JSBool js_obj_size_get_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->size.weight);
	
	return(JS_TRUE);
}

JSBool js_obj_size_get_clickDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->click.distance);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_size_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.x=JSVAL_TO_INT(*vp);
	object_set_radius(obj);
	
	return(JS_TRUE);
}

JSBool js_obj_size_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.y=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_size_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.z=JSVAL_TO_INT(*vp);
	object_set_radius(obj);
	
	return(JS_TRUE);
}

JSBool js_obj_size_set_eyeOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.eye_offset=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_size_set_weight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.weight=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_size_set_clickDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->click.distance=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Function
      
======================================================= */

JSBool js_obj_size_grow_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			resize;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_direct(obj,resize);
	
	return(JS_TRUE);
}

JSBool js_obj_size_grow_over_time_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msec;
	float			resize;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	msec=JSVAL_TO_INT(argv[1]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,NULL,NULL);
	
	return(JS_TRUE);
}

JSBool js_obj_size_grow_over_time_change_size_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msec;
	float			resize;
	d3pnt			size;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	
	size.x=JSVAL_TO_INT(argv[1]);
	size.z=JSVAL_TO_INT(argv[2]);
	size.y=JSVAL_TO_INT(argv[3]);

	msec=JSVAL_TO_INT(argv[4]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,&size,NULL);
	
	return(JS_TRUE);
}

JSBool js_obj_size_grow_over_time_change_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msec;
	float			resize;
	d3pnt			offset;
	obj_type		*obj;

	resize=script_value_to_float(argv[0]);
	
	offset.x=JSVAL_TO_INT(argv[1]);
	offset.z=JSVAL_TO_INT(argv[2]);
	offset.y=JSVAL_TO_INT(argv[3]);

	msec=JSVAL_TO_INT(argv[4]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,NULL,&offset);
	
	return(JS_TRUE);
}
