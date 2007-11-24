/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.crosshair.pickupColor object

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

JSBool js_get_weap_crosshair_pickup_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_crosshair_pickup_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			weap_crosshair_pickup_color_class={"weap_crosshair_pickup_color_class",0,
							script_add_property,JS_PropertyStub,
							js_get_weap_crosshair_pickup_color_property,js_set_weap_crosshair_pickup_color_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	weap_crosshair_pickup_color_props[]={
							{"red",					weap_crosshair_pickup_color_prop_red,   JSPROP_PERMANENT|JSPROP_SHARED},
							{"green",				weap_crosshair_pickup_color_prop_green,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"blue",				weap_crosshair_pickup_color_prop_blue,  JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_crosshair_pickup_color_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"pickupColor",&weap_crosshair_pickup_color_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,weap_crosshair_pickup_color_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_crosshair_pickup_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_crosshair_pickup_color_prop_red:
            *vp=script_float_to_value(weap->crosshair.pickup_col.r);
			break;
		case weap_crosshair_pickup_color_prop_green:
            *vp=script_float_to_value(weap->crosshair.pickup_col.g);
			break;
		case weap_crosshair_pickup_color_prop_blue:
            *vp=script_float_to_value(weap->crosshair.pickup_col.b);
			break;
	}

	return(JS_TRUE);
}

JSBool js_set_weap_crosshair_pickup_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_crosshair_pickup_color_prop_red:
			weap->crosshair.pickup_col.r=script_value_to_float(*vp);
			break;
		case weap_crosshair_pickup_color_prop_green:
			weap->crosshair.pickup_col.g=script_value_to_float(*vp);
			break;
		case weap_crosshair_pickup_color_prop_blue:
			weap->crosshair.pickup_col.b=script_value_to_float(*vp);
			break;

	}

	return(JS_TRUE);
}

