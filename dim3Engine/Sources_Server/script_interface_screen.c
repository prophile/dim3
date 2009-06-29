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

JSBool js_get_interface_screen_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			interface_screen_class={"interface_screen_class",0,
							script_add_property,JS_PropertyStub,
							js_get_interface_screen_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	interface_screen_props[]={
							{"width",				interface_screen_prop_width,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"height",				interface_screen_prop_height,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_screen_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"screen",&interface_screen_class,interface_screen_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_interface_screen_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case interface_screen_prop_width:
			*vp=INT_TO_JSVAL(setup.screen.x_sz);
			break;
		case interface_screen_prop_height:
			*vp=INT_TO_JSVAL(setup.screen.y_sz);
			break;
	}

	return(JS_TRUE);
}
