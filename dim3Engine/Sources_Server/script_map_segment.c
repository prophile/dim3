/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.segment object

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

JSBool js_map_segment_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_move_over_time_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_move_over_time_on_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_move_over_time_freeze_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_move_over_time_thaw_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_get_center_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_set_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_set_solid_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_set_texture_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_set_texture_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_set_texture_shift_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_segment_set_texture_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			map_segment_class={"map_segment_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	map_segment_functions[]={
							{"move",				js_map_segment_move_func,				4},
							{"moveOverTime",		js_map_segment_move_over_time_func,		6},
							{"moveOverTimeOn",		js_map_segment_move_over_time_on_func,	1},
							{"moveOverTimeFreeze",	js_map_segment_move_over_time_freeze_func,1},
							{"moveOverTimeThaw",	js_map_segment_move_over_time_thaw_func,1},
							{"getCenter",			js_map_segment_get_center_func,			1},
							{"setShow",				js_map_segment_set_show_func,			2},
							{"setSolid",			js_map_segment_set_solid_func,			2},
							{"setTexture",			js_map_segment_set_texture_func,		2},
							{"setTextureOffset",	js_map_segment_set_texture_offset_func,	3},
							{"setTextureShift",		js_map_segment_set_texture_shift_func,	3},
							{"setTextureAlpha",		js_map_segment_set_texture_alpha_func,	2},
							{0}};

extern void segment_move(int group_idx,int xmove,int zmove,int ymove);
extern void segment_show(int group_idx,bool show);
extern void segment_solid(int group_idx,bool solid);
extern void segment_texture(int group_idx,int index);
extern void segment_texture_offset(int group_idx,float x_offset,float y_offset);
extern void segment_texture_shift(int group_idx,float x_shift,float y_shift);
extern void segment_texture_alpha(int group_idx,float alpha);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_segment_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"segment",&map_segment_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,map_segment_functions);
}

/* =======================================================

      Move Function
      
======================================================= */

JSBool js_map_segment_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	segment_move(group_idx,JSVAL_TO_INT(argv[1]),JSVAL_TO_INT(argv[2]),JSVAL_TO_INT(argv[3]));
	return(JS_TRUE);
}

JSBool js_map_segment_move_over_time_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	if (!script_move_add(group_idx,-1,JSVAL_TO_INT(argv[1]),JSVAL_TO_INT(argv[3]),JSVAL_TO_INT(argv[2]),(JSVAL_TO_INT(argv[4])*10),JSVAL_TO_INT(argv[5]))) return(JS_FALSE);
	return(JS_TRUE);
}

JSBool js_map_segment_move_over_time_on_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	if (script_move_on(group_idx)) {
		*rval=JSVAL_TRUE;
	}
	else {
		*rval=JSVAL_FALSE;
	}

	return(JS_TRUE);
}

JSBool js_map_segment_move_over_time_freeze_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	script_move_freeze(group_idx,TRUE);
	return(JS_TRUE);
}

JSBool js_map_segment_move_over_time_thaw_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	script_move_freeze(group_idx,FALSE);
	return(JS_TRUE);
}

/* =======================================================

      Information Functions
      
======================================================= */

JSBool js_map_segment_get_center_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx,x,y,z;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	map_group_get_center(&map,group_idx,&x,&y,&z);
	*rval=script_point_to_value(x,y,z);
	
	return(JS_TRUE);
}

/* =======================================================

      Show and Solid Function
      
======================================================= */

JSBool js_map_segment_set_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);
	
	segment_show(group_idx,JSVAL_TO_BOOLEAN(argv[1]));
	return(JS_TRUE);
}

JSBool js_map_segment_set_solid_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	segment_solid(group_idx,JSVAL_TO_BOOLEAN(argv[1]));
	return(JS_TRUE);
}

/* =======================================================

      Texture Function
      
======================================================= */

JSBool js_map_segment_set_texture_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	segment_texture(group_idx,JSVAL_TO_INT(argv[1]));
	return(JS_TRUE);
}

JSBool js_map_segment_set_texture_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	segment_texture_offset(group_idx,script_value_to_float(argv[1]),script_value_to_float(argv[2]));
	return(JS_TRUE);
}

JSBool js_map_segment_set_texture_shift_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	segment_texture_shift(group_idx,script_value_to_float(argv[1]),script_value_to_float(argv[2]));
	return(JS_TRUE);
}

JSBool js_map_segment_set_texture_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_segment_from_name(argv[0]);
	if (group_idx==-1) return(JS_FALSE);

	segment_texture_alpha(group_idx,script_value_to_float(argv[1]));
	return(JS_TRUE);
}

