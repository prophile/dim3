/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Area Palette

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
#include "common_view.h"
#include "dialog.h"

#define kAreaColor						FOUR_CHAR_CODE('colr')

extern map_type				map;

WindowRef					palette_area_wind;

/* =======================================================

      Palette Area Data
      
======================================================= */

void palette_area_load(void)
{
	int						type,main_idx,sub_idx;
	map_area_type			*area;
	
		// get area
		
	select_get(0,&type,&main_idx,&sub_idx);
	area=&map.areas[main_idx];
	
		// set controls
	
	dialog_set_color(palette_area_wind,kAreaColor,0,&area->col);
	
	DrawControls(palette_area_wind);
}

void palette_area_save(void)
{
	int						type,main_idx,sub_idx;
	map_area_type			*area;
	
		// get area
		
	select_get(0,&type,&main_idx,&sub_idx);
	area=&map.areas[main_idx];
	
		// save data
		
	dialog_get_color(palette_area_wind,kAreaColor,0,&area->col);
	
	main_wind_draw();
}

static pascal OSStatus palette_area_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
		// force save on all control changes
		
	palette_area_save();
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Area Open/Close
      
======================================================= */

void palette_area_open(int x,int y)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassControl,kEventControlHit},
										  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_area_wind,"AreaPalette");
	MoveWindow(palette_area_wind,x,y,FALSE);

		// show palette
		
	ShowWindow(palette_area_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(palette_area_event_proc);
	InstallWindowEventHandler(palette_area_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
}

void palette_area_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_area_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_area_wind);
}



