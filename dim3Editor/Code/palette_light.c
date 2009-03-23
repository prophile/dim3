/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Light Palette

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

#define kLightType									FOUR_CHAR_CODE('type')
#define kLightDirection								FOUR_CHAR_CODE('dirr')
#define kLightIntensity								FOUR_CHAR_CODE('ints')
#define kLightExponent								FOUR_CHAR_CODE('fall')
#define kLightColor									FOUR_CHAR_CODE('colr')
#define kLightOn									FOUR_CHAR_CODE('lgon')

extern map_type				map;

WindowRef					palette_light_wind;

/* =======================================================

      Light Poly Data
      
======================================================= */

void palette_light_load(void)
{
	int						type,main_idx,poly_idx;
	map_light_type			*light;
	
		// get light
		
	select_get(0,&type,&main_idx,&poly_idx);
	light=&map.lights[main_idx];
	
		// get controls
		
	dialog_set_combo(palette_light_wind,kLightType,0,light->type);
	dialog_set_combo(palette_light_wind,kLightDirection,0,light->direction);
	dialog_set_int(palette_light_wind,kLightIntensity,0,light->intensity);
	dialog_set_float(palette_light_wind,kLightExponent,0,light->exponent);
	dialog_set_boolean(palette_light_wind,kLightOn,0,light->on);
	dialog_set_color(palette_light_wind,kLightColor,0,&light->col);
	
	DrawControls(palette_light_wind);
}

void palette_light_save(void)
{
	int						type,main_idx,poly_idx;
	map_light_type			*light;
	
		// get light
		
	select_get(0,&type,&main_idx,&poly_idx);
	light=&map.lights[main_idx];
	
		// set controls
	
	light->type=dialog_get_combo(palette_light_wind,kLightType,0);
	light->direction=dialog_get_combo(palette_light_wind,kLightDirection,0);
	light->intensity=dialog_get_int(palette_light_wind,kLightIntensity,0);
	light->exponent=dialog_get_float(palette_light_wind,kLightExponent,0);
	light->on=dialog_get_boolean(palette_light_wind,kLightOn,0);
	dialog_get_color(palette_light_wind,kLightColor,0,&light->col);
	
	main_wind_draw();
}

static pascal OSStatus palette_light_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	palette_light_save();
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Light Open/Close
      
======================================================= */

void palette_light_open(int x,int y)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassControl,kEventControlHit},
										  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_light_wind,"LightPalette");
	MoveWindow(palette_light_wind,x,y,FALSE);

		// show palette
		
	ShowWindow(palette_light_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(palette_light_event_proc);
	InstallWindowEventHandler(palette_light_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
}

void palette_light_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_light_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_light_wind);
}



