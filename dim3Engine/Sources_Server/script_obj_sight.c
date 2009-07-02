/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.sight object

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

extern server_type		server;
extern js_type			js;

JSBool js_get_obj_sight_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_sight_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_test_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_sight_test_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_sight_class={"obj_sight_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_sight_props[]={
							{"sideFieldAngle",		js_obj_sight_get_sideFieldAngle,	js_obj_sight_set_sideFieldAngle},
							{"lookFieldAngle",		js_obj_sight_get_lookFieldAngle,	js_obj_sight_set_lookFieldAngle},
							{"sideFieldDivision",	js_obj_sight_get_sideFieldDivision,	js_obj_sight_set_sideFieldDivision},
							{"lookFieldDivision",	js_obj_sight_get_lookFieldDivision,	js_obj_sight_set_lookFieldDivision},
							{"distance",			js_obj_sight_get_distance,			js_obj_sight_set_distance},
							{0}};

script_js_function	obj_sight_functions[]={
							{"testObject",			js_obj_sight_test_object_func,		1},
							{"testPlayer",			js_obj_sight_test_player_func,		0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_sight_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"sight",&obj_sight_class,obj_sight_props,obj_sight_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_sight_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_sight_prop_side_field_angle:
			*vp=script_float_to_value(obj->sight.side_angle);
			break;
		case obj_sight_prop_look_field_angle:
			*vp=script_float_to_value(obj->sight.look_angle);
			break;
		case obj_sight_prop_side_field_division:
			*vp=INT_TO_JSVAL(obj->sight.side_division);
			break;
		case obj_sight_prop_look_field_division:
			*vp=INT_TO_JSVAL(obj->sight.look_division);
			break;
		case obj_sight_prop_distance:
			*vp=INT_TO_JSVAL(obj->sight.distance);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_sight_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_sight_prop_side_field_angle:
			obj->sight.side_angle=script_value_to_float(*vp);
			break;
		case obj_sight_prop_look_field_angle:
			obj->sight.look_angle=script_value_to_float(*vp);
			break;
		case obj_sight_prop_side_field_division:
			obj->sight.side_division=JSVAL_TO_INT(*vp);
			break;
		case obj_sight_prop_look_field_division:
			obj->sight.look_division=JSVAL_TO_INT(*vp);
			break;
		case obj_sight_prop_distance:
			obj->sight.distance=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_obj_sight_test_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				id;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	id=JSVAL_TO_INT(argv[0]);
	*rval=BOOLEAN_TO_JSVAL(object_sight_test_object(obj,id));
	
	return(JS_TRUE);
}

JSBool js_obj_sight_test_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	*rval=BOOLEAN_TO_JSVAL(object_sight_test_object(obj,server.player_obj_uid));

	return(JS_TRUE);
}
