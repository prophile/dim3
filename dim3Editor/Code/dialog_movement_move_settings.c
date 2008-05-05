/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Movement Move Setting Dialog

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

extern map_type			map;

#define kMoveUserID						FOUR_CHAR_CODE('user')
#define kMoveMillisecond				FOUR_CHAR_CODE('msec')
#define kMoveSoundName					FOUR_CHAR_CODE('snds')
#define kMoveSoundPitch					FOUR_CHAR_CODE('pith')
#define kMoveMoveX						FOUR_CHAR_CODE('movx')
#define kMoveMoveY						FOUR_CHAR_CODE('movy')
#define kMoveMoveZ						FOUR_CHAR_CODE('movz')
#define kMoveRotX						FOUR_CHAR_CODE('rotx')
#define kMoveRotY						FOUR_CHAR_CODE('roty')
#define kMoveRotZ						FOUR_CHAR_CODE('rotz')

bool						dialog_movement_move_settings_cancel;
WindowRef					dialog_movement_move_settings_wind;

/* =======================================================

      Movement Move Setting Event Handlers
      
======================================================= */

static pascal OSStatus movement_move_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_movement_move_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_movement_move_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_movement_move_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Movement Move Setting
      
======================================================= */

bool dialog_movement_move_settings_run(movement_move_type *move)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_movement_move_settings_wind,"MovementMoveSettings");

		// set controls

	dialog_set_int(dialog_movement_move_settings_wind,kMoveMillisecond,0,move->msec);
	dialog_set_int(dialog_movement_move_settings_wind,kMoveUserID,0,move->user_id);
	dialog_special_combo_fill_sound(dialog_movement_move_settings_wind,kMoveSoundName,0,move->sound_name);
	dialog_set_float(dialog_movement_move_settings_wind,kMoveSoundPitch,0,move->sound_pitch);
	
	dialog_set_int(dialog_movement_move_settings_wind,kMoveMoveX,0,move->mov.x);
	dialog_set_int(dialog_movement_move_settings_wind,kMoveMoveY,0,move->mov.y);
	dialog_set_int(dialog_movement_move_settings_wind,kMoveMoveZ,0,move->mov.z);
	
	dialog_set_float(dialog_movement_move_settings_wind,kMoveRotX,0,move->rot.x);
	dialog_set_float(dialog_movement_move_settings_wind,kMoveRotY,0,move->rot.y);
	dialog_set_float(dialog_movement_move_settings_wind,kMoveRotZ,0,move->rot.z);

		// show window
	
	ShowWindow(dialog_movement_move_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(movement_move_settings_event_proc);
	InstallWindowEventHandler(dialog_movement_move_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_movement_move_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_movement_move_settings_wind);
	
		// dialog to data
		
	if (!dialog_movement_move_settings_cancel) {
		move->msec=dialog_get_int(dialog_movement_move_settings_wind,kMoveMillisecond,0);
		move->user_id=dialog_get_int(dialog_movement_move_settings_wind,kMoveUserID,0);
		dialog_special_combo_get_sound(dialog_movement_move_settings_wind,kMoveSoundName,0,move->sound_name,name_str_len);
		move->sound_pitch=dialog_get_float(dialog_movement_move_settings_wind,kMoveSoundPitch,0);
		
		move->mov.x=dialog_get_int(dialog_movement_move_settings_wind,kMoveMoveX,0);
		move->mov.y=dialog_get_int(dialog_movement_move_settings_wind,kMoveMoveY,0);
		move->mov.z=dialog_get_int(dialog_movement_move_settings_wind,kMoveMoveZ,0);
		
		move->rot.x=dialog_get_float(dialog_movement_move_settings_wind,kMoveRotX,0);
		move->rot.y=dialog_get_float(dialog_movement_move_settings_wind,kMoveRotY,0);
		move->rot.z=dialog_get_float(dialog_movement_move_settings_wind,kMoveRotZ,0);
	}

		// close window
		
	DisposeWindow(dialog_movement_move_settings_wind);
	
	return(!dialog_movement_move_settings_cancel);
}

