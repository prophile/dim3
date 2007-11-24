/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Special Bone Setting Dialog

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

#define kLightBoneTag						FOUR_CHAR_CODE('lbtg')
#define kHaloBoneTag						FOUR_CHAR_CODE('hbtg')
#define kNameBoneTag						FOUR_CHAR_CODE('nbtg')

bool						dialog_special_bone_settings_cancel;
WindowRef					dialog_special_bone_settings_wind;

/* =======================================================

      Special Bone Setting Event Handlers
      
======================================================= */

static pascal OSStatus special_bone_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_special_bone_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_special_bone_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_special_bone_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Special Bone Setting
      
======================================================= */

bool dialog_special_bones_settings_run(void)
{
	int						n,idx;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_special_bone_settings_wind,"SpecialBonesSettings");

		// set controls
	
	for (n=0;n!=max_model_light;n++) {
		idx=model_find_bone(&model,model.tags.light_bone_tag[n]);
		dialog_set_bone_combo(dialog_special_bone_settings_wind,kLightBoneTag,n,idx);
	}
	
	for (n=0;n!=max_model_halo;n++) {
		idx=model_find_bone(&model,model.tags.halo_bone_tag[n]);
		dialog_set_bone_combo(dialog_special_bone_settings_wind,kHaloBoneTag,n,idx);
	}
	
	idx=model_find_bone(&model,model.tags.name_bone_tag);
	dialog_set_bone_combo(dialog_special_bone_settings_wind,kNameBoneTag,0,idx);
	
		// show window
	
	ShowWindow(dialog_special_bone_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(special_bone_settings_event_proc);
	InstallWindowEventHandler(dialog_special_bone_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_special_bone_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_special_bone_settings_wind);
	
		// dialog to data
		
	if (!dialog_special_bone_settings_cancel) {
		
		for (n=0;n!=max_model_light;n++) {
			idx=dialog_get_bone_combo(dialog_special_bone_settings_wind,kLightBoneTag,n);
			if (idx==-1) {
				model.tags.light_bone_tag[n]=model_null_tag;
			}
			else {
				model.tags.light_bone_tag[n]=model.bones[idx].tag;
			}
		}
		
		for (n=0;n!=max_model_halo;n++) {
			idx=dialog_get_bone_combo(dialog_special_bone_settings_wind,kHaloBoneTag,n);
			if (idx==-1) {
				model.tags.halo_bone_tag[n]=model_null_tag;
			}
			else {
				model.tags.halo_bone_tag[n]=model.bones[idx].tag;
			}
		}
		
		idx=dialog_get_bone_combo(dialog_special_bone_settings_wind,kNameBoneTag,n);
		if (idx==-1) {
			model.tags.name_bone_tag=model_null_tag;
		}
		else {
			model.tags.name_bone_tag=model.bones[idx].tag;
		}
	}

		// close window
		
	DisposeWindow(dialog_special_bone_settings_wind);
	
	return(!dialog_special_bone_settings_cancel);
}

