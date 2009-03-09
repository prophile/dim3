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
#define kLiquidWaveFlat						FOUR_CHAR_CODE('wflt')

#define kLiquidHarm							FOUR_CHAR_CODE('harm')
#define kLiquidDrownTick					FOUR_CHAR_CODE('dwat')
#define kLiquidDrownHarm					FOUR_CHAR_CODE('dhrm')
#define kLiquidColor						FOUR_CHAR_CODE('colr')
#define kLiquidTintAlpha					FOUR_CHAR_CODE('talh')
#define kLiquidDepth						FOUR_CHAR_CODE('dept')

#define kLiquidOffX							FOUR_CHAR_CODE('offx')
#define kLiquidOffY							FOUR_CHAR_CODE('offy')
#define kLiquidSizeX						FOUR_CHAR_CODE('sizx')
#define kLiquidSizeY						FOUR_CHAR_CODE('sizy')
#define kLiquidAlpha						FOUR_CHAR_CODE('alph')
#define kLiquidShiftX						FOUR_CHAR_CODE('sftx')
#define kLiquidShiftY						FOUR_CHAR_CODE('sfty')

bool						dialog_liquid_settings_cancel;
WindowRef					dialog_liquid_settings_wind;

/* =======================================================

      Liquid Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus liquid_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kLiquidColor:
					dialog_click_color(dialog_liquid_settings_wind,kLiquidColor,0);
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
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_liquid_settings_wind,"LiquidSettings");

		// set controls
		
	dialog_set_value(dialog_liquid_settings_wind,kLiquidSpeedAlter,0,(int)(liq->speed_alter*100));
	
	dialog_set_value(dialog_liquid_settings_wind,kLiquidWaveSize,0,liq->tide.division);
	dialog_set_value(dialog_liquid_settings_wind,kLiquidTideSize,0,liq->tide.high);
	dialog_set_value(dialog_liquid_settings_wind,kLiquidTideRate,0,liq->tide.rate);
	dialog_set_combo(dialog_liquid_settings_wind,kLiquidTideDirection,0,liq->tide.direction);
	dialog_set_boolean(dialog_liquid_settings_wind,kLiquidWaveFlat,0,liq->tide.flat);
	
	dialog_set_int(dialog_liquid_settings_wind,kLiquidHarm,0,liq->harm.in_harm);
	dialog_set_int(dialog_liquid_settings_wind,kLiquidDrownTick,0,liq->harm.drown_tick);
	dialog_set_int(dialog_liquid_settings_wind,kLiquidDrownHarm,0,liq->harm.drown_harm);
	
	dialog_set_color(dialog_liquid_settings_wind,kLiquidColor,0,&liq->col);
	
	dialog_set_float(dialog_liquid_settings_wind,kLiquidTintAlpha,0,liq->tint_alpha);
	dialog_set_int(dialog_liquid_settings_wind,kLiquidDepth,0,liq->depth);
	
	dialog_set_float(dialog_liquid_settings_wind,kLiquidOffX,0,liq->x_txtoff);
	dialog_set_float(dialog_liquid_settings_wind,kLiquidOffY,0,liq->y_txtoff);
	dialog_set_float(dialog_liquid_settings_wind,kLiquidSizeX,0,liq->x_txtfact);
	dialog_set_float(dialog_liquid_settings_wind,kLiquidSizeY,0,liq->y_txtfact);

	dialog_set_float(dialog_liquid_settings_wind,kLiquidAlpha,0,liq->alpha);
	dialog_set_float(dialog_liquid_settings_wind,kLiquidShiftX,0,liq->x_shift);
	dialog_set_float(dialog_liquid_settings_wind,kLiquidShiftY,0,liq->y_shift);
	
		// show window
	
	ShowWindow(dialog_liquid_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(liquid_settings_event_proc);
	InstallWindowEventHandler(dialog_liquid_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_liquid_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_liquid_settings_wind);
	
		// dialog to data
		
	if (!dialog_liquid_settings_cancel) {
		liq->speed_alter=((float)dialog_get_value(dialog_liquid_settings_wind,kLiquidSpeedAlter,0))/100.0f;
		
		liq->tide.division=dialog_get_value(dialog_liquid_settings_wind,kLiquidWaveSize,0);
		liq->tide.high=dialog_get_value(dialog_liquid_settings_wind,kLiquidTideSize,0);
		liq->tide.rate=dialog_get_value(dialog_liquid_settings_wind,kLiquidTideRate,0);
		liq->tide.direction=dialog_get_combo(dialog_liquid_settings_wind,kLiquidTideDirection,0);
		liq->tide.flat=dialog_get_boolean(dialog_liquid_settings_wind,kLiquidWaveFlat,0);
		
		liq->harm.in_harm=dialog_get_int(dialog_liquid_settings_wind,kLiquidHarm,0);
		liq->harm.drown_tick=dialog_get_int(dialog_liquid_settings_wind,kLiquidDrownTick,0);
		liq->harm.drown_harm=dialog_get_int(dialog_liquid_settings_wind,kLiquidDrownHarm,0);
		
		dialog_get_color(dialog_liquid_settings_wind,kLiquidColor,0,&liq->col);
		
		liq->tint_alpha=dialog_get_float(dialog_liquid_settings_wind,kLiquidTintAlpha,0);
		liq->depth=dialog_get_int(dialog_liquid_settings_wind,kLiquidDepth,0);
		
		liq->x_txtoff=dialog_get_float(dialog_liquid_settings_wind,kLiquidOffX,0);
		liq->y_txtoff=dialog_get_float(dialog_liquid_settings_wind,kLiquidOffY,0);
		liq->x_txtfact=dialog_get_float(dialog_liquid_settings_wind,kLiquidSizeX,0);
		liq->y_txtfact=dialog_get_float(dialog_liquid_settings_wind,kLiquidSizeY,0);
		
		liq->alpha=dialog_get_float(dialog_liquid_settings_wind,kLiquidAlpha,0);
		liq->x_shift=dialog_get_float(dialog_liquid_settings_wind,kLiquidShiftX,0);
		liq->y_shift=dialog_get_float(dialog_liquid_settings_wind,kLiquidShiftY,0);
	}

		// close window
		
	DisposeWindow(dialog_liquid_settings_wind);
	
	return(!dialog_liquid_settings_cancel);
}

