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

JSBool js_get_obj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_obj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_get_parameter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_change_ambient_pitch_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_setting_class={"obj_setting_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_setting_property,js_set_obj_setting_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_setting_props[]={
							{"id",					obj_setting_prop_id,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"name",				obj_setting_prop_name,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"team",				obj_setting_prop_team,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"hidden",				obj_setting_prop_hidden,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"suspend",				obj_setting_prop_suspend,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"fly",					obj_setting_prop_fly,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"find",				obj_setting_prop_find,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"contact",				obj_setting_prop_contact,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"contactObject",		obj_setting_prop_contact_object,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"contactProjectile",	obj_setting_prop_contact_projectile,JSPROP_PERMANENT|JSPROP_SHARED},
							{"contactForce",		obj_setting_prop_contact_force,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"hitBox",				obj_setting_prop_hit_box,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"damage",				obj_setting_prop_damage,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"crushable",			obj_setting_prop_crushable,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"invincible",			obj_setting_prop_invincible,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"clickable",			obj_setting_prop_clickable,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"pickUp",				obj_setting_prop_pickup,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"ignorePickUpItems",	obj_setting_prop_ignore_pickup_items,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"ignoreMouse",			obj_setting_prop_ignore_mouse,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"turnOnlyWhenMoving",	obj_setting_prop_turn_only_when_moving,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"restrictPlayerTurning",obj_setting_prop_restrict_player_turning,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"quickReverse",		obj_setting_prop_quick_reverse,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"sideStep",			obj_setting_prop_side_step,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"jump",				obj_setting_prop_jump,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"duck",				obj_setting_prop_duck,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"crawl",				obj_setting_prop_crawl,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"singleSpeed",			obj_setting_prop_single_speed,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"bumpUp",				obj_setting_prop_bump_up,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"slopeGravity",		obj_setting_prop_slope_gravity,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"pushable",			obj_setting_prop_pushable,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"openDoors",			obj_setting_prop_open_doors,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"inputMode",			obj_setting_prop_input_mode,		JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

JSFunctionSpec	obj_setting_functions[]={
							{"getParameter",		js_obj_get_parameter_func,			1},
							{"setAmbient",			js_obj_set_ambient_func,			2},
							{"changeAmbientPitch",	js_obj_change_ambient_pitch_func,	1},
							{"clearAmbient",		js_obj_clear_ambient_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_setting_object(JSObject *parent_obj)
{
	JSObject		*j_obj;
	
	j_obj=JS_DefineObject(js.cx,parent_obj,"setting",&obj_setting_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_setting_props);
	JS_DefineFunctions(js.cx,j_obj,obj_setting_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_setting_prop_id:
			*vp=INT_TO_JSVAL(obj->uid);
			break;
		case obj_setting_prop_name:
			*vp=script_string_to_value(obj->name);
			break;
		case obj_setting_prop_team:
			*vp=INT_TO_JSVAL(obj->team_idx+sd_team_none);
			break;
		case obj_setting_prop_hidden:
			*vp=BOOLEAN_TO_JSVAL(obj->hidden);
			break;
		case obj_setting_prop_suspend:
			*vp=BOOLEAN_TO_JSVAL(obj->suspend);
			break;
		case obj_setting_prop_fly:
			*vp=BOOLEAN_TO_JSVAL(obj->fly);
			break;
		case obj_setting_prop_find:
			*vp=BOOLEAN_TO_JSVAL(obj->find_on);
			break;
		case obj_setting_prop_contact:
			*vp=BOOLEAN_TO_JSVAL((obj->contact.object_on) || (obj->contact.projectile_on) || (obj->contact.force_on));
			break;
		case obj_setting_prop_contact_object:
			*vp=BOOLEAN_TO_JSVAL(obj->contact.object_on);
			break;
		case obj_setting_prop_contact_projectile:
			*vp=BOOLEAN_TO_JSVAL(obj->contact.projectile_on);
			break;
		case obj_setting_prop_contact_force:
			*vp=BOOLEAN_TO_JSVAL(obj->contact.force_on);
			break;
		case obj_setting_prop_hit_box:
			*vp=BOOLEAN_TO_JSVAL(obj->hit_box.on);
			break;
		case obj_setting_prop_damage:
			*vp=BOOLEAN_TO_JSVAL(obj->damage.on);
			break;
		case obj_setting_prop_crushable:
			*vp=BOOLEAN_TO_JSVAL(obj->damage.crushable);
			break;
		case obj_setting_prop_invincible:
			*vp=BOOLEAN_TO_JSVAL(obj->damage.invincible);
			break;
		case obj_setting_prop_clickable:
			*vp=BOOLEAN_TO_JSVAL(obj->click.on);
			break;
		case obj_setting_prop_pickup:
			*vp=BOOLEAN_TO_JSVAL(obj->pickup.on);
			break;
		case obj_setting_prop_ignore_pickup_items:
			*vp=BOOLEAN_TO_JSVAL(obj->pickup.ignore);
			break;
		case obj_setting_prop_ignore_mouse:
			*vp=BOOLEAN_TO_JSVAL(obj->turn.ignore_mouse);
			break;
		case obj_setting_prop_turn_only_when_moving:
			*vp=BOOLEAN_TO_JSVAL(obj->turn.only_when_moving);
			break;
		case obj_setting_prop_restrict_player_turning:
			*vp=BOOLEAN_TO_JSVAL(obj->turn.restrict_player_turning);
			break;
		case obj_setting_prop_quick_reverse:
			*vp=BOOLEAN_TO_JSVAL(obj->quick_reverse);
			break;
		case obj_setting_prop_side_step:
			*vp=BOOLEAN_TO_JSVAL(obj->side_step);
			break;
		case obj_setting_prop_jump:
			*vp=BOOLEAN_TO_JSVAL(obj->jump.on);
			break;
		case obj_setting_prop_duck:
			*vp=BOOLEAN_TO_JSVAL(obj->duck.on);
			break;
		case obj_setting_prop_crawl:
			*vp=BOOLEAN_TO_JSVAL(obj->crawl);
			break;
		case obj_setting_prop_single_speed:
			*vp=BOOLEAN_TO_JSVAL(obj->single_speed);
			break;
		case obj_setting_prop_bump_up:
			*vp=BOOLEAN_TO_JSVAL(obj->bump.on);
			break;
		case obj_setting_prop_slope_gravity:
			*vp=BOOLEAN_TO_JSVAL(obj->slope_gravity);
			break;
		case obj_setting_prop_pushable:
			*vp=BOOLEAN_TO_JSVAL(obj->contact.pushable);
			break;
		case obj_setting_prop_open_doors:
			*vp=BOOLEAN_TO_JSVAL(obj->open_doors);
			break;
		case obj_setting_prop_input_mode:
			*vp=INT_TO_JSVAL(obj->input_mode+sd_input_mode_fpp);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_obj_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	bool			on;
	obj_type		*obj;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);
	
	switch (JSVAL_TO_INT(id)) {
	
		case obj_setting_prop_team:
			obj->team_idx=JSVAL_TO_INT(*vp)-sd_team_none;
			break;
		case obj_setting_prop_hidden:
			object_hide(obj,JSVAL_TO_BOOLEAN(*vp));
			break;
		case obj_setting_prop_suspend:
			obj->suspend=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_fly:
			obj->fly=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_find:
			obj->find_on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_contact:
			on=JSVAL_TO_BOOLEAN(*vp);
			obj->contact.object_on=on;
			obj->contact.projectile_on=on;
			obj->contact.force_on=on;
			break;
		case obj_setting_prop_contact_object:
			obj->contact.object_on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_contact_projectile:
			obj->contact.projectile_on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_contact_force:
			obj->contact.force_on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_hit_box:
			obj->hit_box.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_damage:
			obj->damage.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_crushable:
			obj->damage.crushable=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_invincible:
			obj->damage.invincible=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_clickable:
			obj->click.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_pickup:
			obj->pickup.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_ignore_pickup_items:
			obj->pickup.ignore=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_ignore_mouse:
			obj->turn.ignore_mouse=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_turn_only_when_moving:
			obj->turn.only_when_moving=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_restrict_player_turning:
			obj->turn.restrict_player_turning=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_quick_reverse:
			obj->quick_reverse=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_side_step:
			obj->side_step=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_jump:
			obj->jump.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_duck:
			obj->duck.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_crawl:
			obj->crawl=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_single_speed:
			obj->single_speed=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_bump_up:
			obj->bump.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_slope_gravity:
			obj->slope_gravity=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_pushable:
			obj->contact.pushable=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_open_doors:
			obj->open_doors=JSVAL_TO_BOOLEAN(*vp);
			break;
		case obj_setting_prop_input_mode:
			obj->input_mode=JSVAL_TO_INT(*vp)-sd_input_mode_fpp;
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Setting Functions
      
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





