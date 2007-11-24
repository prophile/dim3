/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Ambient Light Setting Dialog

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

#define kLightColor									FOUR_CHAR_CODE('colr')
#define kLightDropOffFactor							FOUR_CHAR_CODE('ldof')
#define kSoundName									FOUR_CHAR_CODE('snam')
#define kSoundPitch									FOUR_CHAR_CODE('pith')

#define kMapLightButtonColor						FOUR_CHAR_CODE('colh')

bool						dialog_map_ambient_light_sound_settings_cancel;
WindowRef					dialog_map_ambient_light_sound_settings_wind;
RGBColor					dialog_map_ambient_light_sound_color;

/* =======================================================

      Map Ambient Light/Sound Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_ambient_light_sound_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	Point			pt;
	RGBColor		color;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kMapLightButtonColor:
					pt.h=pt.v=-1;
					if (GetColor(pt,"\pChoose the Light Color:",&dialog_map_ambient_light_sound_color,&color)) {
						dialog_map_ambient_light_sound_color=color;
					}
					dialog_draw_color(dialog_map_ambient_light_sound_settings_wind,kLightColor,0,&dialog_map_ambient_light_sound_color);
					return(noErr);
				
				case kHICommandCancel:
					dialog_map_ambient_light_sound_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_ambient_light_sound_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_ambient_light_sound_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Ambient Light/Sound Setting
      
======================================================= */

bool dialog_map_ambient_light_sound_settings_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_ambient_light_sound_settings_wind,"MapAmbientLightSoundSettings");

		// set controls
		
	dialog_map_ambient_light_sound_color.red=(int)(map.ambient.light_color.r*(float)0xFFFF);
	dialog_map_ambient_light_sound_color.green=(int)(map.ambient.light_color.g*(float)0xFFFF);
	dialog_map_ambient_light_sound_color.blue=(int)(map.ambient.light_color.b*(float)0xFFFF);
	dialog_set_float(dialog_map_ambient_light_sound_settings_wind,kLightDropOffFactor,0,map.ambient.light_drop_off_factor);

	dialog_special_combo_fill_sound(dialog_map_ambient_light_sound_settings_wind,kSoundName,0,map.ambient.sound_name);
	dialog_set_float(dialog_map_ambient_light_sound_settings_wind,kSoundPitch,0,map.ambient.sound_pitch);
	
		// show window
	
	ShowWindow(dialog_map_ambient_light_sound_settings_wind);
	
		// draw color
		
	dialog_draw_color(dialog_map_ambient_light_sound_settings_wind,kLightColor,0,&dialog_map_ambient_light_sound_color);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_ambient_light_sound_settings_event_proc);
	InstallWindowEventHandler(dialog_map_ambient_light_sound_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_ambient_light_sound_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_ambient_light_sound_settings_wind);
	
		// dialog to data
		
	if (!dialog_map_ambient_light_sound_settings_cancel) {
		map.ambient.light_color.r=((float)dialog_map_ambient_light_sound_color.red/(float)0xFFFF);
		map.ambient.light_color.g=((float)dialog_map_ambient_light_sound_color.green/(float)0xFFFF);
		map.ambient.light_color.b=((float)dialog_map_ambient_light_sound_color.blue/(float)0xFFFF);
		map.ambient.light_drop_off_factor=dialog_get_float(dialog_map_ambient_light_sound_settings_wind,kLightDropOffFactor,0);
		dialog_special_combo_get_sound(dialog_map_ambient_light_sound_settings_wind,kSoundName,0,map.ambient.sound_name,name_str_len);
		map.ambient.sound_pitch=dialog_get_float(dialog_map_ambient_light_sound_settings_wind,kSoundPitch,0);
	}

		// close window
		
	DisposeWindow(dialog_map_ambient_light_sound_settings_wind);
	
	return(!dialog_map_ambient_light_sound_settings_cancel);
}

