/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.weapon object

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

JSBool js_obj_weapon_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_select_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_set_select_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_hide_single_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_reset_single_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_is_hidden_single_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_max_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_max_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_alt_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_alt_max_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_alt_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_weapon_get_alt_max_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_weapon_class={"obj_weapon_class",0,
							script_add_property,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};
                            
script_js_function	obj_weapon_functions[]={
							{"add",					js_obj_weapon_add_func,						1},
							{"getSelect",			js_obj_weapon_get_select_func,				0},
							{"setSelect",			js_obj_weapon_set_select_func,				1},
							{"fire",				js_obj_weapon_fire_func,					2},
							{"hide",				js_obj_weapon_hide_func,					1},
							{"reset",				js_obj_weapon_reset_func,					0},
							{"hideSingle",			js_obj_weapon_hide_single_func,				2},
							{"resetSingle",			js_obj_weapon_reset_single_func,			1},
							{"isHiddenSingle",		js_obj_weapon_is_hidden_single_func,		1},
							{"getAmmoCount",		js_obj_weapon_get_ammo_count_func,			1},
							{"getMaxAmmoCount",		js_obj_weapon_get_max_ammo_count_func,		1},
							{"getClipCount",		js_obj_weapon_get_clip_count_func,			1},
							{"getMaxClipCount",		js_obj_weapon_get_max_clip_count_func,		1},
							{"getAltAmmoCount",		js_obj_weapon_get_alt_ammo_count_func,		1},
							{"getAltMaxAmmoCount",	js_obj_weapon_get_alt_max_ammo_count_func,	1},
							{"getAltClipCount",		js_obj_weapon_get_alt_clip_count_func,		1},
							{"getAltMaxClipCount",	js_obj_weapon_get_alt_max_clip_count_func,	1},
							{0}};

/* =======================================================

      Create Weapon Array
      
======================================================= */

void script_add_obj_weapon_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"weapon",&obj_weapon_class,NULL,obj_weapon_functions);
}

/* =======================================================

      Weapon Add and Get Functions
      
======================================================= */

JSBool js_obj_weapon_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];
    obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	script_value_to_string(argv[0],name,name_str_len);
	
		// add weapon
		
	if (!weapon_create(obj,name)) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}

	*rval=JSVAL_TRUE;

	return(JS_TRUE);
}

/* =======================================================

      Weapon Select Functions
      
======================================================= */

JSBool js_obj_weapon_get_select_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

    obj=object_find_uid(js.attach.thing_uid);
	weap=weapon_find_uid(obj->held_weapon.next_uid);
	if (weap==NULL) {
		*rval=JSVAL_NULL;
		return(JS_TRUE);
	}
	
    *rval=script_string_to_value(weap->name);

    return(JS_TRUE);
}

JSBool js_obj_weapon_set_select_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	weapon_set(obj,weap);

	return(JS_TRUE);
}

/* =======================================================

      Fire Weapon Function
      
======================================================= */

JSBool js_obj_weapon_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);

	*rval=JSVAL_FALSE;
	
	if (weapon_script_fire(js.time.current_tick,obj,weap,JSVAL_TO_INT(argv[1]))) *rval=JSVAL_TRUE;

	return(JS_TRUE);
}

/* =======================================================

      Hide and Reset Weapons Function
      
======================================================= */

JSBool js_obj_weapon_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	obj->hide_all_weapons=JSVAL_TO_BOOLEAN(argv[0]);

	return(JS_TRUE);
}

JSBool js_obj_weapon_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	weapon_reset_ammo_object(obj);

	return(JS_TRUE);
}

JSBool js_obj_weapon_hide_single_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	weap->hidden=JSVAL_TO_BOOLEAN(argv[1]);

	return(JS_TRUE);
}

JSBool js_obj_weapon_reset_single_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	weapon_reset_ammo(weap);

	return(JS_TRUE);
}

JSBool js_obj_weapon_is_hidden_single_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	*rval=BOOLEAN_TO_JSVAL(weap->hidden);

	return(JS_TRUE);
}

/* =======================================================

      Ammo Count Functions
      
======================================================= */

JSBool js_obj_weapon_get_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	*rval=INT_TO_JSVAL(weap->ammo.count);

	return(JS_TRUE);
}

JSBool js_obj_weapon_get_max_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);

	*rval=INT_TO_JSVAL(weap->ammo.max_count);

	return(JS_TRUE);
}

JSBool js_obj_weapon_get_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	*rval=INT_TO_JSVAL(weap->ammo.clip_count);

	return(JS_TRUE);
}

JSBool js_obj_weapon_get_max_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);

	*rval=INT_TO_JSVAL(weap->ammo.max_clip_count);

	return(JS_TRUE);
}

/* =======================================================

      Alt Ammo Count Functions
      
======================================================= */

JSBool js_obj_weapon_get_alt_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	*rval=INT_TO_JSVAL(weap->alt_ammo.count);

	return(JS_TRUE);
}

JSBool js_obj_weapon_get_alt_max_ammo_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);

	*rval=INT_TO_JSVAL(weap->alt_ammo.max_count);

	return(JS_TRUE);
}

JSBool js_obj_weapon_get_alt_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);
	
	*rval=INT_TO_JSVAL(weap->alt_ammo.clip_count);

	return(JS_TRUE);
}

JSBool js_obj_weapon_get_alt_max_clip_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(JS_FALSE);

	*rval=INT_TO_JSVAL(weap->alt_ammo.max_clip_count);

	return(JS_TRUE);
}



