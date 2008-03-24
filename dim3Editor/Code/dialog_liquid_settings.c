/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Liquid Setting Dialog

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

#define kLiquidSpeedAlter					FOUR_CHAR_CODE('salt')
#define kLiquidWaveSize						FOUR_CHAR_CODE('wvsz')
#define kLiquidTideSize						FOUR_CHAR_CODE('tsiz')
#define kLiquidTideRate						FOUR_CHAR_CODE('trat')
#define kLiquidTideDirection				FOUR_CHAR_CODE('tdir')
#define kLiquidHarm							FOUR_CHAR_CODE('harm')
#define kLiquidDrownTick					FOUR_CHAR_CODE('dwat')
#define kLiquidDrownHarm					FOUR_CHAR_CODE('dhrm')
#define kLiquidColor						FOUR_CHAR_CODE('colr')
#define kLiquidTintAlpha					FOUR_CHAR_CODE('talh')

#define kLiquidButtonColor					FOUR_CHAR_CODE('colh')

bool						dialog_liquid_settings_cancel;
WindowRef					dialog_liquid_settings_wind;
RGBColor					dialog_liquid_settings_color;

/* =======================================================

      Liquid Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus liquid_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	Point			pt;
	RGBColor		color;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kLiquidButtonColor:
					pt.h=pt.v=-1;
					if (GetColor(pt,"\pChoose the Liquid Color:",&dialog_liquid_settings_color,&color)) {
						dialog_liquid_settings_color=color;
					}
					dialog_draw_color(dialog_liquid_settings_wind,kLiquidColor,0,&dialog_liquid_settings_color);
					return(noErr);
				
				case kHICommandCancel:
					dialog_liquid_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_liquid_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_liquid_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Liquid Setting
      
======================================================= */

bool dialog_liquid_settings_run(map_liquid_type *liq)
{
/* supergumba

	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_liquid_settings_wind,"LiquidSettings");

		// set controls
		
	dialog_set_value(dialog_liquid_settings_wind,kLiquidSpeedAlter,0,(int)(seg->data.liquid.speed_alter*100));
	
	dialog_set_int(dialog_liquid_settings_wind,kLiquidWaveSize,0,seg->data.liquid.wavesize);
	dialog_set_int(dialog_liquid_settings_wind,kLiquidTideSize,0,seg->data.liquid.tidesize);
	dialog_set_int(dialog_liquid_settings_wind,kLiquidTideRate,0,seg->data.liquid.tiderate);
	dialog_set_combo(dialog_liquid_settings_wind,kLiquidTideDirection,0,seg->data.liquid.tidedirection);
	
	dialog_set_int(dialog_liquid_settings_wind,kLiquidHarm,0,seg->data.liquid.harm);
	dialog_set_int(dialog_liquid_settings_wind,kLiquidDrownTick,0,seg->data.liquid.drown_tick);
	dialog_set_int(dialog_liquid_settings_wind,kLiquidDrownHarm,0,seg->data.liquid.drown_harm);
	
	dialog_liquid_settings_color.red=(int)(seg->data.liquid.col.r*(float)0xFFFF);
	dialog_liquid_settings_color.green=(int)(seg->data.liquid.col.g*(float)0xFFFF);
	dialog_liquid_settings_color.blue=(int)(seg->data.liquid.col.b*(float)0xFFFF);
	
	dialog_set_float(dialog_liquid_settings_wind,kLiquidTintAlpha,0,seg->data.liquid.tint_alpha);
	
		// show window
	
	ShowWindow(dialog_liquid_settings_wind);
	
		// draw color
		
	dialog_draw_color(dialog_liquid_settings_wind,kLiquidColor,0,&dialog_liquid_settings_color);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(liquid_settings_event_proc);
	InstallWindowEventHandler(dialog_liquid_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_liquid_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_liquid_settings_wind);
	
		// dialog to data
		
	if (!dialog_liquid_settings_cancel) {
		seg->data.liquid.speed_alter=((float)dialog_get_value(dialog_liquid_settings_wind,kLiquidSpeedAlter,0))/100.0f;
		
		seg->data.liquid.wavesize=dialog_get_int(dialog_liquid_settings_wind,kLiquidWaveSize,0);
		seg->data.liquid.tidesize=dialog_get_int(dialog_liquid_settings_wind,kLiquidTideSize,0);
		seg->data.liquid.tiderate=dialog_get_int(dialog_liquid_settings_wind,kLiquidTideRate,0);
		seg->data.liquid.tidedirection=dialog_get_combo(dialog_liquid_settings_wind,kLiquidTideDirection,0);
		
		seg->data.liquid.harm=dialog_get_int(dialog_liquid_settings_wind,kLiquidHarm,0);
		seg->data.liquid.drown_tick=dialog_get_int(dialog_liquid_settings_wind,kLiquidDrownTick,0);
		seg->data.liquid.drown_harm=dialog_get_int(dialog_liquid_settings_wind,kLiquidDrownHarm,0);
		
		seg->data.liquid.col.r=((float)dialog_liquid_settings_color.red/(float)0xFFFF);
		seg->data.liquid.col.g=((float)dialog_liquid_settings_color.green/(float)0xFFFF);
		seg->data.liquid.col.b=((float)dialog_liquid_settings_color.blue/(float)0xFFFF);
		
		seg->data.liquid.tint_alpha=dialog_get_float(dialog_liquid_settings_wind,kLiquidTintAlpha,0);
	}

		// close window
		
	DisposeWindow(dialog_liquid_settings_wind);
	
	return(!dialog_liquid_settings_cancel);
	*/
	return(TRUE);
}

