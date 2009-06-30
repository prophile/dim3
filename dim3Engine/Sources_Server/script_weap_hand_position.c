/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.handPosition object

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

JSBool js_get_weap_hand_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_hand_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			weap_hand_position_class={"weap_hand_position_class",0,
							script_add_property,JS_PropertyStub,
							js_get_weap_hand_position_property,js_set_weap_hand_position_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	weap_hand_position_props[]={
							{"x",						weap_hand_position_prop_x,						FALSE},
							{"y",						weap_hand_position_prop_y,						FALSE},
							{"z",						weap_hand_position_prop_z,						FALSE},
							{0}};
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_hand_position_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"handPosition",&weap_hand_position_class,weap_hand_position_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_hand_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_hand_position_prop_x:
			*vp=INT_TO_JSVAL(weap->hand.shift.x);
			break;
		case weap_hand_position_prop_y:
			*vp=INT_TO_JSVAL(weap->hand.shift.y);
			break;
		case weap_hand_position_prop_z:
			*vp=INT_TO_JSVAL(weap->hand.shift.z);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_hand_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_hand_position_prop_x:
			weap->hand.shift.x=JSVAL_TO_INT(*vp);
			break;
		case weap_hand_position_prop_y:
			weap->hand.shift.y=JSVAL_TO_INT(*vp);
			break;
		case weap_hand_position_prop_z:
			weap->hand.shift.z=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}



