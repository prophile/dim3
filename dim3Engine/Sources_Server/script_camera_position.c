/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.position object

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
#include "cameras.h"

extern camera_type		camera;
extern view_type		view;
extern js_type			js;

JSBool js_get_camera_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			camera_position_class={"camera_position_class",JSCLASS_HAS_PRIVATE,
							script_add_property,JS_PropertyStub,
							js_get_camera_position_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	camera_position_props[]={
							{"x",					camera_position_prop_x,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"z",					camera_position_prop_z,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"y",					camera_position_prop_y,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_camera_position_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"position",&camera_position_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,camera_position_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_camera_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case camera_position_prop_x:
			*vp=INT_TO_JSVAL(view.render->camera.pnt.x);
			break;
		case camera_position_prop_z:
			*vp=INT_TO_JSVAL(view.render->camera.pnt.z);
			break;
		case camera_position_prop_y:
			*vp=INT_TO_JSVAL(view.render->camera.pnt.y);
			break;

	}

	return(JS_TRUE);
}

