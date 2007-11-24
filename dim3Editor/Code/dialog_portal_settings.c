/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Portal Setting Dialog

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

#define kPortalName									FOUR_CHAR_CODE('name')
#define kPortalSendMessageEnter						FOUR_CHAR_CODE('smen')
#define kPortalSendMessageEnterId					FOUR_CHAR_CODE('meid')
#define kPortalSendMessageExit						FOUR_CHAR_CODE('smex')
#define kPortalSendMessageExitId					FOUR_CHAR_CODE('mxid')
#define kPortalSendMessageMapChange					FOUR_CHAR_CODE('samc')
#define kPortalSendMessageMapChangeName				FOUR_CHAR_CODE('amnm')
#define kPortalSendMessageMapChangeSpotName			FOUR_CHAR_CODE('amsn')
#define kPortalSendMessageMapChangeSpotType			FOUR_CHAR_CODE('amst')
#define kPortalSendMessageBase						FOUR_CHAR_CODE('tbon')
#define kPortalSendMessageBaseTeam					FOUR_CHAR_CODE('tbtx')

bool						dialog_portal_settings_cancel;
WindowRef					dialog_portal_settings_wind;

/* =======================================================

      Portal Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus portal_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_portal_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_portal_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_portal_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Portal Setting
      
======================================================= */

bool dialog_portal_settings_run(portal_type *portal)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_portal_settings_wind,"PortalSettings");

		// set controls
		
	dialog_set_text(dialog_portal_settings_wind,kPortalName,0,portal->name);
	dialog_set_boolean(dialog_portal_settings_wind,kPortalSendMessageEnter,0,portal->msg.entry_on);
	dialog_set_int(dialog_portal_settings_wind,kPortalSendMessageEnterId,0,portal->msg.entry_id);
	dialog_set_boolean(dialog_portal_settings_wind,kPortalSendMessageExit,0,portal->msg.exit_on);
	dialog_set_int(dialog_portal_settings_wind,kPortalSendMessageExitId,0,portal->msg.exit_id);
	dialog_set_boolean(dialog_portal_settings_wind,kPortalSendMessageMapChange,0,portal->msg.map_change_on);
	dialog_set_text(dialog_portal_settings_wind,kPortalSendMessageMapChangeName,0,portal->msg.map_name);
	dialog_set_text(dialog_portal_settings_wind,kPortalSendMessageMapChangeSpotName,0,portal->msg.map_spot_name);
	dialog_set_text(dialog_portal_settings_wind,kPortalSendMessageMapChangeSpotType,0,portal->msg.map_spot_type);
	dialog_set_boolean(dialog_portal_settings_wind,kPortalSendMessageBase,0,portal->msg.base_on);
	dialog_set_combo(dialog_portal_settings_wind,kPortalSendMessageBaseTeam,0,portal->msg.base_team);
	
		// show window
	
	ShowWindow(dialog_portal_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(portal_setting_event_proc);
	InstallWindowEventHandler(dialog_portal_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_portal_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_portal_settings_wind);
	
		// dialog to data
		
	if (!dialog_portal_settings_cancel) {
		dialog_get_text(dialog_portal_settings_wind,kPortalName,0,portal->name,name_str_len);
		portal->msg.entry_on=dialog_get_boolean(dialog_portal_settings_wind,kPortalSendMessageEnter,0);
		portal->msg.entry_id=dialog_get_int(dialog_portal_settings_wind,kPortalSendMessageEnterId,0);
		portal->msg.exit_on=dialog_get_boolean(dialog_portal_settings_wind,kPortalSendMessageExit,0);
		portal->msg.exit_id=dialog_get_int(dialog_portal_settings_wind,kPortalSendMessageExitId,0);
		portal->msg.map_change_on=dialog_get_boolean(dialog_portal_settings_wind,kPortalSendMessageMapChange,0);
		dialog_get_text(dialog_portal_settings_wind,kPortalSendMessageMapChangeName,0,portal->msg.map_name,name_str_len);
		dialog_get_text(dialog_portal_settings_wind,kPortalSendMessageMapChangeSpotName,0,portal->msg.map_spot_name,name_str_len);
		dialog_get_text(dialog_portal_settings_wind,kPortalSendMessageMapChangeSpotType,0,portal->msg.map_spot_type,name_str_len);
		portal->msg.base_on=dialog_get_boolean(dialog_portal_settings_wind,kPortalSendMessageBase,0);
		portal->msg.base_team=dialog_get_combo(dialog_portal_settings_wind,kPortalSendMessageBaseTeam,0);
	}

		// close window
		
	DisposeWindow(dialog_portal_settings_wind);
	
	return(!dialog_portal_settings_cancel);
}

