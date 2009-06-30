/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.lightColor object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "lights.h"

extern map_type			map;
extern js_type			js;

JSBool js_get_map_light_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_map_light_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			map_light_color_class={"map_light_color_class",0,
							script_add_property,JS_PropertyStub,
							js_get_map_light_color_property,js_set_map_light_color_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	map_light_color_props[]={
							{"red",					map_light_color_prop_red,				FALSE},
							{"green",				map_light_color_prop_green,				FALSE},
							{"blue",				map_light_color_prop_blue,				FALSE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_light_color_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"lightColor",&map_light_color_class,map_light_color_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_map_light_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case map_light_color_prop_red:
            *vp=script_float_to_value(map.ambient.light_color.r);
			break;
		case map_light_color_prop_green:
            *vp=script_float_to_value(map.ambient.light_color.g);
			break;
		case map_light_color_prop_blue:
            *vp=script_float_to_value(map.ambient.light_color.b);
			break;

	}
	
	return(JS_TRUE);
}

JSBool js_set_map_light_color_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case map_light_color_prop_red:
            map.ambient.light_color.r=script_value_to_float(*vp);
			break;
		case map_light_color_prop_green:
            map.ambient.light_color.g=script_value_to_float(*vp);
 			break;
		case map_light_color_prop_blue:
            map.ambient.light_color.b=script_value_to_float(*vp);
 			break;

	}
	
	return(JS_TRUE);
}

