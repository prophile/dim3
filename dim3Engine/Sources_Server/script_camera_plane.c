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

JSBool js_get_camera_plane_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_camera_plane_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			camera_plane_class={"camera_plane_class",JSCLASS_HAS_PRIVATE,
							script_add_property,JS_PropertyStub,
							js_get_camera_plane_property,js_set_camera_plane_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	camera_plane_props[]={
							{"projectionType",		camera_plane_prop_projection_type,	FALSE},
							{"fov",					camera_plane_prop_fov,				FALSE},
							{"aspectRatio",			camera_plane_prop_aspect_ratio,		FALSE},
							{"left",				camera_plane_prop_left,				FALSE},
							{"right",				camera_plane_prop_right,			FALSE},
							{"top",					camera_plane_prop_top,				FALSE},
							{"bottom",				camera_plane_prop_bottom,			FALSE},
							{"near",				camera_plane_prop_near,				FALSE},
							{"far",					camera_plane_prop_far,				FALSE},
							{"nearOffset",			camera_plane_prop_near_offset,		FALSE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_plane_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"plane",&camera_plane_class,camera_plane_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_camera_plane_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case camera_plane_prop_projection_type:
			*vp=INT_TO_JSVAL(camera.plane.type+sd_projection_type_fov);
			break;
		case camera_plane_prop_fov:
			*vp=script_float_to_value(camera.plane.fov);
			break;
		case camera_plane_prop_aspect_ratio:
			*vp=script_float_to_value(camera.plane.aspect_ratio);
			break;
		case camera_plane_prop_left:
			*vp=INT_TO_JSVAL(camera.plane.lft);
			break;
		case camera_plane_prop_right:
			*vp=INT_TO_JSVAL(camera.plane.rgt);
			break;
		case camera_plane_prop_top:
			*vp=INT_TO_JSVAL(camera.plane.top);
			break;
		case camera_plane_prop_bottom:
			*vp=INT_TO_JSVAL(camera.plane.bot);
			break;
		case camera_plane_prop_near:
			*vp=INT_TO_JSVAL(camera.plane.near_z);
			break;
		case camera_plane_prop_far:
			*vp=INT_TO_JSVAL(camera.plane.far_z);
			break;
		case camera_plane_prop_near_offset:
			*vp=INT_TO_JSVAL(camera.plane.near_z_offset);
			break;

	}

	return(JS_TRUE);
}

JSBool js_set_camera_plane_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case camera_plane_prop_projection_type:
			camera.plane.type=JSVAL_TO_INT(*vp)-sd_projection_type_fov;
			break;
		case camera_plane_prop_fov:
			camera.plane.fov=script_value_to_float(*vp);
			break;
		case camera_plane_prop_aspect_ratio:
			camera.plane.aspect_ratio=script_value_to_float(*vp);
			break;
		case camera_plane_prop_left:
			camera.plane.lft=JSVAL_TO_INT(*vp);
			break;
		case camera_plane_prop_right:
			camera.plane.rgt=JSVAL_TO_INT(*vp);
			break;
		case camera_plane_prop_top:
			camera.plane.top=JSVAL_TO_INT(*vp);
			break;
		case camera_plane_prop_bottom:
			camera.plane.bot=JSVAL_TO_INT(*vp);
			break;
		case camera_plane_prop_near:
			camera.plane.near_z=JSVAL_TO_INT(*vp);
			break;
		case camera_plane_prop_far:
			camera.plane.far_z=JSVAL_TO_INT(*vp);
			break;
		case camera_plane_prop_near_offset:
			camera.plane.near_z_offset=JSVAL_TO_INT(*vp);
			break;
			
	}
	
	return(JS_TRUE);
}
