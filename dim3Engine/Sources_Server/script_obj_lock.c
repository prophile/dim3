/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.lock object

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

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSBool js_get_obj_lock_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_lock_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_lock_class={"obj_lock_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_lock_property,js_set_obj_lock_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_lock_props[]={
							{"x",					obj_lock_prop_x,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"y",					obj_lock_prop_y,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"z",					obj_lock_prop_z,					JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_lock_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"lock",&obj_lock_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_lock_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_lock_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
    
		case obj_lock_prop_x:
            *vp=BOOLEAN_TO_JSVAL(obj->lock.x);
            break;
		case obj_lock_prop_y:
            *vp=BOOLEAN_TO_JSVAL(obj->lock.y);
            break;
		case obj_lock_prop_z:
            *vp=BOOLEAN_TO_JSVAL(obj->lock.z);
            break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_lock_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_lock_prop_x:
            obj->lock.x=JSVAL_TO_BOOLEAN(*vp);
            break;
		case obj_lock_prop_y:
            obj->lock.y=JSVAL_TO_BOOLEAN(*vp);
            break;
		case obj_lock_prop_z:
            obj->lock.z=JSVAL_TO_BOOLEAN(*vp);
            break;

	}
	
	return(JS_TRUE);
}

