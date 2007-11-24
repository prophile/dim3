/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.vertSpeed object

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

extern js_type			js;

JSBool js_get_obj_vert_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_vert_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_vert_speed_class={"obj_vert_speed_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_vert_speed_property,js_set_obj_vert_speed_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_vert_speed_props[]={
							{"normal",					obj_vert_speed_prop_normal,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"acceleration",			obj_vert_speed_prop_acceleration,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"deceleration",			obj_vert_speed_prop_deceleration,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"flySlop",					obj_vert_speed_prop_fly_slop,			JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_vert_speed_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"verticalSpeed",&obj_vert_speed_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_vert_speed_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_vert_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
    
		case obj_vert_speed_prop_normal:
            *vp=script_float_to_value(obj->vert_move.max_walk_speed);
            break;
		case obj_vert_speed_prop_acceleration:
            *vp=script_float_to_value(obj->vert_move.accelerate);
            break;
		case obj_vert_speed_prop_deceleration:
            *vp=script_float_to_value(obj->vert_move.decelerate);
            break;
		case obj_vert_speed_prop_fly_slop:
            *vp=script_float_to_value(obj->vert_move.slop);
            break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_vert_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_vert_speed_prop_normal:
            obj->vert_move.max_walk_speed=fabsf(script_value_to_float(*vp));
            break;
		case obj_vert_speed_prop_acceleration:
            obj->vert_move.accelerate=fabsf(script_value_to_float(*vp));
            break;
		case obj_vert_speed_prop_deceleration:
            obj->vert_move.decelerate=fabsf(script_value_to_float(*vp));
            break;
		case obj_vert_speed_prop_fly_slop:
            obj->vert_move.slop=fabsf(script_value_to_float(*vp));
            break;

	}
	
	return(JS_TRUE);
}

