/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.rotate object

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

JSBool js_get_model_rotate_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_model_rotate_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			model_rotate_class={"model_rotate_class",0,
							script_add_property,JS_PropertyStub,
							js_get_model_rotate_property,js_set_model_rotate_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	model_rotate_props[]={
							{"x",					js_model_rotate_get_x,				js_model_rotate_set_x},
							{"y",					js_model_rotate_get_y,				js_model_rotate_set_y},
							{"z",					js_model_rotate_get_z,				js_model_rotate_set_z},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_rotate_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"rotate",&model_rotate_class,model_rotate_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_model_rotate_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_rotate_prop_x:
			*vp=script_float_to_value(draw->rot.x);
			break;
		case model_rotate_prop_z:
			*vp=script_float_to_value(draw->rot.z);
			break;
		case model_rotate_prop_y:
			*vp=script_float_to_value(draw->rot.y);
			break;
			
	}

	return(JS_TRUE);
}

JSBool js_set_model_rotate_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_rotate_prop_x:
			draw->rot.x=script_value_to_float(*vp);
			break;
		case model_rotate_prop_z:
			draw->rot.z=script_value_to_float(*vp);
			break;
		case model_rotate_prop_y:
			draw->rot.y=script_value_to_float(*vp);
			break;

	}

	return(JS_TRUE);
}


