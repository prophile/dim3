/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.score object

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

extern map_type			map;
extern js_type			js;

JSBool js_get_obj_score_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_score_add_goal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_score_class={"obj_score_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_score_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_score_props[]={
							{"kill",				obj_score_prop_kill,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"death",				obj_score_prop_death,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"suicide",				obj_score_prop_suicide,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"goal",				obj_score_prop_goal,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"score",				obj_score_prop_score,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

JSFunctionSpec	obj_score_functions[]={
							{"addGoal",				js_obj_score_add_goal_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_score_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"score",&obj_score_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_score_props);
	JS_DefineFunctions(js.cx,j_obj,obj_score_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_score_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

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
		case obj_score_prop_score:
            *vp=INT_TO_JSVAL(obj->score.score);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Goal Functions
      
======================================================= */

JSBool js_obj_score_add_goal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_score_goal(obj);

	return(JS_TRUE);
}




