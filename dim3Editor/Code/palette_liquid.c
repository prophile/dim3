/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Liquid Palette

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

#define kLiquidSettingTabCount				3
#define kLiquidSettingTab					FOUR_CHAR_CODE('tabb')

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

extern map_type				map;

WindowRef					palette_liquid_wind;
ControlRef					palette_liquid_tab;

/* =======================================================

      Liquid Setting Setting Event Handlers
      
======================================================= */
/*
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

static pascal OSStatus liquid_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				event_class,event_kind;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
		dialog_switch_tab(dialog_liquid_settings_wind,kLiquidSettingTab,0,kLiquidSettingTabCount);
	}
	
	return(eventNotHandledErr);
}
*/
/* =======================================================

      Run Liquid Setting
      
======================================================= */
/*
bool dialog_liquid_settings_run(map_liquid_type *liq)
{
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_liquid_settings_wind,"LiquidSettings");
	
		// tab
		
	dialog_set_tab(dialog_liquid_settings_wind,kLiquidSettingTab,0,0,kLiquidSettingTabCount);
	
	ctrl_id.signature=kLiquidSettingTab;
	ctrl_id.id=0;
	GetControlByID(dialog_liquid_settings_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(liquid_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_liquid_settings_wind,NULL);

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
	}

		// close window
		
	DisposeWindow(dialog_liquid_settings_wind);
	
	return(!dialog_liquid_settings_cancel);
}

*/





void palette_liquid_load(void)
{
	int						type,liq_idx,poly_idx;
	map_liquid_type			*liq;

		// get liquid
		
	select_get(0,&type,&liq_idx,&poly_idx);
	liq=&map.liquid.liquids[liq_idx];

		// set controls
	
	dialog_set_value(palette_liquid_wind,kLiquidSpeedAlter,0,(int)(liq->speed_alter*100));
	
	dialog_set_value(palette_liquid_wind,kLiquidWaveSize,0,liq->tide.division);
	dialog_set_value(palette_liquid_wind,kLiquidTideSize,0,liq->tide.high);
	dialog_set_value(palette_liquid_wind,kLiquidTideRate,0,liq->tide.rate);
	dialog_set_combo(palette_liquid_wind,kLiquidTideDirection,0,liq->tide.direction);
	dialog_set_boolean(palette_liquid_wind,kLiquidWaveFlat,0,liq->tide.flat);
	
	dialog_set_int(palette_liquid_wind,kLiquidHarm,0,liq->harm.in_harm);
	dialog_set_int(palette_liquid_wind,kLiquidDrownTick,0,liq->harm.drown_tick);
	dialog_set_int(palette_liquid_wind,kLiquidDrownHarm,0,liq->harm.drown_harm);
	
	dialog_set_color(palette_liquid_wind,kLiquidColor,0,&liq->col);
	
	dialog_set_float(palette_liquid_wind,kLiquidTintAlpha,0,liq->tint_alpha);
	dialog_set_int(palette_liquid_wind,kLiquidDepth,0,liq->depth);
}

void palette_liquid_save(void)
{
	int						type,liq_idx,poly_idx;
	map_liquid_type			*liq;

		// get liquid
		
	select_get(0,&type,&liq_idx,&poly_idx);
	liq=&map.liquid.liquids[liq_idx];
		
		// get controls

	liq->speed_alter=((float)dialog_get_value(palette_liquid_wind,kLiquidSpeedAlter,0))/100.0f;
	
	liq->tide.division=dialog_get_value(palette_liquid_wind,kLiquidWaveSize,0);
	liq->tide.high=dialog_get_value(palette_liquid_wind,kLiquidTideSize,0);
	liq->tide.rate=dialog_get_value(palette_liquid_wind,kLiquidTideRate,0);
	liq->tide.direction=dialog_get_combo(palette_liquid_wind,kLiquidTideDirection,0);
	liq->tide.flat=dialog_get_boolean(palette_liquid_wind,kLiquidWaveFlat,0);
	
	liq->harm.in_harm=dialog_get_int(palette_liquid_wind,kLiquidHarm,0);
	liq->harm.drown_tick=dialog_get_int(palette_liquid_wind,kLiquidDrownTick,0);
	liq->harm.drown_harm=dialog_get_int(palette_liquid_wind,kLiquidDrownHarm,0);
	
	dialog_get_color(palette_liquid_wind,kLiquidColor,0,&liq->col);
	
	liq->tint_alpha=dialog_get_float(palette_liquid_wind,kLiquidTintAlpha,0);
	liq->depth=dialog_get_int(palette_liquid_wind,kLiquidDepth,0);
}


static pascal OSStatus palette_liquid_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				event_class,event_kind;
	HICommand		cmd;
	ControlRef		ctrl;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
		// button inside tab
		
	if ((event_class==kEventClassCommand) && (event_kind==kEventProcessCommand)) {
		GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
		switch (cmd.commandID) {
			
			case kLiquidColor:
				dialog_click_color(palette_liquid_wind,kLiquidColor,0);
				return(noErr);
		}
		
		return(eventNotHandledErr);
	}
	
		// control changes
		
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
	
			// save the changes
			
		palette_liquid_save();
		
			// tab change?
			
		GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
		if (ctrl==palette_liquid_tab) dialog_switch_tab(palette_liquid_wind,kLiquidSettingTab,0,kLiquidSettingTabCount);
	}
	
	return(eventNotHandledErr);
}

void palette_liquid_open(int x,int y)
{
	ControlID				ctrl_id;
	EventHandlerUPP			tab_event_upp;
	EventTypeSpec			tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit}};

		// open the window
		
	dialog_open(&palette_liquid_wind,"LiquidPalette");
	MoveWindow(palette_liquid_wind,x,y,FALSE);
	
		// setup the tabs
		
	dialog_set_tab(palette_liquid_wind,kLiquidSettingTab,0,0,kLiquidSettingTabCount);

	ctrl_id.signature=kLiquidSettingTab;
	ctrl_id.id=0;
	GetControlByID(palette_liquid_wind,&ctrl_id,&palette_liquid_tab);
	
	tab_event_upp=NewEventHandlerUPP(palette_liquid_tab_proc);
	InstallControlEventHandler(palette_liquid_tab,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,palette_liquid_wind,NULL);

		// show palette
		
	ShowWindow(palette_liquid_wind);
}

void palette_liquid_close(void)
{
	DisposeWindow(palette_liquid_wind);
}



