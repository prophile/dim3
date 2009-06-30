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
JSBool js_obj_watch_set_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_watch_clear_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_watch_class={"obj_watch_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_watch_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	obj_watch_props[]={
							{"objectId",				obj_watch_prop_object_id,					TRUE},
							{"objectName",				obj_watch_prop_object_name,					TRUE},
							{"objectIsPlayer",			obj_watch_prop_object_is_player,			TRUE},
							{"objectIsRemote",			obj_watch_prop_object_is_remote,			TRUE},
							{"objectIsBot",				obj_watch_prop_object_is_bot,				TRUE},
							{"objectIsPlayerRemoteBot",	obj_watch_prop_object_is_player_remote_bot,	TRUE},
							{"objectTeam",				obj_watch_prop_object_team,					TRUE},
							{"baseTeam",				obj_watch_prop_base_team,					TRUE},
							{"soundName",				obj_watch_prop_sound_name,					TRUE},
							{0}};
							
script_js_function	obj_watch_functions[]={
							{"start",					js_obj_watch_start_func,					1},
							{"stop",					js_obj_watch_stop_func,						0},
							{"setRestrictSight",		js_obj_watch_set_restrict_sight_func,		1},
							{"clearRestrictSight",		js_obj_watch_clear_restrict_sight_func,		0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_watch_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"watch",&obj_watch_class,obj_watch_props,obj_watch_functions);
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

			*vp=BOOLEAN_TO_JSVAL(watch_obj->bot);
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

			*vp=BOOLEAN_TO_JSVAL((watch_obj->remote.on) || (watch_obj->bot));
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

/* =======================================================

      Sight Restriction Functions
      
======================================================= */

JSBool js_obj_watch_set_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.restrict_on=TRUE;
	obj->watch.restrict_ang=script_value_to_float(argv[0]);
	
	return(JS_TRUE);
}

JSBool js_obj_watch_clear_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.restrict_on=FALSE;
	
	return(JS_TRUE);
}
