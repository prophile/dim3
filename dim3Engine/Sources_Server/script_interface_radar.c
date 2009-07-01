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

JSBool js_interface_radar_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_get_displayRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_get_viewRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_set_displayRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_radar_set_viewRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			interface_radar_class={"interface_radar_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	interface_radar_props[]={
							{"on",					js_interface_radar_get_on,				js_interface_radar_set_on},
							{"x",					js_interface_radar_get_x,				js_interface_radar_set_x},
							{"y",					js_interface_radar_get_y,				js_interface_radar_set_y},
							{"displayRadius",		js_interface_radar_get_displayRadius,	js_interface_radar_set_displayRadius},
							{"viewRadius",			js_interface_radar_get_viewRadius,		js_interface_radar_set_viewRadius},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_radar_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"radar",&interface_radar_class,interface_radar_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_interface_radar_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
			*vp=BOOLEAN_TO_JSVAL(hud.radar.on);
	return(JS_TRUE);
}

JSBool js_interface_radar_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(hud.radar.x);
	return(JS_TRUE);
}

JSBool js_interface_radar_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(hud.radar.y);
	return(JS_TRUE);
}

JSBool js_interface_radar_get_displayRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(hud.radar.display_radius);
	return(JS_TRUE);
}

JSBool js_interface_radar_get_viewRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(hud.radar.view_radius);
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_interface_radar_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	hud.radar.on=JSVAL_TO_BOOLEAN(*vp);
	return(JS_TRUE);
}

JSBool js_interface_radar_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	hud.radar.x=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_interface_radar_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	hud.radar.y=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_interface_radar_set_displayRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	hud.radar.display_radius=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

JSBool js_interface_radar_set_viewRadius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	hud.radar.view_radius=JSVAL_TO_INT(*vp);
	return(JS_TRUE);
}

