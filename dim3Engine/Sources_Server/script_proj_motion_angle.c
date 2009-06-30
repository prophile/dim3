/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.motionAngle object

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
#include "projectiles.h"

extern js_type			js;

JSBool js_get_proj_motion_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			proj_motion_angle_class={"proj_motion_angle_class",0,
							script_add_property,JS_PropertyStub,
							js_get_proj_motion_angle_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	proj_motion_angle_props[]={
							{"x",					proj_motion_angle_prop_x,			TRUE},
							{"y",					proj_motion_angle_prop_y,			TRUE},
							{"z",					proj_motion_angle_prop_z,			TRUE},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_motion_angle_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"motionAngle",&proj_motion_angle_class,proj_motion_angle_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_proj_motion_angle_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_type			*proj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case proj_motion_angle_prop_x:
			*vp=script_float_to_value(proj->motion.ang.x);
			break;
		case proj_motion_angle_prop_y:
			*vp=script_float_to_value(proj->motion.ang.y);
			break;
		case proj_motion_angle_prop_z:
			*vp=script_float_to_value(proj->motion.ang.z);
			break;
			
	}
	
	return(JS_TRUE);
}

