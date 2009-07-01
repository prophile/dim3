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

JSClass			global_class={"global_class",0,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

/* =======================================================

      Initialize/Release Classes
      
======================================================= */

void script_initialize_classes(void)
{
	/*
*/	
	script_set_property_lock(FALSE);
}

void script_release_classes(void)
{
	/*
	*/
}

/* =======================================================

      Add Global Objects to Script
      
======================================================= */

bool script_add_global_object(script_type *script,char *err_str)
{
		// must remove other globals to stop parenting problems
		
	JS_SetGlobalObject(js.cx,NULL);
	
		// create global for this script
	
	script->global=JS_NewObject(js.cx,&global_class,NULL,NULL);
	if (script->global==NULL) {
		strcpy(err_str,"Not enough memory to create global object");
		return(FALSE);
	}

		// set sub objects

	script_set_property_lock(TRUE);

	script_add_global_map_object(script->global);
	script_add_global_multiplayer_object(script->global);
	script_add_global_camera_object(script->global);
	script_add_global_interface_object(script->global);
	script_add_global_data_object(script->global);
	script_add_global_sound_object(script->global);
	script_add_global_spawn_object(script->global);
	script_add_global_utility_object(script->global);

	script_set_property_lock(FALSE);
	
		// set global

	JS_SetGlobalObject(js.cx,script->global);

		// initialize standard classes

	if (!JS_InitStandardClasses(js.cx,script->global)) {
		strcpy(err_str,"Could not initialize standard classes");
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Create dim3 Script Objects
      
======================================================= */

JSObject* script_create_main_object(attach_type *attach)
{
	JSObject		*j_obj;

	j_obj=JS_NewObject(js.cx,&dim3_class,NULL,NULL);
	if (j_obj==NULL) return(NULL);

		// don't trigger adding lock

	script_set_property_lock(TRUE);

		// add sub-objects

	switch (attach->thing_type) {

		case thing_type_game:
			script_add_game_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_game_score_object(j_obj);
			script_add_game_join_object(j_obj);
			break;

		case thing_type_course:
			script_add_event_object(j_obj);
			break;

		case thing_type_object:
			script_add_obj_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_obj_position_object(j_obj);
			script_add_obj_angle_object(j_obj);
			script_add_obj_lock_object(j_obj);
			script_add_obj_status_object(j_obj);
			script_add_obj_motion_vector_object(j_obj);
			script_add_obj_motion_angle_object(j_obj);
			script_add_obj_forward_speed_object(j_obj);
			script_add_obj_side_speed_object(j_obj);
			script_add_obj_vert_speed_object(j_obj);
			script_add_obj_turn_speed_object(j_obj);
			script_add_obj_object_speed_object(j_obj);
			script_add_obj_thrust_object(j_obj);
			script_add_obj_look_object(j_obj);
			script_add_obj_sight_object(j_obj);
			script_add_obj_health_object(j_obj);
			script_add_obj_size_object(j_obj);
			script_add_obj_rigid_body_object(j_obj);
			script_add_obj_vehicle_object(j_obj);
			script_add_obj_radar_object(j_obj);
			script_add_obj_score_object(j_obj);
			script_add_obj_touch_object(j_obj);
			script_add_obj_touch_position_object(j_obj);
			script_add_obj_touch_angle_object(j_obj);
			script_add_obj_hit_object(j_obj);
			script_add_obj_hit_position_object(j_obj);
			script_add_obj_hit_angle_object(j_obj);
			script_add_obj_click_object(j_obj);
			script_add_obj_held_object(j_obj);
			script_add_obj_pickup_object(j_obj);
			script_add_obj_watch_object(j_obj);
			script_add_obj_weapon_object(j_obj);
			script_add_obj_weapon_fire_object(j_obj);
			script_add_obj_melee_object(j_obj);
			script_add_model_object(j_obj);
			break;

		case thing_type_weapon:
			script_add_weap_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_weap_hand_object(j_obj);
			script_add_weap_hand_position_object(j_obj);
			script_add_weap_hand_angle_object(j_obj);
			script_add_weap_projectile_object(j_obj);
			script_add_weap_crosshair_object(j_obj);
			script_add_weap_kickback_object(j_obj);
			script_add_weap_recoil_object(j_obj);
			script_add_weap_ammo_object(j_obj);
			script_add_weap_alt_ammo_object(j_obj);
			script_add_weap_melee_object(j_obj);
			script_add_weap_fire_object(j_obj);
			script_add_weap_dual_object(j_obj);
			script_add_weap_target_object(j_obj);
			script_add_weap_zoom_object(j_obj);
			script_add_model_object(j_obj);
			break;

		case thing_type_projectile:
		case thing_type_projectile_setup:
			script_add_proj_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_proj_position_object(j_obj);
			script_add_proj_origin_object(j_obj);
			script_add_proj_motion_vector_object(j_obj);
			script_add_proj_motion_angle_object(j_obj);
			script_add_proj_speed_object(j_obj);
			script_add_proj_hit_object(j_obj);
			script_add_proj_action_object(j_obj);
			script_add_proj_push_object(j_obj);
			script_add_proj_size_object(j_obj);
			script_add_proj_mark_object(j_obj);
			script_add_proj_melee_object(j_obj);
			script_add_model_object(j_obj);
			break;

	}

		// clear lock

	script_set_property_lock(FALSE);
	
	return(j_obj);
}

JSObject* script_create_child_object(JSObject *parent_obj,char *name,JSClass *class,script_js_property *props,script_js_function *funcs)
{
	int					flags;
	script_js_property	*prop;
	script_js_function	*func;
	JSObject			*j_obj;

		// object

	j_obj=JS_DefineObject(js.cx,parent_obj,name,class,NULL,0);

		// properties

	if (props!=NULL) {

		prop=props;

		while (prop->name!=NULL) {

			flags=JSPROP_PERMANENT|JSPROP_SHARED;
			if (prop->setter==NULL) flags|=JSPROP_READONLY;

			JS_DefineProperty(js.cx,j_obj,prop->name,JSVAL_NULL,prop->getter,prop->setter,flags);

			prop++;
		}
	}

		// functions

	if (funcs!=NULL) {

		func=funcs;

		while (func->name!=NULL) {
			JS_DefineFunction(js.cx,j_obj,func->name,func->call,func->nargs,0);
			func++;
		}
	}

	return(j_obj);
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

