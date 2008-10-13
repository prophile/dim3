/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.action object

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
extern setup_type			setup;
extern network_setup_type	net_setup;
extern js_type				js;

JSBool js_map_action_set_map_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_action_set_host_map_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_action_restart_map_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_action_restart_map_from_save_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			map_action_class={"map_action_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	map_action_functions[]={
							{"setMap",				js_map_action_set_map_func,					3},
							{"setHostMap",			js_map_action_set_host_map_func,			0},
							{"restartMap",			js_map_action_restart_map_func,				0},
							{"restartMapFromSave",	js_map_action_restart_map_from_save_func,	0},
							{0}};

extern bool game_file_reload_ok(void);
extern bool game_file_reload(char *err_str);
extern void game_time_pause_end(void);

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_action_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"action",&map_action_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,map_action_functions);
}

/* =======================================================

      Map Functions
      
======================================================= */

JSBool js_map_action_set_map_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
		// clients can not set maps
		
	if (net_setup.client.joined) {
		JS_ReportError(cx,"setMap() illegal for client games");
		return(JS_FALSE);
	}
	
		// set the map
		
	script_value_to_string(argv[0],map.info.name,name_str_len);
	script_value_to_string(argv[1],map.info.player_start_name,name_str_len);
	script_value_to_string(argv[2],map.info.player_start_type,name_str_len);
	map.info.in_load=FALSE;

	server.map_change=TRUE;
	
	return(JS_TRUE);
}

JSBool js_map_action_set_host_map_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
		// clients can not set maps

	if ((!net_setup.host.hosting) && (!net_setup.client.joined)) {
		JS_ReportError(cx,"setHostMap() illegal for normal games");
		return(JS_FALSE);
	}

		// set the map
		
	strcpy(map.info.name,map.info.host_name);
	map.info.player_start_name[0]=0x0;
	map.info.player_start_type[0]=0x0;
	map.info.in_load=FALSE;

	server.map_change=TRUE;
	
	return(JS_TRUE);
}

JSBool js_map_action_restart_map_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
		// clients can not restart maps
		
	if (net_setup.client.joined) {
		JS_ReportError(cx,"restartMap() illegal for client games");
		return(JS_FALSE);
	}
	
		// use the last values to restart
		
	server.map_change=TRUE;
	
	return(JS_TRUE);
}

JSBool js_map_action_restart_map_from_save_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			err_str[256];
	
		// clients can not restart maps from saves
		
	if (net_setup.host.hosting) {
		JS_ReportError(cx,"restartMapFromSave() illegal for client games");
		return(JS_FALSE);
	}
	
		// if no save game file, restart from map
		
	if (!game_file_reload_ok()) {
		server.map_change=TRUE;
		return(JS_TRUE);
	}
	
		// else reload
	
	if (!game_file_reload(err_str)) {
		JS_ReportError(cx,"Reload failed (%s)",err_str);
		return(JS_FALSE);
	}
	
	game_time_pause_end();			// loaded files are in paused mode
	
	return(JS_TRUE);
}

