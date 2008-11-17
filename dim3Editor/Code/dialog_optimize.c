/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Optimize Dialog

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

#define kPolyThreshold		FOUR_CHAR_CODE('cont')

bool						dialog_optimize_cancel;
WindowRef					dialog_optimize_wind;

/* =======================================================

      Optimize Event Handlers
      
======================================================= */

static pascal OSStatus dialog_optimize_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_optimize_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_optimize_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_optimize_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Optimize Dialog
      
======================================================= */

bool dialog_optimize_run(int *poly_threshold)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_optimize_wind,"MapOptimize");

		// set controls
		
	dialog_set_int(dialog_optimize_wind,kPolyThreshold,0,5);
	dialog_set_focus(dialog_optimize_wind,kPolyThreshold,0);
	
		// show window
	
	ShowWindow(dialog_optimize_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_optimize_event_proc);
	InstallWindowEventHandler(dialog_optimize_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_optimize_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_optimize_wind);
	
		// dialog to data
		
	if (!dialog_optimize_cancel) {
		*poly_threshold=(float)dialog_get_int(dialog_optimize_wind,kPolyThreshold,0);
	}

		// close window
		
	DisposeWindow(dialog_optimize_wind);
	
	return(!dialog_optimize_cancel);
}

