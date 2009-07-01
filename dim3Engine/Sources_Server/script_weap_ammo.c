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

script_js_property	weap_ammo_props[]={
							{"ammo",				js_weap_ammo_get_ammo,					js_weap_ammo_set_ammo},
							{"clip",				js_weap_ammo_get_clip,					js_weap_ammo_set_clip},
							{"count",				js_weap_ammo_get_count,					js_weap_ammo_set_count},
							{"initCount",			js_weap_ammo_get_initCount,				js_weap_ammo_set_initCount},
							{"maxCount",			js_weap_ammo_get_maxCount,				js_weap_ammo_set_maxCount},
							{"clipCount",			js_weap_ammo_get_clipCount,				js_weap_ammo_set_clipCount},
							{"initClipCount",		js_weap_ammo_get_initClipCount,			js_weap_ammo_set_initClipCount},
							{"maxClipCount",		js_weap_ammo_get_maxClipCount,			js_weap_ammo_set_maxClipCount},
							{"lastReloadTick",		js_weap_ammo_get_lastReloadTick,		NULL},
							{0}};
							
script_js_function	weap_ammo_functions[]={
							{"useAmmo",				js_weap_ammo_use_ammo_func,				1},
							{"addAmmo",				js_weap_ammo_add_ammo_func,				1},
							{"changeClip",			js_weap_ammo_change_clip_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_weap_ammo_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"ammo",&weap_ammo_class,weap_ammo_props,weap_ammo_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_weap_ammo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_ammo_prop_ammo:
			*vp=BOOLEAN_TO_JSVAL(ammo->use_ammo);
			break;
		case weap_ammo_prop_clip:
			*vp=BOOLEAN_TO_JSVAL(ammo->use_clips);
			break;
		case weap_ammo_prop_count:
			if (ammo->use_ammo) {
				if (weap->dual.in_dual) {
					*vp=INT_TO_JSVAL(ammo->count_dual);
				}
				else {
					*vp=INT_TO_JSVAL(ammo->count);
				}
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_init_count:
			if (ammo->use_ammo) {
				*vp=INT_TO_JSVAL(ammo->init_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_max_count:
			if (ammo->use_ammo) {
				*vp=INT_TO_JSVAL(ammo->max_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_clip_count:
			if ((ammo->use_ammo) && (ammo->use_clips)) {
				*vp=INT_TO_JSVAL(ammo->clip_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_init_clip_count:
			if ((ammo->use_ammo) && (ammo->use_clips)) {
				*vp=INT_TO_JSVAL(ammo->init_clip_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_max_clip_count:
			if ((ammo->use_ammo) && (ammo->use_clips)) {
				*vp=INT_TO_JSVAL(ammo->max_clip_count);
			}
			else {
				*vp=INT_TO_JSVAL(0);
			}
			break;
		case weap_ammo_prop_last_reload_tick:
			if (weap->dual.in_dual) {
				*vp=INT_TO_JSVAL(ammo->last_reload_dual_tick);
			}
			else {
				*vp=INT_TO_JSVAL(ammo->last_reload_tick);
			}
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_weap_ammo_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	switch (JSVAL_TO_INT(id)) {
	
		case weap_ammo_prop_ammo:
			ammo->use_ammo=JSVAL_TO_BOOLEAN(*vp);
			break;
		case weap_ammo_prop_clip:
			ammo->use_clips=JSVAL_TO_BOOLEAN(*vp);
			break;
		case weap_ammo_prop_count:
			if (weap->dual.in_dual) {
				ammo->count_dual=JSVAL_TO_INT(*vp);
			}
			else {
				ammo->count=JSVAL_TO_INT(*vp);
			}
			break;
		case weap_ammo_prop_init_count:
			ammo->init_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_max_count:
			ammo->max_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_clip_count:
			ammo->clip_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_init_clip_count:
			ammo->init_clip_count=JSVAL_TO_INT(*vp);
			break;
		case weap_ammo_prop_max_clip_count:
			ammo->max_clip_count=JSVAL_TO_INT(*vp);
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
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	count=JSVAL_TO_INT(argv[0]);
	
	*rval=JSVAL_FALSE;
	
	if (weap->dual.in_dual) {
		if (ammo->count_dual>=count) {
			ammo->count_dual-=count;
			*rval=JSVAL_TRUE;
		}
	}
	else {
		if (ammo->count>=count) {
			ammo->count-=count;
			*rval=JSVAL_TRUE;
		}
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_add_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				add;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	add=JSVAL_TO_INT(argv[0]);
	
	*rval=JSVAL_TRUE;

	if (weap->dual.in_dual) {
		if (ammo->count_dual==ammo->max_count) {
			*rval=JSVAL_FALSE;
		}
		else {
			ammo->count_dual+=add;
			if (ammo->count_dual>ammo->max_count) ammo->count_dual=ammo->max_count;
		}
	}
	else {
		if (ammo->count==ammo->max_count) {
			*rval=JSVAL_FALSE;
		}
		else {
			ammo->count+=add;
			if (ammo->count>ammo->max_count) ammo->count=ammo->max_count;
		}
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_change_clip_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	obj=object_find_uid(weap->obj_uid);
	
		// is this a clip based weapon and enough clips?
		
	if ((!ammo->use_clips) || (ammo->clip_count==0)) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}
	
		// change the clip
		
	if (weap->dual.in_dual) {
		ammo->count_dual=ammo->max_count;
		ammo->last_reload_dual_tick=js.time.current_tick;
	}
	else {
		ammo->count=ammo->max_count;
		ammo->last_reload_tick=js.time.current_tick;
	}
	
	ammo->clip_count--;
	
		// alert object of clip change
		
	scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_clip_change,0);
	
	*rval=JSVAL_TRUE;
	return(JS_TRUE);
}


