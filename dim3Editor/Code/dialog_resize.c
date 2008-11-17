/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Resize Dialog

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

#define kResizeX							FOUR_CHAR_CODE('rszx')
#define kResizeY							FOUR_CHAR_CODE('rszy')
#define kResizeZ							FOUR_CHAR_CODE('rszz')

bool						dialog_resize_cancel;
WindowRef					dialog_resize_wind;

/* =======================================================

      Resize Event Handlers
      
======================================================= */

static pascal OSStatus dialog_resize_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_resize_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_resize_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_resize_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Resize Dialog
      
======================================================= */

bool dialog_resize_run(float *fct_x,float *fct_y,float *fct_z)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_resize_wind,"Resize");

		// set controls
		
	dialog_set_int(dialog_resize_wind,kResizeX,0,100);
	dialog_set_int(dialog_resize_wind,kResizeY,0,100);
	dialog_set_int(dialog_resize_wind,kResizeZ,0,100);
	
	dialog_set_focus(dialog_resize_wind,kResizeX,0);
	
		// show window
	
	ShowWindow(dialog_resize_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_resize_event_proc);
	InstallWindowEventHandler(dialog_resize_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_resize_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_resize_wind);
	
		// dialog to data
		
	if (!dialog_resize_cancel) {
		*fct_x=(float)dialog_get_int(dialog_resize_wind,kResizeX,0);
		*fct_y=(float)dialog_get_int(dialog_resize_wind,kResizeY,0);
		*fct_z=(float)dialog_get_int(dialog_resize_wind,kResizeZ,0);
	}

		// close window
		
	DisposeWindow(dialog_resize_wind);
	
	return(!dialog_resize_cancel);
}

