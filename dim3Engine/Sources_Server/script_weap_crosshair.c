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

JSBool js_get_weap_crosshair_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_crosshair_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			weap_crosshair_class={"weap_crosshair_class",0,
							script_add_property,JS_PropertyStub,
							js_get_weap_crosshair_property,js_set_weap_crosshair_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	weap_crosshair_props[]={
							{"on",					weap_crosshair_prop_on,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"type",				weap_crosshair_prop_type,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"name",				weap_crosshair_prop_name,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"minSize",				weap_crosshair_prop_min_size,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"maxSize",				weap_crosshair_prop_max_size,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"distance",			weap_crosshair_prop_distance,		JSPROP_PERMANENT|JSPROP_SHARED},
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

      Properties
      
======================================================= */

JSBool js_get_weap_crosshair_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_crosshair_prop_on:
			*vp=BOOLEAN_TO_JSVAL(weap->crosshair.on);
			break;
		case weap_crosshair_prop_type:
			*vp=INT_TO_JSVAL(weap->crosshair.type+sd_weap_crosshair_type_center);
			break;
		case weap_crosshair_prop_name:
			*vp=script_string_to_value(weap->crosshair.fire_name);
			break;
		case weap_crosshair_prop_min_size:
			*vp=INT_TO_JSVAL(weap->crosshair.min_size);
			break;
		case weap_crosshair_prop_max_size:
			*vp=INT_TO_JSVAL(weap->crosshair.max_size);
			break;
		case weap_crosshair_prop_distance:
			*vp=INT_TO_JSVAL(weap->crosshair.distance);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_crosshair_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

        case weap_crosshair_prop_on:
            weap->crosshair.on=JSVAL_TO_BOOLEAN(*vp);
            break;
        case weap_crosshair_prop_type:
			weap->crosshair.type=JSVAL_TO_INT(*vp)-sd_weap_crosshair_type_center;
            break;
 		case weap_crosshair_prop_name:
			script_value_to_string(*vp,weap->crosshair.fire_name,name_str_len);
			weapon_attach_fire_crosshair(weap);
			break;
		case weap_crosshair_prop_min_size:
			weap->crosshair.min_size=JSVAL_TO_INT(*vp);
            break;
 		case weap_crosshair_prop_max_size:
			weap->crosshair.max_size=JSVAL_TO_INT(*vp);
            break;
		case weap_crosshair_prop_distance:
			weap->crosshair.distance=JSVAL_TO_INT(*vp);
            break;
         
	}
	
	return(JS_TRUE);
}



