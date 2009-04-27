/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.console object

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
#include "consoles.h"

extern char				console_input_str[max_console_txt_sz];
extern js_type			js;

JSBool js_interface_console_write_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_console_read_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			interface_console_class={"interface_console_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	interface_console_functions[]={
							{"write",				js_interface_console_write_func,		1},
							{"read",				js_interface_console_read_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_console_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"console",&interface_console_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,interface_console_functions);
}

/* =======================================================

      Console Function
      
======================================================= */

JSBool js_interface_console_write_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			txt[256];
	
	script_value_to_string(argv[0],txt,256);
	console_add(txt);
	
	return(JS_TRUE);
}

JSBool js_interface_console_read_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval=script_string_to_value(console_input_str);
	return(JS_TRUE);
}


