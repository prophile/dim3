/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Setting Dialog

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

#define kModelSettingTabCount				4
#define kModelSettingTab					FOUR_CHAR_CODE('tabb')

#define kRotateMode							FOUR_CHAR_CODE('rotm')
#define kCenterXOff							FOUR_CHAR_CODE('cxof')
#define kCenterYOff							FOUR_CHAR_CODE('cyof')
#define kCenterZOff							FOUR_CHAR_CODE('czof')
#define kViewXSize							FOUR_CHAR_CODE('vxsz')
#define kViewYSize							FOUR_CHAR_CODE('vysz')
#define kViewZSize							FOUR_CHAR_CODE('vzsz')
#define kViewXOff							FOUR_CHAR_CODE('vxof')
#define kViewYOff							FOUR_CHAR_CODE('vyof')
#define kViewZOff							FOUR_CHAR_CODE('vzof')
#define kShadowXSize						FOUR_CHAR_CODE('sxsz')
#define kShadowYSize						FOUR_CHAR_CODE('sysz')
#define kShadowZSize						FOUR_CHAR_CODE('szsz')
#define kShadowXOff							FOUR_CHAR_CODE('sxof')
#define kShadowYOff							FOUR_CHAR_CODE('syof')
#define kShadowZOff							FOUR_CHAR_CODE('szof')
#define kShadowFudge						FOUR_CHAR_CODE('fdge')

#define kLightBoneTag						FOUR_CHAR_CODE('lbtg')
#define kHaloBoneTag						FOUR_CHAR_CODE('hbtg')
#define kNameBoneTag						FOUR_CHAR_CODE('nbtg')

bool						dialog_model_settings_cancel;
WindowRef					dialog_model_settings_wind;

extern model_type			model;

/* =======================================================

      Model Setting Event Handlers
      
======================================================= */

static pascal OSStatus model_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_model_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_model_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_model_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus pose_move_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				event_class,event_kind;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
		dialog_switch_tab(dialog_model_settings_wind,kModelSettingTab,0,kModelSettingTabCount);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Model Setting
      
======================================================= */

bool dialog_model_settings_run(void)
{
	int						n,idx;
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_model_settings_wind,"ModelSettings");
	
		// tab
		
	dialog_set_tab(dialog_model_settings_wind,kModelSettingTab,0,0,kModelSettingTabCount);
	
	ctrl_id.signature=kModelSettingTab;
	ctrl_id.id=0;
	GetControlByID(dialog_model_settings_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(pose_move_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_model_settings_wind,NULL);

		// set controls
		
	dialog_set_int(dialog_model_settings_wind,kCenterXOff,0,model.center.x);
	dialog_set_int(dialog_model_settings_wind,kCenterYOff,0,model.center.y);
	dialog_set_int(dialog_model_settings_wind,kCenterZOff,0,model.center.z);
			
	dialog_set_int(dialog_model_settings_wind,kViewXSize,0,model.view_box.size.x);
	dialog_set_int(dialog_model_settings_wind,kViewYSize,0,model.view_box.size.y);
	dialog_set_int(dialog_model_settings_wind,kViewZSize,0,model.view_box.size.z);
	dialog_set_int(dialog_model_settings_wind,kViewXOff,0,model.view_box.offset.x);
	dialog_set_int(dialog_model_settings_wind,kViewYOff,0,model.view_box.offset.y);
	dialog_set_int(dialog_model_settings_wind,kViewZOff,0,model.view_box.offset.z);

	dialog_set_int(dialog_model_settings_wind,kShadowXSize,0,model.shadow_box.size.x);
	dialog_set_int(dialog_model_settings_wind,kShadowYSize,0,model.shadow_box.size.y);
	dialog_set_int(dialog_model_settings_wind,kShadowZSize,0,model.shadow_box.size.z);
	dialog_set_int(dialog_model_settings_wind,kShadowXOff,0,model.shadow_box.offset.x);
	dialog_set_int(dialog_model_settings_wind,kShadowYOff,0,model.shadow_box.offset.y);
	dialog_set_int(dialog_model_settings_wind,kShadowZOff,0,model.shadow_box.offset.z);
	dialog_set_int(dialog_model_settings_wind,kShadowFudge,0,model.shadow_fudge);
	
	dialog_set_boolean(dialog_model_settings_wind,kRotateMode,0,(model.deform_mode==deform_mode_comulative_rotate));

	idx=model_find_bone(&model,model.tags.name_bone_tag);
	dialog_set_bone_combo(dialog_model_settings_wind,kNameBoneTag,0,idx);
	
	for (n=0;n!=max_model_light;n++) {
		idx=model_find_bone(&model,model.tags.light_bone_tag[n]);
		dialog_set_bone_combo(dialog_model_settings_wind,kLightBoneTag,n,idx);
	}
	
	for (n=0;n!=max_model_halo;n++) {
		idx=model_find_bone(&model,model.tags.halo_bone_tag[n]);
		dialog_set_bone_combo(dialog_model_settings_wind,kHaloBoneTag,n,idx);
	}
	
		// show window
	
	ShowWindow(dialog_model_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(model_settings_event_proc);
	InstallWindowEventHandler(dialog_model_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_model_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_model_settings_wind);
	
		// dialog to data
		
	if (!dialog_model_settings_cancel) {
		model.center.x=dialog_get_int(dialog_model_settings_wind,kCenterXOff,0);
		model.center.y=dialog_get_int(dialog_model_settings_wind,kCenterYOff,0);
		model.center.z=dialog_get_int(dialog_model_settings_wind,kCenterZOff,0);
		
		model.view_box.size.x=dialog_get_int(dialog_model_settings_wind,kViewXSize,0);
		model.view_box.size.y=dialog_get_int(dialog_model_settings_wind,kViewYSize,0);
		model.view_box.size.z=dialog_get_int(dialog_model_settings_wind,kViewZSize,0);
		model.view_box.offset.x=dialog_get_int(dialog_model_settings_wind,kViewXOff,0);
		model.view_box.offset.y=dialog_get_int(dialog_model_settings_wind,kViewYOff,0);
		model.view_box.offset.z=dialog_get_int(dialog_model_settings_wind,kViewZOff,0);
		
		model.shadow_box.size.x=dialog_get_int(dialog_model_settings_wind,kShadowXSize,0);
		model.shadow_box.size.y=dialog_get_int(dialog_model_settings_wind,kShadowYSize,0);
		model.shadow_box.size.z=dialog_get_int(dialog_model_settings_wind,kShadowZSize,0);
		model.shadow_box.offset.x=dialog_get_int(dialog_model_settings_wind,kShadowXOff,0);
		model.shadow_box.offset.y=dialog_get_int(dialog_model_settings_wind,kShadowYOff,0);
		model.shadow_box.offset.z=dialog_get_int(dialog_model_settings_wind,kShadowZOff,0);
		model.shadow_fudge=dialog_get_int(dialog_model_settings_wind,kShadowFudge,0);
		
		if (dialog_get_boolean(dialog_model_settings_wind,kRotateMode,0)) {
			model.deform_mode=deform_mode_comulative_rotate;
		}
		else {
			model.deform_mode=deform_mode_single_rotate;
		}
		
		idx=dialog_get_bone_combo(dialog_model_settings_wind,kNameBoneTag,n);
		if (idx==-1) {
			model.tags.name_bone_tag=model_null_tag;
		}
		else {
			model.tags.name_bone_tag=model.bones[idx].tag;
		}

		for (n=0;n!=max_model_light;n++) {
			idx=dialog_get_bone_combo(dialog_model_settings_wind,kLightBoneTag,n);
			if (idx==-1) {
				model.tags.light_bone_tag[n]=model_null_tag;
			}
			else {
				model.tags.light_bone_tag[n]=model.bones[idx].tag;
			}
		}
		
		for (n=0;n!=max_model_halo;n++) {
			idx=dialog_get_bone_combo(dialog_model_settings_wind,kHaloBoneTag,n);
			if (idx==-1) {
				model.tags.halo_bone_tag[n]=model_null_tag;
			}
			else {
				model.tags.halo_bone_tag[n]=model.bones[idx].tag;
			}
		}
	}

		// close window
		
	DisposeWindow(dialog_model_settings_wind);
	
	return(!dialog_model_settings_cancel);
}

