/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.position object

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

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern network_setup_type	net_setup;

JSBool js_get_obj_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_position_place_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_place_random_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_place_network_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_distance_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_distance_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			obj_position_class={"obj_position_class",0,
							script_add_property,JS_PropertyStub,
							js_get_obj_position_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	obj_position_props[]={
							{"x",					obj_position_prop_x,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"y",					obj_position_prop_y,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"z",					obj_position_prop_z,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

JSFunctionSpec	obj_position_functions[]={
							{"place",				js_obj_position_place_func,					4},
							{"placeRandomSpot",		js_obj_position_place_random_spot_func,		2},
							{"placeNetworkSpot",	js_obj_position_place_network_spot_func,	0},
							{"move",				js_obj_position_move_func,					3},
							{"reset",				js_obj_position_reset_func,					0},
							{"distanceToPlayer",	js_obj_position_distance_to_player_func,	0},
							{"distanceToObject",	js_obj_position_distance_to_object_func,	1},
							{0}};

extern void object_setup_motion(obj_type *obj,float ang,float speed);
extern JSBool js_obj_position_place_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
extern JSBool js_obj_position_pause_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
							
/* =======================================================

      Create Object
      
======================================================= */

void script_add_obj_position_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"position",&obj_position_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,obj_position_props);
	JS_DefineFunctions(js.cx,j_obj,obj_position_functions);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_obj_position_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	obj=object_find_uid(js.attach.thing_uid);

	switch (JSVAL_TO_INT(id)) {
	
		case obj_position_prop_x:
			*vp=INT_TO_JSVAL(obj->pnt.x);
			break;
		case obj_position_prop_y:
			*vp=INT_TO_JSVAL(obj->pnt.y);
			break;
		case obj_position_prop_z:
			*vp=INT_TO_JSVAL(obj->pnt.z);
			break;
			
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Reposition Functions
      
======================================================= */

JSBool js_obj_position_place_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	obj->pnt.x=JSVAL_TO_INT(argv[0]);
	obj->pnt.z=JSVAL_TO_INT(argv[1]);
	obj->pnt.y=JSVAL_TO_INT(argv[2]);
    
    obj->ang.y=obj->motion.ang.y=script_value_to_float(argv[3]);
	
	object_telefrag_check(obj);
    
    *rval=JSVAL_TRUE;

	return(JS_TRUE);
}

JSBool js_obj_position_place_random_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				spot_idx;
	char			name[name_str_len],type[name_str_len];
	obj_type		*obj;
	spot_type		*spot;
	
	obj=object_find_uid(js.attach.thing_uid);
	
		// find spot
		
	script_value_to_string(argv[0],name,name_str_len);
	script_value_to_string(argv[1],type,name_str_len);
	
	spot_idx=map_find_random_spot(&map,name,type);
	if (spot_idx==-1) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}
	
	spot=&map.spots[spot_idx];
	
		// move player
	
	object_set_position(obj,spot->pnt.x,spot->pnt.z,spot->pnt.y,spot->ang.y,0);
	obj->turn.ang_to.y=spot->ang.y;
	
 	object_telefrag_check(obj);
  
    *rval=JSVAL_TRUE;

	return(JS_TRUE);
}

JSBool js_obj_position_place_network_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				spot_idx;
	obj_type		*obj;
	spot_type		*spot;
	
	obj=object_find_uid(js.attach.thing_uid);

		// get spot

	if (!net_setup.client.joined) {
		spot_idx=map_find_random_spot(&map,map.info.player_start_name,map.info.player_start_type);
	}
	else {
		if (obj->spawn_spot_name[0]==0x0) {
			spot_idx=map_find_random_spot(&map,NULL,"Spawn");
		}
		else {
			spot_idx=map_find_random_spot(&map,obj->spawn_spot_name,"Spawn");
		}
	}

	if (spot_idx==-1) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}
	
	spot=&map.spots[spot_idx];
	
		// move player
	
	object_set_position(obj,spot->pnt.x,spot->pnt.z,spot->pnt.y,spot->ang.y,0);
	obj->turn.ang_to.y=spot->ang.y;
 
	object_telefrag_check(obj);
   
    *rval=JSVAL_TRUE;

	return(JS_TRUE);
}

JSBool js_obj_position_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				xadd,zadd,yadd;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	xadd=JSVAL_TO_INT(argv[0]);
	zadd=JSVAL_TO_INT(argv[1]);
	yadd=JSVAL_TO_INT(argv[2]);
	
	obj->pnt.x+=xadd;
	obj->pnt.z+=zadd;
	obj->pnt.y+=yadd;
	
	object_telefrag_check(obj);

	*rval=JSVAL_TRUE;

	return(JS_TRUE);
}

JSBool js_obj_position_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_reset(obj);
	object_telefrag_check(obj);
	
	return(JS_TRUE);
}

JSBool js_obj_position_distance_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj,*player_obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	player_obj=object_find_uid(server.player_obj_uid);

	*rval=INT_TO_JSVAL(distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,player_obj->pnt.x,player_obj->pnt.y,player_obj->pnt.z));
	
	return(JS_TRUE);
}

JSBool js_obj_position_distance_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj,*dist_obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	dist_obj=object_find_uid(JSVAL_TO_INT(argv[0]));
	if (dist_obj==NULL) return(JS_FALSE);

	*rval=INT_TO_JSVAL(distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,dist_obj->pnt.x,dist_obj->pnt.y,dist_obj->pnt.z));
	
	return(JS_TRUE);
}

