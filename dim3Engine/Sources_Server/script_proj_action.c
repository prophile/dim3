/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.action object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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
#include "weapons.h"
#include "projectiles.h"

extern server_type		server;
extern js_type			js;

JSBool js_proj_action_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_get_collision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_get_auto_hitTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_get_auto_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_get_auto_bounceMinMove(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_get_auto_bounceReduce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_get_auto_reflect(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_set_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_set_collision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_set_auto_hitTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_set_auto_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_set_auto_bounceMinMove(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_set_auto_bounceReduce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_set_auto_reflect(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_action_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_turn_towards_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_seek_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_seek_target_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_bounce_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_reflect_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_stick_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_proj_action_destroy_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			proj_action_class={"proj_action_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	proj_action_props[]={
							{"damage",				js_proj_action_get_damage,				js_proj_action_set_damage},
							{"collision",			js_proj_action_get_collision,			js_proj_action_set_collision},
							{"autoHitTick",			js_proj_action_get_auto_hitTick,		js_proj_action_set_auto_hitTick},
							{"autoBounce",			js_proj_action_get_auto_bounce,			js_proj_action_set_auto_bounce},
							{"autoBounceMinMove",	js_proj_action_get_auto_bounceMinMove,	js_proj_action_set_auto_bounceMinMove},
							{"autoBounceReduce",	js_proj_action_get_auto_bounceReduce,	js_proj_action_set_auto_bounceReduce},
							{"autoReflect",			js_proj_action_get_auto_reflect,		js_proj_action_set_auto_reflect},
							{0}};

script_js_function	proj_action_functions[]={
							{"rotate",				js_proj_action_rotate_func,				1},
							{"turnTowards",			js_proj_action_turn_towards_func,		2},
							{"seek",				js_proj_action_seek_func,				3},
							{"seekTarget",			js_proj_action_seek_target_func,		2},
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
	script_create_child_object(parent_obj,"action",&proj_action_class,proj_action_props,proj_action_functions);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_proj_action_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=INT_TO_JSVAL(proj_setup->damage);
	
	return(JS_TRUE);
}

JSBool js_proj_action_get_collision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=BOOLEAN_TO_JSVAL(proj_setup->collision);
	
	return(JS_TRUE);
}

JSBool js_proj_action_get_auto_hitTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=INT_TO_JSVAL(proj_setup->action.hit_tick);
	
	return(JS_TRUE);
}

JSBool js_proj_action_get_auto_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=BOOLEAN_TO_JSVAL(proj_setup->action.bounce);
	
	return(JS_TRUE);
}

JSBool js_proj_action_get_auto_bounceMinMove(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_float_to_value(proj_setup->action.bounce_min_move);
	
	return(JS_TRUE);
}

JSBool js_proj_action_get_auto_bounceReduce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_float_to_value(proj_setup->action.bounce_reduce);
	
	return(JS_TRUE);
}

JSBool js_proj_action_get_auto_reflect(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=BOOLEAN_TO_JSVAL(proj_setup->action.reflect);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_proj_action_set_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->damage=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_action_set_collision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->collision=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_action_set_auto_hitTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->action.hit_tick=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_action_set_auto_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->action.bounce=JSVAL_TO_BOOLEAN(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_action_set_auto_bounceMinMove(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->action.bounce_min_move=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_action_set_auto_bounceReduce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->action.bounce_reduce=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_action_set_auto_reflect(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->action.reflect=JSVAL_TO_BOOLEAN(*vp);
	
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

JSBool js_proj_action_seek_target_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type			*to_obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	if (proj_setup==NULL) return(JS_TRUE);
	
	weap=weapon_find_uid(proj_setup->weap_uid);
		
	to_obj=object_find_uid(weap->target.obj_uid);
	if (to_obj==NULL) return(JS_FALSE);
	
	projectile_seek(proj,to_obj,script_value_to_float(argv[0]),script_value_to_float(argv[1]));
	
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
