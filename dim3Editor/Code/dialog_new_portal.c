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

#define kPortalXSize					FOUR_CHAR_CODE('xsiz')
#define kPortalYSize					FOUR_CHAR_CODE('ysiz')
#define kPortalZSize					FOUR_CHAR_CODE('zsiz')

#define kPortalMesh						FOUR_CHAR_CODE('mesh')
#define kPortalEditButton				FOUR_CHAR_CODE('edit')
#define kPortalClearButton				FOUR_CHAR_CODE('cler')

bool									dialog_new_portal_cancel;
WindowRef								dialog_new_portal_wind;

/* =======================================================

      New Portal Event Handlers
      
======================================================= */

static pascal OSStatus portal_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	char			mesh_name[file_str_len];
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kPortalEditButton:
					if (dialog_choose_library_object_run(mesh_name)) {
						dialog_set_text(dialog_new_portal_wind,kPortalMesh,0,mesh_name);
						dialog_redraw(dialog_new_portal_wind,kPortalMesh,0);
					}
					return(noErr);
					
				case kPortalClearButton:
					dialog_set_text(dialog_new_portal_wind,kPortalMesh,0,NULL);
					dialog_redraw(dialog_new_portal_wind,kPortalMesh,0);
					return(noErr);
				
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

bool dialog_new_portal_run(int *x_size,int *y_size,int *z_size,char *mesh_name)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_new_portal_wind,"NewPortal");

		// set controls
		
	dialog_set_int(dialog_new_portal_wind,kPortalXSize,0,15000);
	dialog_set_int(dialog_new_portal_wind,kPortalYSize,0,8000);
	dialog_set_int(dialog_new_portal_wind,kPortalZSize,0,15000);
	
	dialog_set_text(dialog_new_portal_wind,kPortalMesh,0,NULL);
	
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
		*y_size=dialog_get_int(dialog_new_portal_wind,kPortalYSize,0);
		*z_size=dialog_get_int(dialog_new_portal_wind,kPortalZSize,0);
		dialog_get_text(dialog_new_portal_wind,kPortalMesh,0,mesh_name,file_str_len);
	}

		// close window
		
	DisposeWindow(dialog_new_portal_wind);
	
	return(!dialog_new_portal_cancel);
}

