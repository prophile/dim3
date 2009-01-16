/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.status object

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
extern js_type			js;

JSBool js_get_obj_status_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_status_freeze_input_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_status_tint_view_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_status_class={"obj_status_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_status_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_status_props[]={
							{"speed",				obj_status_prop_speed,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"moving",				obj_status_prop_moving,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"running",				obj_status_prop_running,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"backward",			obj_status_prop_backward,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"sliding",				obj_status_prop_sliding,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"stand",				obj_status_prop_stand,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"air",					obj_status_prop_air,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"liquid",				obj_status_prop_liquid,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"standOnObjectId",		obj_status_prop_stand_on_object_id,	JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"standUnderObjectId",	obj_status_prop_stand_under_object_id,JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	obj_status_functions[]={
							{"freezeInput",			js_obj_status_freeze_input_func,	1},
							{"tintView",			js_obj_status_tint_view_func,		7},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_status_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"status",&obj_status_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_status_props);
	JS_DefineFunctions(js.cx,j_obj,obj_status_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_status_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_status_prop_speed:
            *vp=script_float_to_value(obj->forward_move.speed);
			break;
		case obj_status_prop_moving:
			*vp=BOOLEAN_TO_JSVAL(obj->forward_move.moving);
			break;
		case obj_status_prop_running:
			*vp=BOOLEAN_TO_JSVAL(obj->forward_move.running);
			break;
		case obj_status_prop_backward:
			*vp=BOOLEAN_TO_JSVAL(obj->forward_move.reverse);
			break;
		case obj_status_prop_sliding:
			*vp=BOOLEAN_TO_JSVAL(obj->side_move.moving);
			break;
		case obj_status_prop_stand:
			*vp=INT_TO_JSVAL(obj->duck.mode+sd_stand_standing);
			break;
		case obj_status_prop_air:
			*vp=INT_TO_JSVAL(obj->air_mode+sd_air_up);
			break;
		case obj_status_prop_liquid:
			*vp=INT_TO_JSVAL(obj->liquid_mode+sd_liquid_out);
			break;
		case obj_status_prop_stand_on_object_id:
			*vp=INT_TO_JSVAL(obj->stand_obj_uid);
			break;
		case obj_status_prop_stand_under_object_id:
			*vp=INT_TO_JSVAL(object_find_uid_by_stood_on_object_uid(obj->uid));
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Input Freeze Functions
      
======================================================= */

JSBool js_obj_status_freeze_input_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_input_freeze(obj,JSVAL_TO_BOOLEAN(argv[0]));

	return(JS_TRUE);
}

/* =======================================================

      Full Screen Effects
      
======================================================= */

JSBool js_obj_status_tint_view_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			r,g,b;
	obj_type		*obj;
	
	r=script_value_to_float(argv[0]);
	g=script_value_to_float(argv[1]);
	b=script_value_to_float(argv[2]);

	obj=object_find_uid(js.attach.thing_uid);
	object_fs_tint_start(js.time.current_tick,obj,r,g,b,script_value_to_float(argv[3]),JSVAL_TO_INT(argv[4]),JSVAL_TO_INT(argv[5]),JSVAL_TO_INT(argv[6]));
    *rval=JSVAL_TRUE;
    
	return(JS_TRUE);
}
