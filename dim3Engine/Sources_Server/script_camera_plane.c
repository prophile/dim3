/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.plane object

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

JSBool js_camera_plane_get_projectionType(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_fov(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_aspectRatio(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_left(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_right(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_top(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_bottom(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_near(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_far(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_get_nearOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_projectionType(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_fov(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_aspectRatio(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_left(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_right(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_top(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_bottom(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_near(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_far(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_nearOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			camera_plane_class={"camera_plane_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	camera_plane_props[]={
							{"projectionType",		js_camera_plane_get_projectionType,		js_camera_plane_set_projectionType},
							{"fov",					js_camera_plane_get_fov,				js_camera_plane_set_fov},
							{"aspectRatio",			js_camera_plane_get_aspectRatio,		js_camera_plane_set_aspectRatio},
							{"left",				js_camera_plane_get_left,				js_camera_plane_set_left},
							{"right",				js_camera_plane_get_right,				js_camera_plane_set_right},
							{"top",					js_camera_plane_get_top,				js_camera_plane_set_top},
							{"bottom",				js_camera_plane_get_bottom,				js_camera_plane_set_bottom},
							{"near",				js_camera_plane_get_near,				js_camera_plane_set_near},
							{"far",					js_camera_plane_get_far,				js_camera_plane_set_far},
							{"nearOffset",			js_camera_plane_get_nearOffset,			js_camera_plane_set_nearOffset},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_plane_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"plane",&camera_plane_class,camera_plane_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_camera_plane_get_projectionType(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.type+sd_projection_type_fov);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_fov(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(camera.plane.fov);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_aspectRatio(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(camera.plane.aspect_ratio);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_left(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.lft);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_right(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.rgt);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_top(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.top);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_bottom(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.bot);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_near(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.near_z);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_far(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.far_z);
	return(JS_TRUE);
}

JSBool js_camera_plane_get_nearOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(camera.plane.near_z_offset);
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_camera_plane_set_projectionType(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.type=JSVAL_TO_INT(*vp)-sd_projection_type_fov;
	return(JS_TRUE);
}

JSBool js_camera_plane_set_fov(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.fov=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_aspectRatio(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.aspect_ratio=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_left(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.lft=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_right(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.rgt=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_top(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.top=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_bottom(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.bot=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_near(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.near_z=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_far(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.far_z=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_camera_plane_set_nearOffset(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	camera.plane.near_z_offset=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}
