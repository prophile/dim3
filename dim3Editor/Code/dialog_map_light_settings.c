/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Light Setting Dialog

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

#define kLightType									FOUR_CHAR_CODE('type')
#define kLightIntensity								FOUR_CHAR_CODE('ints')
#define kLightExponent								FOUR_CHAR_CODE('fall')
#define kLightColor									FOUR_CHAR_CODE('colr')
#define kLightOn									FOUR_CHAR_CODE('lgon')

bool						dialog_map_light_settings_cancel;
WindowRef					dialog_map_light_settings_wind;

/* =======================================================

      Map Light Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_light_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kLightColor:
					dialog_click_color(dialog_map_light_settings_wind,kLightColor,0);
					return(noErr);
				
				case kHICommandCancel:
					dialog_map_light_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_light_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_light_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Light Setting
      
======================================================= */

bool dialog_map_light_settings_run(map_light_type *light)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_light_settings_wind,"MapLightSettings");

		// set controls
		
	dialog_set_combo(dialog_map_light_settings_wind,kLightType,0,light->type);
	dialog_set_int(dialog_map_light_settings_wind,kLightIntensity,0,light->intensity);
	dialog_set_float(dialog_map_light_settings_wind,kLightExponent,0,light->exponent);
	dialog_set_boolean(dialog_map_light_settings_wind,kLightOn,0,light->on);

	dialog_set_color(dialog_map_light_settings_wind,kLightColor,0,&light->col);
	
		// show window
	
	ShowWindow(dialog_map_light_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_light_settings_event_proc);
	InstallWindowEventHandler(dialog_map_light_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_light_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_light_settings_wind);
	
		// dialog to data
		
	if (!dialog_map_light_settings_cancel) {
		light->type=dialog_get_combo(dialog_map_light_settings_wind,kLightType,0);
		light->intensity=dialog_get_int(dialog_map_light_settings_wind,kLightIntensity,0);
		light->exponent=dialog_get_float(dialog_map_light_settings_wind,kLightExponent,0);
		light->on=dialog_get_boolean(dialog_map_light_settings_wind,kLightOn,0);
		
		dialog_get_color(dialog_map_light_settings_wind,kLightColor,0,&light->col);
	}

		// close window
		
	DisposeWindow(dialog_map_light_settings_wind);
	
	return(!dialog_map_light_settings_cancel);
}

