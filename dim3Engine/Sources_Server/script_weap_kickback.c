/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.kickback object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_kickback_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_kickback_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_kickback_kick_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_kickback_class={"weap_kickback_class",0,
							script_add_property,JS_PropertyStub,
                            JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};
                            
script_js_property	weap_kickback_props[]={
							{"size",				js_weap_kickback_get_size,				js_weap_kickback_set_size},
							{0}};
							
script_js_function	weap_kickback_functions[]={
							{"kick",				js_weap_kickback_kick_func,				0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_kickback_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"kickback",&weap_kickback_class,weap_kickback_props,weap_kickback_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_weap_kickback_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->kickback.size);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_weap_kickback_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->kickback.size=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Kickback Functions
      
======================================================= */

JSBool js_weap_kickback_kick_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	weapon_kickback(js.time.current_tick,obj,weap);

	return(JS_TRUE);
}

