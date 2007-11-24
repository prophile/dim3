/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.bitmap object

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
#include "interfaces.h"

extern js_type			js;

JSBool js_interface_bitmap_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_hide_all_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_move_relative_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_resize_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_repeat_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_flash_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_set_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bitmap_start_fade_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			interface_bitmap_class={"interface_bitmap_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	interface_bitmap_functions[]={
							{"show",				js_interface_bitmap_show_func,			1},
							{"hide",				js_interface_bitmap_hide_func,			1},
							{"hideAll",				js_interface_bitmap_hide_all_func,		0},
							{"move",				js_interface_bitmap_move_func,			3},
							{"moveRelative",		js_interface_bitmap_move_relative_func,	3},
							{"resize",				js_interface_bitmap_resize_func,		3},
							{"setAlpha",			js_interface_bitmap_set_alpha_func,		2},
							{"setRepeatCount",		js_interface_bitmap_set_repeat_count_func,2},
							{"setFlash",			js_interface_bitmap_set_flash_func,		2},
							{"setRotate",			js_interface_bitmap_set_rotate_func,	2},
							{"startFade",			js_interface_bitmap_start_fade_func,	1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_bitmap_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"bitmap",&interface_bitmap_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,interface_bitmap_functions);
}

/* =======================================================

      Bitmap Functions
      
======================================================= */

JSBool js_interface_bitmap_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->show=TRUE;
	bitmap->show_tick=js.time.current_tick;		// use to synch animations with shows
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->show=FALSE;
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_hide_all_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmaps_hide_all();
	return(JS_TRUE);
}

JSBool js_interface_bitmap_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->x=JSVAL_TO_INT(argv[1]);
	bitmap->y=JSVAL_TO_INT(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_move_relative_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->x+=JSVAL_TO_INT(argv[1]);
	bitmap->y+=JSVAL_TO_INT(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_resize_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->x_size=JSVAL_TO_INT(argv[1]);
	bitmap->y_size=JSVAL_TO_INT(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->alpha=script_value_to_float(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_repeat_count_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->repeat.count=JSVAL_TO_INT(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_flash_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->flash=JSVAL_TO_BOOLEAN(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_set_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->rot=script_value_to_float(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bitmap_start_fade_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bitmap_type			*bitmap;
	
	bitmap=script_find_bitmap_from_name(argv[0]);
	if (bitmap==NULL) return(JS_FALSE);
	
	bitmap->show=TRUE;
	bitmap->fade.on=TRUE;
	bitmap->fade.start_tick=js.time.current_tick;
	
	return(JS_TRUE);
}
