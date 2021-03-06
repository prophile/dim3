/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.zoom object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

JSBool js_weap_zoom_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_active(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_fovMinimum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_fovMaximum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_fovSteps(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_turnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_crawlTurnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_lookFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_maskName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_showWeapon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_get_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_fovMinimum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_fovMaximum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_fovSteps(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_turnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_crawlTurnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_lookFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_maskName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_showWeapon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			weap_zoom_class={"weap_zoom_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	weap_zoom_props[]={
							{"on",					js_weap_zoom_get_on,						js_weap_zoom_set_on},
							{"active",				js_weap_zoom_get_active,					NULL},
							{"fovMinimum",			js_weap_zoom_get_fovMinimum,				js_weap_zoom_set_fovMinimum},
							{"fovMaximum",			js_weap_zoom_get_fovMaximum,				js_weap_zoom_set_fovMaximum},
							{"fovSteps",			js_weap_zoom_get_fovSteps,					js_weap_zoom_set_fovSteps},
							{"turnFactor",			js_weap_zoom_get_turnFactor,				js_weap_zoom_set_turnFactor},
							{"crawlTurnFactor",		js_weap_zoom_get_crawlTurnFactor,			js_weap_zoom_set_crawlTurnFactor},
							{"lookFactor",			js_weap_zoom_get_lookFactor,				js_weap_zoom_set_lookFactor},
							{"maskName",			js_weap_zoom_get_maskName,					js_weap_zoom_set_maskName},
							{"showWeapon",			js_weap_zoom_get_showWeapon,				js_weap_zoom_set_showWeapon},
							{"tick",				js_weap_zoom_get_tick,						js_weap_zoom_set_tick},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_zoom_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"zoom",&weap_zoom_class,weap_zoom_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_weap_zoom_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(weap->zoom.on);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_active(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(weap->zoom.mode!=zoom_mode_off);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_fovMinimum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.fov_min);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_fovMaximum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.fov_max);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_fovSteps(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->zoom.step_count);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_turnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.turn_factor);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_crawlTurnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.crawl_turn_factor);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_lookFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.look_factor);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_maskName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->zoom.mask_name);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_showWeapon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(weap->zoom.show_weapon);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_get_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->zoom.tick);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_weap_zoom_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_fovMinimum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.fov_min=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_fovMaximum(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.fov_max=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_fovSteps(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.step_count=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_turnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.turn_factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_crawlTurnFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.crawl_turn_factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_lookFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.look_factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_maskName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->zoom.mask_name,name_str_len);
	weapon_attach_zoom_mask(weap);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_showWeapon(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.show_weapon=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_zoom_set_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.tick=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}




