/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.mesh object

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
#include "models.h"

JSBool js_model_mesh_show_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_mesh_show_all_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_mesh_show_only_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_mesh_hide_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_mesh_hide_all_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

extern js_type			js;

JSClass			model_mesh_class={"model_mesh_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	model_mesh_functions[]={
							{"showMesh",			js_model_mesh_show_mesh_func,				1},
							{"showAllMesh",			js_model_mesh_show_all_mesh_func,			0},
							{"showOnlyMesh",		js_model_mesh_show_only_mesh_func,			1},
							{"hideMesh",			js_model_mesh_hide_mesh_func,				1},
							{"hideAllMesh",			js_model_mesh_hide_all_mesh_func,			0},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_mesh_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"mesh",&model_mesh_class,NULL,model_mesh_functions);
}

/* =======================================================

      Model Mesh Functions
      
======================================================= */

JSBool js_model_mesh_show_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_show_mesh(draw,name)) {
		JS_ReportError(js.cx,"Named mesh does not exist: %s",name);
		return(JS_FALSE);
	}
	
	return(JS_TRUE);
}

JSBool js_model_mesh_show_all_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);
	model_show_all_mesh(draw);

	return(JS_TRUE);
}

JSBool js_model_mesh_show_only_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_show_only_mesh(draw,name)) {
		JS_ReportError(js.cx,"Named mesh does not exist: %s",name);
		return(JS_FALSE);
	}
	
	return(JS_TRUE);
}

JSBool js_model_mesh_hide_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_hide_mesh(draw,name)) {
		JS_ReportError(js.cx,"Named mesh does not exist: %s",name);
		return(JS_FALSE);
	}
	
	return(JS_TRUE);
}

JSBool js_model_mesh_hide_all_mesh_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);
	model_hide_all_mesh(draw);

	return(JS_TRUE);
}

