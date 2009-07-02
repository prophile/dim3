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

JSBool js_obj_click_get_crosshairUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_click_get_crosshairDown(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_click_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_click_get_objectName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_click_set_crosshairUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_click_set_crosshairDown(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_click_class={"obj_click_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_click_props[]={
							{"crosshairUp",			js_obj_click_get_crosshairUp,			js_obj_click_set_crosshairUp},
							{"crosshairDown",		js_obj_click_get_crosshairDown,			js_obj_click_set_crosshairDown},
							{"objectId",			js_obj_click_get_objectId,				NULL},
							{"objectName",			js_obj_click_get_objectName,			NULL},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_click_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"click",&obj_click_class,obj_click_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_click_get_crosshairUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->click.crosshair_up_name);
	
	return(JS_TRUE);
}

JSBool js_obj_click_get_crosshairDown(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->click.crosshair_down_name);
	
	return(JS_TRUE);
}

JSBool js_obj_click_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->click.current_click_obj_uid);
	
	return(JS_TRUE);
}

JSBool js_obj_click_get_objectName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj,*click_obj;

	obj=object_find_uid(js.attach.thing_uid);

	if (obj->click.current_click_obj_uid==-1) {
		*vp=JSVAL_NULL;
	}
	else {
		click_obj=object_find_uid(obj->click.current_click_obj_uid);
		*vp=script_string_to_value(click_obj->name);
	}

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_click_set_crosshairUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,obj->click.crosshair_up_name,name_str_len);
	object_attach_click_crosshair_up(obj);
	
	return(JS_TRUE);
}

JSBool js_obj_click_set_crosshairDown(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,obj->click.crosshair_down_name,name_str_len);
	object_attach_click_crosshair_down(obj);
	
	return(JS_TRUE);
}

