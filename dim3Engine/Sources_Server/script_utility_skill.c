/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.skill object

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

extern setup_type		setup;
extern js_type			js;

JSBool js_utility_skill_get_from_min_max_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			utility_skill_class={"utility_skill_class",0,
							script_add_property,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	utility_skill_functions[]={
							{"getFromMinMax",		js_utility_skill_get_from_min_max_func,		2},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_utility_skill_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"skill",&utility_skill_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,utility_skill_functions);
}

/* =======================================================

      Skill Functions
      
======================================================= */

JSBool js_utility_skill_get_from_min_max_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			skill,min,max;

	min=script_value_to_float(argv[0]);
	max=script_value_to_float(argv[1]);
	
	skill=(float)setup.network.bot.skill;

	*rval=script_float_to_value(min+(((max-min)*skill)/4.0f));
	
	return(JS_TRUE);
}

