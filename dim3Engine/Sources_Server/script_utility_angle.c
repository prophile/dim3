/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.angle object

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

JSBool js_utility_angle_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_angle_sub_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_angle_dif_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			utility_angle_class={"utility_angle_class",0,
							script_add_property,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	utility_angle_functions[]={
							{"add",					js_utility_angle_add_func,			2},
							{"sub",					js_utility_angle_sub_func,			2},
							{"dif",					js_utility_angle_dif_func,			2},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_utility_angle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"angle",&utility_angle_class,NULL,utility_angle_functions);
}

/* =======================================================

      Angle Functions
      
======================================================= */

JSBool js_utility_angle_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			ang1,ang2,rang;
    
    ang1=script_value_to_float(argv[0]);
	ang2=script_value_to_float(argv[1]);
	
	rang=angle_add(ang1,ang2);
    *rval=script_float_to_value(rang);
	
	return(JS_TRUE);
}

JSBool js_utility_angle_sub_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			ang1,ang2,rang;
	bool			cwise;

    ang1=script_value_to_float(argv[0]);
    ang2=script_value_to_float(argv[1]);
	
	rang=angle_dif(ang1,ang2,&cwise);
	if (!cwise) {
		rang=-rang;
	}
    *rval=script_float_to_value(rang);
	
	return(JS_TRUE);
}

JSBool js_utility_angle_dif_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			ang1,ang2,rang;
	bool			cwise;

    ang1=script_value_to_float(argv[0]);
    ang2=script_value_to_float(argv[1]);
	
	rang=angle_dif(ang1,ang2,&cwise);
    *rval=script_float_to_value(rang);
	
	return(JS_TRUE);
}
