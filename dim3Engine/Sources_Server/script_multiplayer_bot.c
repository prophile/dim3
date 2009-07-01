/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: multiplayer.bot object

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

#include "objects.h"
#include "effects.h"
#include "scripts.h"
#include "physics.h"

extern server_type		server;
extern js_type			js;
extern setup_type		setup;

JSBool js_multiplayer_bot_get_skill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_bot_get_from_min_max_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			multiplayer_bot_class={"multiplayer_bot_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	multiplayer_bot_props[]={
							{"skill",				js_multiplayer_bot_get_skill,				NULL},
							{0}};
							
script_js_function	multiplayer_bot_functions[]={
							{"getFromMinMax",		js_multiplayer_bot_get_from_min_max_func,	2},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_multiplayer_bot_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"bot",&multiplayer_bot_class,multiplayer_bot_props,multiplayer_bot_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_multiplayer_bot_get_skill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=INT_TO_JSVAL(server.skill);
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_multiplayer_bot_get_from_min_max_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			skill,min,max;

	min=script_value_to_float(argv[0]);
	max=script_value_to_float(argv[1]);
	
	skill=(float)setup.network.bot.skill;

	*rval=script_float_to_value(min+(((max-min)*skill)/4.0f));
	
	return(JS_TRUE);
}

