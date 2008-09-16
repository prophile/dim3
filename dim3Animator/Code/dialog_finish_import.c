/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Dialog Import Finish

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

#define kImportHeight					FOUR_CHAR_CODE('scle')

bool						dialog_import_finish_cancel;
WindowRef					dialog_import_finish_wind;

/* =======================================================

      Import Finish Event Handlers
      
======================================================= */

static pascal OSStatus import_finish_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_import_finish_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_import_finish_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_import_finish_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Import Finish
      
======================================================= */

void dialog_import_finish_run(model_type *mdl,float *scale)
{
	int						minx,maxx,minz,maxz,miny,maxy;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_import_finish_wind,"ImportFinish");

		// show window
	
	ShowWindow(dialog_import_finish_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(import_finish_event_proc);
	InstallWindowEventHandler(dialog_import_finish_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_import_finish_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_import_finish_wind);
	
		// get scaling factor
		
	model_get_vertex_extent_all(mdl,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	*scale=dialog_get_float(dialog_import_finish_wind,kImportHeight,0)/(float)abs(maxy-miny);

		// close window
		
	DisposeWindow(dialog_import_finish_wind);
}

