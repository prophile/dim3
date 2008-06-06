/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Setting Dialog

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
#include "dialog.h"

extern map_type				map;

#define kMapTitle									FOUR_CHAR_CODE('tite')
#define kMapAuthor									FOUR_CHAR_CODE('auth')

#define kMapGravity									FOUR_CHAR_CODE('grav')
#define kMapGravityPower							FOUR_CHAR_CODE('gpwr')
#define kMapGravitySpeed							FOUR_CHAR_CODE('gspd')
#define kMapResistance								FOUR_CHAR_CODE('rest')
#define kMapTextureXFactor							FOUR_CHAR_CODE('trxf')
#define kMapTextureYFactor							FOUR_CHAR_CODE('tryf')

#define kMapGameTypeList							FOUR_CHAR_CODE('gtyp')

#define kMapSpecularsHilite							FOUR_CHAR_CODE('shil')
#define kMapEditorLinkStartAlways					FOUR_CHAR_CODE('elss')

bool						dialog_map_settings_cancel;
WindowRef					dialog_map_settings_wind;

/* =======================================================

      Map Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_map_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_map_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Setting
      
======================================================= */

bool dialog_map_settings_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_settings_wind,"MapSettings");

		// set controls
		
	dialog_set_text(dialog_map_settings_wind,kMapTitle,0,map.info.title);
	dialog_set_text(dialog_map_settings_wind,kMapAuthor,0,map.info.author);
	
	dialog_set_float(dialog_map_settings_wind,kMapGravity,0,map.settings.gravity);
	dialog_set_float(dialog_map_settings_wind,kMapGravityPower,0,map.settings.gravity_max_power);
	dialog_set_float(dialog_map_settings_wind,kMapGravitySpeed,0,map.settings.gravity_max_speed);
	dialog_set_float(dialog_map_settings_wind,kMapResistance,0,map.settings.resistance);
	dialog_set_float(dialog_map_settings_wind,kMapTextureXFactor,0,map.settings.txt_scale_x);
	dialog_set_float(dialog_map_settings_wind,kMapTextureYFactor,0,map.settings.txt_scale_y);
	
	dialog_set_text(dialog_map_settings_wind,kMapGameTypeList,0,map.settings.network_game_list);
	
	dialog_set_boolean(dialog_map_settings_wind,kMapSpecularsHilite,0,map.settings.speculars_hilite);
	dialog_set_boolean(dialog_map_settings_wind,kMapEditorLinkStartAlways,0,map.settings.editor_link_always_start);
	
		// show window
	
	ShowWindow(dialog_map_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_map_setting_event_proc);
	InstallWindowEventHandler(dialog_map_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_settings_wind);
	
		// dialog to data
		
	if (!dialog_map_settings_cancel) {
		dialog_get_text(dialog_map_settings_wind,kMapTitle,0,map.info.title,name_str_len);
		dialog_get_text(dialog_map_settings_wind,kMapAuthor,0,map.info.author,name_str_len);
		
		map.settings.gravity=dialog_get_float(dialog_map_settings_wind,kMapGravity,0);
		map.settings.gravity_max_power=dialog_get_float(dialog_map_settings_wind,kMapGravityPower,0);
		map.settings.gravity_max_speed=dialog_get_float(dialog_map_settings_wind,kMapGravitySpeed,0);
		map.settings.resistance=dialog_get_float(dialog_map_settings_wind,kMapResistance,0);
		map.settings.txt_scale_x=dialog_get_float(dialog_map_settings_wind,kMapTextureXFactor,0);
		map.settings.txt_scale_y=dialog_get_float(dialog_map_settings_wind,kMapTextureYFactor,0);
		
		dialog_get_text(dialog_map_settings_wind,kMapGameTypeList,0,map.settings.network_game_list,256);
		
		map.settings.speculars_hilite=dialog_get_boolean(dialog_map_settings_wind,kMapSpecularsHilite,0);
		map.settings.editor_link_always_start=dialog_get_boolean(dialog_map_settings_wind,kMapEditorLinkStartAlways,0);
	}

		// close window
		
	DisposeWindow(dialog_map_settings_wind);
	
	return(!dialog_map_settings_cancel);
}

