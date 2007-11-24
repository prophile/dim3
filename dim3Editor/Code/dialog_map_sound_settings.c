/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Sound Setting Dialog

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

#define kSoundName									FOUR_CHAR_CODE('snam')
#define kSoundPitch									FOUR_CHAR_CODE('pith')
#define kSoundOn									FOUR_CHAR_CODE('snon')

bool						dialog_map_sound_settings_cancel;
WindowRef					dialog_map_sound_settings_wind;

/* =======================================================

      Map Sound Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_sound_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_map_sound_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_sound_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_sound_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Sound Setting
      
======================================================= */

bool dialog_map_sound_settings_run(map_sound_type *sound)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_sound_settings_wind,"MapSoundSettings");

		// set controls

	dialog_special_combo_fill_sound(dialog_map_sound_settings_wind,kSoundName,0,sound->name);
	dialog_set_float(dialog_map_sound_settings_wind,kSoundPitch,0,sound->pitch);
	dialog_set_boolean(dialog_map_sound_settings_wind,kSoundOn,0,sound->on);
	
		// show window
	
	ShowWindow(dialog_map_sound_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_sound_settings_event_proc);
	InstallWindowEventHandler(dialog_map_sound_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_sound_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_sound_settings_wind);
	
		// dialog to data
		
	if (!dialog_map_sound_settings_cancel) {
		dialog_special_combo_get_sound(dialog_map_sound_settings_wind,kSoundName,0,sound->name,name_str_len);
		sound->pitch=dialog_get_float(dialog_map_sound_settings_wind,kSoundPitch,0);
		sound->on=dialog_get_boolean(dialog_map_sound_settings_wind,kSoundOn,0);
	}

		// close window
		
	DisposeWindow(dialog_map_sound_settings_wind);
	
	return(!dialog_map_sound_settings_cancel);
}

