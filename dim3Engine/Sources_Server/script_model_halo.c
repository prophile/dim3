/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.halo object

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
#include "lights.h"

JSBool js_model_halo_get_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_minDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_maxDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_minAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_maxAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_noClipObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_get_noClipSelf(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_minDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_maxDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_minAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_maxAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_noClipObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_halo_set_noClipSelf(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern server_type		server;
extern js_type			js;

JSClass			model_halo_class={"model_halo_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	model_halo_props[]={
							{"index",				js_model_halo_get_index,				js_model_halo_set_index},
							{"on",					js_model_halo_get_on,					js_model_halo_set_on},
							{"name",				js_model_halo_get_name,					js_model_halo_set_name},
							{"minDistance",			js_model_halo_get_minDistance,			js_model_halo_set_minDistance},
							{"maxDistance",			js_model_halo_get_maxDistance,			js_model_halo_set_maxDistance},
							{"minSize",				js_model_halo_get_minSize,				js_model_halo_set_minSize},
							{"maxSize",				js_model_halo_get_maxSize,				js_model_halo_set_maxSize},
							{"minAlpha",			js_model_halo_get_minAlpha,				js_model_halo_set_minAlpha},
							{"maxAlpha",			js_model_halo_get_maxAlpha,				js_model_halo_set_maxAlpha},
							{"noClipObject",		js_model_halo_get_noClipObject,			js_model_halo_set_noClipObject},
							{"noClipSelf",			js_model_halo_get_noClipSelf,			js_model_halo_set_noClipSelf},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_halo_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"halo",&model_halo_class,model_halo_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_model_halo_get_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=INT_TO_JSVAL(draw->script_halo_idx);

	return(JS_TRUE);
}

JSBool js_model_halo_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=BOOLEAN_TO_JSVAL(halo->on);

	return(JS_TRUE);
}

JSBool js_model_halo_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	if (halo->idx==-1) {
		*vp=JSVAL_NULL;
	}
	else {
		*vp=script_string_to_value(server.halos[halo->idx].name);
	}

	return(JS_TRUE);
}

JSBool js_model_halo_get_minDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=INT_TO_JSVAL(halo->min_dist);

	return(JS_TRUE);
}

JSBool js_model_halo_get_maxDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=INT_TO_JSVAL(halo->max_dist);

	return(JS_TRUE);
}

JSBool js_model_halo_get_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=INT_TO_JSVAL(halo->min_size);

	return(JS_TRUE);
}

JSBool js_model_halo_get_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=INT_TO_JSVAL(halo->max_size);

	return(JS_TRUE);
}

JSBool js_model_halo_get_minAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
    *vp=script_float_to_value(halo->min_alpha);

	return(JS_TRUE);
}

JSBool js_model_halo_get_maxAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
    *vp=script_float_to_value(halo->max_alpha);

	return(JS_TRUE);
}

JSBool js_model_halo_get_noClipObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=BOOLEAN_TO_JSVAL(halo->no_clip_object);

	return(JS_TRUE);
}

JSBool js_model_halo_get_noClipSelf(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	*vp=BOOLEAN_TO_JSVAL(halo->no_clip_self);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_model_halo_set_index(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	draw->script_halo_idx=JSVAL_TO_INT(*vp);
	if ((draw->script_halo_idx<0) || (draw->script_halo_idx>=max_model_halo)) draw->script_halo_idx=0;

	return(JS_TRUE);
}

JSBool js_model_halo_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->on=JSVAL_TO_BOOLEAN(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				name[name_str_len];
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	script_value_to_string(*vp,name,name_str_len);
	halo->idx=halo_find(name);

	return(JS_TRUE);
}

JSBool js_model_halo_set_minDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->min_dist=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_maxDistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->max_dist=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_minSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->min_size=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_maxSize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->max_size=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_minAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->min_alpha=script_value_to_float(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_maxAlpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->max_alpha=script_value_to_float(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_noClipObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->no_clip_object=JSVAL_TO_BOOLEAN(*vp);

	return(JS_TRUE);
}

JSBool js_model_halo_set_noClipSelf(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];

	halo->no_clip_self=JSVAL_TO_BOOLEAN(*vp);

	return(JS_TRUE);
}

