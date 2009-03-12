/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Mesh Dialog

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

#include "interface.h"
#include "common_view.h"
#include "dialog.h"

#define kMeshSettingTabCount					2
#define kMeshSettingTab							FOUR_CHAR_CODE('tabb')

#define kMeshSettingOn							FOUR_CHAR_CODE('fson')
#define kMeshSettingPassThrough					FOUR_CHAR_CODE('fpth')
#define kMeshSettingMovable						FOUR_CHAR_CODE('fmov')
#define kMeshSettingShiftable					FOUR_CHAR_CODE('fsht')
#define kMeshSettingClimbable					FOUR_CHAR_CODE('fclb')
#define kMeshSettingHilite						FOUR_CHAR_CODE('hilt')
#define kMeshSettingLockUV						FOUR_CHAR_CODE('lkuv')
#define kMeshSettingNoSelfObscure				FOUR_CHAR_CODE('nsob')
#define kMeshSettingNeverObscure				FOUR_CHAR_CODE('nvob')
#define kMeshSettingRotIndependent				FOUR_CHAR_CODE('rtip')

#define kMeshSettingRotX						FOUR_CHAR_CODE('rotx')
#define kMeshSettingRotY						FOUR_CHAR_CODE('roty')
#define kMeshSettingRotZ						FOUR_CHAR_CODE('rotz')

#define kMeshSendMessageEnter					FOUR_CHAR_CODE('smen')
#define kMeshSendMessageEnterId					FOUR_CHAR_CODE('meid')
#define kMeshSendMessageExit					FOUR_CHAR_CODE('smex')
#define kMeshSendMessageExitId					FOUR_CHAR_CODE('mxid')
#define kMeshSendMessageMapChange				FOUR_CHAR_CODE('samc')
#define kMeshSendMessageMapChangeName			FOUR_CHAR_CODE('amnm')
#define kMeshSendMessageMapChangeSpotName		FOUR_CHAR_CODE('amsn')
#define kMeshSendMessageMapChangeSpotType		FOUR_CHAR_CODE('amst')
#define kMeshSendMessageBase					FOUR_CHAR_CODE('tbon')
#define kMeshSendMessageBaseTeam				FOUR_CHAR_CODE('tbtx')

extern map_type				map;

bool						dialog_mesh_setting_cancel;
WindowRef					dialog_mesh_setting_wind;

/* =======================================================

      Mesh Setting Event Handlers
      
======================================================= */

static pascal OSStatus mesh_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_mesh_setting_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_mesh_setting_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_mesh_setting_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus mesh_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				event_class,event_kind;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
		dialog_switch_tab(dialog_mesh_setting_wind,kMeshSettingTab,0,kMeshSettingTabCount);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Mesh Setting
      
======================================================= */

bool dialog_mesh_setting_run(void)
{
	int						n,cnt,type,mesh_idx,poly_idx;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_mesh_setting_wind,"MeshSetting");
	
		// use first selected mesh as basis
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	
	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// tab
		
	dialog_set_tab(dialog_mesh_setting_wind,kMeshSettingTab,0,0,kMeshSettingTabCount);
	
	ctrl_id.signature=kMeshSettingTab;
	ctrl_id.id=0;
	GetControlByID(dialog_mesh_setting_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(mesh_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_mesh_setting_wind,NULL);

		// set controls
	
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingOn,0,mesh->flag.on);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingPassThrough,0,mesh->flag.pass_through);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingMovable,0,mesh->flag.moveable);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingClimbable,0,mesh->flag.climbable);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingHilite,0,mesh->flag.hilite);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingLockUV,0,mesh->flag.lock_uv);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingNoSelfObscure,0,mesh->flag.no_self_obscure);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingNeverObscure,0,mesh->flag.never_obscure);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingRotIndependent,0,mesh->flag.rot_independent);

	dialog_set_int(dialog_mesh_setting_wind,kMeshSettingRotX,0,mesh->rot_off.x);
	dialog_set_int(dialog_mesh_setting_wind,kMeshSettingRotY,0,mesh->rot_off.y);
	dialog_set_int(dialog_mesh_setting_wind,kMeshSettingRotZ,0,mesh->rot_off.z);
	
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSendMessageEnter,0,mesh->msg.entry_on);
	dialog_set_int(dialog_mesh_setting_wind,kMeshSendMessageEnterId,0,mesh->msg.entry_id);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSendMessageExit,0,mesh->msg.exit_on);
	dialog_set_int(dialog_mesh_setting_wind,kMeshSendMessageExitId,0,mesh->msg.exit_id);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSendMessageMapChange,0,mesh->msg.map_change_on);
	dialog_set_text(dialog_mesh_setting_wind,kMeshSendMessageMapChangeName,0,mesh->msg.map_name);
	dialog_set_text(dialog_mesh_setting_wind,kMeshSendMessageMapChangeSpotName,0,mesh->msg.map_spot_name);
	dialog_set_text(dialog_mesh_setting_wind,kMeshSendMessageMapChangeSpotType,0,mesh->msg.map_spot_type);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSendMessageBase,0,mesh->msg.base_on);
	dialog_set_combo(dialog_mesh_setting_wind,kMeshSendMessageBaseTeam,0,mesh->msg.base_team);
	
		// show window
	
	ShowWindow(dialog_mesh_setting_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(mesh_setting_event_proc);
	InstallWindowEventHandler(dialog_mesh_setting_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_mesh_setting_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_mesh_setting_wind);
	
		// dialog to data
		
	if (!dialog_mesh_setting_cancel) {
	
		cnt=select_count();
		
		for (n=0;n!=cnt;n++) {
			
			select_get(n,&type,&mesh_idx,&poly_idx);
			if (type!=mesh_piece) continue;
	
			mesh=&map.mesh.meshes[mesh_idx];
			poly=&mesh->polys[poly_idx];

			mesh->flag.on=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingOn,0);
			mesh->flag.pass_through=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingPassThrough,0);
			mesh->flag.moveable=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingMovable,0);
			mesh->flag.climbable=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingClimbable,0);
			mesh->flag.hilite=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingHilite,0);
			mesh->flag.lock_uv=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingLockUV,0);
			mesh->flag.no_self_obscure=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingNoSelfObscure,0);
			mesh->flag.never_obscure=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingNeverObscure,0);
			mesh->flag.rot_independent=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingRotIndependent,0);
			
			mesh->rot_off.x=dialog_get_int(dialog_mesh_setting_wind,kMeshSettingRotX,0);
			mesh->rot_off.y=dialog_get_int(dialog_mesh_setting_wind,kMeshSettingRotY,0);
			mesh->rot_off.z=dialog_get_int(dialog_mesh_setting_wind,kMeshSettingRotZ,0);
			
			mesh->msg.entry_on=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSendMessageEnter,0);
			mesh->msg.entry_id=dialog_get_int(dialog_mesh_setting_wind,kMeshSendMessageEnterId,0);
			mesh->msg.exit_on=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSendMessageExit,0);
			mesh->msg.exit_id=dialog_get_int(dialog_mesh_setting_wind,kMeshSendMessageExitId,0);
			mesh->msg.map_change_on=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSendMessageMapChange,0);
			dialog_get_text(dialog_mesh_setting_wind,kMeshSendMessageMapChangeName,0,mesh->msg.map_name,name_str_len);
			dialog_get_text(dialog_mesh_setting_wind,kMeshSendMessageMapChangeSpotName,0,mesh->msg.map_spot_name,name_str_len);
			dialog_get_text(dialog_mesh_setting_wind,kMeshSendMessageMapChangeSpotType,0,mesh->msg.map_spot_type,name_str_len);
			mesh->msg.base_on=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSendMessageBase,0);
			mesh->msg.base_team=dialog_get_combo(dialog_mesh_setting_wind,kMeshSendMessageBaseTeam,0);
		}
	}
	
		// close window
		
	DisposeWindow(dialog_mesh_setting_wind);
	
	return(!dialog_mesh_setting_cancel);
}

