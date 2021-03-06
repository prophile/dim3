/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

extern js_type			js;

JSClass			camera_class={"camera_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

extern void script_add_camera_setting_object(JSObject *parent_obj);
extern void script_add_camera_position_object(JSObject *parent_obj);
extern void script_add_camera_angle_object(JSObject *parent_obj);
extern void script_add_camera_chase_object(JSObject *parent_obj);
extern void script_add_camera_chase_angle_object(JSObject *parent_obj);
extern void script_add_camera_chase_offset_object(JSObject *parent_obj);
extern void script_add_camera_chase_slop_object(JSObject *parent_obj);
extern void script_add_camera_static_position_object(JSObject *parent_obj);
extern void script_add_camera_plane_object(JSObject *parent_obj);
extern void script_add_camera_state_object(JSObject *parent_obj);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_global_camera_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"camera",&camera_class,NULL,0);
	
	script_add_camera_setting_object(j_obj);
	script_add_camera_position_object(j_obj);
	script_add_camera_angle_object(j_obj);
	script_add_camera_chase_object(j_obj);
	script_add_camera_chase_angle_object(j_obj);
	script_add_camera_chase_offset_object(j_obj);
	script_add_camera_chase_slop_object(j_obj);
	script_add_camera_static_position_object(j_obj);
	script_add_camera_plane_object(j_obj);
	script_add_camera_state_object(j_obj);
}

