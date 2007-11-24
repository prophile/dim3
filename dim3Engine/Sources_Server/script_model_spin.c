/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.spin object

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

JSBool js_get_model_spin_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_model_spin_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

JSClass			model_spin_class={"model_spin_class",0,
							script_add_property,JS_PropertyStub,
							js_get_model_spin_property,js_set_model_spin_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	model_spin_props[]={
							{"x",					model_spin_prop_x,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"z",					model_spin_prop_z,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"y",					model_spin_prop_y,					JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_spin_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"spin",&model_spin_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,model_spin_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_model_spin_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_spin_prop_x:
			*vp=script_float_to_value(draw->spin.x);
			break;
		case model_spin_prop_z:
			*vp=script_float_to_value(draw->spin.z);
			break;
		case model_spin_prop_y:
			*vp=script_float_to_value(draw->spin.y);
			break;
			
	}

	return(JS_TRUE);
}

JSBool js_set_model_spin_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_spin_prop_x:
			draw->spin.x=script_value_to_float(*vp);
			draw->spin_tick=js.time.current_tick;
			break;
		case model_spin_prop_z:
			draw->spin.z=script_value_to_float(*vp);
			draw->spin_tick=js.time.current_tick;
			break;
		case model_spin_prop_y:
			draw->spin.y=script_value_to_float(*vp);
			draw->spin_tick=js.time.current_tick;
			break;

	}

	return(JS_TRUE);
}


