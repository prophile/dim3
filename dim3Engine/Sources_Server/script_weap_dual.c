/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.dual object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "weapons.h"

extern js_type				js;

JSBool js_get_weap_dual_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_dual_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_dual_switch_hand_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_dual_class={"weap_dual_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	weap_dual_props[]={
							{"on",					js_weap_dual_get_on,					js_weap_dual_set_on},
							{"active",				js_weap_dual_get_active,				js_weap_dual_set_active},
							{"handOffset",			js_weap_dual_get_handOffset,			js_weap_dual_set_handOffset},
							{0}};
							
script_js_function	weap_dual_functions[]={
							{"switchHand",			js_weap_dual_switch_hand_func,		1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_dual_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"dual",&weap_dual_class,weap_dual_props,weap_dual_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_dual_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_dual_prop_on:
			*vp=BOOLEAN_TO_JSVAL(weap->dual.on);
			break;
		case weap_dual_prop_active:
			*vp=BOOLEAN_TO_JSVAL(weap->dual.active);
			break;
		case weap_dual_prop_hand_offset:
			*vp=INT_TO_JSVAL(weap->dual.hand_offset);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_dual_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_dual_prop_on:
			weap->dual.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case weap_dual_prop_active:
			weap->dual.active=JSVAL_TO_BOOLEAN(*vp);
			break;
		case weap_dual_prop_hand_offset:
			weap->dual.hand_offset=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Dual Functions
      
======================================================= */

JSBool js_weap_dual_switch_hand_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
	if ((!weap->dual.on) && (!weap->dual.active)) {
		weap->dual.in_dual=FALSE;
	}
	else {
		weap->dual.in_dual=JSVAL_TO_BOOLEAN(argv[0]);
	}
	
	return(JS_TRUE);
}

