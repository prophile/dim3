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

      Liquid Palette Data
      
======================================================= */

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

	DrawControls(palette_liquid_wind);
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
	ControlRef		ctrl;
	
		// save the changes
		
	palette_liquid_save();
	
		// tab change?
		
	GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
	if (ctrl==palette_liquid_tab) dialog_switch_tab(palette_liquid_wind,kLiquidSettingTab,0,kLiquidSettingTabCount);
	
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Liquid Open/Close
      
======================================================= */

void palette_liquid_open(int x,int y)
{
	ControlID				ctrl_id;
	EventHandlerUPP			tab_event_upp;
	EventTypeSpec			tab_event_list[]={{kEventClassControl,kEventControlHit},
											  {kEventClassKeyboard,kEventRawKeyUp}};

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

void palette_liquid_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_liquid_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_liquid_wind);
}



