/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Global Data

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

extern js_type			js;

/* =======================================================

      Start Globals
      
======================================================= */

void script_globals_initialize(void)
{
	js.count.global=0;
}

/* =======================================================

      Find Globals
      
======================================================= */

int script_find_global(char *name,int script_uid)
{
	int				i;
	global_type		*global;
	
	global=js.globals;
	
	for (i=0;i!=js.count.global;i++) {
		if (strcasecmp(global->name,name)==0) {
			if (global->script_uid==script_uid) return(i);
		}
		global++;
	}
	
	return(-1);
}

/* =======================================================

      Set and Get Globals
      
======================================================= */

void script_set_global_by_index(int idx,jsval val)
{
	global_type		*global;
	
	global=&js.globals[idx];
	
	if (JSVAL_IS_INT(val)) {
		global->type=d3_jsval_type_int;
		global->data.d3_int=JSVAL_TO_INT(val);
		return;
	}
	
	if (JSVAL_IS_DOUBLE(val)) {
		global->type=d3_jsval_type_float;
		global->data.d3_float=script_value_to_float(val);
		return;
	}
	
	if (JSVAL_IS_BOOLEAN(val)) {
		global->type=d3_jsval_type_boolean;
		global->data.d3_boolean=JSVAL_TO_BOOLEAN(val);
		return;
	}
	
	global->type=d3_jsval_type_string;
	script_value_to_string(val,global->data.d3_string,max_d3_jsval_str_len);
}

bool script_set_global(char *name,int script_uid,jsval val)
{
	int				idx;
	
	idx=script_find_global(name,script_uid);
	if (idx==-1) return(FALSE);
	
	script_set_global_by_index(idx,val);
	return(TRUE);
}

jsval script_get_global(char *name,int script_uid)
{
	int				idx;
	global_type		*global;
	
	idx=script_find_global(name,script_uid);
	if (idx==-1) return(JSVAL_NULL);
	
	global=&js.globals[idx];
	
	switch (global->type) {
		case d3_jsval_type_int:
			return(INT_TO_JSVAL(global->data.d3_int));
		case d3_jsval_type_float:
			return(script_float_to_value(global->data.d3_float));
		case d3_jsval_type_boolean:
			return(BOOLEAN_TO_JSVAL(global->data.d3_boolean));
		case d3_jsval_type_string:
			return(script_string_to_value(global->data.d3_string));
	}
	
	return(JSVAL_NULL);
}

/* =======================================================

      Add and Delete Globals
      
======================================================= */

bool script_add_global(char *name,int script_uid,jsval val)
{
	int				idx;
	
		// does it already exist?
		
	idx=script_find_global(name,script_uid);
	if (idx!=-1) {
		script_set_global_by_index(idx,val);
		return(TRUE);
	}
	
		// try to add it
	
	if (js.count.global>=max_globals) return(FALSE);
	
	idx=js.count.global;
	js.count.global++;
	
	strcpy(js.globals[idx].name,name);
	js.globals[idx].script_uid=script_uid;
	
	script_set_global_by_index(idx,val);
	
	return(TRUE);
}

void script_delete_global(char *name,int script_uid)
{
	int				idx;
	
	if (js.count.global==0) return;
	
	idx=script_find_global(name,script_uid);
	if (idx==-1) return;
	
	if (idx<(js.count.global-1)) {
		memmove(&js.globals[idx],&js.globals[idx+1],(sizeof(global_type)*((js.count.global-idx)-1)));
	}
	
	js.count.global--;
}

