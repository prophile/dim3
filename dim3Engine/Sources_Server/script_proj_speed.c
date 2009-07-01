/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.speed object

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

JSBool js_get_proj_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_proj_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			proj_speed_class={"proj_speed_class",0,
							script_add_property,JS_PropertyStub,
							js_get_proj_speed_property,js_set_proj_speed_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	proj_speed_props[]={
							{"speed",					js_proj_speed_get_speed,					js_proj_speed_set_speed},
							{"deceleration",			js_proj_speed_get_deceleration,				js_proj_speed_set_deceleration},
							{"decelerationWait",		js_proj_speed_get_decelerationWait,			js_proj_speed_set_decelerationWait},
							{"decelerationMinSpeed",	js_proj_speed_get_decelerationMinSpeed,		js_proj_speed_set_decelerationMinSpeed},
							{"maxHitScanDistance",		js_proj_speed_get_maxHitscanDistance,		js_proj_speed_set_maxHitscanDistance},
							{"inheritMotionFactor",		js_proj_speed_get_inheritMotionFactor,		js_proj_speed_set_inheritMotionFactor},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_speed_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"speed",&proj_speed_class,proj_speed_props,NULL);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_proj_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case proj_speed_prop_speed:
            *vp=script_float_to_value(proj_setup->speed);
			break;
		case proj_speed_prop_deceleration:
            *vp=script_float_to_value(proj_setup->decel_speed);
			break;
		case proj_speed_prop_deceleration_wait:
			*vp=INT_TO_JSVAL(proj_setup->decel_grace);
			break;
		case proj_speed_prop_deceleration_min_speed:
            *vp=script_float_to_value(proj_setup->decel_min_speed);
			break;
		case proj_speed_prop_max_hitscan_distance:
			*vp=INT_TO_JSVAL(proj_setup->hitscan.max_dist);
			break;
		case proj_speed_prop_inherit_motion_factor:
			*vp=script_float_to_value(proj_setup->inherit_motion_factor);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_proj_speed_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {

		case proj_speed_prop_speed:
            proj_setup->speed=script_value_to_float(*vp);
			break;
		case proj_speed_prop_deceleration:
			proj_setup->decel_speed=script_value_to_float(*vp);
			break;
		case proj_speed_prop_deceleration_wait:
			proj_setup->decel_grace=JSVAL_TO_INT(*vp);
			break;
		case proj_speed_prop_deceleration_min_speed:
            proj_setup->decel_min_speed=script_value_to_float(*vp);
			break;
		case proj_speed_prop_max_hitscan_distance:
			proj_setup->hitscan.max_dist=JSVAL_TO_INT(*vp);
			break;
		case proj_speed_prop_inherit_motion_factor:
			proj_setup->inherit_motion_factor=script_value_to_float(*vp);
			break;
         
	}
	
	return(JS_TRUE);
}



