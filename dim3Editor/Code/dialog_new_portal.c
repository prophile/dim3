/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: New Portal Dialog

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

#define kPortalXSize								FOUR_CHAR_CODE('xsiz')
#define kPortalZSize								FOUR_CHAR_CODE('zsiz')
#define kPortalDivisions							FOUR_CHAR_CODE('divv')
#define kPortalHeight								FOUR_CHAR_CODE('high')
#define kPortalFloors								FOUR_CHAR_CODE('aflr')
#define kPortalCeilings								FOUR_CHAR_CODE('aclg')
#define kPortalWalls								FOUR_CHAR_CODE('awal')

bool						dialog_new_portal_cancel;
WindowRef					dialog_new_portal_wind;

/* =======================================================

      New Portal Event Handlers
      
======================================================= */

static pascal OSStatus portal_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_new_portal_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_new_portal_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_new_portal_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run New Portal
      
======================================================= */

bool dialog_new_portal_run(int *x_size,int *z_size,bool *floor_on,bool *ceiling_on,bool *wall_on,int *div,int *high)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_new_portal_wind,"NewPortal");

		// set controls
		
	dialog_set_int(dialog_new_portal_wind,kPortalXSize,0,15000);
	dialog_set_int(dialog_new_portal_wind,kPortalZSize,0,15000);
	dialog_set_int(dialog_new_portal_wind,kPortalDivisions,0,5);
	dialog_set_int(dialog_new_portal_wind,kPortalHeight,0,20);
	dialog_set_boolean(dialog_new_portal_wind,kPortalFloors,0,TRUE);
	dialog_set_boolean(dialog_new_portal_wind,kPortalCeilings,0,FALSE);
	dialog_set_boolean(dialog_new_portal_wind,kPortalWalls,0,TRUE);
	
		// show window
	
	ShowWindow(dialog_new_portal_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(portal_setting_event_proc);
	InstallWindowEventHandler(dialog_new_portal_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_new_portal_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_new_portal_wind);
	
		// dialog to data
		
	if (!dialog_new_portal_cancel) {
		*x_size=dialog_get_int(dialog_new_portal_wind,kPortalXSize,0);
		*z_size=dialog_get_int(dialog_new_portal_wind,kPortalZSize,0);
		*div=dialog_get_int(dialog_new_portal_wind,kPortalDivisions,0);
		*high=dialog_get_int(dialog_new_portal_wind,kPortalHeight,0);
		*floor_on=dialog_get_boolean(dialog_new_portal_wind,kPortalFloors,0);
		*ceiling_on=dialog_get_boolean(dialog_new_portal_wind,kPortalCeilings,0);
		*wall_on=dialog_get_boolean(dialog_new_portal_wind,kPortalWalls,0);
	}

		// close window
		
	DisposeWindow(dialog_new_portal_wind);
	
	return(!dialog_new_portal_cancel);
}

