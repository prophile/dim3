/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.size object

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

JSBool js_get_proj_size_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_proj_size_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			proj_size_class={"proj_size_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	proj_size_props[]={
							{"x",					js_proj_size_get_x,					js_proj_size_set_x},
							{"y",					js_proj_size_get_y,					js_proj_size_set_y},
							{"z",					js_proj_size_get_z,					js_proj_size_set_z},
							{"weight",				js_proj_size_get_weight,			js_proj_size_set_weight},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_size_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"size",&proj_size_class,proj_size_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_proj_size_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_size_prop_x:
			*vp=INT_TO_JSVAL(proj_setup->size.x);
			break;
		case obj_size_prop_z:
			*vp=INT_TO_JSVAL(proj_setup->size.z);
			break;
		case obj_size_prop_y:
			*vp=INT_TO_JSVAL(proj_setup->size.y);
			break;
		case obj_size_prop_weight:
			*vp=INT_TO_JSVAL(proj_setup->size.weight);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_proj_size_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case proj_size_prop_x:
			proj_setup->size.x=JSVAL_TO_INT(*vp);
			proj_setup_set_radius(proj_setup);
			break;
		case proj_size_prop_z:
			proj_setup->size.z=JSVAL_TO_INT(*vp);
			proj_setup_set_radius(proj_setup);
			break;
		case proj_size_prop_y:
			proj_setup->size.y=JSVAL_TO_INT(*vp);
			break;
		case proj_size_prop_weight:
			proj_setup->size.weight=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}


