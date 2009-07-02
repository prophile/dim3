/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.hit object

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
#include "projectiles.h"

extern server_type		server;
extern js_type			js;

JSBool js_obj_hit_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_hit_get_objectName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_hit_get_objectIsPlayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_hit_get_weaponName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_hit_get_projectileName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_hit_get_hitBoxName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_hit_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			obj_hit_class={"obj_hit_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_hit_props[]={
							{"objectId",			js_obj_hit_get_objectId,			NULL},
							{"objectName",			js_obj_hit_get_objectName,			NULL},
							{"objectIsPlayer",		js_obj_hit_get_objectIsPlayer,		NULL},
							{"weaponName",			js_obj_hit_get_weaponName,			NULL},
							{"projectileName",		js_obj_hit_get_projectileName,		NULL},
							{"hitBoxName",			js_obj_hit_get_hitBoxName,			NULL},
							{"damage",				js_obj_hit_get_damage,				NULL},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_hit_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"hit",&obj_hit_class,obj_hit_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_hit_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->hit.obj_uid);

	return(JS_TRUE);
}

JSBool js_obj_hit_get_objectName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj,*hit_obj;

	obj=object_find_uid(js.attach.thing_uid);

	hit_obj=object_find_uid(obj->hit.obj_uid);
	if (hit_obj==NULL) {
		*vp=JSVAL_NULL;
	}
	else {
		*vp=script_string_to_value(hit_obj->name);
	}

	return(JS_TRUE);
}

JSBool js_obj_hit_get_objectIsPlayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->hit.obj_uid==server.player_obj_uid);
	
	return(JS_TRUE);
}

JSBool js_obj_hit_get_weaponName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;
	weapon_type			*hit_weap;

	obj=object_find_uid(js.attach.thing_uid);

	hit_weap=weapon_find_uid(obj->hit.weap_uid);
	if (hit_weap==NULL) {
		*vp=JSVAL_NULL;
	}
	else {
		*vp=script_string_to_value(hit_weap->name);
	}

	return(JS_TRUE);
}

JSBool js_obj_hit_get_projectileName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;
	proj_type			*hit_proj;
	proj_setup_type		*hit_proj_setup;

	obj=object_find_uid(js.attach.thing_uid);

	hit_proj=projectile_find_uid(obj->hit.proj_uid);
	if (hit_proj==NULL) {
		*vp=JSVAL_NULL;
	}
	else {
		hit_proj_setup=proj_setups_find_uid(hit_proj->proj_setup_uid);
		*vp=script_string_to_value(hit_proj_setup->name);
	}

	return(JS_TRUE);
}

JSBool js_obj_hit_get_hitBoxName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);

	if (obj->hit.hit_box_name[0]==0x0) {
		*vp=JSVAL_NULL;
	}
	else {
		*vp=script_string_to_value(obj->hit.hit_box_name);
	}

	return(JS_TRUE);
}

JSBool js_obj_hit_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->hit.damage);
	
	return(JS_TRUE);
}

