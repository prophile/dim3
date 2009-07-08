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

JSBool js_obj_rigid_body_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_maxDropY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_resetFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_smoothFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_maxAngleX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_resetFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_smoothFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_maxAngleZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_resetFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_get_smoothFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_maxDropY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_resetFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_smoothFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_maxAngleX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_resetFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_smoothFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_maxAngleZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_resetFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_rigid_body_set_smoothFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_rigid_body_class={"obj_rigid_body_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_rigid_body_props[]={
							{"on",					js_obj_rigid_body_get_on,				js_obj_rigid_body_set_on},
							{"maxDropY",			js_obj_rigid_body_get_maxDropY,			js_obj_rigid_body_set_maxDropY},
							{"resetFactorY",		js_obj_rigid_body_get_resetFactorY,		js_obj_rigid_body_set_resetFactorY},
							{"smoothFactorY",		js_obj_rigid_body_get_smoothFactorY,	js_obj_rigid_body_set_smoothFactorY},
							{"maxAngleX",			js_obj_rigid_body_get_maxAngleX,		js_obj_rigid_body_set_maxAngleX},
							{"resetFactorX",		js_obj_rigid_body_get_resetFactorX,		js_obj_rigid_body_set_resetFactorX},
							{"smoothFactorX",		js_obj_rigid_body_get_smoothFactorX,	js_obj_rigid_body_set_smoothFactorX},
							{"maxAngleZ",			js_obj_rigid_body_get_maxAngleZ,		js_obj_rigid_body_set_maxAngleZ},
							{"resetFactorZ",		js_obj_rigid_body_get_resetFactorZ,		js_obj_rigid_body_set_resetFactorZ},
							{"smoothFactorZ",		js_obj_rigid_body_get_smoothFactorZ,	js_obj_rigid_body_set_smoothFactorZ},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_rigid_body_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"rigidBody",&obj_rigid_body_class,obj_rigid_body_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_rigid_body_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->rigid_body.on);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_maxDropY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->rigid_body.max_drop_y);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_resetFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.reset_factor_y);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_smoothFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.smooth_factor_y);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_maxAngleX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.max_ang_x);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_resetFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.reset_factor_x);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_smoothFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.smooth_factor_x);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_maxAngleZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.max_ang_z);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_resetFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.reset_factor_z);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_get_smoothFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.smooth_factor_z);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_rigid_body_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_maxDropY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.max_drop_y=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_resetFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.reset_factor_y=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_smoothFactorY(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.smooth_factor_y=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_maxAngleX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.max_ang_x=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_resetFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.reset_factor_x=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_smoothFactorX(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.smooth_factor_x=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_maxAngleZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.max_ang_z=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_resetFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.reset_factor_z=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_rigid_body_set_smoothFactorZ(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.smooth_factor_z=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

