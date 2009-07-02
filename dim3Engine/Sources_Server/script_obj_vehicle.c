/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.vehicle object

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
extern server_type		server;
extern js_type			js;

JSBool js_get_obj_vehicle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_vehicle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vehicle_enter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_vehicle_exit_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_vehicle_remove_occupant_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_vehicle_class={"obj_vehicle_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_vehicle_props[]={
							{"on",					js_obj_vehicle_get_on,				js_obj_vehicle_set_on},
							{"hasOccupant",			js_obj_vehicle_get_hasOccupant,		NULL},
							{0}};

script_js_function	obj_vehicle_functions[]={
							{"enter",				js_obj_vehicle_enter_func,			0},
							{"exit",				js_obj_vehicle_exit_func,			0},
							{"removeOccupant",		js_obj_vehicle_exit_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_vehicle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"vehicle",&obj_vehicle_class,obj_vehicle_props,obj_vehicle_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_vehicle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_vehicle_prop_on:
			*vp=BOOLEAN_TO_JSVAL(obj->vehicle.on);
			break;
			
		case obj_vehicle_prop_has_occupant:
			*vp=BOOLEAN_TO_JSVAL(obj->vehicle.attach_obj_uid!=-1);
			break;
			
	}
	
	return(JS_TRUE);
}


JSBool js_set_obj_vehicle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_vehicle_prop_on:
			obj->vehicle.on=JSVAL_TO_BOOLEAN(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Vehicle Functions
      
======================================================= */

JSBool js_obj_vehicle_enter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			err_str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	if (!object_enter_vehicle(obj,err_str)) {
		JS_ReportError(js.cx,"Vehicle: %s",err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_obj_vehicle_exit_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			err_str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	if (!object_exit_vehicle(obj,FALSE,err_str)) {
		JS_ReportError(js.cx,"Vehicle: %s",err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

