/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.held object

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
#include "objects.h"

extern server_type		server;
extern js_type			js;

JSBool js_obj_held_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_held_drop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_held_class={"obj_held_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	obj_held_functions[]={
							{"add",					js_obj_held_add_func,				4},
							{"drop",				js_obj_held_drop_func,				3},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_held_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"held",&obj_held_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,obj_held_functions);
}

/* =======================================================

      Add/Drop Functions
      
======================================================= */

JSBool js_obj_held_add_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int			uid;
	char		name[name_str_len],type[name_str_len],
				script[file_str_len],params[param_str_len],
				err_str[256];
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	script_value_to_string(argv[0],name,name_str_len);
	script_value_to_string(argv[1],type,name_str_len);
	script_value_to_string(argv[2],script,file_str_len);
	script_value_to_string(argv[3],params,param_str_len);

	uid=object_held_add(obj,name,type,script,params,err_str);
	if (uid==-1) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	*rval=INT_TO_JSVAL(uid);
	
	return(JS_TRUE);
}

JSBool js_obj_held_drop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int			uid,y_change;
	float		y_ang;
	char		err_str[256];
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	uid=JSVAL_TO_INT(argv[0]);
	y_ang=script_value_to_float(argv[1]);
	y_change=JSVAL_TO_INT(argv[2]);

	if (!object_held_drop(obj,uid,y_ang,y_change,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

