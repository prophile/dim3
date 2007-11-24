/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Media Dialog

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

#define kMapMediaOpenType							FOUR_CHAR_CODE('otyp')
#define kMapMediaOpenName							FOUR_CHAR_CODE('onme')
#define kMapMediaTitleSoundName						FOUR_CHAR_CODE('snam')
#define kMapMediaMusicName							FOUR_CHAR_CODE('musc')
#define kMapMediaMusicFadeIn						FOUR_CHAR_CODE('mfdi')

bool						dialog_map_media_cancel;
WindowRef					dialog_map_media_wind;

/* =======================================================

      Map Media Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_map_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_map_media_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_media_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_media_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Media
      
======================================================= */

bool dialog_map_media_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_media_wind,"MediaSettings");

		// set controls

	dialog_set_combo(dialog_map_media_wind,kMapMediaOpenType,0,map.media.type);
	dialog_set_text(dialog_map_media_wind,kMapMediaOpenName,0,map.media.name);
	dialog_special_combo_fill_sound(dialog_map_media_wind,kMapMediaTitleSoundName,0,map.media.title_sound_name);
	dialog_set_text(dialog_map_media_wind,kMapMediaMusicName,0,map.music.name);
	dialog_set_int(dialog_map_media_wind,kMapMediaMusicFadeIn,0,map.music.fade_msec);

		// show window
	
	ShowWindow(dialog_map_media_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_map_setting_event_proc);
	InstallWindowEventHandler(dialog_map_media_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_media_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_media_wind);
	
		// dialog to data
		
	if (!dialog_map_media_cancel) {
		map.media.type=dialog_get_combo(dialog_map_media_wind,kMapMediaOpenType,0);
		dialog_get_text(dialog_map_media_wind,kMapMediaOpenName,0,map.media.name,name_str_len);
		dialog_special_combo_get_sound(dialog_map_media_wind,kMapMediaTitleSoundName,0,map.media.title_sound_name,name_str_len);
		dialog_get_text(dialog_map_media_wind,kMapMediaMusicName,0,map.music.name,name_str_len);
		map.music.fade_msec=dialog_get_int(dialog_map_media_wind,kMapMediaMusicFadeIn,0);
	}

		// close window
		
	DisposeWindow(dialog_map_media_wind);
	
	return(!dialog_map_media_cancel);
}

