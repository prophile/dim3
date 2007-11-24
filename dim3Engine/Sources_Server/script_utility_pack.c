/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.pack object

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

JSBool js_utility_pack_pack_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_pack_unpack_high_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_pack_unpack_low_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			utility_pack_class={"utility_pack_class",0,
							script_add_property,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	utility_pack_functions[]={
							{"pack",				js_utility_pack_pack_func,				2},
							{"unpackHigh",			js_utility_pack_unpack_high_func,		1},
							{"unpackLow",			js_utility_pack_unpack_low_func,		1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_utility_pack_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"pack",&utility_pack_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,utility_pack_functions);
}

/* =======================================================

      Random Functions
      
======================================================= */

JSBool js_utility_pack_pack_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	unsigned long			i,high,low;

	high=(unsigned long)JSVAL_TO_INT(argv[0]);
	low=(unsigned long)JSVAL_TO_INT(argv[1]);
	i=(int)(((high<<16)&0xFFFF0000)|(low&0xFFFF));

	*rval=INT_TO_JSVAL(i);
	
	return(JS_TRUE);
}

JSBool js_utility_pack_unpack_high_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	unsigned int			i;
	int						high;
	
	i=(unsigned int)JSVAL_TO_INT(argv[0]);
	high=(int)((i>>16)&0xFFFF);
	
	*rval=INT_TO_JSVAL(high);
	
	return(JS_TRUE);
}

JSBool js_utility_pack_unpack_low_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	unsigned int			i;
	int						low;
	
	i=(unsigned int)JSVAL_TO_INT(argv[0]);
	low=(int)(i&0xFFFF);
	
	*rval=INT_TO_JSVAL(low);
	
	return(JS_TRUE);
}
