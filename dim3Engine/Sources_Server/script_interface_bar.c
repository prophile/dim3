/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.bar object

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

JSBool js_interface_bar_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bar_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bar_hide_all_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bar_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bar_resize_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bar_set_value_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_bar_set_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			interface_bar_class={"interface_bar_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	interface_bar_functions[]={
							{"show",				js_interface_bar_show_func,				1},
							{"hide",				js_interface_bar_hide_func,				1},
							{"hideAll",				js_interface_bar_hide_all_func,			0},
							{"move",				js_interface_bar_move_func,				3},
							{"resize",				js_interface_bar_resize_func,			3},
							{"setValue",			js_interface_bar_set_value_func,		2},
							{"setAlpha",			js_interface_bar_set_alpha_func,		2},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_bar_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"bar",&interface_bar_class,NULL,interface_bar_functions);
}

/* =======================================================

      Bar Functions
      
======================================================= */

JSBool js_interface_bar_show_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bar_type			*bar;
	
	bar=script_find_bar_from_name(argv[0]);
	if (bar==NULL) return(JS_FALSE);
	
	bar->show=TRUE;
	
	return(JS_TRUE);
}

JSBool js_interface_bar_hide_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bar_type			*bar;
	
	bar=script_find_bar_from_name(argv[0]);
	if (bar==NULL) return(JS_FALSE);
	
	bar->show=FALSE;
	
	return(JS_TRUE);
}

JSBool js_interface_bar_hide_all_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bars_hide_all();
	return(JS_TRUE);
}

JSBool js_interface_bar_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bar_type			*bar;
	
	bar=script_find_bar_from_name(argv[0]);
	if (bar==NULL) return(JS_FALSE);
	
	bar->x=JSVAL_TO_INT(argv[1]);
	bar->y=JSVAL_TO_INT(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bar_resize_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bar_type			*bar;
	
	bar=script_find_bar_from_name(argv[0]);
	if (bar==NULL) return(JS_FALSE);
	
	bar->x_size=JSVAL_TO_INT(argv[1]);
	bar->y_size=JSVAL_TO_INT(argv[2]);
	
	return(JS_TRUE);
}

JSBool js_interface_bar_set_value_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bar_type			*bar;
	
	bar=script_find_bar_from_name(argv[0]);
	if (bar==NULL) return(JS_FALSE);
	
	bar->value=script_value_to_float(argv[1]);
	
	return(JS_TRUE);
}

JSBool js_interface_bar_set_alpha_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	hud_bar_type			*bar;
	
	bar=script_find_bar_from_name(argv[0]);
	if (bar==NULL) return(JS_FALSE);
	
	bar->fill_alpha=script_value_to_float(argv[1]);
	
	return(JS_TRUE);
}
