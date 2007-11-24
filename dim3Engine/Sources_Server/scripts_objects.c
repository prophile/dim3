/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Object Creation

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

extern js_type			js;
extern JSClass			dim3_class;

JSClass			global_class={"global_class",0,
							JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

/* =======================================================

      Initialize/Release Prototype Objects
      
======================================================= */

void script_initialize_prototype_objects(void)
{
	js.global_prototype=NULL;
	js.game_prototype=NULL;
	js.course_prototype=NULL;
	js.obj_prototype=NULL;
	js.weap_prototype=NULL;
	js.proj_setup_prototype=NULL;
	
	script_set_property_lock(FALSE);
}

void script_release_prototype_objects(void)
{
	if (js.global_prototype!=NULL) JS_RemoveRoot(js.cx,&js.global_prototype);
	if (js.game_prototype!=NULL) JS_RemoveRoot(js.cx,&js.game_prototype);
	if (js.course_prototype!=NULL) JS_RemoveRoot(js.cx,&js.course_prototype);
	if (js.obj_prototype!=NULL) JS_RemoveRoot(js.cx,&js.obj_prototype);
	if (js.weap_prototype!=NULL) JS_RemoveRoot(js.cx,&js.weap_prototype);
	if (js.proj_setup_prototype!=NULL) JS_RemoveRoot(js.cx,&js.proj_setup_prototype);
}

/* =======================================================

      Create Prototype Objects
      
======================================================= */

bool script_create_prototype_objects(char *err_str)
{
		// global prototype

	js.global_prototype=JS_NewObject(js.cx,&global_class,NULL,NULL);
	if (js.global_prototype==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create global prototype object");
		return(FALSE);
	}
	
	JS_AddNamedRoot(js.cx,&js.global_prototype,"d3_global_prototype");
		
	script_set_property_lock(TRUE);
	
	script_add_global_map_object(js.global_prototype);
	script_add_global_camera_object(js.global_prototype);
	script_add_global_interface_object(js.global_prototype);
	script_add_global_data_object(js.global_prototype);
	script_add_global_sound_object(js.global_prototype);
	script_add_global_spawn_object(js.global_prototype);
	script_add_global_utility_object(js.global_prototype);
	
	script_set_property_lock(FALSE);
	
		// game prototype
		
	js.game_prototype=JS_NewObject(js.cx,&dim3_class,NULL,NULL);
	if (js.game_prototype==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create game prototype object");
		return(FALSE);
	}
	
	JS_AddNamedRoot(js.cx,&js.game_prototype,"d3_game_prototype");
	
	script_set_property_lock(TRUE);
	
	script_add_game_setting_object(js.game_prototype);
	script_add_event_object(js.game_prototype);
	script_add_game_score_object(js.game_prototype);
	script_add_game_join_object(js.game_prototype);
	
	script_set_property_lock(FALSE);
	
		// course prototype
		
	js.course_prototype=JS_NewObject(js.cx,&dim3_class,NULL,NULL);
	if (js.course_prototype==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create course prototype object");
		return(FALSE);
	}
	
	JS_AddNamedRoot(js.cx,&js.course_prototype,"d3_course_prototype");
	
	script_set_property_lock(TRUE);
	
	script_add_event_object(js.course_prototype);
	
	script_set_property_lock(FALSE);
	
		// object prototype

	js.obj_prototype=JS_NewObject(js.cx,&dim3_class,NULL,NULL);
	if (js.obj_prototype==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create object prototype object");
		return(FALSE);
	}
	
	JS_AddNamedRoot(js.cx,&js.obj_prototype,"d3_object_prototype");
		
	script_set_property_lock(TRUE);
	
	script_add_obj_setting_object(js.obj_prototype);
	script_add_event_object(js.obj_prototype);
	script_add_obj_position_object(js.obj_prototype);
	script_add_obj_angle_object(js.obj_prototype);
	script_add_obj_lock_object(js.obj_prototype);
	script_add_obj_status_object(js.obj_prototype);
	script_add_obj_motion_vector_object(js.obj_prototype);
	script_add_obj_motion_angle_object(js.obj_prototype);
	script_add_obj_forward_speed_object(js.obj_prototype);
	script_add_obj_side_speed_object(js.obj_prototype);
	script_add_obj_vert_speed_object(js.obj_prototype);
	script_add_obj_turn_speed_object(js.obj_prototype);
	script_add_obj_object_speed_object(js.obj_prototype);
	script_add_obj_look_object(js.obj_prototype);
	script_add_obj_sight_object(js.obj_prototype);
	script_add_obj_health_object(js.obj_prototype);
	script_add_obj_size_object(js.obj_prototype);
	script_add_obj_rigid_body_object(js.obj_prototype);
	script_add_obj_vehicle_object(js.obj_prototype);
	script_add_obj_radar_object(js.obj_prototype);
	script_add_obj_score_object(js.obj_prototype);
	script_add_obj_touch_object(js.obj_prototype);
	script_add_obj_touch_position_object(js.obj_prototype);
	script_add_obj_touch_angle_object(js.obj_prototype);
	script_add_obj_hit_object(js.obj_prototype);
	script_add_obj_hit_position_object(js.obj_prototype);
	script_add_obj_hit_angle_object(js.obj_prototype);
	script_add_obj_click_object(js.obj_prototype);
	script_add_obj_held_object(js.obj_prototype);
	script_add_obj_pickup_object(js.obj_prototype);
	script_add_obj_watch_object(js.obj_prototype);
	script_add_obj_weapon_object(js.obj_prototype);
	script_add_obj_weapon_fire_object(js.obj_prototype);
	script_add_obj_melee_object(js.obj_prototype);
	script_add_model_object(js.obj_prototype);
	
	script_set_property_lock(FALSE);
	
		// weapon prototype

	js.weap_prototype=JS_NewObject(js.cx,&dim3_class,NULL,NULL);
	if (js.weap_prototype==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create weapon prototype object");
		return(FALSE);
	}
	
	JS_AddNamedRoot(js.cx,&js.weap_prototype,"d3_weapon_prototype");
		
	script_set_property_lock(TRUE);

	script_add_weap_setting_object(js.weap_prototype);
	script_add_event_object(js.weap_prototype);
	script_add_weap_hand_object(js.weap_prototype);
	script_add_weap_hand_position_object(js.weap_prototype);
	script_add_weap_hand_angle_object(js.weap_prototype);
	script_add_weap_projectile_object(js.weap_prototype);
	script_add_weap_crosshair_object(js.weap_prototype);
	script_add_weap_kickback_object(js.weap_prototype);
	script_add_weap_recoil_object(js.weap_prototype);
	script_add_weap_ammo_object(js.weap_prototype);
	script_add_weap_alt_ammo_object(js.weap_prototype);
	script_add_weap_melee_object(js.weap_prototype);
	script_add_weap_fire_object(js.weap_prototype);
	script_add_weap_zoom_object(js.weap_prototype);
	script_add_model_object(js.weap_prototype);
	
	script_set_property_lock(FALSE);
	
		// projectile setup prototype

	js.proj_setup_prototype=JS_NewObject(js.cx,&dim3_class,NULL,NULL);
	if (js.proj_setup_prototype==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create projectile prototype object");
		return(FALSE);
	}
	
	JS_AddNamedRoot(js.cx,&js.proj_setup_prototype,"d3_projectile_setup_prototype");
		
	script_set_property_lock(TRUE);
	
	script_add_proj_setting_object(js.proj_setup_prototype);
	script_add_event_object(js.proj_setup_prototype);
	script_add_proj_position_object(js.proj_setup_prototype);
	script_add_proj_origin_object(js.proj_setup_prototype);
	script_add_proj_motion_vector_object(js.proj_setup_prototype);
	script_add_proj_motion_angle_object(js.proj_setup_prototype);
	script_add_proj_speed_object(js.proj_setup_prototype);
	script_add_proj_hit_object(js.proj_setup_prototype);
	script_add_proj_action_object(js.proj_setup_prototype);
	script_add_proj_push_object(js.proj_setup_prototype);
	script_add_proj_size_object(js.proj_setup_prototype);
	script_add_proj_mark_object(js.proj_setup_prototype);
	script_add_proj_melee_object(js.proj_setup_prototype);
	script_add_model_object(js.proj_setup_prototype);
			
	script_set_property_lock(FALSE);
	
	return(TRUE);
}

/* =======================================================

      Get Prototype Object
      
======================================================= */

JSObject* script_get_prototype_objects(attach_type *attach)
{
	switch (attach->thing_type) {
		case thing_type_game:
			return(js.game_prototype);
		case thing_type_course:
			return(js.course_prototype);
		case thing_type_object:
			return(js.obj_prototype);
		case thing_type_weapon:
			return(js.weap_prototype);
		case thing_type_projectile:
		case thing_type_projectile_setup:
			return(js.proj_setup_prototype);
	}
	
	return(NULL);
}

/* =======================================================

      Add Global Objects to Script
      
======================================================= */

bool script_add_global_object(script_type *script,char *err_str)
{
		// must remove other globals to stop parenting problems
		
	JS_SetGlobalObject(js.cx,NULL);
	
		// new global for this script
	
	script->global=JS_NewObject(js.cx,&global_class,js.global_prototype,NULL);
	if (script->global==NULL) {
		strcpy(err_str,"Not enough memory to create global object");
		return(FALSE);
	}
	
	JS_SetGlobalObject(js.cx,script->global);

		// initialize standard classes

	if (!JS_InitStandardClasses(js.cx,script->global)) {
		strcpy(err_str,"Could not initialize standard classes");
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Script Add Property
      
======================================================= */

void script_set_property_lock(bool lock)
{
	js.add_property_lock=lock;
}

JSBool script_add_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char		prop_name[256];
	
	if (js.add_property_lock) return(JS_TRUE);
	
		// only allow dim3 to add property to game objects

	script_value_to_string(id,prop_name,256);
	JS_ReportError(js.cx,"%s is not a property or function of this object",prop_name);
	
	return(JS_FALSE);
}

