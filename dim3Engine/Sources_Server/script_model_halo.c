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

JSBool js_get_model_halo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_model_halo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern server_type		server;
extern js_type			js;

JSClass			model_halo_class={"model_halo_class",0,
							script_add_property,JS_PropertyStub,
							js_get_model_halo_property,js_set_model_halo_property,
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

      Properties
      
======================================================= */

JSBool js_get_model_halo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_halo_prop_index:
			*vp=INT_TO_JSVAL(draw->script_halo_idx);
			break;
		case model_halo_prop_on:
			*vp=BOOLEAN_TO_JSVAL(halo->on);
			break;
		case model_halo_prop_name:
			if (halo->idx==-1) {
				*vp=JSVAL_NULL;
			}
			else {
				*vp=script_string_to_value(server.halos[halo->idx].name);
			}
			break;
		case model_halo_prop_min_distance:
			*vp=INT_TO_JSVAL(halo->min_dist);
			break;
		case model_halo_prop_max_distance:
			*vp=INT_TO_JSVAL(halo->max_dist);
			break;
		case model_halo_prop_min_size:
			*vp=INT_TO_JSVAL(halo->min_size);
			break;
		case model_halo_prop_max_size:
			*vp=INT_TO_JSVAL(halo->max_size);
			break;
		case model_halo_prop_min_alpha:
            *vp=script_float_to_value(halo->min_alpha);
			break;
		case model_halo_prop_max_alpha:
            *vp=script_float_to_value(halo->max_alpha);
			break;
		case model_halo_prop_no_clip_object:
			*vp=BOOLEAN_TO_JSVAL(halo->no_clip_object);
			break;
		case model_halo_prop_no_clip_self:
			*vp=BOOLEAN_TO_JSVAL(halo->no_clip_self);
			break;

	}

	return(JS_TRUE);
}

JSBool js_set_model_halo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				name[name_str_len];
	model_draw			*draw;
	model_draw_halo		*halo;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,TRUE);
	halo=&draw->halos[draw->script_halo_idx];
	
	switch (JSVAL_TO_INT(id)) {
	
		case model_halo_prop_index:
			draw->script_halo_idx=JSVAL_TO_INT(*vp);
			if ((draw->script_halo_idx<0) || (draw->script_halo_idx>=max_model_halo)) draw->script_halo_idx=0;
			break;
		case model_halo_prop_on:
			halo->on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_halo_prop_name:
			script_value_to_string(*vp,name,name_str_len);
			halo->idx=halo_find(name);
			break;
		case model_halo_prop_min_distance:
			halo->min_dist=JSVAL_TO_INT(*vp);
			break;
		case model_halo_prop_max_distance:
			halo->max_dist=JSVAL_TO_INT(*vp);
			break;
		case model_halo_prop_min_size:
			halo->min_size=JSVAL_TO_INT(*vp);
			break;
		case model_halo_prop_max_size:
			halo->max_size=JSVAL_TO_INT(*vp);
			break;
		case model_halo_prop_min_alpha:
			halo->min_alpha=script_value_to_float(*vp);
			break;
		case model_halo_prop_max_alpha:
			halo->max_alpha=script_value_to_float(*vp);
			break;
		case model_halo_prop_no_clip_object:
			halo->no_clip_object=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_halo_prop_no_clip_self:
			halo->no_clip_self=JSVAL_TO_BOOLEAN(*vp);
			break;
			
	}

	return(JS_TRUE);
}

