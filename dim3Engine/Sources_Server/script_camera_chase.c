/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.chase object

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

JSBool js_camera_chase_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_get_trackSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_chase_set_trackSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			camera_chase_class={"camera_chase_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	camera_chase_props[]={
							{"size",				js_camera_chase_get_size,				js_camera_chase_set_size},
							{"distance",			js_camera_chase_get_distance,			js_camera_chase_set_distance},
							{"trackSpeed",			js_camera_chase_get_trackSpeed,			js_camera_chase_set_trackSpeed},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_chase_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"chase",&camera_chase_class,camera_chase_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_camera_chase_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.chase_size);
	return(JS_TRUE);
}

JSBool js_camera_chase_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.chase_distance);
	return(JS_TRUE);
}

JSBool js_camera_chase_get_trackSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(camera.chase_track_speed);
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_camera_chase_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.chase_size=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_chase_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.chase_distance=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_chase_set_trackSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.chase_track_speed=script_value_to_float(*vp);
	return(JS_TRUE);
}
