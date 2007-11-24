/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Copy Mesh Dialog

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

extern model_type		model;

#define kCopyMeshMesh						FOUR_CHAR_CODE('mesh')

bool						dialog_copy_mesh_cancel;
WindowRef					dialog_copy_mesh_wind;

/* =======================================================

      Copy Mesh Event Handlers
      
======================================================= */

static pascal OSStatus copy_mesh_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_copy_mesh_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_copy_mesh_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_copy_mesh_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Copy Mesh
      
======================================================= */

int dialog_copy_mesh_run(void)
{
	int						i,mesh_idx;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_copy_mesh_wind,"CopyMesh");

		// set controls

	dialog_clear_combo(dialog_copy_mesh_wind,kCopyMeshMesh,0);
	
	for (i=0;i!=model.nmesh;i++) {
		dialog_add_combo_item(dialog_copy_mesh_wind,kCopyMeshMesh,0,model.meshes[i].name,FOUR_CHAR_CODE('\?\?\?\?'));
	}
	
	dialog_set_combo(dialog_copy_mesh_wind,kCopyMeshMesh,0,0);
	
		// show window
	
	ShowWindow(dialog_copy_mesh_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(copy_mesh_event_proc);
	InstallWindowEventHandler(dialog_copy_mesh_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_copy_mesh_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_copy_mesh_wind);
	
		// dialog to data
		
	mesh_idx=-1;
	
	if (!dialog_copy_mesh_cancel) {
		mesh_idx=dialog_get_combo(dialog_copy_mesh_wind,kCopyMeshMesh,0);
	}

		// close window
		
	DisposeWindow(dialog_copy_mesh_wind);
	
	return(mesh_idx);
}

