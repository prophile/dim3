/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Curved Surfaces Dialog

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

#define kCurveRadio									FOUR_CHAR_CODE('curv')

#define kCurveButton_0								FOUR_CHAR_CODE('crv0')
#define kCurveButton_1								FOUR_CHAR_CODE('crv1')
#define kCurveButton_2								FOUR_CHAR_CODE('crv2')

bool						dialog_curved_surfaces_cancel;
WindowRef					dialog_curved_surfaces_wind;

/* =======================================================

      Curved Surfaces Setting Event Handlers
      
======================================================= */

static pascal OSStatus curved_surfaces_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kCurveButton_0:
					dialog_set_radio_buttons(dialog_curved_surfaces_wind,kCurveRadio,0,3);
					return(noErr);
					
				case kCurveButton_1:
					dialog_set_radio_buttons(dialog_curved_surfaces_wind,kCurveRadio,1,3);
					return(noErr);
					
				case kCurveButton_2:
					dialog_set_radio_buttons(dialog_curved_surfaces_wind,kCurveRadio,2,3);
					return(noErr);
				
				case kHICommandCancel:
					dialog_curved_surfaces_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_curved_surfaces_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_curved_surfaces_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Curved Surfaces
      
======================================================= */

bool dialog_curved_surfaces_run(segment_type *seg)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_curved_surfaces_wind,"CurvedSurfaces");

		// set controls
		
	dialog_set_radio_buttons(dialog_curved_surfaces_wind,kCurveRadio,seg->curve,3);
	
		// show window
	
	ShowWindow(dialog_curved_surfaces_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(curved_surfaces_event_proc);
	InstallWindowEventHandler(dialog_curved_surfaces_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_curved_surfaces_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_curved_surfaces_wind);
	
		// dialog to data
		
	if (!dialog_curved_surfaces_cancel) {
		seg->curve=dialog_get_radio_buttons(dialog_curved_surfaces_wind,kCurveRadio,3);
	}

		// close window
		
	DisposeWindow(dialog_curved_surfaces_wind);
	
	return(!dialog_curved_surfaces_cancel);
}

