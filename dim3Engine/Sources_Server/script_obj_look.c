/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.look object

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

JSBool js_get_obj_look_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_look_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_look_set_look_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_look_set_look_at_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_look_class={"obj_look_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_look_property,js_set_obj_look_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_look_props[]={
							{"upAngle",					obj_look_prop_up_angle,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"downAngle",				obj_look_prop_down_angle,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"effectWeapons",			obj_look_prop_effect_weapons,			JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	obj_look_functions[]={
							{"setLook",					js_obj_look_set_look_func,				1},
							{"setLookAt",				js_obj_look_set_look_at_func,			1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_look_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"look",&obj_look_class,obj_look_props,obj_look_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_look_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
    
		case obj_look_prop_up_angle:
            *vp=script_float_to_value(obj->look.up_angle);
            break;
		case obj_look_prop_down_angle:
            *vp=script_float_to_value(obj->look.down_angle);
            break;
		case obj_look_prop_effect_weapons:
            *vp=BOOLEAN_TO_JSVAL(obj->look.effect_weapons);
            break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_look_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {

		case obj_look_prop_up_angle:
           obj->look.up_angle=fabsf(script_value_to_float(*vp));
            break;
		case obj_look_prop_down_angle:
            obj->look.down_angle=fabsf(script_value_to_float(*vp));
            break;
		case obj_look_prop_effect_weapons:
			obj->look.effect_weapons=JSVAL_TO_BOOLEAN(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Look Functions
      
======================================================= */

JSBool js_obj_look_set_look_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->view_ang.x=script_value_to_float(argv[0]);

	return(JS_TRUE);
}

JSBool js_obj_look_set_look_at_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				dist,y,look_y;
	float			ang;
	obj_type		*obj,*look_obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	look_obj=script_find_obj_from_uid_arg(argv[0]);
	if (look_obj==NULL) return(JS_FALSE);
	
		// no change if within object size
		
	y=obj->pnt.y-(obj->size.y>>1);
	look_y=look_obj->pnt.y-(look_obj->size.y>>1);
	
	if (abs(y-look_y)<(look_obj->size.y>>1)) {
		obj->view_ang.x=0.0f;
		return(JS_TRUE);
	}
	
		// angle to object
		
	dist=distance_2D_get(obj->pnt.x,obj->pnt.z,look_obj->pnt.x,look_obj->pnt.z);
	ang=angle_find(y,dist,look_y,0);
	
	if (ang>180.0f) ang-=360.0f;
	
	obj->view_ang.x=-ang;

	return(JS_TRUE);
}
