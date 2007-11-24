/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.objectSpeed object

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

JSBool js_get_obj_object_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_object_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_object_speed_class={"obj_object_speed_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_object_speed_property,js_set_obj_object_speed_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_object_speed_props[]={
							{"jumpHeight",				obj_object_speed_prop_jump_height,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"duckAdd",					obj_object_speed_prop_duck_add,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"duckChange",				obj_object_speed_prop_duck_change,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"bumpHeight",				obj_object_speed_prop_bump_height,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"bumpSmoothFactor",		obj_object_speed_prop_bump_smooth_factor,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"bounceFactor",			obj_object_speed_prop_bounce_factor,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"noSlide",					obj_object_speed_prop_no_slide,				JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_object_speed_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"objectSpeed",&obj_object_speed_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_object_speed_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_object_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
    
		case obj_object_speed_prop_jump_height:
			*vp=INT_TO_JSVAL(obj->jump.y_add);
            break;
		case obj_object_speed_prop_duck_add:
			*vp=INT_TO_JSVAL(obj->duck.y_add);
            break;
		case obj_object_speed_prop_duck_change:
			*vp=INT_TO_JSVAL(obj->duck.y_change);
            break;
		case obj_object_speed_prop_bump_height:
			*vp=INT_TO_JSVAL(obj->bump.high);
            break;
		case obj_object_speed_prop_bump_smooth_factor:
			*vp=script_float_to_value(obj->bump.smooth_factor);
            break;
		case obj_object_speed_prop_bounce_factor:
			*vp=script_float_to_value(obj->bounce.factor);
            break;
		case obj_object_speed_prop_no_slide:
			*vp=BOOLEAN_TO_JSVAL(obj->no_slide);
            break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_object_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_object_speed_prop_jump_height:
			obj->jump.y_add=JSVAL_TO_INT(*vp);
            break;
		case obj_object_speed_prop_duck_add:
			obj->duck.y_add=JSVAL_TO_INT(*vp);
            break;
		case obj_object_speed_prop_duck_change:
			obj->duck.y_change=JSVAL_TO_INT(*vp);
            break;
		case obj_object_speed_prop_bump_height:
			obj->bump.high=JSVAL_TO_INT(*vp);
            break;
		case obj_object_speed_prop_bump_smooth_factor:
			obj->bump.smooth_factor=script_value_to_float(*vp);
            break;
		case obj_object_speed_prop_bounce_factor:
			obj->bounce.factor=script_value_to_float(*vp);
            break;
		case obj_object_speed_prop_no_slide:
			obj->no_slide=JSVAL_TO_BOOLEAN(*vp);
            break;

	}
	
	return(JS_TRUE);
}

