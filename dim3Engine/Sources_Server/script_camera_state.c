/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.state object

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
#include "consoles.h"
#include "cameras.h"

extern js_type			js;

JSBool js_camera_state_save_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_camera_state_restore_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			camera_state_class={"camera_state_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	camera_state_functions[]={
							{"save",				js_camera_state_save_func,				0},
							{"restore",				js_camera_state_restore_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_state_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"state",&camera_state_class,NULL,camera_state_functions);
}

/* =======================================================

      State Function
      
======================================================= */

JSBool js_camera_state_save_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	camera_save();
	return(JS_TRUE);
}

JSBool js_camera_state_restore_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	camera_restore();
	return(JS_TRUE);
}

