/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Create Grid Mesh Dialog

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

#include "dialog.h"

#define kGridMeshXDivision				FOUR_CHAR_CODE('xdiv')
#define kGridMeshYDivision				FOUR_CHAR_CODE('ydiv')
#define kGridMeshZDivision				FOUR_CHAR_CODE('zdiv')

bool									dialog_create_grid_mesh_cancel;
WindowRef								dialog_create_grid_mesh_wind;

/* =======================================================

      Create Grid Mesh Event Handlers
      
======================================================= */

static pascal OSStatus create_grid_mesh_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandNew:
					QuitAppModalLoopForWindow(dialog_create_grid_mesh_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_create_grid_mesh_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_create_grid_mesh_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Create Grid Mesh
      
======================================================= */

bool dialog_create_grid_mesh_run(int *xdiv,int *ydiv,int *zdiv)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_create_grid_mesh_wind,"GridMesh");
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(create_grid_mesh_event_proc);
	InstallWindowEventHandler(dialog_create_grid_mesh_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// setup controls
		
	dialog_set_int(dialog_create_grid_mesh_wind,kGridMeshXDivision,0,10);
	dialog_set_int(dialog_create_grid_mesh_wind,kGridMeshYDivision,0,10);
	dialog_set_int(dialog_create_grid_mesh_wind,kGridMeshZDivision,0,10);
		
		// show window
	
	ShowWindow(dialog_create_grid_mesh_wind);
	
		// modal window
		
	dialog_create_grid_mesh_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_create_grid_mesh_wind);
	
		// get object name
		
	if (!dialog_create_grid_mesh_cancel) {
		*xdiv=dialog_get_int(dialog_create_grid_mesh_wind,kGridMeshXDivision,0);
		*ydiv=dialog_get_int(dialog_create_grid_mesh_wind,kGridMeshYDivision,0);
		*zdiv=dialog_get_int(dialog_create_grid_mesh_wind,kGridMeshZDivision,0);
	}
	
		// close window
		
	DisposeWindow(dialog_create_grid_mesh_wind);
	
	return(!dialog_create_grid_mesh_cancel);
}

