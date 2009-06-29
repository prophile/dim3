/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.shadow object

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

JSBool js_get_model_shadow_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_model_shadow_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			model_shadow_class={"model_shadow_class",0,
							script_add_property,JS_PropertyStub,
							js_get_model_shadow_property,js_set_model_shadow_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	model_shadow_props[]={
							{"on",					model_shadow_prop_on,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"mode",				model_shadow_prop_mode,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"alwaysInAir",			model_shadow_prop_always_in_air,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"staticInAir",			model_shadow_prop_static_in_air,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"staticFaceAngle",		model_shadow_prop_static_face_angle,JSPROP_PERMANENT|JSPROP_SHARED},
							{"staticSize",			model_shadow_prop_static_size,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"staticAlpha",			model_shadow_prop_static_alpha,		JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_shadow_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"shadow",&model_shadow_class,model_shadow_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_model_shadow_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_shadow	*shadow;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	shadow=&draw->shadow;
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_shadow_prop_on:
			*vp=BOOLEAN_TO_JSVAL(shadow->on);
			break;
		case model_shadow_prop_mode:
			*vp=INT_TO_JSVAL(shadow->mode+sd_model_shadow_mode_normal);
			break;
		case model_shadow_prop_always_in_air:
			*vp=BOOLEAN_TO_JSVAL(shadow->always_in_air);
			break;
		case model_shadow_prop_static_in_air:
			*vp=BOOLEAN_TO_JSVAL(shadow->static_in_air);
			break;
		case model_shadow_prop_static_face_angle:
			*vp=script_float_to_value(shadow->static_face_angle);
			break;
		case model_shadow_prop_static_size:
			*vp=INT_TO_JSVAL(shadow->static_size);
			break;
		case model_shadow_prop_static_alpha:
			*vp=script_float_to_value(shadow->static_alpha);
			break;
			
	}

	return(JS_TRUE);
}

JSBool js_set_model_shadow_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_shadow	*shadow;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	shadow=&draw->shadow;
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_shadow_prop_on:
			shadow->on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_shadow_prop_mode:
			shadow->mode=JSVAL_TO_INT(*vp)-sd_model_shadow_mode_normal;
			break;
		case model_shadow_prop_always_in_air:
			shadow->always_in_air=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_shadow_prop_static_in_air:
			shadow->static_in_air=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_shadow_prop_static_face_angle:
			shadow->static_face_angle=script_value_to_float(*vp);
			break;
		case model_shadow_prop_static_size:
			shadow->static_size=JSVAL_TO_INT(*vp);
			break;
		case model_shadow_prop_static_alpha:
			shadow->static_alpha=script_value_to_float(*vp);
			break;
			
	}

	return(JS_TRUE);
}

