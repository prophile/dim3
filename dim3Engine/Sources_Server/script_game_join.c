/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: game.join object

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

extern js_type				js;
extern network_setup_type	net_setup;

extern int					game_obj_rule_uid;

JSBool js_get_game_join_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_join_set_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_game_join_set_team_even_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_game_join_clear_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_game_join_count_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_game_join_set_spawn_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_game_join_set_spawn_spot_to_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_game_join_clear_spawn_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			game_join_class={"game_join_class",0,
							script_add_property,JS_PropertyStub,
							js_get_game_join_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	game_join_props[]={
							{"name",				game_join_prop_name,					TRUE},
							{"team",				game_join_prop_team,					TRUE},
							{0}};
							
script_js_function	game_join_functions[]={
							{"setTeam",				js_game_join_set_team_func,					1},
							{"setTeamEven",			js_game_join_set_team_even_func,			0},
							{"clearTeam",			js_game_join_clear_team_func,				0},
							{"countTeam",			js_game_join_count_team_func,				1},
							{"setSpawnSpot",		js_game_join_set_spawn_spot_func,			1},
							{"setSpawnSpotToTeam",	js_game_join_set_spawn_spot_to_team_func,	0},
							{"clearSpawnSpot",		js_game_join_clear_spawn_spot_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_game_join_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"join",&game_join_class,game_join_props,game_join_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_game_join_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case game_join_prop_name:
            *vp=script_string_to_value(obj->name);
			break;
		case game_join_prop_team:
            *vp=INT_TO_JSVAL(obj->team_idx+sd_team_none);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Join Functions
      
======================================================= */

JSBool js_game_join_set_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	obj->team_idx=JSVAL_TO_INT(argv[0])-sd_team_none;

	return(JS_TRUE);
}

JSBool js_game_join_set_team_even_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	object_set_even_team(obj);

	return(JS_TRUE);
}

JSBool js_game_join_clear_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	obj->team_idx=net_team_none;

	return(JS_TRUE);
}

JSBool js_game_join_count_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int			team_idx;

	team_idx=JSVAL_TO_INT(argv[0])-sd_team_none;
	*rval=INT_TO_JSVAL(object_count_team(team_idx,game_obj_rule_uid));

	return(JS_TRUE);
}

JSBool js_game_join_set_spawn_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	script_value_to_string(argv[0],obj->spawn_spot_name,name_str_len);

	return(JS_TRUE);
}

JSBool js_game_join_set_spawn_spot_to_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	
	if (obj->team_idx==net_team_red) {
		strcpy(obj->spawn_spot_name,"Red");
	}
	else {
		strcpy(obj->spawn_spot_name,"Blue");
	}

	return(JS_TRUE);
}

JSBool js_game_join_clear_spawn_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	obj->spawn_spot_name[0]=0x0;

	return(JS_TRUE);
}
