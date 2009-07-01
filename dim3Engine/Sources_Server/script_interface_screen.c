/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.screen object

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
extern setup_type		setup;

JSBool js_interface_screen_get_width(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_screen_get_height(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			interface_screen_class={"interface_screen_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	interface_screen_props[]={
							{"width",				js_interface_screen_get_width,		NULL},
							{"height",				js_interface_screen_get_height,		NULL},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_screen_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"screen",&interface_screen_class,interface_screen_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_interface_screen_get_width(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(setup.screen.x_sz);
	return(JS_TRUE);
}


JSBool js_interface_screen_get_height(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(setup.screen.y_sz);
	return(JS_TRUE);
}
