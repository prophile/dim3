/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.staticPosition object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "cameras.h"

extern map_type			map;
extern camera_type		camera;
extern js_type			js;

JSBool js_get_camera_static_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_camera_static_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_static_position_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_camera_static_position_move_to_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_camera_static_position_walk_to_node_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			camera_static_position_class={"camera_static_position_class",JSCLASS_HAS_PRIVATE,
							script_add_property,JS_PropertyStub,
							js_get_camera_static_position_property,js_set_camera_static_position_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	camera_static_position_props[]={
							{"follow",				camera_static_position_prop_follow,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"nodeSlop",			camera_static_position_prop_node_slop,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"walkTurnSpeed",		camera_static_position_prop_walk_turn_speed,	JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	camera_static_position_functions[]={
							{"move",				js_camera_static_position_move_func,			3},
							{"moveToSpot",			js_camera_static_position_move_to_spot_func,	1},
							{"walkToNode",			js_camera_static_position_walk_to_node_func,	6},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_static_position_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"staticPosition",&camera_static_position_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,camera_static_position_props);
	JS_DefineFunctions(js.cx,j_obj,camera_static_position_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_camera_static_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case camera_static_position_prop_follow:
            *vp=BOOLEAN_TO_JSVAL(camera.static_follow);
			break;
		case camera_static_position_prop_node_slop:
            *vp=INT_TO_JSVAL(camera.auto_walk.node_slop);
			break;
		case camera_static_position_prop_walk_turn_speed:
            *vp=script_float_to_value(camera.auto_walk.turn_speed);
			break;

	}

	return(JS_TRUE);
}

JSBool js_set_camera_static_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case camera_static_position_prop_follow:
            camera.static_follow=JSVAL_TO_BOOLEAN(*vp);
			break;
		case camera_static_position_prop_node_slop:
            camera.auto_walk.node_slop=JSVAL_TO_INT(*vp);
			break;
		case camera_static_position_prop_walk_turn_speed:
			camera.auto_walk.turn_speed=script_value_to_float(*vp);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Camera Static Functions
      
======================================================= */

JSBool js_camera_static_position_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	camera_static_update(JSVAL_TO_INT(argv[0]),JSVAL_TO_INT(argv[1]),JSVAL_TO_INT(argv[2]));
	
	return(JS_TRUE);
}

JSBool js_camera_static_position_move_to_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int			idx;
	spot_type	*spot;
	
	idx=JSVAL_TO_INT(argv[0]);
	if ((idx<0) || (idx>=map.nspot)) {
		JS_ReportError(js.cx,"Unknown spot");
		return(JS_FALSE);
	}
	
	spot=&map.spots[idx];
	camera_static_update(spot->pnt.x,spot->pnt.z,spot->pnt.y);
	
	return(JS_TRUE);
}

JSBool js_camera_static_position_walk_to_node_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msec,event_id;
	char			start_name[name_str_len],end_name[name_str_len];
	bool			open_doors,in_freeze;

	script_value_to_string(argv[0],start_name,name_str_len);
	script_value_to_string(argv[1],end_name,name_str_len);

	msec=JSVAL_TO_INT(argv[2]);
	event_id=JSVAL_TO_INT(argv[3]);
	open_doors=JSVAL_TO_BOOLEAN(argv[4]);
	in_freeze=JSVAL_TO_BOOLEAN(argv[5]);
		
	if (!camera_walk_to_node_setup(start_name,end_name,msec,event_id,open_doors,in_freeze)) return(JS_FALSE);

	return(JS_TRUE);
}
