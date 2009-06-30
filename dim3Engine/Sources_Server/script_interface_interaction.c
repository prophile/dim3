/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.interaction object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "interfaces.h"

extern js_type			js;

JSBool js_get_interface_interaction_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_set_interface_interaction_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_interaction_start_story_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_interaction_start_title_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_interaction_start_chooser_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_interaction_start_movie_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_interaction_start_save_load_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_interaction_start_setup_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_interaction_start_menu_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_interaction_quit_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			interface_interaction_class={"interface_interaction_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_function	interface_interaction_functions[]={
							{"startStory",			js_interface_interaction_start_story_func,				2},
							{"startTitle",			js_interface_interaction_start_title_func,				3},
							{"startMovie",			js_interface_interaction_start_movie_func,				2},
							{"startChooser",		js_interface_interaction_start_chooser_func,			1},
							{"startSaveLoad",		js_interface_interaction_start_save_load_func,			0},
							{"startSetup",			js_interface_interaction_start_setup_func,				0},
							{"startMenu",			js_interface_interaction_start_menu_func,				0},
							{"quit",				js_interface_interaction_quit_func,						0},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_interface_interaction_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"interaction",&interface_interaction_class,NULL,interface_interaction_functions);
}

/* =======================================================

      Start Functions
      
======================================================= */

JSBool js_interface_interaction_start_story_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				event_id;
	char			name[name_str_len];

	if (argc<2) {
		event_id=-1;
	}
	else {
		event_id=JSVAL_TO_INT(argv[1]);
	}

	script_value_to_string(argv[0],name,name_str_len);
	story_trigger_set(name,JSVAL_TO_INT(argv[1]));
	
	return(JS_TRUE);
}

JSBool js_interface_interaction_start_title_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				event_id;
	char			name[name_str_len],sound_name[name_str_len];

	if (argc<3) {
		event_id=-1;
	}
	else {
		event_id=JSVAL_TO_INT(argv[2]);
	}

	script_value_to_string(argv[0],name,name_str_len);
	script_value_to_string(argv[1],sound_name,name_str_len);
	
	title_trigger_set("Titles",name,sound_name,event_id);
	
	return(JS_TRUE);
}

JSBool js_interface_interaction_start_movie_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				event_id;
	char			name[name_str_len];

	if (argc<2) {
		event_id=-1;
	}
	else {
		event_id=JSVAL_TO_INT(argv[1]);
	}

	script_value_to_string(argv[0],name,name_str_len);
	movie_trigger_set(name,event_id);
	
	return(JS_TRUE);
}

JSBool js_interface_interaction_start_chooser_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				n,cnt;
	char			name[name_str_len],
					sub_txt[max_chooser_sub_txt][max_chooser_text_data_sz];

		// get name

	script_value_to_string(argv[0],name,name_str_len);

		// get any substitution text

	for (n=0;n!=max_chooser_sub_txt;n++) {
		sub_txt[n][0]=0x0;
	}

	cnt=argc-1;
	if (cnt<0) cnt=0;
	if (cnt>max_chooser_sub_txt) cnt=max_chooser_sub_txt;

	for (n=0;n!=cnt;n++) {
		script_value_to_string(argv[n+1],sub_txt[n],max_chooser_text_data_sz);
	}

		// start chooser

	chooser_trigger_set(name,(char*)sub_txt);
	
	return(JS_TRUE);
}

JSBool js_interface_interaction_start_save_load_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	file_trigger_set();
	return(JS_TRUE);
}

JSBool js_interface_interaction_start_setup_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	setup_game_trigger_set();
	return(JS_TRUE);
}

JSBool js_interface_interaction_start_menu_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	menu_trigger_set();
	return(JS_TRUE);
}

JSBool js_interface_interaction_quit_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	interface_quit_trigger_set();
	return(JS_TRUE);
}
