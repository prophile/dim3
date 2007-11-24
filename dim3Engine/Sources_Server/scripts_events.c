/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Events

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
#include "consoles.h"
#include "interfaces.h"

extern server_type			server;
extern js_type				js;
extern setup_type			setup;

/* =======================================================

      Script Events Setup
      
======================================================= */

void scripts_setup_events(int script_uid)
{
	int				idx;
	script_type		*script;
	
	idx=scripts_find_uid(script_uid);
	if (idx==-1) return;
	
	script=&js.scripts[idx];
	JS_GetProperty(js.cx,script->global,"event",&script->event_func);
}

/* =======================================================

      Script Events
      
======================================================= */

bool scripts_post_event(attach_type *attach,int main_event,int sub_event,int id,char *err_str)
{
	int				idx;
	bool			err_up;
	jsval			rval,argv[5];
	script_type		*script;
	attach_type		old_attach;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	if (attach->script_uid==-1) return(TRUE);
	
	idx=scripts_find_uid(attach->script_uid);
	if (idx==-1) return(TRUE);
	
	script=&js.scripts[idx];
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=OBJECT_TO_JSVAL(script->obj);
	argv[1]=INT_TO_JSVAL(main_event);
	argv[2]=INT_TO_JSVAL(sub_event);
	argv[3]=INT_TO_JSVAL(id);
	argv[4]=INT_TO_JSVAL(js.time.current_tick);

	err_up=FALSE;
	scripts_clear_last_error();
	
	if (!JS_CallFunctionValue(js.cx,script->global,script->event_func,5,argv,&rval)) {
		err_up=TRUE;
		scripts_get_last_error(err_str);
	}
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));
	
	return(!err_up);
}

void scripts_post_event_console(attach_type *attach,int main_event,int sub_event,int id)
{
	char			err_str[256];
	
	if (!scripts_post_event(attach,main_event,sub_event,id,err_str)) {
		console_add_error(err_str);
		if (setup.debug_console) console_show();
	}
}

/* =======================================================

      Script Chains
      
======================================================= */

bool scripts_chain(attach_type *attach,char *func_name,char *err_str)
{
	int				idx;
	bool			err_up;
	jsval			func_val,rval,argv[2];
	script_type		*script;
	attach_type		old_attach;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	if (attach->script_uid==-1) return(TRUE);
	
	idx=scripts_find_uid(attach->script_uid);
	if (idx==-1) return(TRUE);
	
	script=&js.scripts[idx];
	
		// get the function
		
	JS_GetProperty(js.cx,script->global,func_name,&func_val);
	if (func_val==JSVAL_VOID) {
		sprintf(err_str,"Chaining failed, unknown function '%s'",func_name);
		return(FALSE);
	}
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=OBJECT_TO_JSVAL(script->obj);
	argv[1]=INT_TO_JSVAL(js.time.current_tick);

	err_up=FALSE;
	scripts_clear_last_error();
	
	if (!JS_CallFunctionValue(js.cx,script->global,func_val,2,argv,&rval)) {
		err_up=TRUE;
		scripts_get_last_error(err_str);
	}
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));
	
	return(!err_up);
}

void scripts_chain_console(attach_type *attach,char *func_name)
{
	char			err_str[256];
	
	if (!scripts_chain(attach,func_name,err_str)) {
		console_add_error(err_str);
		if (setup.debug_console) console_show();
	}
}
