/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Reposition Dialog

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

#define kRepositionTopLeftX				FOUR_CHAR_CODE('tlpx')
#define kRepositionTopLeftY				FOUR_CHAR_CODE('tlpy')
#define kRepositionTopLeftZ				FOUR_CHAR_CODE('tlpz')
#define kRepositionBottomRightX			FOUR_CHAR_CODE('brpx')
#define kRepositionBottomRightY			FOUR_CHAR_CODE('brpy')
#define kRepositionBottomRightZ			FOUR_CHAR_CODE('brpz')

bool						dialog_reposition_cancel;
WindowRef					dialog_reposition_wind;

/* =======================================================

      Reposition Event Handlers
      
======================================================= */

static pascal OSStatus dialog_reposition_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_reposition_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_reposition_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_reposition_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Reposition Dialog
      
======================================================= */

bool dialog_reposition_run(d3pnt *min,d3pnt *max)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_reposition_wind,"Reposition");

		// set controls
		
	dialog_set_int(dialog_reposition_wind,kRepositionTopLeftX,0,min->x);
	dialog_set_int(dialog_reposition_wind,kRepositionTopLeftY,0,min->y);
	dialog_set_int(dialog_reposition_wind,kRepositionTopLeftZ,0,min->z);
	dialog_set_int(dialog_reposition_wind,kRepositionBottomRightX,0,max->x);
	dialog_set_int(dialog_reposition_wind,kRepositionBottomRightY,0,max->y);
	dialog_set_int(dialog_reposition_wind,kRepositionBottomRightZ,0,max->z);
	
	dialog_set_focus(dialog_reposition_wind,kRepositionTopLeftX,0);
	
		// show window
	
	ShowWindow(dialog_reposition_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_reposition_event_proc);
	InstallWindowEventHandler(dialog_reposition_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_reposition_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_reposition_wind);
	
		// dialog to data
		
	if (!dialog_reposition_cancel) {
		min->x=dialog_get_int(dialog_reposition_wind,kRepositionTopLeftX,0);
		min->y=dialog_get_int(dialog_reposition_wind,kRepositionTopLeftY,0);
		min->z=dialog_get_int(dialog_reposition_wind,kRepositionTopLeftZ,0);
		max->x=dialog_get_int(dialog_reposition_wind,kRepositionBottomRightX,0);
		max->y=dialog_get_int(dialog_reposition_wind,kRepositionBottomRightY,0);
		max->z=dialog_get_int(dialog_reposition_wind,kRepositionBottomRightZ,0);
	}

		// close window
		
	DisposeWindow(dialog_reposition_wind);
	
	return(!dialog_reposition_cancel);
}

