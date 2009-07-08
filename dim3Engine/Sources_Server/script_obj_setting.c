/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.setting object

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

JSBool js_obj_setting_get_id(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_team(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_hidden(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_suspend(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_fly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_find(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_contact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_contactObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_contactProjectile(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_contactForce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_hitBox(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_crushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_invincible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_clickable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_pickup(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_ignorePickUpItems(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_ignoreMouse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_turnOnlyWhenMoving(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_restrictPlayerTurning(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_quickReverse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_sideStep(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_jump(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_duck(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_singleSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_bumpUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_slopeGravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_pushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_openDoors(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_get_inputMode(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_team(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_hidden(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_suspend(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_fly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_find(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_contact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_contactObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_contactProjectile(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_contactForce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_hitBox(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_crushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_invincible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_clickable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_pickup(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_ignorePickUpItems(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_ignoreMouse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_turnOnlyWhenMoving(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_restrictPlayerTurning(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_quickReverse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_sideStep(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_jump(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_duck(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_singleSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_bumpUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_slopeGravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_pushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_openDoors(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_inputMode(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_get_parameter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_change_ambient_pitch_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_setting_class={"obj_setting_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_setting_props[]={
							{"id",						js_obj_setting_get_id,						NULL},
							{"name",					js_obj_setting_get_name,					NULL},
							{"team",					js_obj_setting_get_team,					js_obj_setting_set_team},
							{"hidden",					js_obj_setting_get_hidden,					js_obj_setting_set_hidden},
							{"suspend",					js_obj_setting_get_suspend,					js_obj_setting_set_suspend},
							{"fly",						js_obj_setting_get_fly,						js_obj_setting_set_fly},
							{"find",					js_obj_setting_get_find,					js_obj_setting_set_find},
							{"contact",					js_obj_setting_get_contact,					js_obj_setting_set_contact},
							{"contactObject",			js_obj_setting_get_contactObject,			js_obj_setting_set_contactObject},
							{"contactProjectile",		js_obj_setting_get_contactProjectile,		js_obj_setting_set_contactProjectile},
							{"contactForce",			js_obj_setting_get_contactForce,			js_obj_setting_set_contactForce},
							{"hitBox",					js_obj_setting_get_hitBox,					js_obj_setting_set_hitBox},
							{"damage",					js_obj_setting_get_damage,					js_obj_setting_set_damage},
							{"crushable",				js_obj_setting_get_crushable,				js_obj_setting_set_crushable},
							{"invincible",				js_obj_setting_get_invincible,				js_obj_setting_set_invincible},
							{"clickable",				js_obj_setting_get_clickable,				js_obj_setting_set_clickable},
							{"pickUp",					js_obj_setting_get_pickup,					js_obj_setting_set_pickup},
							{"ignorePickUpItems",		js_obj_setting_get_ignorePickUpItems,		js_obj_setting_set_ignorePickUpItems},
							{"ignoreMouse",				js_obj_setting_get_ignoreMouse,				js_obj_setting_set_ignoreMouse},
							{"turnOnlyWhenMoving",		js_obj_setting_get_turnOnlyWhenMoving,		js_obj_setting_set_turnOnlyWhenMoving},
							{"restrictPlayerTurning",	js_obj_setting_get_restrictPlayerTurning,	js_obj_setting_set_restrictPlayerTurning},
							{"quickReverse",			js_obj_setting_get_quickReverse,			js_obj_setting_set_quickReverse},
							{"sideStep",				js_obj_setting_get_sideStep,				js_obj_setting_set_sideStep},
							{"jump",					js_obj_setting_get_jump,					js_obj_setting_set_jump},
							{"duck",					js_obj_setting_get_duck,					js_obj_setting_set_duck},
							{"crawl",					js_obj_setting_get_crawl,					js_obj_setting_set_crawl},
							{"singleSpeed",				js_obj_setting_get_singleSpeed,				js_obj_setting_set_singleSpeed},
							{"bumpUp",					js_obj_setting_get_bumpUp,					js_obj_setting_set_bumpUp},
							{"slopeGravity",			js_obj_setting_get_slopeGravity,			js_obj_setting_set_slopeGravity},
							{"pushable",				js_obj_setting_get_pushable,				js_obj_setting_set_pushable},
							{"openDoors",				js_obj_setting_get_openDoors,				js_obj_setting_set_openDoors},
							{"inputMode",				js_obj_setting_get_inputMode,				js_obj_setting_set_inputMode},
							{0}};

script_js_function	obj_setting_functions[]={
							{"getParameter",			js_obj_get_parameter_func,					1},
							{"setAmbient",				js_obj_set_ambient_func,					2},
							{"changeAmbientPitch",		js_obj_change_ambient_pitch_func,			1},
							{"clearAmbient",			js_obj_clear_ambient_func,					0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_setting_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"setting",&obj_setting_class,obj_setting_props,obj_setting_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_setting_get_id(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->uid);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->name);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_team(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->team_idx+sd_team_none);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_hidden(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->hidden);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_suspend(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->suspend);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_fly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->fly);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_find(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->find_on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_contact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL((obj->contact.object_on) || (obj->contact.projectile_on) || (obj->contact.force_on));
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_contactObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->contact.object_on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_contactProjectile(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->contact.projectile_on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_contactForce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->contact.force_on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_hitBox(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->hit_box.on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->damage.on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_crushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->damage.crushable);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_invincible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->damage.invincible);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_clickable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->click.on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_pickup(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->pickup.on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_ignorePickUpItems(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->pickup.ignore);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_ignoreMouse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->turn.ignore_mouse);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_turnOnlyWhenMoving(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->turn.only_when_moving);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_restrictPlayerTurning(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->turn.restrict_player_turning);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_quickReverse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->quick_reverse);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_sideStep(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->side_step);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_jump(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->jump.on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_duck(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->duck.on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->crawl);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_singleSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->single_speed);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_bumpUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->bump.on);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_slopeGravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->slope_gravity);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_pushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->contact.pushable);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_openDoors(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(obj->open_doors);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_get_inputMode(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->input_mode+sd_input_mode_fpp);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_setting_set_team(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->team_idx=JSVAL_TO_INT(*vp)-sd_team_none;

	return(JS_TRUE);
}

JSBool js_obj_setting_set_hidden(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_hide(obj,JSVAL_TO_BOOLEAN(*vp));
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_suspend(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->suspend=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_fly(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->fly=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_find(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->find_on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_contact(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	bool			on;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	on=JSVAL_TO_BOOLEAN(*vp);
	obj->contact.object_on=on;
	obj->contact.projectile_on=on;
	obj->contact.force_on=on;
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_contactObject(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.object_on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_contactProjectile(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.projectile_on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_contactForce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.force_on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_hitBox(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->hit_box.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_crushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.crushable=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_invincible(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.invincible=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_clickable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->click.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_pickup(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_ignorePickUpItems(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.ignore=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_ignoreMouse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.ignore_mouse=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_turnOnlyWhenMoving(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.only_when_moving=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_restrictPlayerTurning(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.restrict_player_turning=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_quickReverse(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->quick_reverse=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_sideStep(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_step=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_jump(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->jump.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_duck(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_crawl(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->crawl=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_singleSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->single_speed=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_bumpUp(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.on=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_slopeGravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->slope_gravity=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_pushable(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.pushable=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_openDoors(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->open_doors=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_setting_set_inputMode(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->input_mode=JSVAL_TO_INT(*vp)-sd_input_mode_fpp;
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_obj_get_parameter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx,k;
    char			*c,str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	idx=scripts_find_uid(obj->attach.script_uid);
	if (idx==-1) {
		*rval=JSVAL_NULL;
		return(JS_TRUE);
    }
	
    k=JSVAL_TO_INT(argv[0]);
    if (k<0) k=0;
    
    c=js.scripts[idx].params;
    while (k!=0) {
        c=strchr(c,'|');
        if (c==NULL) break;
        c++;
        k--;
    }
    
    if (c==NULL) {
        *rval=JSVAL_NULL;
        return(JS_TRUE);
    }
	
	strncpy(str,c,256);
	str[255]=0x0;
    
    c=strchr(str,'|');
    if (c!=NULL) *c=0x0;
    
    *rval=script_string_to_value(str);

	return(JS_TRUE);
}

JSBool js_obj_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	script_value_to_string(argv[0],name,name_str_len);
	object_set_ambient(obj,name,script_value_to_float(argv[1]));

	return(JS_TRUE);
}

JSBool js_obj_change_ambient_pitch_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	object_change_ambient_pitch(obj,script_value_to_float(argv[0]));

	return(JS_TRUE);
}

JSBool js_obj_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	object_clear_ambient(obj);

	return(JS_TRUE);
}





