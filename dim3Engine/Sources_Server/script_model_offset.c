/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.offset object

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

JSBool js_model_offset_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_offset_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_offset_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_offset_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_offset_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_offset_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			model_offset_class={"model_offset_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	model_offset_props[]={
							{"x",					js_model_offset_get_x,				js_model_offset_set_x},
							{"y",					js_model_offset_get_y,				js_model_offset_set_y},
							{"z",					js_model_offset_get_z,				js_model_offset_set_z},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_offset_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"offset",&model_offset_class,model_offset_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_model_offset_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=js_find_model_draw(j_obj,TRUE);
	*vp=INT_TO_JSVAL(draw->offset.x);

	return(JS_TRUE);
}

JSBool js_model_offset_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=js_find_model_draw(j_obj,TRUE);
	*vp=INT_TO_JSVAL(draw->offset.y);

	return(JS_TRUE);
}

JSBool js_model_offset_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=js_find_model_draw(j_obj,TRUE);
	*vp=INT_TO_JSVAL(draw->offset.z);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_model_offset_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);
	draw->offset.x=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

JSBool js_model_offset_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);
	draw->offset.y=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

JSBool js_model_offset_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);
	draw->offset.z=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}


