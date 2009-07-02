/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.touch object

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

JSBool js_get_obj_touch_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_touch_class={"obj_touch_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_touch_props[]={
							{"objectId",			js_obj_touch_get_objectId,			NULL},
							{"objectName",			js_obj_touch_get_objectName,		NULL},
							{"objectIsPlayer",		js_obj_touch_get_objectIsPlayer,	NULL},
							{"stand",				js_obj_touch_get_stand,				NULL},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_touch_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"touch",&obj_touch_class,obj_touch_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_touch_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj,*touch_obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_touch_prop_object_id:
			*vp=INT_TO_JSVAL(obj->touch.obj_uid);
			break;

		case obj_touch_prop_object_name:
			touch_obj=object_find_uid(obj->touch.obj_uid);
			if (touch_obj==NULL) {
				*vp=JSVAL_NULL;
				break;
			}
			*vp=script_string_to_value(touch_obj->name);
			break;

		case obj_touch_prop_object_is_player:
			*vp=BOOLEAN_TO_JSVAL(obj->touch.obj_uid==server.player_obj_uid);
			break;

		case obj_touch_prop_stand:
			*vp=BOOLEAN_TO_JSVAL(obj->touch.stand);
			break;
			
	}
	
	return(JS_TRUE);
}

