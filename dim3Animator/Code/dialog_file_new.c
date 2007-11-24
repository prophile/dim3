/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: File New Dialogs

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dialog.h"

#define kFileText					FOUR_CHAR_CODE('text')

bool					fn_cancel;
WindowRef				dialog_file_new_wind;

/* =======================================================

      File New Event Handlers
      
======================================================= */

static pascal OSStatus file_new_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandNew:
					QuitAppModalLoopForWindow(dialog_file_new_wind);
					return(noErr);
					
				case kHICommandCancel:
					fn_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_file_new_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run File New
      
======================================================= */

bool dialog_file_new_run(char *title,char *file_name)
{
	CFStringRef						cfstr;
	ControlRef						ctrl;
	ControlID						ctrl_id;
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_file_new_wind,"FileNew");
	
	cfstr=CFStringCreateWithCString(kCFAllocatorDefault,title,kCFStringEncodingMacRoman);
	SetWindowTitleWithCFString(dialog_file_new_wind,cfstr);
	CFRelease(cfstr);
	
		// show window
	
	ShowWindow(dialog_file_new_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(file_new_event_proc);
	InstallWindowEventHandler(dialog_file_new_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// setup the text
		
	ctrl_id.signature=FOUR_CHAR_CODE('d3ET');
	ctrl_id.id=0;
	GetControlByID(dialog_file_new_wind,&ctrl_id,&ctrl);
	
	SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,strlen(file_name),file_name);
	Draw1Control(ctrl);
	
	SetKeyboardFocus(dialog_file_new_wind,ctrl,kControlFocusNextPart);
	
		// modal window
		
	fn_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_file_new_wind);
	
	if (!fn_cancel) {
		memset(file_name,0x0,256);
		GetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,256,file_name,NULL);
	}
	
		// close window

	DisposeWindow(dialog_file_new_wind);
	
	return(!fn_cancel);
}

