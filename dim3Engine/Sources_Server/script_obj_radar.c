/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.radar object

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

JSBool js_obj_radar_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_radar_get_icon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_radar_get_motionOnly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_radar_get_alwaysVisible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_radar_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_radar_set_icon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_radar_set_motionOnly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_radar_set_alwaysVisible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_radar_class={"obj_radar_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_radar_props[]={
							{"on",					js_obj_radar_get_on,				js_obj_radar_set_on},
							{"icon",				js_obj_radar_get_icon,				js_obj_radar_set_icon},
							{"motionOnly",			js_obj_radar_get_motionOnly,		js_obj_radar_set_motionOnly},
							{"alwaysVisible",		js_obj_radar_get_alwaysVisible,		js_obj_radar_set_alwaysVisible},
							{0}};
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_radar_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"radar",&obj_radar_class,obj_radar_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_radar_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->radar.on);
	
	return(JS_TRUE);
}

JSBool js_obj_radar_get_icon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->radar.icon);
	
	return(JS_TRUE);
}

JSBool js_obj_radar_get_motionOnly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->radar.motion_only);
	
	return(JS_TRUE);
}

JSBool js_obj_radar_get_alwaysVisible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->radar.always_visible);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_radar_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_radar_set_icon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,obj->radar.icon,name_str_len);
	if (!object_set_radar_icon(obj)) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_radar_set_motionOnly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.motion_only=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_radar_set_alwaysVisible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.always_visible=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}


