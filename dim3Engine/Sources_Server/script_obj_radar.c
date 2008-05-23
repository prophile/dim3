/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.radar object

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

JSBool js_get_obj_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_radar_class={"obj_radar_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_radar_property,js_set_obj_radar_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_radar_props[]={
							{"on",					obj_radar_prop_on,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"icon",				obj_radar_prop_icon,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"motionOnly",			obj_radar_prop_motion_only,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"alwaysVisible",		obj_radar_prop_always_visible,	JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_radar_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"radar",&obj_radar_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_radar_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_radar_prop_on:
			*vp=BOOLEAN_TO_JSVAL(obj->radar.on);
			break;
	
		case obj_radar_prop_icon:
			*vp=script_string_to_value(obj->radar.icon);
			break;
			
		case obj_radar_prop_motion_only:
			*vp=BOOLEAN_TO_JSVAL(obj->radar.motion_only);
			break;

		case obj_radar_prop_always_visible:
			*vp=BOOLEAN_TO_JSVAL(obj->radar.always_visible);
			break;

	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_radar_prop_on:
			obj->radar.on=JSVAL_TO_BOOLEAN(*vp);
			break;
	
		case obj_radar_prop_icon:
			script_value_to_string(*vp,obj->radar.icon,name_str_len);
			if (!object_set_radar_icon(obj)) return(JS_FALSE);
			break;
			
		case obj_radar_prop_motion_only:
			obj->radar.motion_only=JSVAL_TO_BOOLEAN(*vp);
			break;

		case obj_radar_prop_always_visible:
			obj->radar.always_visible=JSVAL_TO_BOOLEAN(*vp);
			break;

	}
	
	return(JS_TRUE);
}

