/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Free Rotate Dialog

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

#define kFreeRotateX							FOUR_CHAR_CODE('rotx')
#define kFreeRotateY							FOUR_CHAR_CODE('roty')
#define kFreeRotateZ							FOUR_CHAR_CODE('rotz')

bool						dialog_free_rotate_cancel;
WindowRef					dialog_free_rotate_wind;

/* =======================================================

      Free Rotate Event Handlers
      
======================================================= */

static pascal OSStatus portal_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_free_rotate_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_free_rotate_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_free_rotate_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Free Rotate Dialog
      
======================================================= */

bool dialog_free_rotate_run(float *rot_x,float *rot_y,float *rot_z)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_free_rotate_wind,"FreeRotate");

		// set controls
		
	dialog_set_int(dialog_free_rotate_wind,kFreeRotateX,0,0);
	dialog_set_int(dialog_free_rotate_wind,kFreeRotateY,0,0);
	dialog_set_int(dialog_free_rotate_wind,kFreeRotateZ,0,0);
	dialog_set_focus(dialog_free_rotate_wind,kFreeRotateX,0);
	
		// show window
	
	ShowWindow(dialog_free_rotate_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(portal_setting_event_proc);
	InstallWindowEventHandler(dialog_free_rotate_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_free_rotate_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_free_rotate_wind);
	
		// dialog to data
		
	if (!dialog_free_rotate_cancel) {
		*rot_x=(float)dialog_get_int(dialog_free_rotate_wind,kFreeRotateX,0);
		*rot_y=(float)dialog_get_int(dialog_free_rotate_wind,kFreeRotateY,0);
		*rot_z=(float)dialog_get_int(dialog_free_rotate_wind,kFreeRotateZ,0);
	}

		// close window
		
	DisposeWindow(dialog_free_rotate_wind);
	
	return(!dialog_free_rotate_cancel);
}

