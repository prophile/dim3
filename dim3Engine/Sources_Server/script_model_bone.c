/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.bone object

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
#include "models.h"

JSBool js_model_bone_find_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_bone_find_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_bone_get_brightness_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

extern js_type			js;

JSClass			model_bone_class={"model_bone_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	model_bone_functions[]={
							{"findOffset",			js_model_bone_find_offset_func,			2},
							{"findPosition",		js_model_bone_find_position_func,		2},
							{"getBrightness",		js_model_bone_get_brightness_func,		2},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_bone_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"bone",&model_bone_class,NULL,model_bone_functions);
}

/* =======================================================

      Setup Draw For Bone Functions
      
======================================================= */

model_draw* script_bone_function_setup(void)
{
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	
	switch (js.attach.thing_type) {
	
		case thing_type_object:
			obj=object_find_uid(js.attach.thing_uid);
			model_draw_setup_object(js.time.current_tick,obj);
			return(&obj->draw);
			
		case thing_type_weapon:
			weap=weapon_find_uid(js.attach.thing_uid);
			obj=object_find_uid(weap->obj_uid);
			model_draw_setup_weapon(js.time.current_tick,obj,weap,FALSE,FALSE);
			return(&weap->draw);
			
		case thing_type_projectile:
			proj=projectile_find_uid(js.attach.thing_uid);
			model_draw_setup_projectile(js.time.current_tick,proj);
			return(&proj->draw);
			
		case thing_type_projectile_setup:
			JS_ReportError(js.cx,"There is no model to get bone positions from");
			return(NULL);
			
	}
	
	return(NULL);
}

/* =======================================================

      Model Bone Functions
      
======================================================= */

JSBool js_model_bone_find_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				pose_name[name_str_len],bone_name[name_str_len];
	int					x,y,z;
	model_draw			*draw;
	
		// get proper draw setup
		
	draw=script_bone_function_setup();
	if (draw==NULL) return(JS_FALSE);
	
		// get bone offset
		
	script_value_to_string(argv[0],pose_name,name_str_len);
	script_value_to_string(argv[1],bone_name,name_str_len);
	
	if (!model_find_bone_offset(draw,pose_name,bone_name,&x,&y,&z)) {
		JS_ReportError(js.cx,"Named pose or bone does not exist: %s,%s",pose_name,bone_name);
		return(JS_FALSE);
	}
	
	*rval=script_point_to_value(x,y,z);
	
	return(JS_TRUE);
}

JSBool js_model_bone_find_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				pose_name[name_str_len],bone_name[name_str_len];
	int					x,y,z;
	model_draw			*draw;
	
		// get proper draw setup
		
	draw=script_bone_function_setup();
	if (draw==NULL) return(JS_FALSE);
	
		// get bone position
	
	script_value_to_string(argv[0],pose_name,name_str_len);
	script_value_to_string(argv[1],bone_name,name_str_len);
	
	if (!model_find_bone_position(draw,pose_name,bone_name,&x,&y,&z)) {
		JS_ReportError(js.cx,"Named pose or bone does not exist: %s,%s",pose_name,bone_name);
		return(JS_FALSE);
	}
	
	*rval=script_point_to_value(x,y,z);
	
	return(JS_TRUE);
}

JSBool js_model_bone_get_brightness_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			pose_name[name_str_len],bone_name[name_str_len];
	float			bright;
	model_draw		*draw;
	
		// get proper draw setup
		
	draw=script_bone_function_setup();
	if (draw==NULL) return(JS_FALSE);
	
		// get bone light

	script_value_to_string(argv[0],pose_name,name_str_len);
	script_value_to_string(argv[1],bone_name,name_str_len);
	
	if (!model_get_bone_brightness(draw,pose_name,bone_name,&bright)) {
		JS_ReportError(js.cx,"Named pose or bone does not exist: %s,%s",pose_name,bone_name);
		return(JS_FALSE);
	}
	
	*rval=script_float_to_value(bright);
	
	return(JS_TRUE);
}

