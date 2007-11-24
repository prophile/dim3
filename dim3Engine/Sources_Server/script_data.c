/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: data object

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

extern js_type			js;

JSBool js_data_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_data_sub_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_data_set_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_data_get_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_data_add_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_data_sub_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_data_set_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_data_get_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			data_class={"data_class",0,
							script_add_property,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	data_functions[]={
							{"add",					js_data_add_func,					2},
							{"sub",					js_data_sub_func,					1},
							{"set",					js_data_set_func,					2},
							{"get",					js_data_get_func,					1},
							{"addScriptSpecific",	js_data_add_specific_func,			2},
							{"subScriptSpecific",	js_data_sub_specific_func,			1},
							{"setScriptSpecific",	js_data_set_specific_func,			2},
							{"getScriptSpecific",	js_data_get_specific_func,			1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_global_data_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"data",&data_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,data_functions);
}

/* =======================================================

      Data Functions
      
======================================================= */

JSBool js_data_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_add_global(name,-1,argv[1]);
	
	return(JS_TRUE);
}

JSBool js_data_sub_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_delete_global(name,-1);
	
	return(JS_TRUE);
}

JSBool js_data_set_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_set_global(name,-1,argv[1]);

	return(JS_TRUE);
}

JSBool js_data_get_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	*rval=script_get_global(name,-1);
	
	return(JS_TRUE);
}

JSBool js_data_add_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_add_global(name,js.attach.script_uid,argv[1]);
	
	return(JS_TRUE);
}

JSBool js_data_sub_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_delete_global(name,js.attach.script_uid);
	
	return(JS_TRUE);
}

JSBool js_data_set_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_set_global(name,js.attach.script_uid,argv[1]);

	return(JS_TRUE);
}

JSBool js_data_get_specific_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	*rval=script_get_global(name,js.attach.script_uid);
	
	return(JS_TRUE);
}

