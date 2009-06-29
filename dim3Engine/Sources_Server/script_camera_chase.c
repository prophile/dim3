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

JSBool js_get_camera_chase_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_camera_chase_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			camera_chase_class={"camera_chase_class",JSCLASS_HAS_PRIVATE,
							script_add_property,JS_PropertyStub,
							js_get_camera_chase_property,js_set_camera_chase_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	camera_chase_props[]={
							{"size",				camera_chase_prop_size,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"distance",			camera_chase_prop_distance,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"trackSpeed",			camera_chase_prop_track_speed,		JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_chase_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"chase",&camera_chase_class,camera_chase_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_camera_chase_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case camera_chase_prop_size:
			*vp=INT_TO_JSVAL(camera.chase_size);
			break;
		case camera_chase_prop_distance:
			*vp=INT_TO_JSVAL(camera.chase_distance);
			break;
		case camera_chase_prop_track_speed:
			*vp=script_float_to_value(camera.chase_track_speed);
			break;

	}

	return(JS_TRUE);
}

JSBool js_set_camera_chase_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case camera_chase_prop_size:
			camera.chase_size=JSVAL_TO_INT(*vp);
			break;
		case camera_chase_prop_distance:
			camera.chase_distance=JSVAL_TO_INT(*vp);
			break;
		case camera_chase_prop_track_speed:
			camera.chase_track_speed=script_value_to_float(*vp);
			break;
			
	}
	
	return(JS_TRUE);
}
