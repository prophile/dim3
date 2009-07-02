/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.fog object

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

extern map_type			map;
extern js_type			js;

JSBool js_get_map_fog_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_map_fog_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			map_fog_class={"map_fog_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	map_fog_props[]={
							{"on",					js_map_fog_get_on,					js_map_fog_set_on},
							{"layerCount",			js_map_fog_get_count,				js_map_fog_set_count},
							{"outerRadius",			js_map_fog_get_outerRadius,			js_map_fog_set_outerRadius},
							{"innerRadius",			js_map_fog_get_innerRadius,			js_map_fog_set_innerRadius},
							{"height",				js_map_fog_get_high,				js_map_fog_set_high},
							{"drop",				js_map_fog_get_drop,				js_map_fog_set_drop},
							{"textureIndex",		js_map_fog_get_textureIndex,		js_map_fog_set_textureIndex},
							{"speed",				js_map_fog_get_speed,				js_map_fog_set_speed},
							{"textureXFact",		js_map_fog_get_textureXFact,		js_map_fog_set_textureXFact},
							{"textureYFact",		js_map_fog_get_textureYFact,		js_map_fog_set_textureYFact},
							{"alpha",				js_map_fog_get_alpha,				js_map_fog_set_alpha},
							{"useSolidColor",		js_map_fog_get_useSolidColor,		js_map_fog_set_useSolidColor},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_fog_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"fog",&map_fog_class,map_fog_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_map_fog_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case map_fog_prop_on:
            *vp=BOOLEAN_TO_JSVAL(map.fog.on);
			break;
		case map_fog_prop_count:
			*vp=INT_TO_JSVAL(map.fog.count);
			break;
		case map_fog_prop_outer_radius:
			*vp=INT_TO_JSVAL(map.fog.outer_radius);
			break;
		case map_fog_prop_inner_radius:
			*vp=INT_TO_JSVAL(map.fog.inner_radius);
			break;
		case map_fog_prop_high:
			*vp=INT_TO_JSVAL(map.fog.high);
			break;
		case map_fog_prop_drop:
			*vp=INT_TO_JSVAL(map.fog.drop);
			break;
		case map_fog_prop_texture_index:
			*vp=INT_TO_JSVAL(map.fog.texture_idx);
			break;
		case map_fog_prop_speed:
            *vp=script_float_to_value(map.fog.speed);
			break;
		case map_fog_prop_txt_x_fact:
            *vp=script_float_to_value(map.fog.txt_x_fact);
			break;
		case map_fog_prop_txt_y_fact:
            *vp=script_float_to_value(map.fog.txt_y_fact);
			break;
		case map_fog_prop_alpha:
            *vp=script_float_to_value(map.fog.alpha);
			break;
		case map_fog_prop_use_solid_color:
            *vp=BOOLEAN_TO_JSVAL(map.fog.use_solid_color);
			break;

	}
	
	return(JS_TRUE);
}

JSBool js_set_map_fog_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case map_fog_prop_on:
            map.fog.on=JSVAL_TO_BOOLEAN(*vp);
 			break;
		case map_fog_prop_count:
			map.fog.count=JSVAL_TO_INT(*vp);
			break;
		case map_fog_prop_outer_radius:
			map.fog.outer_radius=JSVAL_TO_INT(*vp);
			break;
		case map_fog_prop_inner_radius:
			map.fog.inner_radius=JSVAL_TO_INT(*vp);
			break;
		case map_fog_prop_high:
			map.fog.high=JSVAL_TO_INT(*vp);
			break;
		case map_fog_prop_drop:
			map.fog.drop=JSVAL_TO_INT(*vp);
			break;
		case map_fog_prop_texture_index:
			map.fog.texture_idx=JSVAL_TO_INT(*vp);
			break;
		case map_fog_prop_speed:
            map.fog.speed=script_value_to_float(*vp);
 			break;
		case map_fog_prop_txt_x_fact:
            map.fog.txt_x_fact=script_value_to_float(*vp);
 			break;
		case map_fog_prop_txt_y_fact:
            map.fog.txt_y_fact=script_value_to_float(*vp);
 			break;
		case map_fog_prop_alpha:
            map.fog.alpha=script_value_to_float(*vp);
 			break;

		case map_fog_prop_use_solid_color:
            map.fog.use_solid_color=JSVAL_TO_BOOLEAN(*vp);
 			break;

	}
	
	return(JS_TRUE);
}

