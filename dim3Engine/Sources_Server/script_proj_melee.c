/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.melee object

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

extern js_type			js;

JSBool js_get_proj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_proj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_melee_spawn_from_projectile_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_melee_spawn_from_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			proj_melee_class={"proj_melee_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	proj_melee_props[]={
							{"strikeBoneTag",			js_proj_melee_get_strikeBoneTag,		js_proj_melee_set_strikeBoneTag},
							{"strikePoseName",			js_proj_melee_get_strikePoseName,		js_proj_melee_set_strikePoseName},
							{"radius",					js_proj_melee_get_radius,				js_proj_melee_set_radius},
							{"distance",				js_proj_melee_get_distance,				js_proj_melee_set_distance},
							{"damage",					js_proj_melee_get_damage,				js_proj_melee_set_damage},
							{"force",					js_proj_melee_get_force,				js_proj_melee_set_force},
							{"fallOff",					js_proj_melee_get_fallOff,				js_proj_melee_set_fallOff},
							{0}};

script_js_function	proj_melee_functions[]={
							{"spawnFromProjectileBone",	js_proj_melee_spawn_from_projectile_bone_func,	0},
							{"spawnFromPosition",		js_proj_melee_spawn_from_position_func,			3},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_melee_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"melee",&proj_melee_class,proj_melee_props,proj_melee_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_proj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				str[32];
	proj_setup_type		*proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case proj_melee_prop_strike_bone_tag:
			model_tag_to_text(proj_setup->melee.strike_bone_tag,str);
			*vp=script_string_to_value(str);
			break;
		case proj_melee_prop_strike_pose_name:
			*vp=script_string_to_value(proj_setup->melee.strike_pose_name);
			break;
		case proj_melee_prop_radius:
			*vp=INT_TO_JSVAL(proj_setup->melee.radius);
			break;
		case proj_melee_prop_distance:
			*vp=INT_TO_JSVAL(proj_setup->melee.distance);
			break;
		case proj_melee_prop_damage:
			*vp=INT_TO_JSVAL(proj_setup->melee.damage);
			break;
		case proj_melee_prop_force:
			*vp=INT_TO_JSVAL(proj_setup->melee.force);
			break;
		case proj_melee_prop_fall_off:
			*vp=BOOLEAN_TO_JSVAL(proj_setup->melee.fall_off);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_proj_melee_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				str[32];
	proj_setup_type		*proj_setup;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case proj_melee_prop_strike_bone_tag:
			script_value_to_string(*vp,str,32);
			proj_setup->melee.strike_bone_tag=text_to_model_tag(str);
			break;
 		case proj_melee_prop_strike_pose_name:
			script_value_to_string(*vp,proj_setup->melee.strike_pose_name,name_str_len);
			break;
		case proj_melee_prop_radius:
			proj_setup->melee.radius=JSVAL_TO_INT(*vp);
			break;
		case proj_melee_prop_distance:
			proj_setup->melee.distance=JSVAL_TO_INT(*vp);
			break;
		case proj_melee_prop_damage:
			proj_setup->melee.damage=JSVAL_TO_INT(*vp);
			break;
		case proj_melee_prop_force:
			proj_setup->melee.force=JSVAL_TO_INT(*vp);
			break;
		case proj_melee_prop_fall_off:
			proj_setup->melee.fall_off=JSVAL_TO_BOOLEAN(*vp);
			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSBool js_proj_melee_spawn_from_projectile_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				err_str[256];
    obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	if (proj_setup==NULL) return(JS_TRUE);
	
    obj=object_find_uid(proj_setup->obj_uid);
	weap=weapon_find_uid(proj_setup->weap_uid);

	if (!melee_script_spawn_projectile_model(js.time.current_tick,obj,weap,proj_setup,proj,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_proj_melee_spawn_from_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	d3pnt				pt;
    obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	if (proj_setup==NULL) return(JS_TRUE);
	
    obj=object_find_uid(proj_setup->obj_uid);
	weap=weapon_find_uid(proj_setup->weap_uid);
	
	pt.x=JSVAL_TO_INT(argv[0]);
	pt.z=JSVAL_TO_INT(argv[1]);
	pt.y=JSVAL_TO_INT(argv[2]);
	
	melee_add(obj,weap,&pt,&proj->ang,&proj_setup->melee,-1);

	return(JS_TRUE);
}

