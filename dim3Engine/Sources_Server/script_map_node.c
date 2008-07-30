/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.node object

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

extern map_type			map;
extern js_type			js;

JSBool js_map_node_find_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_node_nearest_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_node_next_in_path_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_node_get_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_node_get_distance_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_node_get_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_node_get_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			map_node_class={"map_node_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	map_node_functions[]={
							{"find",				js_map_node_find_func,				1},
							{"nearest",				js_map_node_nearest_func,			8},
							{"nextInPath",			js_map_node_next_in_path_func,		2},
							{"getName",				js_map_node_get_name_func,			1},
							{"getDistance",			js_map_node_get_distance_func,		4},
							{"getAngleTo",			js_map_node_get_angle_to_func,		4},
							{"getPosition",			js_map_node_get_position_func,		1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_node_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"node",&map_node_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,map_node_functions);
}

/* =======================================================

      Find Node Functions
      
======================================================= */

JSBool js_map_node_find_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx;
	char			name[name_str_len];
	
	script_value_to_string(argv[0],name,name_str_len);
	idx=map_find_node(&map,name);
	*rval=INT_TO_JSVAL(idx);
	return(JS_TRUE);
}

JSBool js_map_node_nearest_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx,x,z,y,user_value,
					min_dist,max_dist;
	float			ang,ang_sweep;
	
		// x,z,y
		
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);
	
		// user value
		
	user_value=-1;
	if (argv[3]!=JSVAL_NULL) user_value=JSVAL_TO_INT(argv[3]);

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (argv[4]!=JSVAL_NULL) ang=script_value_to_float(argv[4]);
	if (argv[5]!=JSVAL_NULL) ang_sweep=script_value_to_float(argv[5]);
	
		// distances
		
	min_dist=JSVAL_TO_INT(argv[6]);
	max_dist=JSVAL_TO_INT(argv[7]);
	
		// find node

	idx=map_find_nearest_node(&map,x,y,z,user_value,ang,ang_sweep,min_dist,max_dist);
	*rval=INT_TO_JSVAL(idx);
	
	return(JS_TRUE);
}

JSBool js_map_node_next_in_path_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx;
	
	idx=map_find_next_node_in_path(&map,JSVAL_TO_INT(argv[0]),JSVAL_TO_INT(argv[1]));
	*rval=INT_TO_JSVAL(idx);

	return(JS_TRUE);
}

/* =======================================================

      Node Name
      
======================================================= */

JSBool js_map_node_get_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	node_type		*node;

		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(JS_FALSE);
	
		// get position
		
	*rval=script_string_to_value(node->name);
	return(JS_TRUE);
}

/* =======================================================

      Node Distance and Angle To
      
======================================================= */

JSBool js_map_node_get_distance_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y;
	node_type		*node;
	
		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(JS_FALSE);

		// x,z,y
		
	x=JSVAL_TO_INT(argv[1]);
	z=JSVAL_TO_INT(argv[2]);
	y=JSVAL_TO_INT(argv[3]);
	
		// get distance
		
	*rval=INT_TO_JSVAL(distance_get(node->pnt.x,node->pnt.y,node->pnt.z,x,y,z));
	
	return(JS_TRUE);
}

JSBool js_map_node_get_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y;
	float			ang_x,ang_z,ang_y;
	node_type		*node;
	
		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(JS_FALSE);

		// x,z,y
		
	x=JSVAL_TO_INT(argv[1]);
	z=JSVAL_TO_INT(argv[2]);
	y=JSVAL_TO_INT(argv[3]);
	
		// get angles to
		
	ang_x=angle_find(y,z,node->pnt.y,node->pnt.z);
	ang_y=angle_find(x,z,node->pnt.x,node->pnt.z);
	ang_z=angle_find(x,y,node->pnt.x,node->pnt.y);
		
	*rval=script_angle_to_value(ang_x,ang_y,ang_z);
	
	return(JS_TRUE);
}

/* =======================================================

      Node Position and Angle
      
======================================================= */

JSBool js_map_node_get_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	node_type		*node;

		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(JS_FALSE);
	
		// get position
		
	*rval=script_point_to_value(node->pnt.x,node->pnt.y,node->pnt.z);
	return(JS_TRUE);
}

