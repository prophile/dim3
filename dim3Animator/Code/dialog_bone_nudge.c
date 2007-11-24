/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Nudge Bones Dialog

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

#define kXValue						FOUR_CHAR_CODE('xxxx')
#define kZValue						FOUR_CHAR_CODE('zzzz')
#define kYValue						FOUR_CHAR_CODE('yyyy')
#define kIncludeChildren			FOUR_CHAR_CODE('icbn')
#define kIncludeVertexes			FOUR_CHAR_CODE('ivtn')

bool					dialog_bone_nudge_cancel;
WindowRef				dialog_bone_nudge_wind;

/* =======================================================

      Bone Nudge Event Handlers
      
======================================================= */

static pascal OSStatus bone_nudge_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_bone_nudge_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_bone_nudge_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_bone_nudge_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Bone Nudge
      
======================================================= */

bool dialog_bone_nudge_run(int *x,int *z,int *y,bool *nudge_children,bool *nudge_vertexes)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_bone_nudge_wind,"BoneNudgePick");
	
		// setup the controls
		
	dialog_set_int(dialog_bone_nudge_wind,kXValue,0,0);
	dialog_set_int(dialog_bone_nudge_wind,kZValue,0,0);
	dialog_set_int(dialog_bone_nudge_wind,kYValue,0,0);
	dialog_set_boolean(dialog_bone_nudge_wind,kIncludeChildren,0,TRUE);
	dialog_set_boolean(dialog_bone_nudge_wind,kIncludeVertexes,0,TRUE);
	
		// show window
	
	ShowWindow(dialog_bone_nudge_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(bone_nudge_event_proc);
	InstallWindowEventHandler(dialog_bone_nudge_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_bone_nudge_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_bone_nudge_wind);
	
	if (!dialog_bone_nudge_cancel) {
		*x=dialog_get_int(dialog_bone_nudge_wind,kXValue,0);
		*z=dialog_get_int(dialog_bone_nudge_wind,kZValue,0);
		*y=dialog_get_int(dialog_bone_nudge_wind,kYValue,0);
		*nudge_children=dialog_get_boolean(dialog_bone_nudge_wind,kIncludeChildren,0);
		*nudge_vertexes=dialog_get_boolean(dialog_bone_nudge_wind,kIncludeVertexes,0);
	}
	
		// close window

	DisposeWindow(dialog_bone_nudge_wind);
	
	return(!dialog_bone_nudge_cancel);
}

