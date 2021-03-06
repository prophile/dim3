/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Mesh Scale Dialog

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

#define kMeshScaleScale							FOUR_CHAR_CODE('scle')
#define kMeshScaleButtonScale					FOUR_CHAR_CODE('scal')
#define kMeshScaleButtonReplace					FOUR_CHAR_CODE('repl')

bool						dialog_mesh_scale_replace;
WindowRef					dialog_mesh_scale_wind;

/* =======================================================

      Mesh Scale Event Handlers
      
======================================================= */

static pascal OSStatus dialog_mesh_scale_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kMeshScaleButtonScale:
					dialog_mesh_scale_replace=FALSE;
					QuitAppModalLoopForWindow(dialog_mesh_scale_wind);
					return(noErr);
					
				case kMeshScaleButtonReplace:
					dialog_mesh_scale_replace=TRUE;
					QuitAppModalLoopForWindow(dialog_mesh_scale_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Mesh Scale Dialog
      
======================================================= */

bool dialog_mesh_scale_run(float *scale)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_mesh_scale_wind,"MeshScale");

		// set controls
		
	dialog_set_float(dialog_mesh_scale_wind,kMeshScaleScale,0,(*scale));
	dialog_set_focus(dialog_mesh_scale_wind,kMeshScaleScale,0);
	
		// show window
	
	ShowWindow(dialog_mesh_scale_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_mesh_scale_event_proc);
	InstallWindowEventHandler(dialog_mesh_scale_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_mesh_scale_replace=FALSE;
	
	RunAppModalLoopForWindow(dialog_mesh_scale_wind);
	
		// dialog to data
		
	*scale=dialog_get_float(dialog_mesh_scale_wind,kMeshScaleScale,0);

		// close window
		
	DisposeWindow(dialog_mesh_scale_wind);
	
	return(dialog_mesh_scale_replace);
}

