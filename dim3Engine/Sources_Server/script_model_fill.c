/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.fill object

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

JSBool js_model_fill_change_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

extern js_type			js;

JSClass			model_fill_class={"model_fill_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	model_fill_functions[]={
							{"change",				js_model_fill_change_func,				2},
							{0}};

extern model_draw* js_find_model_draw(JSObject *j_obj,bool is_child);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_model_fill_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"fill",&model_fill_class,NULL,model_fill_functions);
}

/* =======================================================

      Model Fill Functions
      
======================================================= */

JSBool js_model_fill_change_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    model_draw		*draw;
	
	draw=js_find_model_draw(j_obj,TRUE);
    
    model_change_fill(draw,JSVAL_TO_INT(argv[0]),JSVAL_TO_INT(argv[1]));
	return(JS_TRUE);
}

