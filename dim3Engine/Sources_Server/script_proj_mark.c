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

JSBool js_get_proj_mark_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_proj_mark_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			proj_mark_class={"proj_mark_class",0,
							script_add_property,JS_PropertyStub,
							js_get_proj_mark_property,js_set_proj_mark_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	proj_mark_props[]={
							{"on",					proj_mark_prop_on,					FALSE},
							{"name",				proj_mark_prop_name,				FALSE},
							{"size",				proj_mark_prop_size,				FALSE},
							{"alpha",				proj_mark_prop_alpha,				FALSE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_mark_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"mark",&proj_mark_class,proj_mark_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_proj_mark_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case proj_mark_prop_on:
			*vp=BOOLEAN_TO_JSVAL(proj_setup->mark.on);
			break;
		case proj_mark_prop_name:
			*vp=script_string_to_value(proj_setup->mark.name);
			break;
		case proj_mark_prop_size:
			*vp=INT_TO_JSVAL(proj_setup->mark.size);
			break;
		case proj_mark_prop_alpha:
            *vp=script_float_to_value(proj_setup->mark.alpha);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_proj_mark_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {

        case proj_mark_prop_on:
            proj_setup->mark.on=JSVAL_TO_BOOLEAN(*vp);
            break;
		case proj_mark_prop_name:
			script_value_to_string(*vp,proj_setup->mark.name,name_str_len);
			proj_setup_attach_mark(proj_setup);
			break;
		case proj_mark_prop_size:
			proj_setup->mark.size=JSVAL_TO_INT(*vp);
			break;
		case proj_mark_prop_alpha:
			proj_setup->mark.alpha=script_value_to_float(*vp);
			break;
            
	}
	
	return(JS_TRUE);
}



