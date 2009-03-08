/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Fog Setting Dialog

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

#define kFogOn										FOUR_CHAR_CODE('fgon')
#define kFogCount									FOUR_CHAR_CODE('fcnt')
#define kFogInnerRadius								FOUR_CHAR_CODE('fird')
#define kFogOuterRadius								FOUR_CHAR_CODE('ford')
#define kFogHigh									FOUR_CHAR_CODE('fhgh')
#define kFogDrop									FOUR_CHAR_CODE('fdrp')

#define kFogTextureIndex							FOUR_CHAR_CODE('fgtx')
#define kFogTextureSpeed							FOUR_CHAR_CODE('fgss')
#define kFogTextureXFact							FOUR_CHAR_CODE('fxsz')
#define kFogTextureYFact							FOUR_CHAR_CODE('fysz')
#define kFogColor									FOUR_CHAR_CODE('cols')
#define kFogAlpha									FOUR_CHAR_CODE('fala')
#define kFogUseSolidColor							FOUR_CHAR_CODE('fscl')

#define kGLFogButtonColor							FOUR_CHAR_CODE('colh')

bool						dialog_map_fog_settings_cancel;
WindowRef					dialog_map_fog_settings_wind;
RGBColor					dialog_map_fog_color;

/* =======================================================

      Map Fog Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_fog_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	Point			pt;
	RGBColor		color;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kGLFogButtonColor:
					pt.h=pt.v=-1;
					if (GetColor(pt,"\pChoose the Solid Fog Color:",&dialog_map_fog_color,&color)) {
						dialog_map_fog_color=color;
					}
				//	dialog_draw_color(dialog_map_fog_settings_wind,kFogColor,0,&dialog_map_fog_color);
					return(noErr);
				
				case kHICommandCancel:
					dialog_map_fog_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_fog_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_fog_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Fog Setting
      
======================================================= */

bool dialog_map_fog_settings_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_fog_settings_wind,"MapFogSettings");

		// set controls

	dialog_set_boolean(dialog_map_fog_settings_wind,kFogOn,0,map.fog.on);
	dialog_set_int(dialog_map_fog_settings_wind,kFogCount,0,map.fog.count);
	dialog_set_int(dialog_map_fog_settings_wind,kFogInnerRadius,0,map.fog.inner_radius);
	dialog_set_int(dialog_map_fog_settings_wind,kFogOuterRadius,0,map.fog.outer_radius);
	dialog_set_int(dialog_map_fog_settings_wind,kFogHigh,0,map.fog.high);
	dialog_set_int(dialog_map_fog_settings_wind,kFogDrop,0,map.fog.drop);

	dialog_fill_texture_combo(dialog_map_fog_settings_wind,kFogTextureIndex,0,FALSE,map.fog.texture_idx);
	dialog_set_float(dialog_map_fog_settings_wind,kFogTextureSpeed,0,map.fog.speed);
	dialog_set_float(dialog_map_fog_settings_wind,kFogTextureXFact,0,map.fog.txt_x_fact);
	dialog_set_float(dialog_map_fog_settings_wind,kFogTextureYFact,0,map.fog.txt_y_fact);
	
	dialog_map_fog_color.red=(int)(map.fog.col.r*(float)0xFFFF);
	dialog_map_fog_color.green=(int)(map.fog.col.g*(float)0xFFFF);
	dialog_map_fog_color.blue=(int)(map.fog.col.b*(float)0xFFFF);

	dialog_set_boolean(dialog_map_fog_settings_wind,kFogUseSolidColor,0,map.fog.use_solid_color);
	dialog_set_float(dialog_map_fog_settings_wind,kFogAlpha,0,map.fog.alpha);
	
		// show window
	
	ShowWindow(dialog_map_fog_settings_wind);
	
		// draw color
		
//	dialog_draw_color(dialog_map_fog_settings_wind,kFogColor,0,&dialog_map_fog_color);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_fog_settings_event_proc);
	InstallWindowEventHandler(dialog_map_fog_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_fog_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_fog_settings_wind);
	
		// dialog to data
		
	if (!dialog_map_fog_settings_cancel) {
		map.fog.on=dialog_get_boolean(dialog_map_fog_settings_wind,kFogOn,0);
		map.fog.count=dialog_get_int(dialog_map_fog_settings_wind,kFogCount,0);
		map.fog.inner_radius=dialog_get_int(dialog_map_fog_settings_wind,kFogInnerRadius,0);
		map.fog.outer_radius=dialog_get_int(dialog_map_fog_settings_wind,kFogOuterRadius,0);
		map.fog.high=dialog_get_int(dialog_map_fog_settings_wind,kFogHigh,0);
		map.fog.drop=dialog_get_int(dialog_map_fog_settings_wind,kFogDrop,0);

		map.fog.texture_idx=dialog_get_texture_combo(dialog_map_fog_settings_wind,kFogTextureIndex,0,FALSE);
		map.fog.speed=dialog_get_float(dialog_map_fog_settings_wind,kFogTextureSpeed,0);
		map.fog.txt_x_fact=dialog_get_float(dialog_map_fog_settings_wind,kFogTextureXFact,0);
		map.fog.txt_y_fact=dialog_get_float(dialog_map_fog_settings_wind,kFogTextureYFact,0);
		
		map.fog.col.r=((float)dialog_map_fog_color.red/(float)0xFFFF);
		map.fog.col.g=((float)dialog_map_fog_color.green/(float)0xFFFF);
		map.fog.col.b=((float)dialog_map_fog_color.blue/(float)0xFFFF);

		map.fog.use_solid_color=dialog_get_boolean(dialog_map_fog_settings_wind,kFogUseSolidColor,0);
		map.fog.alpha=dialog_get_float(dialog_map_fog_settings_wind,kFogAlpha,0);
	}

		// close window
		
	DisposeWindow(dialog_map_fog_settings_wind);
	
	return(!dialog_map_fog_settings_cancel);
}

