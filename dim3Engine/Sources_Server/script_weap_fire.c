/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.fire object

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
#include "weapons.h"

extern js_type			js;

JSBool js_get_weap_fire_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_fire_cancel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_fire_class={"weap_fire_class",0,
							script_add_property,JS_PropertyStub,
							js_get_weap_fire_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	weap_fire_props[]={
							{"method",				weap_fire_prop_method,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"lastFireTick",		weap_fire_prop_last_fire_tick,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	weap_fire_functions[]={
							{"cancel",				js_weap_fire_cancel_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_fire_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"fire",&weap_fire_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,weap_fire_props);
	JS_DefineFunctions(js.cx,j_obj,weap_fire_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_fire_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_fire_prop_method:
			*vp=INT_TO_JSVAL(weap->fire.method);
			break;
			
		case weap_fire_prop_last_fire_tick:
			*vp=INT_TO_JSVAL(weap->fire.last_fire_tick);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Weapon Fire Cancel
      
======================================================= */

JSBool js_weap_fire_cancel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->fire.cancel=TRUE;
    
	return(JS_TRUE);
}

