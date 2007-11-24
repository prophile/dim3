/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Ambient Setting Dialog

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

#include "interface.h"
#include "dialog.h"

extern map_type				map;

#define kAmbientPush								FOUR_CHAR_CODE('push')

bool						dialog_ambient_settings_cancel;
WindowRef					dialog_ambient_settings_wind;

/* =======================================================

      Map Ambient Setting Event Handlers
      
======================================================= */

static pascal OSStatus ambient_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_ambient_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_ambient_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_ambient_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Ambient Setting
      
======================================================= */

bool dialog_ambient_settings_run(segment_type *seg)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_ambient_settings_wind,"AmbientSettings");

		// set controls
		
	dialog_set_combo(dialog_ambient_settings_wind,kAmbientPush,0,seg->data.ambient_wall.push);
	
		// show window
	
	ShowWindow(dialog_ambient_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(ambient_settings_event_proc);
	InstallWindowEventHandler(dialog_ambient_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_ambient_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_ambient_settings_wind);
	
		// dialog to data
		
	if (!dialog_ambient_settings_cancel) {
		seg->data.ambient_wall.push=dialog_get_combo(dialog_ambient_settings_wind,kAmbientPush,0);
	}

		// close window
		
	DisposeWindow(dialog_ambient_settings_wind);
	
	return(!dialog_ambient_settings_cancel);
}

