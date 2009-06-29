/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.light object

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

JSBool js_get_model_light_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_model_light_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			model_light_class={"model_light_class",0,
							script_add_property,JS_PropertyStub,
							js_get_model_light_property,js_set_model_light_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	model_light_props[]={
							{"index",				model_light_prop_index,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"on",					model_light_prop_on,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"type",				model_light_prop_type,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"direction",			model_light_prop_direction,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"intensity",			model_light_prop_intensity,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"exponent",			model_light_prop_exponent,			JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_light_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"light",&model_light_class,model_light_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_model_light_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_light_prop_index:
			*vp=INT_TO_JSVAL(draw->script_light_idx);
			break;
		case model_light_prop_on:
			*vp=BOOLEAN_TO_JSVAL(light->on);
			break;
		case model_light_prop_type:
			*vp=INT_TO_JSVAL(light->type-sd_light_type_normal);
			break;
		case model_light_prop_direction:
			*vp=INT_TO_JSVAL(light->direction+sd_light_direction_all);
			break;
		case model_light_prop_intensity:
			*vp=INT_TO_JSVAL(light->intensity);
			break;
		case model_light_prop_exponent:
			*vp=script_float_to_value(light->exponent);
			break;
	}

	return(JS_TRUE);
}

JSBool js_set_model_light_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_light_prop_index:
			draw->script_light_idx=JSVAL_TO_INT(*vp);
			if ((draw->script_light_idx<0) || (draw->script_light_idx>=max_model_light)) draw->script_light_idx=0;
			break;
		case model_light_prop_on:
			light->on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_light_prop_type:
			light->type=JSVAL_TO_INT(*vp)-sd_light_type_normal;
			break;
		case model_light_prop_direction:
			light->direction=JSVAL_TO_INT(*vp)-sd_light_direction_all;
			break;
		case model_light_prop_intensity:
			light->intensity=JSVAL_TO_INT(*vp);
			break;
		case model_light_prop_exponent:
			light->exponent=script_value_to_float(*vp);
			break;
	}

	return(JS_TRUE);
}

