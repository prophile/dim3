/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.lightColor object

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

JSBool js_model_light_color_get_red(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_color_get_green(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_color_get_blue(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_color_set_red(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_color_set_green(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_color_set_blue(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			model_light_color_class={"model_light_color_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	model_light_color_props[]={
							{"red",					js_model_light_color_get_red,			js_model_light_color_set_red},
							{"green",				js_model_light_color_get_green,			js_model_light_color_set_green},
							{"blue",				js_model_light_color_get_blue,			js_model_light_color_set_blue},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_light_color_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"lightColor",&model_light_color_class,model_light_color_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_model_light_color_get_red(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_float_to_value(light->col.r);

	return(JS_TRUE);
}

JSBool js_model_light_color_get_green(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];

	*vp=script_float_to_value(light->col.g);

	return(JS_TRUE);
}

JSBool js_model_light_color_get_blue(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_float_to_value(light->col.b);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_model_light_color_set_red(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];

	light->col.r=script_value_to_float(*vp);

	return(JS_TRUE);
}

JSBool js_model_light_color_set_green(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];

	light->col.g=script_value_to_float(*vp);

	return(JS_TRUE);
}

JSBool js_model_light_color_set_blue(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=js_find_model_draw(j_obj,TRUE);
	light=&draw->lights[draw->script_light_idx];

	light->col.b=script_value_to_float(*vp);

	return(JS_TRUE);
}

