/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.info object

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

extern map_type			map;
extern js_type			js;

JSBool js_map_info_get_file(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_info_get_title(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_info_get_author(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			map_info_class={"map_info_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

script_js_property	map_info_props[]={
							{"file",				js_map_info_get_file,				NULL},
							{"title",				js_map_info_get_title,				NULL},
							{"author",				js_map_info_get_author,				NULL},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_info_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"info",&map_info_class,map_info_props,NULL);
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_map_info_get_file(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_string_to_value(map.info.name);
	return(JS_TRUE);
}

JSBool js_map_info_get_title(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_string_to_value(map.info.title);
	return(JS_TRUE);
}

JSBool js_map_info_get_author(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_string_to_value(map.info.author);
	return(JS_TRUE);
}


