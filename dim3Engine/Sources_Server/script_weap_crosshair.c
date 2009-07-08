/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.crosshair object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_crosshair_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_get_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_get_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_set_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_set_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_set_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_crosshair_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			weap_crosshair_class={"weap_crosshair_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	weap_crosshair_props[]={
							{"on",					js_weap_crosshair_get_on,			js_weap_crosshair_set_on},
							{"type",				js_weap_crosshair_get_type,			js_weap_crosshair_set_type},
							{"name",				js_weap_crosshair_get_name,			js_weap_crosshair_set_name},
							{"minSize",				js_weap_crosshair_get_minSize,		js_weap_crosshair_set_minSize},
							{"maxSize",				js_weap_crosshair_get_maxSize,		js_weap_crosshair_set_maxSize},
							{"distance",			js_weap_crosshair_get_distance,		js_weap_crosshair_set_distance},
							{0}};

extern void script_add_weap_crosshair_color_object(JSObject *parent_obj);
extern void script_add_weap_crosshair_empty_color_object(JSObject *parent_obj);
extern void script_add_weap_crosshair_pickup_color_object(JSObject *parent_obj);
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_crosshair_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
	
	j_obj=script_create_child_object(parent_obj,"crosshair",&weap_crosshair_class,weap_crosshair_props,NULL);
	
		// crosshair color objects
		
	script_add_weap_crosshair_color_object(j_obj);
	script_add_weap_crosshair_empty_color_object(j_obj);
	script_add_weap_crosshair_pickup_color_object(j_obj);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_weap_crosshair_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(weap->crosshair.on);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->crosshair.type+sd_weap_crosshair_type_center);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->crosshair.fire_name);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_get_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->crosshair.min_size);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_get_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->crosshair.max_size);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->crosshair.distance);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_weap_crosshair_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_set_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.type=JSVAL_TO_INT(*vp)-sd_weap_crosshair_type_center;
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->crosshair.fire_name,name_str_len);
	weapon_attach_fire_crosshair(weap);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_set_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.min_size=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_set_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.max_size=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_crosshair_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->crosshair.distance=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}



