/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.watch object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

JSBool js_get_obj_watch_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_watch_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_watch_class={"obj_watch_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_watch_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_watch_props[]={
							{"objectId",				obj_watch_prop_object_id,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectName",				obj_watch_prop_object_name,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectIsPlayer",			obj_watch_prop_object_is_player,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectIsRemote",			obj_watch_prop_object_is_remote,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectIsBot",				obj_watch_prop_object_is_bot,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectIsPlayerRemoteBot",	obj_watch_prop_object_is_player_remote_bot,	JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"objectTeam",				obj_watch_prop_object_team,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"baseTeam",				obj_watch_prop_base_team,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"soundName",				obj_watch_prop_sound_name,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	obj_watch_functions[]={
							{"start",					js_obj_watch_start_func,			1},
							{"stop",					js_obj_watch_stop_func,				0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_watch_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"watch",&obj_watch_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_watch_props);
	JS_DefineFunctions(js.cx,j_obj,obj_watch_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_watch_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj,*watch_obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_watch_prop_object_id:
			*vp=INT_TO_JSVAL(obj->watch.obj_uid);
			break;
	
		case obj_watch_prop_object_name:
			watch_obj=object_find_uid(obj->watch.obj_uid);
			if (watch_obj==NULL) {
				*vp=JSVAL_NULL;
				break;
			}
			*vp=script_string_to_value(watch_obj->name);
			break;

		case obj_watch_prop_object_is_player:
			*vp=BOOLEAN_TO_JSVAL(obj->watch.obj_uid==server.player_obj_uid);
			break;

		case obj_watch_prop_object_is_remote:
			watch_obj=object_find_uid(obj->watch.obj_uid);
			if (watch_obj==NULL) {
				*vp=JSVAL_FALSE;
				break;
			}

			*vp=BOOLEAN_TO_JSVAL(watch_obj->remote.on);
			break;

		case obj_watch_prop_object_is_bot:
			watch_obj=object_find_uid(obj->watch.obj_uid);
			if (watch_obj==NULL) {
				*vp=JSVAL_FALSE;
				break;
			}

			*vp=BOOLEAN_TO_JSVAL(strcasecmp(watch_obj->type,"bot")==0);
			break;

		case obj_watch_prop_object_is_player_remote_bot:
			if (obj->watch.obj_uid==server.player_obj_uid) {
				*vp=JSVAL_TRUE;
				break;
			}

			watch_obj=object_find_uid(obj->watch.obj_uid);
			if (watch_obj==NULL) {
				*vp=JSVAL_FALSE;
				break;
			}

			*vp=BOOLEAN_TO_JSVAL((watch_obj->remote.on) || (strcasecmp(watch_obj->type,"bot")==0));
			break;

		case obj_watch_prop_object_team:
			watch_obj=object_find_uid(obj->watch.obj_uid);
			if (watch_obj==NULL) {
				*vp=INT_TO_JSVAL(sd_team_none);
				break;
			}
			*vp=INT_TO_JSVAL(watch_obj->team_idx+sd_team_none);
			break;
			
		case obj_watch_prop_base_team:
			*vp=INT_TO_JSVAL(obj->watch.base_team+sd_team_none);
			break;

		case obj_watch_prop_sound_name:
			*vp=script_string_to_value(obj->watch.sound_name);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Start/Stop Functions
      
======================================================= */

JSBool js_obj_watch_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.on=TRUE;
	obj->watch.dist=JSVAL_TO_INT(argv[0]);
	
	return(JS_TRUE);
}

JSBool js_obj_watch_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_clear_watch(&obj->watch);

	return(JS_TRUE);
}

