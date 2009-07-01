/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: *.event object

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
#include "weapons.h"
#include "projectiles.h"

extern server_type		server;
extern js_type			js;

JSBool js_event_start_timer_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_clear_timer_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_start_wait_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_start_wait_random_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_clear_wait_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_chain_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_clear_chain_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_to_object_by_id_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_to_object_by_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_to_course_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_to_game_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_to_held_weapon_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_send_message_to_spawn_weapon_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_set_message_data_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_get_message_data_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_call_object_by_id_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_call_course_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_event_call_game_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			event_class={"event_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	event_functions[]={
							{"startTimer",					js_event_start_timer_func,						2},
							{"clearTimer",					js_event_clear_timer_func,						0},
							{"startWait",					js_event_start_wait_func,						2},
							{"startWaitRandom",				js_event_start_wait_random_func,				3},
							{"clearWait",					js_event_clear_wait_func,						0},
							{"chain",						js_event_chain_func,							2},
							{"clearChain",					js_event_clear_chain_func,						0},
							{"sendMessage",					js_event_send_message_func,						3},
							{"sendMessageToPlayer",			js_event_send_message_to_player_func,			1},
							{"sendMessageToObjectById",		js_event_send_message_to_object_by_id_func,		2},
							{"sendMessageToObjectByName",	js_event_send_message_to_object_by_name_func,	2},
							{"sendMessageToCourse",			js_event_send_message_to_course_func,			1},
							{"sendMessageToGame",			js_event_send_message_to_game_func,				1},
							{"sendMessageToHeldWeapon",		js_event_send_message_to_held_weapon_func,		1},
							{"sendMessageToSpawnWeapon",	js_event_send_message_to_spawn_weapon_func,		1},
							{"setMessageData",				js_event_set_message_data_func,					2},
							{"getMessageData",				js_event_get_message_data_func,					1},
							{"callObjectById",				js_event_call_object_by_id_func,				20},
							{"callCourse",					js_event_call_course_func,						20},
							{"callGame",					js_event_call_game_func,						20},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_event_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"event",&event_class,NULL,event_functions);
}

/* =======================================================

      Start/Clear Timer Functions
      
======================================================= */

JSBool js_event_start_timer_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	if (!timers_add(&js.attach,JSVAL_TO_INT(argv[0]),JSVAL_TO_INT(argv[1]),NULL,timer_mode_repeat)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}

    return(JS_TRUE);
}

JSBool js_event_clear_timer_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	timers_clear(&js.attach,timer_mode_repeat);
    return(JS_TRUE);
}

/* =======================================================

      Start/Clear Wait Functions
      
======================================================= */

JSBool js_event_start_wait_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	if (!timers_add(&js.attach,JSVAL_TO_INT(argv[0]),JSVAL_TO_INT(argv[1]),NULL,timer_mode_single)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}

    return(JS_TRUE);
}

JSBool js_event_start_wait_random_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				min,max,tick;
	
	min=JSVAL_TO_INT(argv[0]);
	max=JSVAL_TO_INT(argv[1]);
	tick=random_int(abs(max-min))+min;
	
	if (!timers_add(&js.attach,tick,JSVAL_TO_INT(argv[2]),NULL,timer_mode_single)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}

    return(JS_TRUE);
}

JSBool js_event_clear_wait_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	timers_clear(&js.attach,timer_mode_single);
    return(JS_TRUE);
}

/* =======================================================

      Chain Functions
      
======================================================= */

JSBool js_event_chain_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			chain_func_name[64];
	
	script_value_to_string(argv[1],chain_func_name,64);
	
	if (!timers_add(&js.attach,JSVAL_TO_INT(argv[0]),0,chain_func_name,timer_mode_chain)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}

    return(JS_TRUE);
}

JSBool js_event_clear_chain_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	timers_clear(&js.attach,timer_mode_chain);
    return(JS_TRUE);
}

/* =======================================================

      Send Message Functions
      
======================================================= */

JSBool js_event_send_message_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				msg_to,id;
	char			name[name_str_len];
	obj_type		*obj;

	msg_to=JSVAL_TO_INT(argv[0]);
	id=JSVAL_TO_INT(argv[2]);
	
	*rval=JSVAL_TRUE;
	
	switch (msg_to) {
	
		case sd_message_to_player:
			obj=object_find_uid(server.player_obj_uid);
			memmove(obj->attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,id);
			break;
	
		case sd_message_to_object:
			script_value_to_string(argv[1],name,name_str_len);
			obj=object_find_name(name);
			if (obj==NULL) {
				*rval=JSVAL_FALSE;
				break;
			}

			memmove(obj->attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,id);
			break;
			
		case sd_message_to_course:
			memmove(js.course_attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));
			scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_script,id);
			break;
		
		case sd_message_to_game:
			memmove(js.game_attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));
			scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_script,id);
			break;
	
	}

	return(JS_TRUE);
}

JSBool js_event_send_message_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	obj=object_find_uid(server.player_obj_uid);

	memmove(obj->attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));

	scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,JSVAL_TO_INT(argv[0]));

	return(JS_TRUE);
}

JSBool js_event_send_message_to_object_by_id_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;

	obj=object_find_uid(JSVAL_TO_INT(argv[0]));
	if (obj==NULL) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}

	memmove(obj->attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));

	scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,JSVAL_TO_INT(argv[1]));

	*rval=JSVAL_TRUE;

	return(JS_TRUE);
}

JSBool js_event_send_message_to_object_by_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];
	obj_type		*obj;

	script_value_to_string(argv[0],name,name_str_len);
	obj=object_find_name(name);
	if (obj==NULL) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}

	memmove(obj->attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));
			
	scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,JSVAL_TO_INT(argv[1]));

	*rval=JSVAL_TRUE;

	return(JS_TRUE);
}

JSBool js_event_send_message_to_course_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	memmove(js.course_attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));

	scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_script,JSVAL_TO_INT(argv[0]));
	return(JS_TRUE);
}

JSBool js_event_send_message_to_game_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	memmove(js.game_attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));

	scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_script,JSVAL_TO_INT(argv[0]));
	return(JS_TRUE);
}

JSBool js_event_send_message_to_held_weapon_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	weapon_type		*weap;

	if (js.attach.thing_type!=thing_type_object) {
		JS_ReportError(js.cx,"Not an object script");
		return(JS_FALSE);
	}

	obj=object_find_uid(js.attach.thing_uid);
	weap=weapon_find_current(obj);
	if (weap==NULL) {
		JS_ReportError(js.cx,"No held weapon");
		return(JS_FALSE);
	}

	memmove(weap->attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));

	scripts_post_event_console(&weap->attach,sd_event_message,sd_event_message_from_script,JSVAL_TO_INT(argv[0]));
	return(JS_TRUE);
}

JSBool js_event_send_message_to_spawn_weapon_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	proj_type		*proj;
	weapon_type		*weap;

	if (js.attach.thing_type!=thing_type_projectile) {
		JS_ReportError(js.cx,"Not an projectile script");
		return(JS_FALSE);
	}

	proj=projectile_find_uid(js.attach.thing_uid);
	if (proj==NULL) {
		JS_ReportError(js.cx,"Could not find projectile");
		return(JS_FALSE);
	}

	weap=weapon_find_uid(proj->weap_uid);
	if (weap==NULL) {
		JS_ReportError(js.cx,"Could not find weapon");
		return(JS_FALSE);
	}

	memmove(weap->attach.get_msg_data,js.attach.set_msg_data,(sizeof(attach_msg_type)*max_msg_data));

	scripts_post_event_console(&weap->attach,sd_event_message,sd_event_message_from_script,JSVAL_TO_INT(argv[0]));
	return(JS_TRUE);
}

/* =======================================================

      Message Data
      
======================================================= */

JSBool js_event_set_message_data_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int			idx;

		// get index

	idx=JSVAL_TO_INT(argv[0]);
	if ((idx<0) || (idx>=max_msg_data)) {
		JS_ReportError(js.cx,"Message data index out of bounds");
		return(JS_FALSE);
	}

		// set data

	if (JSVAL_IS_INT(argv[1])) {
		js.attach.set_msg_data[idx].type=d3_jsval_type_int;
		js.attach.set_msg_data[idx].data.d3_int=JSVAL_TO_INT(argv[1]);
		return(JS_TRUE);
	}
	
	if (JSVAL_IS_DOUBLE(argv[1])) {
		js.attach.set_msg_data[idx].type=d3_jsval_type_float;
		js.attach.set_msg_data[idx].data.d3_float=script_value_to_float(argv[1]);
		return(JS_TRUE);
	}
	
	if (JSVAL_IS_BOOLEAN(argv[1])) {
		js.attach.set_msg_data[idx].type=d3_jsval_type_boolean;
		js.attach.set_msg_data[idx].data.d3_boolean=JSVAL_TO_BOOLEAN(argv[1]);
		return(JS_TRUE);
	}
	
	js.attach.set_msg_data[idx].type=d3_jsval_type_string;
	script_value_to_string(argv[1],js.attach.set_msg_data[idx].data.d3_string,max_d3_jsval_str_len);	
    return(JS_TRUE);
}

JSBool js_event_get_message_data_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int			idx;

 		// get index

	idx=JSVAL_TO_INT(argv[0]);
	if ((idx<0) || (idx>=max_msg_data)) {
		JS_ReportError(js.cx,"Message data index out of bounds");
		return(JS_FALSE);
	}

		// get data

	*rval=JSVAL_NULL;

	switch (js.attach.get_msg_data[idx].type) {

		case d3_jsval_type_int:
			*rval=INT_TO_JSVAL(js.attach.get_msg_data[idx].data.d3_int);
			break;

		case d3_jsval_type_float:
			*rval=script_float_to_value(js.attach.get_msg_data[idx].data.d3_float);
			break;

		case d3_jsval_type_boolean:
			*rval=BOOLEAN_TO_JSVAL(js.attach.get_msg_data[idx].data.d3_boolean);
			break;

		case d3_jsval_type_string:
			*rval=script_string_to_value(js.attach.get_msg_data[idx].data.d3_string);
			break;

	}

	return(JS_TRUE);
}

/* =======================================================

      Direct Function Call
      
======================================================= */

JSBool js_event_call_object_by_id_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				n,arg_count;
	char			func_name[64];
	jsval			args[20];
	obj_type		*obj;

		// get arguments

	obj=object_find_uid(JSVAL_TO_INT(argv[0]));
	if (obj==NULL) {
		*rval=JSVAL_FALSE;
		return(JS_TRUE);
	}

	script_value_to_string(argv[1],func_name,64);

	arg_count=argc-2;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+2];
	}

		// call function

	if (!scripts_direct_call(&obj->attach,func_name,arg_count,args,rval)) return(JS_FALSE);

	return(JS_TRUE);
}

JSBool js_event_call_course_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				n,arg_count;
	char			func_name[64];
	jsval			args[20];

		// get arguments

	script_value_to_string(argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	if (!scripts_direct_call(&js.course_attach,func_name,arg_count,args,rval)) return(JS_FALSE);

	return(JS_TRUE);
}

JSBool js_event_call_game_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				n,arg_count;
	char			func_name[64];
	jsval			args[20];

		// get arguments

	script_value_to_string(argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	if (!scripts_direct_call(&js.game_attach,func_name,arg_count,args,rval)) return(JS_FALSE);

	return(JS_TRUE);
}
