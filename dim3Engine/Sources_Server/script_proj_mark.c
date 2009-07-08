/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.mark object

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
#include "projectiles.h"

extern js_type			js;

JSBool js_proj_mark_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			proj_mark_class={"proj_mark_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	proj_mark_props[]={
							{"on",					js_proj_mark_get_on,				js_proj_mark_set_on},
							{"name",				js_proj_mark_get_name,				js_proj_mark_set_name},
							{"size",				js_proj_mark_get_size,				js_proj_mark_set_size},
							{"alpha",				js_proj_mark_get_alpha,				js_proj_mark_set_alpha},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_mark_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"mark",&proj_mark_class,proj_mark_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_proj_mark_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=BOOLEAN_TO_JSVAL(proj_setup->mark.on);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_string_to_value(proj_setup->mark.name);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=INT_TO_JSVAL(proj_setup->mark.size);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
    *vp=script_float_to_value(proj_setup->mark.alpha);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_proj_mark_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
    proj_setup->mark.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	script_value_to_string(*vp,proj_setup->mark.name,name_str_len);
	proj_setup_attach_mark(proj_setup);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->mark.size=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->mark.alpha=script_value_to_float(*vp);
	
	return(JS_TRUE);
}





