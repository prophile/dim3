/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.radar object

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
#include "interfaces.h"

extern js_type			js;
extern setup_type		setup;
extern hud_type			hud;

JSBool js_get_interface_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_interface_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			interface_radar_class={"interface_radar_class",0,
							script_add_property,JS_PropertyStub,
							js_get_interface_radar_property,js_set_interface_radar_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	interface_radar_props[]={
							{"on",					interface_radar_prop_on,				FALSE},
							{"x",					interface_radar_prop_x,					FALSE},
							{"y",					interface_radar_prop_y,					FALSE},
							{"displayRadius",		interface_radar_prop_display_radius,	FALSE},
							{"viewRadius",			interface_radar_prop_view_radius,		FALSE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_radar_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"radar",&interface_radar_class,interface_radar_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_interface_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case interface_radar_prop_on:
			*vp=BOOLEAN_TO_JSVAL(hud.radar.on);
			break;
		case interface_radar_prop_x:
			*vp=INT_TO_JSVAL(hud.radar.x);
			break;
		case interface_radar_prop_y:
			*vp=INT_TO_JSVAL(hud.radar.y);
			break;
		case interface_radar_prop_display_radius:
			*vp=INT_TO_JSVAL(hud.radar.display_radius);
			break;
		case interface_radar_prop_view_radius:
			*vp=INT_TO_JSVAL(hud.radar.view_radius);
			break;

	}

	return(JS_TRUE);
}

JSBool js_set_interface_radar_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {

		case interface_radar_prop_on:
			hud.radar.on=JSVAL_TO_BOOLEAN(*vp);
			break;
		case interface_radar_prop_x:
			hud.radar.x=JSVAL_TO_INT(*vp);
			break;
		case interface_radar_prop_y:
			hud.radar.y=JSVAL_TO_INT(*vp);
			break;
		case interface_radar_prop_display_radius:
			hud.radar.display_radius=JSVAL_TO_INT(*vp);
			break;
		case interface_radar_prop_view_radius:
			hud.radar.view_radius=JSVAL_TO_INT(*vp);
			break;

	}
	
	return(JS_TRUE);
}

