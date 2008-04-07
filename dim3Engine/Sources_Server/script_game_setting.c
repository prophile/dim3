/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: game.setting object

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

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

JSBool js_get_game_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			game_setting_class={"game_setting_class",0,
							script_add_property,JS_PropertyStub,
							js_get_game_setting_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	game_setting_props[]={
							{"type",				game_setting_prop_type,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"multiplayer",			game_setting_prop_multiplayer,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"skill",				game_setting_prop_skill,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_game_setting_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"setting",&game_setting_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,game_setting_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_game_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case game_setting_prop_type:
			if (!net_setup.client.joined) {
				*vp=JSVAL_NULL;
			}
			else {
				*vp=INT_TO_JSVAL(net_setup.client.game_name);
			}
			break;
		case game_setting_prop_multiplayer:
            *vp=BOOLEAN_TO_JSVAL(net_setup.client.joined);
			break;
		case game_setting_prop_skill:
            *vp=INT_TO_JSVAL(server.skill);
			break;

	}
	
	return(JS_TRUE);
}

