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
JSBool js_map_node_find_nearest_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_node_find_nearest_names_in_path_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
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
							{"find",					js_map_node_find_func,							1},
							{"findNearestToObject",		js_map_node_find_nearest_to_object_func,		1},
							{"findNearestNamesInPath",	js_map_node_find_nearest_names_in_path_func,	2},
							{"nextInPath",				js_map_node_next_in_path_func,					2},
							{"getName",					js_map_node_get_name_func,						1},
							{"getDistance",				js_map_node_get_distance_func,					4},
							{"getAngleTo",				js_map_node_get_angle_to_func,					4},
							{"getPosition",				js_map_node_get_position_func,					1},
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

JSBool js_map_node_find_nearest_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx;
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
	idx=map_find_nearest_node_by_point(&map,&obj->pnt);
	*rval=INT_TO_JSVAL(idx);
	
	return(JS_TRUE);
}

JSBool js_map_node_find_nearest_names_in_path_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				n,idx,from_idx,k,len,d,dist;
	unsigned int	u_len;
	char			name[name_str_len];
	JSObject		*array;
	jsval			vp;
	
		// from node and name
		
	from_idx=JSVAL_TO_INT(argv[0]);
	
		// get number of array elements
	
	array=JSVAL_TO_OBJECT(argv[1]);
	JS_GetArrayLength(js.cx,array,&u_len);
	
	len=(int)u_len;
	
		// check all names
	
	idx=-1;
	dist=-1;
		
	for (n=0;n!=len;n++) {
		JS_GetElement(js.cx,array,n,&vp);
		script_value_to_string(vp,name,name_str_len);
	
		k=map_find_nearest_node_in_path(&map,from_idx,name,&d);
		if (k==-1) continue;
		
		if ((dist==-1) || (d<dist)) {
			dist=d;
			idx=k;
		}
	}
	
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

