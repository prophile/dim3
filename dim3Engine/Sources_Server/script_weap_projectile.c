/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.projectile object

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

JSBool js_weap_projectile_get_fireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_get_barrelBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_get_firePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_get_objectFireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_get_objectFirePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_get_repeat_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_get_repeat_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_set_fireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_set_barrelBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_set_firePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_set_objectFireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_set_objectFirePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_set_repeat_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_set_repeat_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_projectile_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_weapon_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_weapon_bone_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_weapon_bone_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_weapon_bone_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_object_bone_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_object_bone_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_object_bone_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_barrel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_barrel_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_barrel_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_barrel_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_center_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_center_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_center_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_projectile_spawn_from_center_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			weap_projectile_class={"weap_projectile_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};
							
script_js_property	weap_projectile_props[]={
							{"fireBoneTag",				js_weap_projectile_get_fireBoneTag,				js_weap_projectile_set_fireBoneTag},
							{"barrelBoneTag",			js_weap_projectile_get_barrelBoneTag,			js_weap_projectile_set_barrelBoneTag},
							{"firePoseName",			js_weap_projectile_get_firePoseName,			js_weap_projectile_set_firePoseName},
							{"objectFireBoneTag",		js_weap_projectile_get_objectFireBoneTag,		js_weap_projectile_set_objectFireBoneTag},
							{"objectFirePoseName",		js_weap_projectile_get_objectFirePoseName,		js_weap_projectile_set_objectFirePoseName},
							{"repeatOn",				js_weap_projectile_get_repeat_on,				js_weap_projectile_set_repeat_on},
							{"repeatTick",				js_weap_projectile_get_repeat_tick,				js_weap_projectile_set_repeat_tick},
							{0}};

script_js_function	weap_projectile_functions[]={
							{"add",										js_weap_projectile_add_func,											2},
							{"spawnFromWeaponBone",						js_weap_projectile_spawn_from_weapon_bone_func,							1},
							{"spawnFromWeaponBoneSlop",					js_weap_projectile_spawn_from_weapon_bone_slop_func,					2},
							{"spawnFromWeaponBoneMultiSlop",			js_weap_projectile_spawn_from_weapon_bone_multi_slop_func,				3},
							{"spawnFromWeaponBoneOffsetAngle",			js_weap_projectile_spawn_from_weapon_bone_offset_angle_func,			5},
							{"spawnFromObjectBone",						js_weap_projectile_spawn_from_object_bone_func,							1},
							{"spawnFromObjectBoneSlop",					js_weap_projectile_spawn_from_object_bone_slop_func,					2},
							{"spawnFromObjectBoneMultiSlop",			js_weap_projectile_spawn_from_object_bone_multi_slop_func,				3},
							{"spawnFromObjectBoneOffsetAngle",			js_weap_projectile_spawn_from_object_bone_offset_angle_func,			5},
							{"spawnFromBarrel",							js_weap_projectile_spawn_from_barrel_func,								1},
							{"spawnFromBarrelSlop",						js_weap_projectile_spawn_from_barrel_slop_func,							2},
							{"spawnFromBarrelMultiSlop",				js_weap_projectile_spawn_from_barrel_multi_slop_func,					3},
							{"spawnFromBarrelOffsetAngle",				js_weap_projectile_spawn_from_barrel_offset_angle_func,					5},
							{"spawnFromCenter",							js_weap_projectile_spawn_from_center_func,								1},
							{"spawnFromCenterSlop",						js_weap_projectile_spawn_from_center_slop_func,							2},
							{"spawnFromCenterMultiSlop",				js_weap_projectile_spawn_from_center_multi_slop_func,					3},
							{"spawnFromCenterOffsetAngle",				js_weap_projectile_spawn_from_center_offset_angle_func,					5},
							{0}};							

/* =======================================================

      Create Projectile
      
======================================================= */

void script_add_weap_projectile_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"projectile",&weap_projectile_class,weap_projectile_props,weap_projectile_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_weap_projectile_get_fireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	model_tag_to_text(weap->proj.fire_bone_tag,str);
	*vp=script_string_to_value(str);
	
	return(JS_TRUE);
}

JSBool js_weap_projectile_get_barrelBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);

	model_tag_to_text(weap->proj.barrel_bone_tag,str);
	*vp=script_string_to_value(str);
	
	return(JS_TRUE);
}

JSBool js_weap_projectile_get_firePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->proj.fire_pose_name);
	
	return(JS_TRUE);
}

JSBool js_weap_projectile_get_objectFireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);

	model_tag_to_text(weap->proj.object_fire_bone_tag,str);
	*vp=script_string_to_value(str);
	
	return(JS_TRUE);
}

JSBool js_weap_projectile_get_objectFirePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->proj.object_fire_pose_name);
	
	return(JS_TRUE);
}

JSBool js_weap_projectile_get_repeat_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=BOOLEAN_TO_JSVAL(weap->proj.repeat_on);
	
	return(JS_TRUE);
}

JSBool js_weap_projectile_get_repeat_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(weap->proj.repeat_tick);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_weap_projectile_set_fireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(*vp,str,32);
	weap->proj.fire_bone_tag=text_to_model_tag(str);

	return(JS_TRUE);
}

JSBool js_weap_projectile_set_barrelBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(*vp,str,32);
	weap->proj.barrel_bone_tag=text_to_model_tag(str);

	return(JS_TRUE);
}

JSBool js_weap_projectile_set_firePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->proj.fire_pose_name,name_str_len);

	return(JS_TRUE);
}

JSBool js_weap_projectile_set_objectFireBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(*vp,str,32);
	weap->proj.object_fire_bone_tag=text_to_model_tag(str);

	return(JS_TRUE);
}

JSBool js_weap_projectile_set_objectFirePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->proj.object_fire_pose_name,name_str_len);

	return(JS_TRUE);
}

JSBool js_weap_projectile_set_repeat_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->proj.repeat_on=JSVAL_TO_BOOLEAN(*vp);

	return(JS_TRUE);
}

JSBool js_weap_projectile_set_repeat_tick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->proj.repeat_tick=JSVAL_TO_INT(*vp);

	return(JS_TRUE);
}

/* =======================================================

      Projectile Add Function
      
======================================================= */

JSBool js_weap_projectile_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				str[name_str_len];
    obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],str,name_str_len);

	if (!proj_setup_add(obj,weap,str)) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}

	*rval=JSVAL_TRUE;

	return(JS_TRUE);
}

/* =======================================================

      Projectile Spawn from Weapon
      
======================================================= */

JSBool js_weap_projectile_spawn_from_weapon_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_weapon_bone_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_weapon_bone_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=JSVAL_TO_INT(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_weapon_bone_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

/* =======================================================

      Projectile Spawn from Object
      
======================================================= */

JSBool js_weap_projectile_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_object_bone_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_object_bone_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=JSVAL_TO_INT(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_object_bone_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

/* =======================================================

      Projectile Spawn from Barrel
      
======================================================= */

JSBool js_weap_projectile_spawn_from_barrel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_barrel_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_barrel_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=JSVAL_TO_INT(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_barrel_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

/* =======================================================

      Projectile Spawn from Object To Center
      
======================================================= */

JSBool js_weap_projectile_spawn_from_center_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_center_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_center_multi_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=JSVAL_TO_INT(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_projectile_spawn_from_center_offset_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}


