/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Bone Move Setting Dialog

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

extern model_type		model;

#define kBoneMoveRotX						FOUR_CHAR_CODE('rotx')
#define kBoneMoveRotY						FOUR_CHAR_CODE('roty')
#define kBoneMoveRotZ						FOUR_CHAR_CODE('rotz')
#define kBoneMoveMoveX						FOUR_CHAR_CODE('movx')
#define kBoneMoveMoveY						FOUR_CHAR_CODE('movy')
#define kBoneMoveMoveZ						FOUR_CHAR_CODE('movz')
#define kBoneMoveAcceleration				FOUR_CHAR_CODE('accl')
#define kBoneMoveSkipBlended				FOUR_CHAR_CODE('sbld')

bool						dialog_bone_move_settings_cancel;
WindowRef					dialog_bone_move_settings_wind;

/* =======================================================

      Pose Setting Event Handlers
      
======================================================= */

static pascal OSStatus bone_move_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_bone_move_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_bone_move_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_bone_move_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Pose Setting
      
======================================================= */

bool dialog_bone_move_settings_run(model_bone_move_type *bone_move)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_bone_move_settings_wind,"BoneMoveSettings");

		// set controls

	dialog_set_float(dialog_bone_move_settings_wind,kBoneMoveRotX,0,bone_move->rot.x);
	dialog_set_float(dialog_bone_move_settings_wind,kBoneMoveRotY,0,bone_move->rot.y);
	dialog_set_float(dialog_bone_move_settings_wind,kBoneMoveRotZ,0,bone_move->rot.z);
	dialog_set_float(dialog_bone_move_settings_wind,kBoneMoveMoveX,0,bone_move->mov.x);
	dialog_set_float(dialog_bone_move_settings_wind,kBoneMoveMoveY,0,bone_move->mov.y);
	dialog_set_float(dialog_bone_move_settings_wind,kBoneMoveMoveZ,0,bone_move->mov.z);
	dialog_set_float(dialog_bone_move_settings_wind,kBoneMoveAcceleration,0,bone_move->acceleration);
	dialog_set_boolean(dialog_bone_move_settings_wind,kBoneMoveSkipBlended,0,bone_move->skip_blended);
	
		// show window
	
	ShowWindow(dialog_bone_move_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(bone_move_settings_event_proc);
	InstallWindowEventHandler(dialog_bone_move_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_bone_move_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_bone_move_settings_wind);
	
		// dialog to data
		
	if (!dialog_bone_move_settings_cancel) {
		bone_move->rot.x=dialog_get_float(dialog_bone_move_settings_wind,kBoneMoveRotX,0);
		bone_move->rot.y=dialog_get_float(dialog_bone_move_settings_wind,kBoneMoveRotY,0);
		bone_move->rot.z=dialog_get_float(dialog_bone_move_settings_wind,kBoneMoveRotZ,0);
		bone_move->mov.x=dialog_get_float(dialog_bone_move_settings_wind,kBoneMoveMoveX,0);
		bone_move->mov.y=dialog_get_float(dialog_bone_move_settings_wind,kBoneMoveMoveY,0);
		bone_move->mov.z=dialog_get_float(dialog_bone_move_settings_wind,kBoneMoveMoveZ,0);
		bone_move->acceleration=dialog_get_float(dialog_bone_move_settings_wind,kBoneMoveAcceleration,0);
		bone_move->skip_blended=dialog_get_boolean(dialog_bone_move_settings_wind,kBoneMoveSkipBlended,0);
	}

		// close window
		
	DisposeWindow(dialog_bone_move_settings_wind);
	
	return(!dialog_bone_move_settings_cancel);
}

