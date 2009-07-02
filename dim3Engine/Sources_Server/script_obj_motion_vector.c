/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.motionVector object

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

JSBool js_get_obj_motion_vector_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_motion_vector_go_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_jump_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_alter_speed_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_alter_gravity_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_node_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_node_by_id_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_node_resume_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_node_reverse_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_walk_to_position_slop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_turn_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_motion_vector_turn_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_motion_vector_class={"obj_motion_vector_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_motion_vector_props[]={
							{"x",					js_obj_motion_vector_get_x,							NULL},
							{"y",					js_obj_motion_vector_get_y,							NULL},
							{"z",					js_obj_motion_vector_get_z,							NULL},
							{0}};
							
script_js_function	obj_motion_vector_functions[]={
							{"go",					js_obj_motion_vector_go_func,						0},
							{"stop",				js_obj_motion_vector_stop_func,						0},
							{"jump",				js_obj_motion_vector_jump_func,						0},
							{"alterSpeed",			js_obj_motion_vector_alter_speed_func,				1},
							{"alterGravity",		js_obj_motion_vector_alter_gravity_func,			1},
							{"walkToNode",			js_obj_motion_vector_walk_to_node_func,				3},
							{"walkToNodeById",		js_obj_motion_vector_walk_to_node_by_id_func,		3},
							{"walkToNodeResume",	js_obj_motion_vector_walk_to_node_resume_func,		0},
							{"walkToNodeReverse",	js_obj_motion_vector_walk_to_node_reverse_func,		0},
							{"walkToObject",		js_obj_motion_vector_walk_to_object_func,			1},
							{"walkToPlayer",		js_obj_motion_vector_walk_to_player_func,			0},
							{"walkToPosition",		js_obj_motion_vector_walk_to_position_func,			3},
							{"turnToObject",		js_obj_motion_vector_turn_to_object_func,			1},
							{"turnToPlayer",		js_obj_motion_vector_turn_to_player_func,			0},
							{0}};
	
/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_motion_vector_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"motionVector",&obj_motion_vector_class,obj_motion_vector_props,obj_motion_vector_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_motion_vector_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_motion_vector_prop_x:
			*vp=script_float_to_value(obj->motion.vct.x);
			break;
		case obj_motion_vector_prop_y:
			*vp=script_float_to_value(obj->motion.vct.y);
			break;
		case obj_motion_vector_prop_z:
			*vp=script_float_to_value(obj->motion.vct.z);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Stop and Go
      
======================================================= */

JSBool js_obj_motion_vector_go_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_move_start(obj);
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_move_stop(obj);
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_jump_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_start_jump(obj);
	return(JS_TRUE);
}

/* =======================================================

      Alterations
      
======================================================= */

JSBool js_obj_motion_vector_alter_speed_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
    object_alter_speed(obj,script_value_to_float(argv[0]));
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_alter_gravity_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
    object_alter_gravity(obj,script_value_to_float(argv[0]));
	
	return(JS_TRUE);
}

/* =======================================================

      Walk To
      
======================================================= */

JSBool js_obj_motion_vector_walk_to_node_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			start_name[name_str_len],end_name[name_str_len];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
		// get node names
		
	script_value_to_string(argv[0],start_name,name_str_len);
	script_value_to_string(argv[1],end_name,name_str_len);
	
		// start walk
		
	if (!object_auto_walk_node_name_setup(obj,start_name,end_name,JSVAL_TO_INT(argv[2]))) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_walk_to_node_by_id_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_node_setup(obj,JSVAL_TO_INT(argv[0]),JSVAL_TO_INT(argv[1]),JSVAL_TO_INT(argv[2]))) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_walk_to_node_resume_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_node_resume(obj)) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_walk_to_node_reverse_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_node_reverse(obj)) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_walk_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_object_setup(obj,JSVAL_TO_INT(argv[0]),FALSE)) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_walk_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_player_setup(obj,FALSE)) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_walk_to_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	d3pnt			pnt;
	obj_type		*obj;
	
	pnt.x=JSVAL_TO_INT(argv[0]);
	pnt.z=JSVAL_TO_INT(argv[1]);
	pnt.y=JSVAL_TO_INT(argv[2]);
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_position_setup(obj,&pnt)) return(JS_FALSE);
	
	return(JS_TRUE);
}

/* =======================================================

      Turn To
      
======================================================= */

JSBool js_obj_motion_vector_turn_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_object_setup(obj,JSVAL_TO_INT(argv[0]),TRUE)) return(JS_FALSE);
	
	return(JS_TRUE);
}

JSBool js_obj_motion_vector_turn_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	if (!object_auto_walk_player_setup(obj,TRUE)) return(JS_FALSE);
	
	return(JS_TRUE);
}
