/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.setting object

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

JSBool js_get_proj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_proj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			proj_setting_class={"proj_setting_class",0,
							script_add_property,JS_PropertyStub,
							js_get_proj_setting_property,js_set_proj_setting_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	proj_setting_props[]={
							{"name",				proj_setting_prop_name,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"hitScan",				proj_setting_prop_hitscan,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"resetAngle",			proj_setting_prop_reset_angle,		JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_setting_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"setting",&proj_setting_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,proj_setting_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_proj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case proj_setting_prop_name:
			*vp=script_string_to_value(proj_setup->name);
			break;
		case proj_setting_prop_hitscan:
			*vp=BOOLEAN_TO_JSVAL(proj_setup->hitscan.on);
			break;
		case proj_setting_prop_reset_angle:
			*vp=BOOLEAN_TO_JSVAL(proj_setup->reset_angle);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_proj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {

		case proj_setting_prop_hitscan:
			proj_setup->hitscan.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case proj_setting_prop_reset_angle:
			proj_setup->reset_angle=JSVAL_TO_BOOLEAN(*vp);
			break;
            
	}
	
	return(JS_TRUE);
}

