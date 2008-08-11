/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Height Import Dialog

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

#define kHeightDivision						FOUR_CHAR_CODE('hdiv')
#define kHeightSize							FOUR_CHAR_CODE('hsiz')
#define kHeightHeight						FOUR_CHAR_CODE('hhgh')

bool						dialog_height_import_cancel;
WindowRef					dialog_height_import_wind;

/* =======================================================

      Height Import Event Handlers
      
======================================================= */

static pascal OSStatus height_import_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_height_import_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_height_import_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_height_import_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Height Import Dialog
      
======================================================= */

bool dialog_height_import_run(int *div_cnt,int *size,int *high)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_height_import_wind,"HeightImport");
	
		// show window
	
	ShowWindow(dialog_height_import_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(height_import_event_proc);
	InstallWindowEventHandler(dialog_height_import_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_height_import_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_height_import_wind);
	
		// dialog to data
		
	if (!dialog_height_import_cancel) {
		*div_cnt=dialog_get_value(dialog_height_import_wind,kHeightDivision,0);
		*size=dialog_get_value(dialog_height_import_wind,kHeightSize,0);
		*high=dialog_get_value(dialog_height_import_wind,kHeightHeight,0);
	}

		// close window
		
	DisposeWindow(dialog_height_import_wind);
	
	return(!dialog_height_import_cancel);
}

