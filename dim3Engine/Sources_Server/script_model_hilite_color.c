/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.hiliteColor object

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

JSBool js_get_model_hilite_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_model_hilite_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			model_hilite_color_class={"model_hilite_color_class",0,
							script_add_property,JS_PropertyStub,
							js_get_model_hilite_color_property,js_set_model_hilite_color_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	model_hilite_color_props[]={
							{"red",					model_hilite_color_prop_red,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"green",				model_hilite_color_prop_green,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"blue",				model_hilite_color_prop_blue,		JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_hilite_color_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"hiliteColor",&model_hilite_color_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,model_hilite_color_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_model_hilite_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	d3col			*hilite;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	hilite=&draw->hilite;
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_hilite_color_prop_red:
            *vp=script_float_to_value(hilite->r);
			break;
		case model_hilite_color_prop_green:
            *vp=script_float_to_value(hilite->g);
			break;
		case model_hilite_color_prop_blue:
            *vp=script_float_to_value(hilite->b);
			break;
	}

	return(JS_TRUE);
}

JSBool js_set_model_hilite_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	d3col			*hilite;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	hilite=&draw->hilite;
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_hilite_color_prop_red:
			hilite->r=script_value_to_float(*vp);
			break;
		case model_hilite_color_prop_green:
			hilite->g=script_value_to_float(*vp);
			break;
		case model_hilite_color_prop_blue:
			hilite->b=script_value_to_float(*vp);
			break;

	}

	return(JS_TRUE);
}

