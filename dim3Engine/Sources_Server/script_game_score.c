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

extern js_type			js;

extern int				game_obj_rule_uid;

JSBool js_get_game_score_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_score_set_score_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			game_score_class={"game_score_class",0,
							script_add_property,JS_PropertyStub,
							js_get_game_score_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	game_score_props[]={
							{"kill",				game_score_prop_kill,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"death",				game_score_prop_death,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"suicide",				game_score_prop_suicide,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"goal",				game_score_prop_goal,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};
							
JSFunctionSpec	game_score_functions[]={
							{"setScore",			js_game_score_set_score_func,		1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_game_score_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"score",&game_score_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,game_score_props);
	JS_DefineFunctions(js.cx,j_obj,game_score_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_game_score_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_score_prop_kill:
            *vp=INT_TO_JSVAL(obj->score.kill);
			break;
		case obj_score_prop_death:
            *vp=INT_TO_JSVAL(obj->score.death);
			break;
		case obj_score_prop_suicide:
            *vp=INT_TO_JSVAL(obj->score.suicide);
			break;
		case obj_score_prop_goal:
            *vp=INT_TO_JSVAL(obj->score.goal);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Score Functions
      
======================================================= */

JSBool js_game_score_set_score_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(JS_TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	obj->score.score=JSVAL_TO_INT(argv[0]);

	return(JS_TRUE);
}

