/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Rain Setting Dialog

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

#define kRainDensity								FOUR_CHAR_CODE('dnst')
#define kRainRadius									FOUR_CHAR_CODE('rads')
#define kRainHeight									FOUR_CHAR_CODE('high')
#define kRainSpeed									FOUR_CHAR_CODE('sped')
#define kRainWidth									FOUR_CHAR_CODE('widt')
#define kRainLength									FOUR_CHAR_CODE('leng')
#define kRainAlpha									FOUR_CHAR_CODE('apha')
#define kRainStartColor								FOUR_CHAR_CODE('cols')
#define kRainEndColor								FOUR_CHAR_CODE('cole')
#define kRainSlantAdd								FOUR_CHAR_CODE('slnt')
#define kRainSlantTimeMSec							FOUR_CHAR_CODE('slms')
#define kRainSlantChangeMSec						FOUR_CHAR_CODE('slct')
#define kRainOn										FOUR_CHAR_CODE('rnon')

#define kMapRainButtonStartColor					FOUR_CHAR_CODE('clhs')
#define kMapRainButtonEndColor						FOUR_CHAR_CODE('clhe')

bool						dialog_map_rain_settings_cancel;
WindowRef					dialog_map_rain_settings_wind;
RGBColor					dialog_map_rain_start_color,dialog_map_rain_end_color;

/* =======================================================

      Map Rain Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_rain_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	Point			pt;
	RGBColor		color;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kMapRainButtonStartColor:
					pt.h=pt.v=-1;
					if (GetColor(pt,"\pChoose the Start Color:",&dialog_map_rain_start_color,&color)) {
						dialog_map_rain_start_color=color;
					}
					dialog_draw_color(dialog_map_rain_settings_wind,kRainStartColor,0,&dialog_map_rain_start_color);
					return(noErr);
					
				case kMapRainButtonEndColor:
					pt.h=pt.v=-1;
					if (GetColor(pt,"\pChoose the End Color:",&dialog_map_rain_end_color,&color)) {
						dialog_map_rain_end_color=color;
					}
					dialog_draw_color(dialog_map_rain_settings_wind,kRainEndColor,0,&dialog_map_rain_end_color);
					return(noErr);
				
				case kHICommandCancel:
					dialog_map_rain_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_rain_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_rain_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Rain Setting
      
======================================================= */

bool dialog_map_rain_settings_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_rain_settings_wind,"MapRainSettings");

		// set controls

	dialog_set_value(dialog_map_rain_settings_wind,kRainDensity,0,map.rain.density);
	dialog_set_int(dialog_map_rain_settings_wind,kRainRadius,0,map.rain.radius);
	dialog_set_int(dialog_map_rain_settings_wind,kRainHeight,0,map.rain.height);
	dialog_set_int(dialog_map_rain_settings_wind,kRainSpeed,0,map.rain.speed);
	dialog_set_int(dialog_map_rain_settings_wind,kRainWidth,0,map.rain.line_width);
	dialog_set_int(dialog_map_rain_settings_wind,kRainLength,0,map.rain.line_length);
	dialog_set_float(dialog_map_rain_settings_wind,kRainAlpha,0,map.rain.alpha);

	dialog_set_int(dialog_map_rain_settings_wind,kRainSlantAdd,0,map.rain.slant_add);
	dialog_set_int(dialog_map_rain_settings_wind,kRainSlantTimeMSec,0,map.rain.slant_time_msec);
	dialog_set_int(dialog_map_rain_settings_wind,kRainSlantChangeMSec,0,map.rain.slant_change_msec);
		
	dialog_map_rain_start_color.red=(int)(map.rain.start_color.r*(float)0xFFFF);
	dialog_map_rain_start_color.green=(int)(map.rain.start_color.g*(float)0xFFFF);
	dialog_map_rain_start_color.blue=(int)(map.rain.start_color.b*(float)0xFFFF);
	
	dialog_map_rain_end_color.red=(int)(map.rain.end_color.r*(float)0xFFFF);
	dialog_map_rain_end_color.green=(int)(map.rain.end_color.g*(float)0xFFFF);
	dialog_map_rain_end_color.blue=(int)(map.rain.end_color.b*(float)0xFFFF);
	
	dialog_set_boolean(dialog_map_rain_settings_wind,kRainOn,0,map.rain.on);
	
		// show window
	
	ShowWindow(dialog_map_rain_settings_wind);
	
		// draw color
		
	dialog_draw_color(dialog_map_rain_settings_wind,kRainStartColor,0,&dialog_map_rain_start_color);
	dialog_draw_color(dialog_map_rain_settings_wind,kRainEndColor,0,&dialog_map_rain_end_color);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_rain_settings_event_proc);
	InstallWindowEventHandler(dialog_map_rain_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_rain_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_rain_settings_wind);
	
		// dialog to data
		
	if (!dialog_map_rain_settings_cancel) {
		map.rain.density=dialog_get_value(dialog_map_rain_settings_wind,kRainDensity,0);
		map.rain.radius=dialog_get_int(dialog_map_rain_settings_wind,kRainRadius,0);
		map.rain.height=dialog_get_int(dialog_map_rain_settings_wind,kRainHeight,0);
		map.rain.speed=dialog_get_int(dialog_map_rain_settings_wind,kRainSpeed,0);
		map.rain.line_width=dialog_get_int(dialog_map_rain_settings_wind,kRainWidth,0);
		map.rain.line_length=dialog_get_int(dialog_map_rain_settings_wind,kRainLength,0);
		map.rain.alpha=dialog_get_float(dialog_map_rain_settings_wind,kRainAlpha,0);
		
		map.rain.start_color.r=((float)dialog_map_rain_start_color.red/(float)0xFFFF);
		map.rain.start_color.g=((float)dialog_map_rain_start_color.green/(float)0xFFFF);
		map.rain.start_color.b=((float)dialog_map_rain_start_color.blue/(float)0xFFFF);
		
		map.rain.end_color.r=((float)dialog_map_rain_end_color.red/(float)0xFFFF);
		map.rain.end_color.g=((float)dialog_map_rain_end_color.green/(float)0xFFFF);
		map.rain.end_color.b=((float)dialog_map_rain_end_color.blue/(float)0xFFFF);
		
		map.rain.slant_add=dialog_get_int(dialog_map_rain_settings_wind,kRainSlantAdd,0);
		map.rain.slant_time_msec=dialog_get_int(dialog_map_rain_settings_wind,kRainSlantTimeMSec,0);
		map.rain.slant_change_msec=dialog_get_int(dialog_map_rain_settings_wind,kRainSlantChangeMSec,0);
		
		map.rain.on=dialog_get_boolean(dialog_map_rain_settings_wind,kRainOn,0);
	}

		// close window
		
	DisposeWindow(dialog_map_rain_settings_wind);
	
	return(!dialog_map_rain_settings_cancel);
}

