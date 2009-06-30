/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.click object

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
#include "objects.h"

extern server_type		server;
extern js_type			js;

JSBool js_get_obj_click_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_click_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_click_class={"obj_click_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_click_property,js_set_obj_click_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_click_props[]={
							{"crosshairUp",			obj_click_prop_crosshair_up,		FALSE},
							{"crosshairDown",		obj_click_prop_crosshair_down,		FALSE},
							{"objectId",			obj_click_prop_object_id,			TRUE},
							{"objectName",			obj_click_prop_object_name,			TRUE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_click_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"click",&obj_click_class,obj_click_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_click_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj,*click_obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_click_prop_crosshair_up:
			*vp=script_string_to_value(obj->click.crosshair_up_name);
			break;
		case obj_click_prop_crosshair_down:
			*vp=script_string_to_value(obj->click.crosshair_down_name);
			break;
		case obj_click_prop_object_id:
			*vp=INT_TO_JSVAL(obj->click.current_click_obj_uid);
			break;
		case obj_click_prop_object_name:
			if (obj->click.current_click_obj_uid==-1) {
				*vp=JSVAL_NULL;
				break;
			}
			click_obj=object_find_uid(obj->click.current_click_obj_uid);
			*vp=script_string_to_value(click_obj->name);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_click_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_click_prop_crosshair_up:
			script_value_to_string(*vp,obj->click.crosshair_up_name,name_str_len);
			object_attach_click_crosshair_up(obj);
			break;
		case obj_click_prop_crosshair_down:
			script_value_to_string(*vp,obj->click.crosshair_down_name,name_str_len);
			object_attach_click_crosshair_down(obj);
			break;
            
	}
	
	return(JS_TRUE);
}



