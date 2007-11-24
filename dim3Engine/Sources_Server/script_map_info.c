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

JSBool js_get_map_info_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			map_info_class={"map_info_class",0,
							script_add_property,JS_PropertyStub,
							js_get_map_info_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	map_info_props[]={
							{"file",				map_info_prop_file,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"title",				map_info_prop_title,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"author",				map_info_prop_author,				JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_map_info_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"info",&map_info_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,map_info_props);
}

/* =======================================================

      Properties
      
======================================================= */

JSBool js_get_map_info_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case map_info_prop_file:
			*vp=script_string_to_value(map.info.name);
			break;
		case map_info_prop_title:
			*vp=script_string_to_value(map.info.title);
			break;
		case map_info_prop_author:
			*vp=script_string_to_value(map.info.author);
			break;

	}
	
	return(JS_TRUE);
}

