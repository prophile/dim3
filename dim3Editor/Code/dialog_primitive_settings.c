/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Primitive Setting Dialog

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

#include "interface.h"
#include "dialog.h"

extern map_type				map;

#define kPrimitiveName							FOUR_CHAR_CODE('name')

bool						dialog_primitive_settings_cancel;
WindowRef					dialog_primitive_settings_wind;

/* =======================================================

      Primitive Setting Event Handlers
      
======================================================= */

static pascal OSStatus primitive_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_primitive_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_primitive_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_primitive_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Primitive Setting
      
======================================================= */

bool dialog_primitive_settings_run(char *name)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_primitive_settings_wind,"PrimitiveSettings");

		// set controls
		
	dialog_set_text(dialog_primitive_settings_wind,kPrimitiveName,0,"");
	
		// show window
	
	ShowWindow(dialog_primitive_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(primitive_setting_event_proc);
	InstallWindowEventHandler(dialog_primitive_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_primitive_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_primitive_settings_wind);
	
		// dialog to data
		
	if (!dialog_primitive_settings_cancel) {
		dialog_get_text(dialog_primitive_settings_wind,kPrimitiveName,0,name,name_str_len);
	}

		// close window
		
	DisposeWindow(dialog_primitive_settings_wind);
	
	return(!dialog_primitive_settings_cancel);
}

