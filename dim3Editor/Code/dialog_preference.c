/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Preference Dialog

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

#include "common_view.h"
#include "interface.h"
#include "dialog.h"

#define kPrefEngineName						FOUR_CHAR_CODE('name')
#define kPrefMipMapMode						FOUR_CHAR_CODE('mmap')
#define kPrefAutoTexture					FOUR_CHAR_CODE('auto')
#define kPrefDuplicateOffset				FOUR_CHAR_CODE('doff')
#define kPrefBackgroundColor				FOUR_CHAR_CODE('bcol')
#define kPrefLineColor						FOUR_CHAR_CODE('lcol')
#define kPrefMeshSelColor					FOUR_CHAR_CODE('scol')
#define kPrefPolySelColor					FOUR_CHAR_CODE('pcol')

extern map_type				map;
extern setup_type			setup;

bool						dialog_preference_cancel;
WindowRef					dialog_preference_wind;

extern bool setup_xml_write(void);

/* =======================================================

      Preference Event Handlers
      
======================================================= */

static pascal OSStatus preference_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kPrefBackgroundColor:
				case kPrefLineColor:
				case kPrefMeshSelColor:
				case kPrefPolySelColor:
					dialog_click_color(dialog_preference_wind,cmd.commandID,0);
					return(noErr);
				
				case kHICommandCancel:
					dialog_preference_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_preference_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_preference_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Preference
      
======================================================= */

void dialog_preference_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_preference_wind,"Preferences");

		// set controls
		
	dialog_set_text(dialog_preference_wind,kPrefEngineName,0,setup.engine_name);
	dialog_set_combo(dialog_preference_wind,kPrefMipMapMode,0,setup.mipmap_mode);
	dialog_set_boolean(dialog_preference_wind,kPrefAutoTexture,0,setup.auto_texture);
	dialog_set_int(dialog_preference_wind,kPrefDuplicateOffset,0,setup.duplicate_offset);
	
	dialog_set_color(dialog_preference_wind,kPrefBackgroundColor,0,&setup.col.background);
	dialog_set_color(dialog_preference_wind,kPrefLineColor,0,&setup.col.mesh_line);
	dialog_set_color(dialog_preference_wind,kPrefMeshSelColor,0,&setup.col.mesh_sel);
	dialog_set_color(dialog_preference_wind,kPrefPolySelColor,0,&setup.col.poly_sel);
	
		// show window
	
	ShowWindow(dialog_preference_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(preference_event_proc);
	InstallWindowEventHandler(dialog_preference_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_preference_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_preference_wind);
	
		// dialog to data
		
	if (!dialog_preference_cancel) {
		dialog_get_text(dialog_preference_wind,kPrefEngineName,0,setup.engine_name,256);
		setup.mipmap_mode=dialog_get_combo(dialog_preference_wind,kPrefMipMapMode,0);
		setup.auto_texture=dialog_get_boolean(dialog_preference_wind,kPrefAutoTexture,0);
		setup.duplicate_offset=dialog_get_int(dialog_preference_wind,kPrefDuplicateOffset,0);
		
		dialog_get_color(dialog_preference_wind,kPrefBackgroundColor,0,&setup.col.background);
		dialog_get_color(dialog_preference_wind,kPrefLineColor,0,&setup.col.mesh_line);
		dialog_get_color(dialog_preference_wind,kPrefMeshSelColor,0,&setup.col.mesh_sel);
		dialog_get_color(dialog_preference_wind,kPrefPolySelColor,0,&setup.col.poly_sel);
		
		setup_xml_write();
	}

		// close window
		
	DisposeWindow(dialog_preference_wind);
}

