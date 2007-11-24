/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Bone Setting Dialog

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

#define kBoneName							FOUR_CHAR_CODE('name')
#define kBoneX								FOUR_CHAR_CODE('bnex')
#define kBoneY								FOUR_CHAR_CODE('bney')
#define kBoneZ								FOUR_CHAR_CODE('bnez')
#define kBoneTag							FOUR_CHAR_CODE('tagg')
#define kBoneParent							FOUR_CHAR_CODE('prnt')

bool						dialog_bone_settings_cancel;
WindowRef					dialog_bone_settings_wind;

/* =======================================================

      Fix Bones Strings to 4 Characters
      
======================================================= */

void dialog_bone_fix_string(char *str)
{
	int				len;
	
	len=strlen(str);
	if (len==0) return;
	
	while (len<4) {
		str[len]=' ';
		len++;
	}
	
	str[4]=0x0;
}

/* =======================================================

      Bone Setting Event Handlers
      
======================================================= */

static pascal OSStatus bone_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				old_parent_idx;
	char			str[8];
	model_tag		tag,old_bone_tag;
	model_bone_type	*bone;
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_bone_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_bone_settings_wind);
					return(noErr);
					
				case kHICommandOK:
				
					bone=(model_bone_type*)data;
				
						// requires unique bone tag
						
					dialog_get_text(dialog_bone_settings_wind,kBoneTag,0,str,8);
					dialog_bone_fix_string(str);
					memmove(&tag,str,4);
						
					if (tag==model_null_tag) {
						StandardAlert(0,"\pBad Bone Tag","\pBones require a tag to uniquely identify them.  ???? is the null tag and is not allowed.  Please pick a proper 4 charater tag.",NULL,NULL);
						return(noErr);
					}
					
					old_bone_tag=bone->tag;
					bone->tag=tag;
					
					if (model_check_bone_duplicate_tag(&model,bone)) {
						bone->tag=old_bone_tag;
						StandardAlert(0,"\pBad Bone Tag","\pBones require a tag to uniquely identify them.  This tag is already being used by another bone, choose another 4 character tag.",NULL,NULL);
						return(noErr);
					}
					
					bone->tag=old_bone_tag;
					
						// check for circular bone parenting
						
					old_parent_idx=bone->parent_idx;
					bone->parent_idx=dialog_get_bone_combo(dialog_bone_settings_wind,kBoneParent,0);
						
					if (model_check_bone_circular(&model,bone)) {
						bone->parent_idx=old_parent_idx;
						StandardAlert(0,"\pBad Bone Parenting","\pThe parent of this bone will make a set of bones with circular references.  Please change the parent tag.",NULL,NULL);
						return(noErr);
					}
					
					bone->parent_idx=old_parent_idx;

						// ok, quit dialog

					QuitAppModalLoopForWindow(dialog_bone_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Bone Setting
      
======================================================= */

bool dialog_bone_settings_run(model_bone_type *bone)
{
	char					str[8];
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_bone_settings_wind,"BoneSettings");

		// set controls

	dialog_set_text(dialog_bone_settings_wind,kBoneName,0,bone->name);
	dialog_set_int(dialog_bone_settings_wind,kBoneX,0,bone->pnt.x);
	dialog_set_int(dialog_bone_settings_wind,kBoneY,0,bone->pnt.y);
	dialog_set_int(dialog_bone_settings_wind,kBoneZ,0,bone->pnt.z);
	
	memmove(str,&bone->tag,4);
	str[4]=0x0;
	dialog_set_text(dialog_bone_settings_wind,kBoneTag,0,str);
	
	dialog_set_bone_combo(dialog_bone_settings_wind,kBoneParent,0,bone->parent_idx);
	
	dialog_set_focus(dialog_bone_settings_wind,kBoneName,0);
	
		// show window
	
	ShowWindow(dialog_bone_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(bone_settings_event_proc);
	InstallWindowEventHandler(dialog_bone_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,(void*)bone,NULL);
	
		// modal window
		
	dialog_bone_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_bone_settings_wind);
	
		// dialog to data
		
	if (!dialog_bone_settings_cancel) {
		dialog_get_text(dialog_bone_settings_wind,kBoneName,0,bone->name,name_str_len);
		bone->pnt.x=dialog_get_int(dialog_bone_settings_wind,kBoneX,0);
		bone->pnt.y=dialog_get_int(dialog_bone_settings_wind,kBoneY,0);
		bone->pnt.z=dialog_get_int(dialog_bone_settings_wind,kBoneZ,0);
		
		dialog_get_text(dialog_bone_settings_wind,kBoneTag,0,str,8);
		dialog_bone_fix_string(str);
		memmove(&bone->tag,str,4);
		
		bone->parent_idx=dialog_get_bone_combo(dialog_bone_settings_wind,kBoneParent,0);
	}

		// close window
		
	DisposeWindow(dialog_bone_settings_wind);
	
	return(!dialog_bone_settings_cancel);
}

