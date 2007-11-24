/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Mesh Info Dialog

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dialog.h"

#define kMeshInfoName					FOUR_CHAR_CODE('name')
#define kMeshNoLighting					FOUR_CHAR_CODE('nlit')
#define kMeshAdditive					FOUR_CHAR_CODE('badd')

bool					dialog_cancel;
WindowRef				dialog_mesh_info_wind;

/* =======================================================

      Mesh Info Event Handlers
      
======================================================= */

static pascal OSStatus mesh_info_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_mesh_info_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_mesh_info_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run File New
      
======================================================= */

bool dialog_mesh_info_run(model_mesh_type *mesh)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_mesh_info_wind,"MeshInfo");
	
		// setup the controls
		
	dialog_set_text(dialog_mesh_info_wind,kMeshInfoName,0,mesh->name);
	dialog_set_boolean(dialog_mesh_info_wind,kMeshNoLighting,0,mesh->no_lighting);
	dialog_set_boolean(dialog_mesh_info_wind,kMeshAdditive,0,mesh->blend_add);
	
		// show window
	
	ShowWindow(dialog_mesh_info_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(mesh_info_event_proc);
	InstallWindowEventHandler(dialog_mesh_info_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_cancel=FALSE;
	dialog_set_focus(dialog_mesh_info_wind,'name',0);
	
	RunAppModalLoopForWindow(dialog_mesh_info_wind);
	
	if (!dialog_cancel) {
		dialog_get_text(dialog_mesh_info_wind,kMeshInfoName,0,mesh->name,name_str_len);
		mesh->no_lighting=dialog_get_boolean(dialog_mesh_info_wind,kMeshNoLighting,0);
		mesh->blend_add=dialog_get_boolean(dialog_mesh_info_wind,kMeshAdditive,0);
	}
	
		// close window

	DisposeWindow(dialog_mesh_info_wind);
	
	return(!dialog_cancel);
}

