/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.ammo object

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

extern js_type				js;

JSBool js_get_weap_ammo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_weap_ammo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_use_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_ammo_add_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_ammo_change_clip_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_ammo_class={"weap_ammo_class",0,
							script_add_property,JS_PropertyStub,
							js_get_weap_ammo_property,js_set_weap_ammo_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	weap_ammo_props[]={
							{"ammo",				weap_ammo_prop_ammo,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"clip",				weap_ammo_prop_clip,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"count",				weap_ammo_prop_count,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"initCount",			weap_ammo_prop_init_count,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"maxCount",			weap_ammo_prop_max_count,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"clipCount",			weap_ammo_prop_clip_count,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"initClipCount",		weap_ammo_prop_init_clip_count,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"maxClipCount",		weap_ammo_prop_max_clip_count,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"lastReloadTick",		weap_ammo_prop_last_reload_tick,	JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	weap_ammo_functions[]={
							{"useAmmo",				js_weap_ammo_use_ammo_func,				1},
							{"addAmmo",				js_weap_ammo_add_ammo_func,				1},
							{"changeClip",			js_weap_ammo_change_clip_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_ammo_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"ammo",&weap_ammo_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,weap_ammo_props);
	JS_DefineFunctions(js.cx,j_obj,weap_ammo_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_ammo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_ammo_prop_ammo:
			*vp=BOOLEAN_TO_JSVAL(weap->ammo.use_ammo);
			break;
		case weap_ammo_prop_clip:
			*vp=BOOLEAN_TO_JSVAL(weap->ammo.use_clips);
			break;
		case weap_ammo_prop_count:
			if (weap->ammo.use_ammo) {
				*vp=INT_TO_JSVAL(weap->ammo.count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_init_count:
			if (weap->ammo.use_ammo) {
				*vp=INT_TO_JSVAL(weap->ammo.init_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_max_count:
			if (weap->ammo.use_ammo) {
				*vp=INT_TO_JSVAL(weap->ammo.max_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_clip_count:
			if ((weap->ammo.use_ammo) && (weap->ammo.use_clips)) {
				*vp=INT_TO_JSVAL(weap->ammo.clip_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_init_clip_count:
			if ((weap->ammo.use_ammo) && (weap->ammo.use_clips)) {
				*vp=INT_TO_JSVAL(weap->ammo.init_clip_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_max_clip_count:
			if ((weap->ammo.use_ammo) && (weap->ammo.use_clips)) {
				*vp=INT_TO_JSVAL(weap->ammo.max_clip_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_last_reload_tick:
			*vp=INT_TO_JSVAL(weap->ammo.last_reload_tick);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_ammo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_ammo_prop_ammo:
			weap->ammo.use_ammo=JSVAL_TO_BOOLEAN(*vp);
			break;
		case weap_ammo_prop_clip:
			weap->ammo.use_clips=JSVAL_TO_BOOLEAN(*vp);
			break;
		case weap_ammo_prop_count:
			weap->ammo.count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_init_count:
			weap->ammo.init_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_max_count:
			weap->ammo.max_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_clip_count:
			weap->ammo.clip_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_init_clip_count:
			weap->ammo.init_clip_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_max_clip_count:
			weap->ammo.max_clip_count=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Ammo Functions
      
======================================================= */

JSBool js_weap_ammo_use_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				count;
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
		// is there enough ammo?
			
	count=JSVAL_TO_INT(argv[0]);
	if (weap->ammo.count<count) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}
	
		// subtract the ammo
		
	weap->ammo.count-=count;
	
	*rval=JSVAL_TRUE;
	return(JS_TRUE);
}

JSBool js_weap_ammo_add_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
		// add ammo
		
    weap->ammo.count+=JSVAL_TO_INT(argv[0]);
	
		// over maximum
		
    if (weap->ammo.count>weap->ammo.max_count) {
		weap->ammo.count=weap->ammo.max_count;
		*rval=JSVAL_FALSE;
	}
	else {
		*rval=JSVAL_TRUE;
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_change_clip_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
   obj_type			*obj;
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
		// is this a clip based weapon and enough clips?
		
	if ((!weap->ammo.use_clips) || (weap->ammo.clip_count==0)) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}
	
		// change the clip
		
	weap->ammo.count=weap->ammo.max_count;
	weap->ammo.clip_count--;

	weap->ammo.last_reload_tick=js.time.current_tick;
	
		// alert object of clip change
		
	scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_clip_change,0);
	
	*rval=JSVAL_TRUE;
	return(JS_TRUE);
}


