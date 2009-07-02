/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.zoom object

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

JSBool js_get_weap_zoom_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_zoom_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

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
							{"maskName",			js_weap_zoom_get_maskMame,					js_weap_zoom_set_maskMame},
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

      Properties
      
======================================================= */

JSBool js_get_weap_zoom_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_zoom_prop_on:
			*vp=BOOLEAN_TO_JSVAL(weap->zoom.on);
			break;
		case weap_zoom_prop_active:
			*vp=BOOLEAN_TO_JSVAL(weap->zoom.mode!=zoom_mode_off);
			break;
		case weap_zoom_prop_fov_minimum:
			*vp=script_float_to_value(weap->zoom.fov_min);
			break;
		case weap_zoom_prop_fov_maximum:
			*vp=script_float_to_value(weap->zoom.fov_max);
			break;
		case weap_zoom_prop_fov_steps:
			*vp=INT_TO_JSVAL(weap->zoom.step_count);
			break;
		case weap_zoom_prop_turn_factor:
			*vp=script_float_to_value(weap->zoom.turn_factor);
			break;
		case weap_zoom_prop_crawl_turn_factor:
			*vp=script_float_to_value(weap->zoom.crawl_turn_factor);
			break;
		case weap_zoom_prop_look_factor:
			*vp=script_float_to_value(weap->zoom.look_factor);
			break;
		case weap_zoom_prop_mask_name:
			*vp=script_string_to_value(weap->zoom.mask_name);
			break;
		case weap_zoom_prop_show_weapon:
			*vp=BOOLEAN_TO_JSVAL(weap->zoom.show_weapon);
			break;
		case weap_zoom_prop_tick:
			*vp=INT_TO_JSVAL(weap->zoom.tick);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_zoom_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

        case weap_zoom_prop_on:
            weap->zoom.on=JSVAL_TO_BOOLEAN(*vp);
            break;
		case weap_zoom_prop_fov_minimum:
			weap->zoom.fov_min=script_value_to_float(*vp);
			break;
		case weap_zoom_prop_fov_maximum:
			weap->zoom.fov_max=script_value_to_float(*vp);
			break;
		case weap_zoom_prop_fov_steps:
			weap->zoom.step_count=JSVAL_TO_INT(*vp);
			break;
		case weap_zoom_prop_turn_factor:
			weap->zoom.turn_factor=script_value_to_float(*vp);
			break;
		case weap_zoom_prop_crawl_turn_factor:
			weap->zoom.crawl_turn_factor=script_value_to_float(*vp);
			break;
		case weap_zoom_prop_look_factor:
			weap->zoom.look_factor=script_value_to_float(*vp);
			break;
		case weap_zoom_prop_mask_name:
			script_value_to_string(*vp,weap->zoom.mask_name,name_str_len);
			weapon_attach_zoom_mask(weap);
			break;
        case weap_zoom_prop_show_weapon:
            weap->zoom.show_weapon=JSVAL_TO_BOOLEAN(*vp);
            break;
		case weap_zoom_prop_tick:
			weap->zoom.tick=JSVAL_TO_INT(*vp);
			break;
          
	}
	
	return(JS_TRUE);
}



