/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Movement Setting Dialog

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

#include "dialog.h"

extern map_type				map;

#define kMovementName						FOUR_CHAR_CODE('name')
#define kMovementGroup						FOUR_CHAR_CODE('grop')
#define kMovementReverseGroup				FOUR_CHAR_CODE('rgrp')
#define kMovementAutoStart					FOUR_CHAR_CODE('asrt')
#define kMovementAutoOpen					FOUR_CHAR_CODE('aopn')
#define kMovementAutoOpenStand				FOUR_CHAR_CODE('aoso')
#define kMovementAutoOpenDistance			FOUR_CHAR_CODE('dist')
#define kMovementAutoOpenNeverClose			FOUR_CHAR_CODE('aonc')
#define kMovementLoop						FOUR_CHAR_CODE('loop')

bool						dialog_movement_settings_cancel;
WindowRef					dialog_movement_settings_wind;

/* =======================================================

      Movement Setting Event Handlers
      
======================================================= */

static pascal OSStatus movement_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_movement_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_movement_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_movement_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Movement Setting
      
======================================================= */

bool dialog_movement_settings_run(movement_type *movement)
{
	int						n;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_movement_settings_wind,"MovementSettings");

		// set controls

	dialog_set_text(dialog_movement_settings_wind,kMovementName,0,movement->name);
	
	dialog_clear_combo(dialog_movement_settings_wind,kMovementGroup,0);
	dialog_clear_combo(dialog_movement_settings_wind,kMovementReverseGroup,0);
	dialog_add_combo_item(dialog_movement_settings_wind,kMovementReverseGroup,0,"None",0);
	
	for (n=0;n!=map.ngroup;n++) {
		dialog_add_combo_item(dialog_movement_settings_wind,kMovementGroup,0,map.groups[n].name,0);
		dialog_add_combo_item(dialog_movement_settings_wind,kMovementReverseGroup,0,map.groups[n].name,0);
	}
	
	dialog_set_combo(dialog_movement_settings_wind,kMovementGroup,0,movement->group_idx);
	dialog_set_combo(dialog_movement_settings_wind,kMovementReverseGroup,0,(movement->reverse_group_idx+1));
	
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoStart,0,movement->auto_start);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoOpen,0,movement->auto_open);
	dialog_set_int(dialog_movement_settings_wind,kMovementAutoOpenDistance,0,movement->auto_open_distance);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoOpenStand,0,movement->auto_open_stand);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoOpenNeverClose,0,movement->auto_open_never_close);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementLoop,0,movement->loop);
	
	dialog_set_focus(dialog_movement_settings_wind,kMovementName,0);
	
		// show window
	
	ShowWindow(dialog_movement_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(movement_settings_event_proc);
	InstallWindowEventHandler(dialog_movement_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_movement_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_movement_settings_wind);
	
		// dialog to data
		
	if (!dialog_movement_settings_cancel) {
		dialog_get_text(dialog_movement_settings_wind,kMovementName,0,movement->name,name_str_len);
		movement->group_idx=dialog_get_combo(dialog_movement_settings_wind,kMovementGroup,0);
		movement->reverse_group_idx=dialog_get_combo(dialog_movement_settings_wind,kMovementReverseGroup,0)-1;
		movement->auto_start=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoStart,0);
		movement->auto_open=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoOpen,0);
		movement->auto_open_distance=dialog_get_int(dialog_movement_settings_wind,kMovementAutoOpenDistance,0);
		movement->auto_open_stand=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoOpenStand,0);
		movement->auto_open_never_close=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoOpenNeverClose,0);
		movement->loop=dialog_get_boolean(dialog_movement_settings_wind,kMovementLoop,0);
	}

		// close window
		
	DisposeWindow(dialog_movement_settings_wind);
	
	return(!dialog_movement_settings_cancel);
}

