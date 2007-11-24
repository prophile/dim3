/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.action object

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
#include "projectiles.h"

extern server_type		server;
extern js_type			js;

JSBool js_get_proj_action_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_proj_action_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_turn_towards_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_seek_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_bounce_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_reflect_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_stick_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_destroy_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			proj_action_class={"proj_action_class",0,
							script_add_property,JS_PropertyStub,
							js_get_proj_action_property,js_set_proj_action_property,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	proj_action_props[]={
							{"damage",				proj_action_prop_damage,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"collision",			proj_action_prop_collision,				JSPROP_PERMANENT|JSPROP_SHARED},
							{"autoHitTick",			proj_action_prop_auto_hit_tick,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"autoBounce",			proj_action_prop_auto_bounce,			JSPROP_PERMANENT|JSPROP_SHARED},
							{"autoBounceMinMove",	proj_action_prop_auto_bounce_min_move,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"autoBounceReduce",	proj_action_prop_auto_bounce_reduce,	JSPROP_PERMANENT|JSPROP_SHARED},
							{"autoReflect",			proj_action_prop_auto_reflect,			JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

JSFunctionSpec	proj_action_functions[]={
							{"rotate",				js_proj_action_rotate_func,				1},
							{"turnTowards",			js_proj_action_turn_towards_func,		2},
							{"seek",				js_proj_action_seek_func,				3},
							{"bounce",				js_proj_action_bounce_func,				2},
							{"reflect",				js_proj_action_reflect_func,			0},
							{"stick",				js_proj_action_stick_func,				0},
							{"destroy",				js_proj_action_destroy_func,			0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_action_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"action",&proj_action_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,proj_action_props);
	JS_DefineFunctions(js.cx,j_obj,proj_action_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_proj_action_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {
	
		case proj_action_prop_damage:
			*vp=INT_TO_JSVAL(proj_setup->damage);
			break;
		case proj_action_prop_collision:
			*vp=BOOLEAN_TO_JSVAL(proj_setup->collision);
			break;
		case proj_action_prop_auto_hit_tick:
			*vp=INT_TO_JSVAL(proj_setup->action.hit_tick);
			break;
		case proj_action_prop_auto_bounce:
			*vp=BOOLEAN_TO_JSVAL(proj_setup->action.bounce);
			break;
		case proj_action_prop_auto_bounce_min_move:
			*vp=script_float_to_value(proj_setup->action.bounce_min_move);
			break;
		case proj_action_prop_auto_bounce_reduce:
			*vp=script_float_to_value(proj_setup->action.bounce_reduce);
			break;
		case proj_action_prop_auto_reflect:
			*vp=BOOLEAN_TO_JSVAL(proj_setup->action.reflect);
			break;
			
	}
	
	return(JS_TRUE);
}

JSBool js_set_proj_action_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	switch (JSVAL_TO_INT(id)) {

		case proj_action_prop_damage:
			proj_setup->damage=JSVAL_TO_INT(*vp);
			break;
		case proj_action_prop_collision:
			proj_setup->collision=JSVAL_TO_BOOLEAN(*vp);
			break;
 		case proj_action_prop_auto_hit_tick:
			proj_setup->action.hit_tick=JSVAL_TO_INT(*vp);
			break;
		case proj_action_prop_auto_bounce:
			proj_setup->action.bounce=JSVAL_TO_BOOLEAN(*vp);
			break;
		case proj_action_prop_auto_bounce_min_move:
			proj_setup->action.bounce_min_move=script_value_to_float(*vp);
			break;
		case proj_action_prop_auto_bounce_reduce:
			proj_setup->action.bounce_reduce=script_value_to_float(*vp);
			break;
		case proj_action_prop_auto_reflect:
			proj_setup->action.reflect=JSVAL_TO_BOOLEAN(*vp);
			break;
          
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Projectile Changes
      
======================================================= */

JSBool js_proj_action_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);
	
    proj->ang.y=proj->motion.ang.y=script_value_to_float(argv[0]);
	
	return(JS_TRUE);
}

JSBool js_proj_action_turn_towards_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	proj_type			*proj;
	obj_type			*to_obj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);
		
	to_obj=script_find_obj_from_uid_arg(argv[0]);
	if (to_obj==NULL) return(JS_FALSE);
	
	projectile_turn_xz_towards(proj,to_obj,script_value_to_float(argv[1]));
	
	return(JS_TRUE);
}

JSBool js_proj_action_seek_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	proj_type			*proj;
	obj_type			*to_obj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);
		
	to_obj=script_find_obj_from_uid_arg(argv[0]);
	if (to_obj==NULL) return(JS_FALSE);
	
	projectile_seek(proj,to_obj,script_value_to_float(argv[1]),script_value_to_float(argv[2]));
	
	return(JS_TRUE);
}

JSBool js_proj_action_bounce_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    float				min_ymove,reduce;
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);
    
    min_ymove=script_value_to_float(argv[0]);
	reduce=script_value_to_float(argv[1]);
	
	projectile_bounce(proj,min_ymove,reduce,FALSE);
    
	return(JS_TRUE);
}

JSBool js_proj_action_reflect_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);
	
	projectile_reflect(proj,FALSE);
	     
	return(JS_TRUE);
}

JSBool js_proj_action_stick_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);
    
    proj->stick=TRUE;
    proj->ang.x=0;
    
	return(JS_TRUE);
}

JSBool js_proj_action_destroy_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);
	
	projectile_mark_dispose(proj);
    
	return(JS_TRUE);
}
