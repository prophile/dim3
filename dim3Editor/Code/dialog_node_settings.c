/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Node Setting Dialog

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

#define kNodeName						FOUR_CHAR_CODE('name')
#define kNodeEventID					FOUR_CHAR_CODE('vale')
#define kNodeAngleX						FOUR_CHAR_CODE('angx')
#define kNodeAngleY						FOUR_CHAR_CODE('angy')
#define kNodeAngleZ						FOUR_CHAR_CODE('angz')

bool						dialog_node_settings_cancel;
WindowRef					dialog_node_settings_wind;

/* =======================================================

      Node Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus dialog_node_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_node_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_node_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_node_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Node Setting
      
======================================================= */

bool dialog_node_settings_run(node_type *node)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_node_settings_wind,"NodeSettings");

		// set controls
		
	dialog_set_text(dialog_node_settings_wind,kNodeName,0,node->name);
	dialog_set_int(dialog_node_settings_wind,kNodeEventID,0,node->event_id);
	dialog_set_float(dialog_node_settings_wind,kNodeAngleX,0,node->ang.x);
	dialog_set_float(dialog_node_settings_wind,kNodeAngleY,0,node->ang.y);
	dialog_set_float(dialog_node_settings_wind,kNodeAngleZ,0,node->ang.z);
	
		// show window
	
	ShowWindow(dialog_node_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_node_setting_event_proc);
	InstallWindowEventHandler(dialog_node_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_node_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_node_settings_wind);
	
		// dialog to data
		
	if (!dialog_node_settings_cancel) {
		dialog_get_text(dialog_node_settings_wind,kNodeName,0,node->name,name_str_len);
		node->event_id=dialog_get_int(dialog_node_settings_wind,kNodeEventID,0);
		node->ang.x=dialog_get_float(dialog_node_settings_wind,kNodeAngleX,0);
		node->ang.y=dialog_get_float(dialog_node_settings_wind,kNodeAngleY,0);
		node->ang.z=dialog_get_float(dialog_node_settings_wind,kNodeAngleZ,0);
		
		main_wind_tool_fill_node_combo();
	}

		// close window
		
	DisposeWindow(dialog_node_settings_wind);
	
	return(!dialog_node_settings_cancel);
}

