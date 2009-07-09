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

JSBool js_map_fog_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_outerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_innerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_high(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_drop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_textureIndex(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_textureXFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_textureYFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_get_useSolidColor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_outerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_innerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_high(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_drop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_textureIndex(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_textureXFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_textureYFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_fog_set_useSolidColor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

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

      Getters
      
======================================================= */

JSBool js_map_fog_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=BOOLEAN_TO_JSVAL(map.fog.on);
	return(JS_TRUE);
}

JSBool js_map_fog_get_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(map.fog.count);
	return(JS_TRUE);
}

JSBool js_map_fog_get_outerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(map.fog.outer_radius);
	return(JS_TRUE);
}

JSBool js_map_fog_get_innerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(map.fog.inner_radius);
	return(JS_TRUE);
}

JSBool js_map_fog_get_high(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(map.fog.high);
	return(JS_TRUE);
}

JSBool js_map_fog_get_drop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(map.fog.drop);
	return(JS_TRUE);
}

JSBool js_map_fog_get_textureIndex(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(map.fog.texture_idx);
	return(JS_TRUE);
}

JSBool js_map_fog_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(map.fog.speed);
	return(JS_TRUE);
}

JSBool js_map_fog_get_textureXFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(map.fog.txt_x_fact);
	return(JS_TRUE);
}

JSBool js_map_fog_get_textureYFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(map.fog.txt_y_fact);
	return(JS_TRUE);
}

JSBool js_map_fog_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(map.fog.alpha);
	return(JS_TRUE);
}

JSBool js_map_fog_get_useSolidColor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=BOOLEAN_TO_JSVAL(map.fog.use_solid_color);
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_map_fog_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.on=JSVAL_TO_BOOLEAN(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.count=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_outerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.outer_radius=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_innerRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.inner_radius=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_high(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.high=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_drop(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.drop=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_textureIndex(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.texture_idx=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.speed=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_textureXFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.txt_x_fact=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_textureYFact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.txt_y_fact=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.alpha=script_value_to_float(*vp);
	return(JS_TRUE);
}

JSBool js_map_fog_set_useSolidColor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.fog.use_solid_color=JSVAL_TO_BOOLEAN(*vp);
	return(JS_TRUE);
}


