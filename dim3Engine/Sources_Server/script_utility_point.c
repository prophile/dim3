/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.point object

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

extern js_type			js;

extern void view_script_transform_3D_to_2D(int x,int y,int z,int *x2,int *y2);

JSBool js_utility_point_equal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_point_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_point_distance_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_point_transform_3D_to_2D_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			utility_point_class={"utility_point_class",0,
							script_add_property,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	utility_point_functions[]={
							{"equal",				js_utility_point_equal_func,				7},
							{"angleTo",				js_utility_point_angle_to_func,				4},
							{"distanceTo",			js_utility_point_distance_to_func,			6},
							{"transform3Dto2D",		js_utility_point_transform_3D_to_2D_func,	3},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_utility_point_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"point",&utility_point_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,utility_point_functions);
}

/* =======================================================

      Point Functions
      
======================================================= */

JSBool js_utility_point_equal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y,to_x,to_z,to_y,slop;

	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);
	to_x=JSVAL_TO_INT(argv[3]);
	to_z=JSVAL_TO_INT(argv[4]);
	to_y=JSVAL_TO_INT(argv[5]);
	slop=JSVAL_TO_INT(argv[6]);
	
	if ((x<(to_x-slop)) || (x>(to_x+slop)) || (z<(to_z-slop)) || (z>(to_z+slop)) || (y<(to_y-slop)) || (y>(to_y+slop))) {
		*rval=JSVAL_FALSE;
	}
	else {
		*rval=JSVAL_TRUE;
	}
	
	return(JS_TRUE);
}

JSBool js_utility_point_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,to_x,to_z;
	float			ang;
	
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	to_x=JSVAL_TO_INT(argv[2]);
	to_z=JSVAL_TO_INT(argv[3]);
	
	ang=angle_find(x,z,to_x,to_z);
    *rval=script_float_to_value(ang);
	
	return(JS_TRUE);
}

JSBool js_utility_point_distance_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				dist,x,z,y,to_x,to_z,to_y;
	
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);
	to_x=JSVAL_TO_INT(argv[3]);
	to_z=JSVAL_TO_INT(argv[4]);
	to_y=JSVAL_TO_INT(argv[5]);

	dist=distance_get(x,y,z,to_x,to_y,to_z);
	*rval=INT_TO_JSVAL(dist);
	
	return(JS_TRUE);
}

JSBool js_utility_point_transform_3D_to_2D_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y,x2,y2;
	
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);
	
	view_script_transform_3D_to_2D(x,y,z,&x2,&y2);

	*rval=script_point_to_value(x2,y2,z);
	
	return(JS_TRUE);
}



