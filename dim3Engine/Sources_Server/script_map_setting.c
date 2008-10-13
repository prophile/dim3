/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.setting object

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

extern void map_set_ambient(char *name,float pitch);
extern void map_clear_ambient(void);

JSBool js_get_map_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_map_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			map_setting_class={"map_setting_class",0,
							script_add_property,JS_PropertyStub,
							js_get_map_setting_property,js_set_map_setting_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	map_setting_props[]={
							{"scale",				map_setting_prop_scale,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"gravity",				map_setting_prop_gravity,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"resistance",			map_setting_prop_resistance,		JSPROP_PERMANENT|JSPROP_SHARED},
							{"multiplayer",			map_setting_prop_multiplayer,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"multiplayerType",		map_setting_prop_multiplayer_type,	JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	map_setting_functions[]={
							{"setAmbient",			js_map_set_ambient_func,			2},
							{"clearAmbient",		js_map_clear_ambient_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_setting_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"setting",&map_setting_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,map_setting_props);
	JS_DefineFunctions(js.cx,j_obj,map_setting_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_map_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case map_setting_prop_scale:
            *vp=INT_TO_JSVAL(map_enlarge);
			break;
		case map_setting_prop_gravity:
            *vp=script_float_to_value(map.settings.gravity);
			break;
		case map_setting_prop_resistance:
            *vp=script_float_to_value(map.settings.resistance);
			break;
		case map_setting_prop_multiplayer:
            *vp=BOOLEAN_TO_JSVAL((net_setup.host.hosting) || (net_setup.client.joined));
			break;
		case map_setting_prop_multiplayer_type:
			if ((!net_setup.host.hosting) && (!net_setup.client.joined)) {
				*vp=JSVAL_NULL;
			}
			else {
				*vp=script_string_to_value(net_setup.client.game_name);
			}
			break;

	}
	
	return(JS_TRUE);
}

JSBool js_set_map_setting_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case map_setting_prop_gravity:
            map.settings.gravity=script_value_to_float(*vp);
            break;
		case map_setting_prop_resistance:
            map.settings.resistance=script_value_to_float(*vp);
 			break;

	}
	
	return(JS_TRUE);
}

/* =======================================================

      Methods
      
======================================================= */

JSBool js_map_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			pitch;
	char			name[name_str_len];
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[1]);
	
	map_set_ambient(name,pitch);
	
	return(JS_TRUE);
}

JSBool js_map_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	map_clear_ambient();
	return(JS_TRUE);
}

