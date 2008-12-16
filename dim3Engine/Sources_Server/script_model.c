/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model object

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

JSBool js_get_model_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_model_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_start_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_stop_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_cancel_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_change_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_interrupt_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_change_fill_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_find_bone_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_find_bone_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			model_class={"model_class",JSCLASS_HAS_PRIVATE,
							script_add_property,JS_PropertyStub,
							js_get_model_property,js_set_model_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	model_props[]={
							{"on",					model_prop_on,						JSPROP_PERMANENT|JSPROP_SHARED},
							{"name",				model_prop_name,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"lit",					model_prop_lit,						JSPROP_PERMANENT|JSPROP_SHARED},
							{"bounce",				model_prop_bounce,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"truform",				model_prop_truform,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"alpha",				model_prop_alpha,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"resize",				model_prop_resize,					JSPROP_PERMANENT|JSPROP_SHARED},
							{"faceForward",			model_prop_face_forward,			JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

extern void script_add_model_offset_object(JSObject *parent_obj);
extern void script_add_model_rotate_object(JSObject *parent_obj);
extern void script_add_model_spin_object(JSObject *parent_obj);
extern void script_add_model_light_object(JSObject *parent_obj);
extern void script_add_model_light_color_object(JSObject *parent_obj);
extern void script_add_model_hilite_color_object(JSObject *parent_obj);
extern void script_add_model_halo_object(JSObject *parent_obj);
extern void script_add_model_shadow_object(JSObject *parent_obj);
extern void script_add_model_animation_object(JSObject *parent_obj);
extern void script_add_model_mesh_object(JSObject *parent_obj);
extern void script_add_model_bone_object(JSObject *parent_obj);
extern void script_add_model_fill_object(JSObject *parent_obj);

/* =======================================================

      Create Model
      
======================================================= */

void script_add_model_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"model",&model_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,model_props);
	
	script_add_model_offset_object(j_obj);
	script_add_model_rotate_object(j_obj);
	script_add_model_spin_object(j_obj);
	script_add_model_light_object(j_obj);
	script_add_model_light_color_object(j_obj);
	script_add_model_hilite_color_object(j_obj);
	script_add_model_halo_object(j_obj);
	script_add_model_shadow_object(j_obj);
	script_add_model_animation_object(j_obj);
	script_add_model_mesh_object(j_obj);
	script_add_model_bone_object(j_obj);
	script_add_model_fill_object(j_obj);
}

/* =======================================================

      Find Objects from Attachments
      
======================================================= */

model_draw* js_find_model_draw(JSObject *j_obj,bool is_child)
{
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;
	
		// get correct model from attachment
		
	switch (js.attach.thing_type) {
	
		case thing_type_object:
			obj=object_find_uid(js.attach.thing_uid);
			return(&obj->draw);
			
		case thing_type_weapon:
			weap=weapon_find_uid(js.attach.thing_uid);
			if (weap->dual.in_dual) return(&weap->draw_dual);
			return(&weap->draw);
			
		case thing_type_projectile_setup:
			proj_setup=proj_setups_find_uid(js.attach.thing_uid);
			return(&proj_setup->draw);
			
		case thing_type_projectile:
			proj=projectile_find_uid(js.attach.thing_uid);
			return(&proj->draw);
			
	}
	
	return(NULL);
}
	
/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_model_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,FALSE);

	switch (JSVAL_TO_INT(id)) {
	
		case model_prop_on:
			*vp=BOOLEAN_TO_JSVAL(draw->on);
			break;
		case model_prop_name:
			*vp=script_string_to_value(draw->name);
			break;
		case model_prop_lit:
			*vp=INT_TO_JSVAL(draw->lit_type+sd_model_lit_flat);
			break;
		case model_prop_bounce:
			*vp=BOOLEAN_TO_JSVAL(draw->bounce);
			break;
		case model_prop_truform:
			// depreciated
			*vp=BOOLEAN_TO_JSVAL(FALSE);
			break;
		case model_prop_alpha:
			*vp=script_float_to_value(draw->alpha);
			break;
		case model_prop_resize:
			*vp=script_float_to_value(draw->resize);
			break;
		case model_prop_face_forward:
			*vp=BOOLEAN_TO_JSVAL(draw->face_forward);
			break;
	}

	return(JS_TRUE);
}

JSBool js_set_model_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	draw=js_find_model_draw(j_obj,FALSE);

	switch (JSVAL_TO_INT(id)) {
	
		case model_prop_on:
			draw->on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_prop_name:
			script_value_to_string(*vp,draw->name,name_str_len);
			break;
		case model_prop_lit:
			draw->lit_type=JSVAL_TO_INT(*vp)-sd_model_lit_flat;
			break;
		case model_prop_bounce:
			draw->bounce=JSVAL_TO_BOOLEAN(*vp);
			break;
		case model_prop_truform:
			// depreciated
			break;
		case model_prop_alpha:
			draw->alpha=script_value_to_float(*vp);
			break;
		case model_prop_resize:
			draw->resize=script_value_to_float(*vp);
			break;
		case model_prop_face_forward:
			draw->face_forward=JSVAL_TO_BOOLEAN(*vp);
			break;
	}
	
	return(JS_TRUE);
}

