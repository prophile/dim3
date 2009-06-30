/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.chaseAngle object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "cameras.h"

extern camera_type		camera;
extern js_type			js;

JSBool js_get_camera_chase_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_camera_chase_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			camera_chase_angle_class={"camera_chase_angle_class",JSCLASS_HAS_PRIVATE,
							script_add_property,JS_PropertyStub,
							js_get_camera_chase_angle_property,js_set_camera_chase_angle_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	camera_chase_angle_props[]={
							{"x",					camera_chase_angle_prop_x,			FALSE},
							{"y",					camera_chase_angle_prop_y,			FALSE},
							{"z",					camera_chase_angle_prop_z,			FALSE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_chase_angle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"chaseAngle",&camera_chase_angle_class,camera_chase_angle_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_camera_chase_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case camera_chase_angle_prop_x:
			*vp=script_float_to_value(camera.chase_ang.x);
			break;
		case camera_chase_angle_prop_y:
			*vp=script_float_to_value(camera.chase_ang.y);
			break;
		case camera_chase_angle_prop_z:
			*vp=script_float_to_value(camera.chase_ang.z);
			break;

	}

	return(JS_TRUE);
}

JSBool js_set_camera_chase_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case camera_chase_angle_prop_x:
			camera.chase_ang.x=script_value_to_float(*vp);
			break;
		case camera_chase_angle_prop_y:
			camera.chase_ang.y=script_value_to_float(*vp);
			break;
		case camera_chase_angle_prop_z:
			camera.chase_ang.z=script_value_to_float(*vp);
			break;
			
	}
	
	return(JS_TRUE);
}
