/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Background-Sky Dialog

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

#define kBackSkySettingBackgroundOn					FOUR_CHAR_CODE('bgon')
#define kBackSkySettingBackgroundTextureIndex		FOUR_CHAR_CODE('bgti')
#define kBackSkySettingBackgroundXShift				FOUR_CHAR_CODE('bgxs')
#define kBackSkySettingBackgroundYShift				FOUR_CHAR_CODE('bgys')
#define kBackSkySettingSkyOn						FOUR_CHAR_CODE('syon')
#define kBackSkySettingSkyType						FOUR_CHAR_CODE('syty')
#define kBackSkySettingSkyTextureIndex				FOUR_CHAR_CODE('syti')
#define kBackSkySettingSkyTextureBottomIndex		FOUR_CHAR_CODE('stxb')
#define kBackSkySettingSkyTextureNorthIndex			FOUR_CHAR_CODE('stxn')
#define kBackSkySettingSkyTextureSouthIndex			FOUR_CHAR_CODE('stxs')
#define kBackSkySettingSkyTextureEastIndex			FOUR_CHAR_CODE('stxe')
#define kBackSkySettingSkyTextureWestIndex			FOUR_CHAR_CODE('stxw')
#define kBackSkySettingSkyRadius					FOUR_CHAR_CODE('syrd')
#define kBackSkySettingSkyDomeY						FOUR_CHAR_CODE('sdmy')
#define kBackSkySettingSkyDomeMirror				FOUR_CHAR_CODE('smdm')
#define kBackSkySettingSkyTextureRepeat				FOUR_CHAR_CODE('sytr')
#define kBackSkySettingSkyTextureXShift				FOUR_CHAR_CODE('stxx')
#define kBackSkySettingSkyTextureYShift				FOUR_CHAR_CODE('styy')

extern void dialog_fill_texture_combo(WindowRef wind,unsigned long sig,int id,bool none,int idx);
extern int dialog_get_texture_combo(WindowRef wind,unsigned long sig,int id,bool none);

bool						dialog_background_sky_setting_cancel;
WindowRef					dialog_background_sky_setting_wind;

/* =======================================================

      Map Background/Sky Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_background_sky_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_background_sky_setting_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_background_sky_setting_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_background_sky_setting_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Background-Sky Setting
      
======================================================= */

bool dialog_background_sky_setting_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_background_sky_setting_wind,"BackgroundSkySetting");

		// set controls
		
	dialog_set_boolean(dialog_background_sky_setting_wind,kBackSkySettingBackgroundOn,0,map.background.on);
	dialog_fill_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingBackgroundTextureIndex,0,TRUE,map.background.fill);
	dialog_set_float(dialog_background_sky_setting_wind,kBackSkySettingBackgroundXShift,0,map.background.x_scroll_fact);
	dialog_set_float(dialog_background_sky_setting_wind,kBackSkySettingBackgroundYShift,0,map.background.y_scroll_fact);
	
	dialog_set_boolean(dialog_background_sky_setting_wind,kBackSkySettingSkyOn,0,map.sky.on);
	dialog_set_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyType,0,map.sky.type);
	dialog_fill_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureIndex,0,TRUE,map.sky.fill);
	dialog_fill_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureBottomIndex,0,TRUE,map.sky.bottom_fill);
	dialog_fill_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureNorthIndex,0,TRUE,map.sky.north_fill);
	dialog_fill_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureSouthIndex,0,TRUE,map.sky.south_fill);
	dialog_fill_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureEastIndex,0,TRUE,map.sky.east_fill);
	dialog_fill_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureWestIndex,0,TRUE,map.sky.west_fill);
	dialog_set_int(dialog_background_sky_setting_wind,kBackSkySettingSkyRadius,0,map.sky.radius);
	dialog_set_int(dialog_background_sky_setting_wind,kBackSkySettingSkyDomeY,0,map.sky.dome_y);
	dialog_set_boolean(dialog_background_sky_setting_wind,kBackSkySettingSkyDomeMirror,0,map.sky.dome_mirror);
	dialog_set_float(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureRepeat,0,map.sky.txt_fact);
	dialog_set_float(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureXShift,0,map.sky.txt_x_shift);
	dialog_set_float(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureYShift,0,map.sky.txt_y_shift);
	
		// show window
	
	ShowWindow(dialog_background_sky_setting_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_background_sky_setting_event_proc);
	InstallWindowEventHandler(dialog_background_sky_setting_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_background_sky_setting_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_background_sky_setting_wind);
	
		// dialog to data
		
	if (!dialog_background_sky_setting_cancel) {
		map.background.on=dialog_get_boolean(dialog_background_sky_setting_wind,kBackSkySettingBackgroundOn,0);
		map.background.fill=dialog_get_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingBackgroundTextureIndex,0,TRUE);
		map.background.x_scroll_fact=dialog_get_float(dialog_background_sky_setting_wind,kBackSkySettingBackgroundXShift,0);
		map.background.y_scroll_fact=dialog_get_float(dialog_background_sky_setting_wind,kBackSkySettingBackgroundYShift,0);
		
		map.sky.on=dialog_get_boolean(dialog_background_sky_setting_wind,kBackSkySettingSkyOn,0);
		map.sky.type=dialog_get_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyType,0);
		map.sky.fill=dialog_get_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureIndex,0,TRUE);
		map.sky.bottom_fill=dialog_get_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureBottomIndex,0,TRUE);
		map.sky.north_fill=dialog_get_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureNorthIndex,0,TRUE);
		map.sky.south_fill=dialog_get_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureSouthIndex,0,TRUE);
		map.sky.east_fill=dialog_get_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureEastIndex,0,TRUE);
		map.sky.west_fill=dialog_get_texture_combo(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureWestIndex,0,TRUE);
		map.sky.radius=dialog_get_int(dialog_background_sky_setting_wind,kBackSkySettingSkyRadius,0);
		map.sky.dome_y=dialog_get_int(dialog_background_sky_setting_wind,kBackSkySettingSkyDomeY,0);
		map.sky.dome_mirror=dialog_get_boolean(dialog_background_sky_setting_wind,kBackSkySettingSkyDomeMirror,0);
		map.sky.txt_fact=dialog_get_float(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureRepeat,0);
		map.sky.txt_x_shift=dialog_get_float(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureXShift,0);
		map.sky.txt_y_shift=dialog_get_float(dialog_background_sky_setting_wind,kBackSkySettingSkyTextureYShift,0);
	
		main_wind_draw();
	}

		// close window
		
	DisposeWindow(dialog_background_sky_setting_wind);
	
	return(!dialog_background_sky_setting_cancel);
}

