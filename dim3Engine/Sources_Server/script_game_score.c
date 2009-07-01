/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: game.score object

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
#include "interfaces.h"

extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int				game_obj_rule_uid;

JSBool js_game_score_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_score_get_kill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_score_get_death(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_score_get_suicide(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_score_get_goal(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_score_set_score_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			game_score_class={"game_score_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	game_score_props[]={
							{"objectId",			js_game_score_get_objectId,			NULL},
							{"kill",				js_game_score_get_kill,				NULL},
							{"death",				js_game_score_get_death,			NULL},
							{"suicide",				js_game_score_get_suicide,			NULL},
							{"goal",				js_game_score_get_goal,				NULL},
							{0}};
							
script_js_function	game_score_functions[]={
							{"setScore",			js_game_score_set_score_func,		1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_game_score_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"score",&game_score_class,game_score_props,game_score_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_game_score_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=INT_TO_JSVAL(obj->uid);
	
	return(JS_TRUE);
}

JSBool js_game_score_get_kill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=INT_TO_JSVAL(obj->score.kill);
	
	return(JS_TRUE);
}

JSBool js_game_score_get_death(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=INT_TO_JSVAL(obj->score.death);
	
	return(JS_TRUE);
}

JSBool js_game_score_get_suicide(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=INT_TO_JSVAL(obj->score.suicide);
	
	return(JS_TRUE);
}

JSBool js_game_score_get_goal(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=INT_TO_JSVAL(obj->score.goal);
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_game_score_set_score_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

		// update score

	obj=object_find_uid(game_obj_rule_uid);
	obj->score.score=JSVAL_TO_INT(argv[0]);

		// have we hit a network score limit?

	if (net_setup.client.joined) score_limit_trigger_set_check_scores();

	return(JS_TRUE);
}

