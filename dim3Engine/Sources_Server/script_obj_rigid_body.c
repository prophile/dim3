/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.rigidBody object

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

JSBool js_get_obj_rigid_body_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_rigid_body_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_rigid_body_class={"obj_rigid_body_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_rigid_body_property,js_set_obj_rigid_body_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_rigid_body_props[]={
							{"on",					obj_rigid_body_prop_on,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"maxDropY",			obj_rigid_body_prop_max_drop_y,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"resetFactorY",		obj_rigid_body_prop_reset_factor_y,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"smoothFactorY",		obj_rigid_body_prop_smooth_factor_y,JSPROP_PERMANENT|JSPROP_SHARED},
							{"maxAngleX",			obj_rigid_body_prop_max_angle_x,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"resetFactorX",		obj_rigid_body_prop_reset_factor_x,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"smoothFactorX",		obj_rigid_body_prop_smooth_factor_x,JSPROP_PERMANENT|JSPROP_SHARED},
							{"maxAngleZ",			obj_rigid_body_prop_max_angle_z,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"resetFactorZ",		obj_rigid_body_prop_reset_factor_z,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"smoothFactorZ",		obj_rigid_body_prop_smooth_factor_z,JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_rigid_body_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"rigidBody",&obj_rigid_body_class,obj_rigid_body_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_rigid_body_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_rigid_body_prop_on:
			*vp=BOOLEAN_TO_JSVAL(obj->rigid_body.on);
			break;
		case obj_rigid_body_prop_max_drop_y:
			*vp=INT_TO_JSVAL(obj->rigid_body.max_drop_y);
			break;
		case obj_rigid_body_prop_reset_factor_y:
			*vp=script_float_to_value(obj->rigid_body.reset_factor_y);
			break;
		case obj_rigid_body_prop_smooth_factor_y:
			*vp=script_float_to_value(obj->rigid_body.smooth_factor_y);
			break;
		case obj_rigid_body_prop_max_angle_x:
			*vp=script_float_to_value(obj->rigid_body.max_ang_x);
			break;
		case obj_rigid_body_prop_reset_factor_x:
			*vp=script_float_to_value(obj->rigid_body.reset_factor_x);
			break;
		case obj_rigid_body_prop_smooth_factor_x:
			*vp=script_float_to_value(obj->rigid_body.smooth_factor_x);
			break;
		case obj_rigid_body_prop_max_angle_z:
			*vp=script_float_to_value(obj->rigid_body.max_ang_z);
			break;
		case obj_rigid_body_prop_reset_factor_z:
			*vp=script_float_to_value(obj->rigid_body.reset_factor_z);
			break;
		case obj_rigid_body_prop_smooth_factor_z:
			*vp=script_float_to_value(obj->rigid_body.smooth_factor_z);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_rigid_body_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_rigid_body_prop_on:
			obj->rigid_body.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_rigid_body_prop_max_drop_y:
			obj->rigid_body.max_drop_y=JSVAL_TO_INT(*vp);
			break;
		case obj_rigid_body_prop_reset_factor_y:
			obj->rigid_body.reset_factor_y=script_value_to_float(*vp);
			break;
		case obj_rigid_body_prop_smooth_factor_y:
			obj->rigid_body.smooth_factor_y=script_value_to_float(*vp);
			break;
		case obj_rigid_body_prop_max_angle_x:
			obj->rigid_body.max_ang_x=script_value_to_float(*vp);
			break;
		case obj_rigid_body_prop_reset_factor_x:
			obj->rigid_body.reset_factor_x=script_value_to_float(*vp);
			break;
		case obj_rigid_body_prop_smooth_factor_x:
			obj->rigid_body.smooth_factor_x=script_value_to_float(*vp);
			break;
		case obj_rigid_body_prop_max_angle_z:
			obj->rigid_body.max_ang_z=script_value_to_float(*vp);
			break;
		case obj_rigid_body_prop_reset_factor_z:
			obj->rigid_body.reset_factor_z=script_value_to_float(*vp);
			break;
		case obj_rigid_body_prop_smooth_factor_z:
			obj->rigid_body.smooth_factor_z=script_value_to_float(*vp);
			break;

	}
	
	return(JS_TRUE);
}


