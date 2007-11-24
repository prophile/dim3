/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.object object

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

JSBool js_map_object_find_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_find_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_find_all_players_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_nearest_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_nearest_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_nearest_remote_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_nearest_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_type_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_distance_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_size_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_get_health_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_is_hidden_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_is_contact_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_shove_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_shove_direct_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_object_add_goal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			map_object_class={"map_object_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	map_object_functions[]={
							{"find",				js_map_object_find_func,					1},
							{"findPlayer",			js_map_object_find_player_func,				0},
							{"findAllPlayers",		js_map_object_find_all_players_func,		0},
							{"nearest",				js_map_object_nearest_func,					9},
							{"nearestPlayer",		js_map_object_nearest_player_func,			7},
							{"nearestRemotePlayer",	js_map_object_nearest_remote_player_func,	7},
							{"nearestTeam",			js_map_object_nearest_team_func,			8},
							{"getName",				js_map_object_get_name_func,				1},
							{"getType",				js_map_object_get_type_func,				1},
							{"getTeam",				js_map_object_get_team_func,				1},
							{"getDistance",			js_map_object_get_distance_func,			4},
							{"getAngleTo",			js_map_object_get_angle_to_func,			4},
							{"getPosition",			js_map_object_get_position_func,			1},
							{"getAngle",			js_map_object_get_angle_func,				1},
							{"getSize",				js_map_object_get_size_func,				1},
							{"getHealth",			js_map_object_get_health_func,				1},
							{"isHidden",			js_map_object_is_hidden_func,				1},
							{"isContact",			js_map_object_is_contact_func,				1},
							{"shove",				js_map_object_shove_func,					5},
							{"shoveDirect",			js_map_object_shove_direct_func,			4},
							{"addGoal",				js_map_object_add_goal_func,				1},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_object_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"object",&map_object_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,map_object_functions);
}

/* =======================================================

      Find Object Function
      
======================================================= */

JSBool js_map_object_find_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				name[name_str_len];
	obj_type			*obj;
	
	script_value_to_string(argv[0],name,name_str_len);
	
	obj=object_find_name(name);
	if (obj==NULL) {
		*rval=INT_TO_JSVAL(-1);
		return(JS_TRUE);
	}

	*rval=INT_TO_JSVAL(obj->uid);
	return(JS_TRUE);
}

/* =======================================================

      Find Player Object Function
      
======================================================= */

JSBool js_map_object_find_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	*rval=INT_TO_JSVAL(server.player_obj_uid);
	return(JS_TRUE);
}

JSBool js_map_object_find_all_players_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int			n,cnt,uids[max_object];
	obj_type	*obj;

	cnt=0;
	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->player) uids[cnt++]=obj->uid;
		obj++;
	}

	if (cnt==0) {
		*rval=JSVAL_NULL;
		return(TRUE);
	}

	*rval=script_int_array_to_value(cnt,uids);
	return(JS_TRUE);
}

/* =======================================================

      Find Nearest Object Functions
      
======================================================= */

JSBool js_map_object_nearest_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int					x,z,y,min_dist,max_dist;
	char				*name_ptr,name[name_str_len],
						*type_ptr,type[name_str_len];
	float				ang,ang_sweep;
	obj_type			*obj;
	
		// x,z,y
		
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);
	
		// name
		
	name_ptr=NULL;
	if (argv[3]!=JSVAL_NULL) {
		script_value_to_string(argv[3],name,name_str_len);
		name_ptr=name;
	}
	
		// type
		
	type_ptr=NULL;
	if (argv[4]!=JSVAL_NULL) {
		script_value_to_string(argv[4],type,name_str_len);
		type_ptr=type;
	}

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (argv[5]!=JSVAL_NULL) ang=script_value_to_float(argv[5]);
	if (argv[6]!=JSVAL_NULL) ang_sweep=script_value_to_float(argv[6]);
	
		// distances
		
	min_dist=JSVAL_TO_INT(argv[7]);
	max_dist=JSVAL_TO_INT(argv[8]);
	
		// find object

	obj=object_find_nearest(x,z,y,name_ptr,type_ptr,-1,ang,ang_sweep,min_dist,max_dist,FALSE,FALSE);
	if (obj==NULL) {
		*rval=INT_TO_JSVAL(-1);
		return(JS_TRUE);
	}
	
	*rval=INT_TO_JSVAL(obj->uid);
	return(JS_TRUE);
}

JSBool js_map_object_nearest_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int					x,z,y,min_dist,max_dist;
	float				ang,ang_sweep;
	obj_type			*obj;
	
		// x,z,y
		
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (argv[3]!=JSVAL_NULL) ang=script_value_to_float(argv[3]);
	if (argv[4]!=JSVAL_NULL) ang_sweep=script_value_to_float(argv[4]);
	
		// distances
		
	min_dist=JSVAL_TO_INT(argv[5]);
	max_dist=JSVAL_TO_INT(argv[6]);
	
		// find object

	obj=object_find_nearest(x,z,y,NULL,NULL,-1,ang,ang_sweep,min_dist,max_dist,TRUE,FALSE);
	if (obj==NULL) {
		*rval=INT_TO_JSVAL(-1);
		return(JS_TRUE);
	}
	
	*rval=INT_TO_JSVAL(obj->uid);
	return(JS_TRUE);
}

JSBool js_map_object_nearest_remote_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int					x,z,y,min_dist,max_dist;
	float				ang,ang_sweep;
	obj_type			*obj;
	
		// x,z,y
		
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (argv[3]!=JSVAL_NULL) ang=script_value_to_float(argv[3]);
	if (argv[4]!=JSVAL_NULL) ang_sweep=script_value_to_float(argv[4]);
	
		// distances
		
	min_dist=JSVAL_TO_INT(argv[5]);
	max_dist=JSVAL_TO_INT(argv[6]);
	
		// find object

	obj=object_find_nearest(x,z,y,NULL,NULL,-1,ang,ang_sweep,min_dist,max_dist,TRUE,TRUE);
	if (obj==NULL) {
		*rval=INT_TO_JSVAL(-1);
		return(JS_TRUE);
	}
	
	*rval=INT_TO_JSVAL(obj->uid);
	return(JS_TRUE);
}

JSBool js_map_object_nearest_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int					x,z,y,min_dist,max_dist,team_idx;
	float				ang,ang_sweep;
	obj_type			*obj;
	
		// x,z,y
		
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);
	
		// team
		
	team_idx=JSVAL_TO_INT(argv[3])-sd_team_none;

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (argv[4]!=JSVAL_NULL) ang=script_value_to_float(argv[4]);
	if (argv[5]!=JSVAL_NULL) ang_sweep=script_value_to_float(argv[5]);
	
		// distances
		
	min_dist=JSVAL_TO_INT(argv[6]);
	max_dist=JSVAL_TO_INT(argv[7]);
	
		// find object

	obj=object_find_nearest(x,z,y,NULL,NULL,team_idx,ang,ang_sweep,min_dist,max_dist,FALSE,FALSE);
	if (obj==NULL) {
		*rval=INT_TO_JSVAL(-1);
		return(JS_TRUE);
	}
	
	*rval=INT_TO_JSVAL(obj->uid);
	return(JS_TRUE);
}

/* =======================================================

      Get Object Information
      
======================================================= */

JSBool js_map_object_get_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);

	*rval=script_string_to_value(obj->name);
	return(JS_TRUE);
}

JSBool js_map_object_get_type_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);

	*rval=script_string_to_value(obj->type);
	return(JS_TRUE);
}

JSBool js_map_object_get_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);

	*rval=INT_TO_JSVAL(obj->team_idx+sd_team_none);
	return(JS_TRUE);
}

/* =======================================================

      Object Distance and Angle To
      
======================================================= */

JSBool js_map_object_get_distance_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y;
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// x,z,y
		
	x=JSVAL_TO_INT(argv[1]);
	z=JSVAL_TO_INT(argv[2]);
	y=JSVAL_TO_INT(argv[3]);
	
		// get distance
		
	*rval=INT_TO_JSVAL(distance_get(obj->pos.x,obj->pos.y,obj->pos.z,x,y,z));
	
	return(JS_TRUE);
}

JSBool js_map_object_get_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y;
	float			ang_x,ang_z,ang_y;
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);

		// x,z,y
		
	x=JSVAL_TO_INT(argv[1]);
	z=JSVAL_TO_INT(argv[2]);
	y=JSVAL_TO_INT(argv[3]);
	
		// get angles to
		
	ang_x=angle_find(y,z,obj->pos.y,obj->pos.z);
	ang_y=angle_find(x,z,obj->pos.x,obj->pos.z);
	ang_z=angle_find(x,y,obj->pos.x,obj->pos.y);
		
	*rval=script_angle_to_value(ang_x,ang_y,ang_z);
	
	return(JS_TRUE);
}

/* =======================================================

      Object Position, Angle, Size
      
======================================================= */

JSBool js_map_object_get_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get position
		
	*rval=script_point_to_value(obj->pos.x,obj->pos.y,obj->pos.z);
	return(JS_TRUE);
}

JSBool js_map_object_get_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get angle
		
	*rval=script_angle_to_value(obj->ang.x,obj->ang.y,obj->ang.z);
	return(JS_TRUE);
}

JSBool js_map_object_get_size_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get size
		
	*rval=script_point_to_value(obj->size.x,obj->size.y,obj->size.z);
	return(JS_TRUE);
}

JSBool js_map_object_get_health_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get size
		
	*rval=INT_TO_JSVAL(obj->status.health);
	return(JS_TRUE);
}

JSBool js_map_object_is_hidden_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get size
		
	*rval=INT_TO_JSVAL(obj->hidden);
	return(JS_TRUE);
}

JSBool js_map_object_is_contact_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get size
		
	*rval=INT_TO_JSVAL(obj->contact.on);
	return(JS_TRUE);
}

/* =======================================================

      Object Shoving
      
======================================================= */

JSBool js_map_object_shove_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			speed;
	d3ang			ang;
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get angle
		
	ang.x=script_value_to_float(argv[1]);
	ang.z=script_value_to_float(argv[2]);
	ang.y=script_value_to_float(argv[3]);
	
	speed=script_value_to_float(argv[4]);
	
		// shove object
		
	object_shove(obj,&ang,speed);
	return(JS_TRUE);
}

JSBool js_map_object_shove_direct_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	d3vct			vct;
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);
	
		// get direction
		
	vct.x=script_value_to_float(argv[1]);
	vct.z=script_value_to_float(argv[2]);
	vct.y=script_value_to_float(argv[3]);
	
		// shove object
		
	object_shove_direct(obj,&vct);
	return(JS_TRUE);
}

/* =======================================================

      Object Scoring
      
======================================================= */

JSBool js_map_object_add_goal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);

	object_score_goal(obj);

	return(JS_TRUE);
}
