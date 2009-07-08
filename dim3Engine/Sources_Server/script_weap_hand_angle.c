/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.handAngle object

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

JSBool js_weap_hand_angle_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_angle_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_angle_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_angle_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_angle_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_angle_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			weap_hand_angle_class={"weap_hand_angle_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	weap_hand_angle_props[]={
							{"x",				js_weap_hand_angle_get_x,			js_weap_hand_angle_set_x},
							{"y",				js_weap_hand_angle_get_y,			js_weap_hand_angle_set_y},
							{"z",				js_weap_hand_angle_get_z,			js_weap_hand_angle_set_z},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_hand_angle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"handAngle",&weap_hand_angle_class,weap_hand_angle_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_weap_hand_angle_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.ang.x);
	
	return(JS_TRUE);
}

JSBool js_weap_hand_angle_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.ang.y);
	
	return(JS_TRUE);
}

JSBool js_weap_hand_angle_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.ang.z);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_weap_hand_angle_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.ang.x=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_hand_angle_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.ang.y=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_hand_angle_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.ang.z=script_value_to_float(*vp);
	
	return(JS_TRUE);
}


