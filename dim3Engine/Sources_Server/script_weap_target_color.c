/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.target.color object

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

JSBool js_get_weap_target_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_target_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			weap_target_color_class={"weap_target_color_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	weap_target_color_props[]={
							{"red",					js_weap_target_color_get_red,			js_weap_target_color_set_red},
							{"green",				js_weap_target_color_get_green,			js_weap_target_color_set_green},
							{"blue",				js_weap_target_color_get_blue,			js_weap_target_color_set_blue},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_target_color_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"color",&weap_target_color_class,weap_target_color_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_target_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_target_color_prop_red:
            *vp=script_float_to_value(weap->target.col.r);
			break;
		case weap_target_color_prop_green:
            *vp=script_float_to_value(weap->target.col.g);
			break;
		case weap_target_color_prop_blue:
            *vp=script_float_to_value(weap->target.col.b);
			break;
	}

	return(JS_TRUE);
}

JSBool js_set_weap_target_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_target_color_prop_red:
			weap->target.col.r=script_value_to_float(*vp);
			break;
		case weap_target_color_prop_green:
			weap->target.col.g=script_value_to_float(*vp);
			break;
		case weap_target_color_prop_blue:
			weap->target.col.b=script_value_to_float(*vp);
			break;

	}

	return(JS_TRUE);
}

