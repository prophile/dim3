/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.group object

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

extern map_type			map;
extern js_type			js;

JSBool js_map_group_get_center_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_group_set_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_group_set_solid_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_group_set_texture_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_group_set_texture_shift_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_group_set_texture_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			map_group_class={"map_group_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	map_group_functions[]={
							{"getCenter",			js_map_group_get_center_func,			1},
							{"setShow",				js_map_group_set_show_func,				2},
							{"setSolid",			js_map_group_set_solid_func,			2},
							{"setTexture",			js_map_group_set_texture_func,			2},
							{"setTextureShift",		js_map_group_set_texture_shift_func,	3},
							{"setTextureAlpha",		js_map_group_set_texture_alpha_func,	2},
							{0}};

extern void group_show(int group_idx,bool show);
extern void group_solid(int group_idx,bool solid);
extern void group_texture(int group_idx,int index);
extern void group_texture_shift(int group_idx,float x_shift,float y_shift);
extern void group_texture_alpha(int group_idx,float alpha);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_group_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"group",&map_group_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,map_group_functions);

		// depreciated; here for backwards compatiability

	j_obj=JS_DefineObject(js.cx,parent_obj,"segment",&map_group_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,map_group_functions);
}

/* =======================================================

      Information Functions
      
======================================================= */

JSBool js_map_group_get_center_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	group_type		*group;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	if ((group_idx>=0) && (group_idx<map.ngroup)) {
		group=&map.groups[group_idx];
		*rval=script_point_to_value(group->center_pnt.x,group->center_pnt.y,group->center_pnt.z);
	}
	else {
		*rval=script_point_to_value(0,0,0);
	}

	return(JS_TRUE);
}

/* =======================================================

      Show and Solid Function
      
======================================================= */

JSBool js_map_group_set_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	group_show(group_idx,JSVAL_TO_BOOLEAN(argv[1]));
	return(JS_TRUE);
}

JSBool js_map_group_set_solid_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	group_solid(group_idx,JSVAL_TO_BOOLEAN(argv[1]));
	return(JS_TRUE);
}

/* =======================================================

      Texture Function
      
======================================================= */

JSBool js_map_group_set_texture_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	group_texture(group_idx,JSVAL_TO_INT(argv[1]));
	return(JS_TRUE);
}

JSBool js_map_group_set_texture_shift_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	group_texture_shift(group_idx,script_value_to_float(argv[1]),script_value_to_float(argv[2]));
	return(JS_TRUE);
}

JSBool js_map_group_set_texture_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	group_texture_alpha(group_idx,script_value_to_float(argv[1]));
	return(JS_TRUE);
}

