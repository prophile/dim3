/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Spot Palette

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

#define kSpotSettingTabCount					2
#define kSpotSettingTab							FOUR_CHAR_CODE('tabb')

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

WindowRef					palette_spot_wind;

/* =======================================================

      Palette Spot Data
      
======================================================= */

void palette_spot_load(void)
{
	int						type,mesh_idx,poly_idx;
	map_mesh_type			*mesh;

		// get mesh
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	mesh=&map.mesh.meshes[mesh_idx];

		// set controls
	
	dialog_set_boolean(palette_spot_wind,kMeshSettingOn,0,mesh->flag.on);
	dialog_set_boolean(palette_spot_wind,kMeshSettingPassThrough,0,mesh->flag.pass_through);
	dialog_set_boolean(palette_spot_wind,kMeshSettingMovable,0,mesh->flag.moveable);
	dialog_set_boolean(palette_spot_wind,kMeshSettingClimbable,0,mesh->flag.climbable);
	dialog_set_boolean(palette_spot_wind,kMeshSettingHilite,0,mesh->flag.hilite);
	dialog_set_boolean(palette_spot_wind,kMeshSettingLockUV,0,mesh->flag.lock_uv);
	dialog_set_boolean(palette_spot_wind,kMeshSettingNoSelfObscure,0,mesh->flag.no_self_obscure);
	dialog_set_boolean(palette_spot_wind,kMeshSettingNeverObscure,0,mesh->flag.never_obscure);
	dialog_set_boolean(palette_spot_wind,kMeshSettingRotIndependent,0,mesh->flag.rot_independent);

	dialog_set_int(palette_spot_wind,kMeshSettingRotX,0,mesh->rot_off.x);
	dialog_set_int(palette_spot_wind,kMeshSettingRotY,0,mesh->rot_off.y);
	dialog_set_int(palette_spot_wind,kMeshSettingRotZ,0,mesh->rot_off.z);
	
	dialog_set_boolean(palette_spot_wind,kMeshSendMessageEnter,0,mesh->msg.entry_on);
	dialog_set_int(palette_spot_wind,kMeshSendMessageEnterId,0,mesh->msg.entry_id);
	dialog_set_boolean(palette_spot_wind,kMeshSendMessageExit,0,mesh->msg.exit_on);
	dialog_set_int(palette_spot_wind,kMeshSendMessageExitId,0,mesh->msg.exit_id);
	dialog_set_boolean(palette_spot_wind,kMeshSendMessageMapChange,0,mesh->msg.map_change_on);
	dialog_set_text(palette_spot_wind,kMeshSendMessageMapChangeName,0,mesh->msg.map_name);
	dialog_set_text(palette_spot_wind,kMeshSendMessageMapChangeSpotName,0,mesh->msg.map_spot_name);
	dialog_set_text(palette_spot_wind,kMeshSendMessageMapChangeSpotType,0,mesh->msg.map_spot_type);
	dialog_set_boolean(palette_spot_wind,kMeshSendMessageBase,0,mesh->msg.base_on);
	dialog_set_combo(palette_spot_wind,kMeshSendMessageBaseTeam,0,mesh->msg.base_team);

	DrawControls(palette_spot_wind);
}

void palette_spot_save(void)
{
	int						type,mesh_idx,poly_idx;
	map_mesh_type			*mesh;

		// get mesh
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	mesh=&map.mesh.meshes[mesh_idx];
		
		// get controls

	mesh->flag.on=dialog_get_boolean(palette_spot_wind,kMeshSettingOn,0);
	mesh->flag.pass_through=dialog_get_boolean(palette_spot_wind,kMeshSettingPassThrough,0);
	mesh->flag.moveable=dialog_get_boolean(palette_spot_wind,kMeshSettingMovable,0);
	mesh->flag.climbable=dialog_get_boolean(palette_spot_wind,kMeshSettingClimbable,0);
	mesh->flag.hilite=dialog_get_boolean(palette_spot_wind,kMeshSettingHilite,0);
	mesh->flag.lock_uv=dialog_get_boolean(palette_spot_wind,kMeshSettingLockUV,0);
	mesh->flag.no_self_obscure=dialog_get_boolean(palette_spot_wind,kMeshSettingNoSelfObscure,0);
	mesh->flag.never_obscure=dialog_get_boolean(palette_spot_wind,kMeshSettingNeverObscure,0);
	mesh->flag.rot_independent=dialog_get_boolean(palette_spot_wind,kMeshSettingRotIndependent,0);
	
	mesh->rot_off.x=dialog_get_int(palette_spot_wind,kMeshSettingRotX,0);
	mesh->rot_off.y=dialog_get_int(palette_spot_wind,kMeshSettingRotY,0);
	mesh->rot_off.z=dialog_get_int(palette_spot_wind,kMeshSettingRotZ,0);
	
	mesh->msg.entry_on=dialog_get_boolean(palette_spot_wind,kMeshSendMessageEnter,0);
	mesh->msg.entry_id=dialog_get_int(palette_spot_wind,kMeshSendMessageEnterId,0);
	mesh->msg.exit_on=dialog_get_boolean(palette_spot_wind,kMeshSendMessageExit,0);
	mesh->msg.exit_id=dialog_get_int(palette_spot_wind,kMeshSendMessageExitId,0);
	mesh->msg.map_change_on=dialog_get_boolean(palette_spot_wind,kMeshSendMessageMapChange,0);
	dialog_get_text(palette_spot_wind,kMeshSendMessageMapChangeName,0,mesh->msg.map_name,name_str_len);
	dialog_get_text(palette_spot_wind,kMeshSendMessageMapChangeSpotName,0,mesh->msg.map_spot_name,name_str_len);
	dialog_get_text(palette_spot_wind,kMeshSendMessageMapChangeSpotType,0,mesh->msg.map_spot_type,name_str_len);
	mesh->msg.base_on=dialog_get_boolean(palette_spot_wind,kMeshSendMessageBase,0);
	mesh->msg.base_team=dialog_get_combo(palette_spot_wind,kMeshSendMessageBaseTeam,0);
}

static pascal OSStatus palette_spot_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				event_class,event_kind;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
		palette_spot_save();
		dialog_switch_tab(palette_spot_wind,kSpotSettingTab,0,kSpotSettingTabCount);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Spot Open/Close
      
======================================================= */

void palette_spot_open(int x,int y)
{
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			tab_event_upp;
	EventTypeSpec			tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit}};

		// open the window
		
	dialog_open(&palette_spot_wind,"SpotPalette");
	MoveWindow(palette_spot_wind,x,y,FALSE);
	
		// setup the tabs
		
	dialog_set_tab(palette_spot_wind,kSpotSettingTab,0,0,kSpotSettingTabCount);

	ctrl_id.signature=kSpotSettingTab;
	ctrl_id.id=0;
	GetControlByID(palette_spot_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(palette_spot_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,palette_spot_wind,NULL);

		// show palette
		
	ShowWindow(palette_spot_wind);
}

void palette_spot_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_spot_wind,kWindowTitleBarRgn,&box);
	*x=box.left;
	*y=box.top;
	
	DisposeWindow(palette_spot_wind);
}


