/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Wall Clipping Dialog

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

#define kClipRadio									FOUR_CHAR_CODE('clip')

#define kClipButton_0								FOUR_CHAR_CODE('clp0')
#define kClipButton_1								FOUR_CHAR_CODE('clp1')
#define kClipButton_2								FOUR_CHAR_CODE('clp2')
#define kClipButton_3								FOUR_CHAR_CODE('clp3')
#define kClipButton_4								FOUR_CHAR_CODE('clp4')
#define kClipButton_5								FOUR_CHAR_CODE('clp5')
#define kClipButton_6								FOUR_CHAR_CODE('clp6')
#define kClipButton_7								FOUR_CHAR_CODE('clp7')
#define kClipButton_8								FOUR_CHAR_CODE('clp8')
#define kClipButton_9								FOUR_CHAR_CODE('clp9')

bool						dialog_wall_clipping_cancel;
WindowRef					dialog_wall_clipping_wind;

/* =======================================================

      Curved Surfaces Setting Event Handlers
      
======================================================= */

static pascal OSStatus wall_clipping_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kClipButton_0:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,0,10);
					return(noErr);
					
				case kClipButton_1:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,1,10);
					return(noErr);

				case kClipButton_2:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,2,10);
					return(noErr);

				case kClipButton_3:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,3,10);
					return(noErr);

				case kClipButton_4:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,4,10);
					return(noErr);

				case kClipButton_5:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,5,10);
					return(noErr);

				case kClipButton_6:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,6,10);
					return(noErr);
					
				case kClipButton_7:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,7,10);
					return(noErr);
					
				case kClipButton_8:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,8,10);
					return(noErr);
					
				case kClipButton_9:
					dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,9,10);
					return(noErr);
				
				case kHICommandCancel:
					dialog_wall_clipping_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_wall_clipping_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_wall_clipping_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Wall Clipping
      
======================================================= */

bool dialog_wall_clipping_run(segment_type *seg)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_wall_clipping_wind,"WallClipping");

		// set controls
		
	dialog_set_radio_buttons(dialog_wall_clipping_wind,kClipRadio,seg->clip,10);
	
		// show window
	
	ShowWindow(dialog_wall_clipping_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(wall_clipping_event_proc);
	InstallWindowEventHandler(dialog_wall_clipping_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_wall_clipping_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_wall_clipping_wind);
	
		// dialog to data
		
	if (!dialog_wall_clipping_cancel) {
		seg->clip=dialog_get_radio_buttons(dialog_wall_clipping_wind,kClipRadio,10);
	}

		// close window
		
	DisposeWindow(dialog_wall_clipping_wind);
	
	return(!dialog_wall_clipping_cancel);
}

