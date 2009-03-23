/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Sound Palette

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

#define kSoundName									FOUR_CHAR_CODE('snam')
#define kSoundPitch									FOUR_CHAR_CODE('pith')
#define kSoundOn									FOUR_CHAR_CODE('snon')

extern map_type				map;

WindowRef					palette_sound_wind;

/* =======================================================

      Sound Poly Data
      
======================================================= */

void palette_sound_load(void)
{
	int						type,main_idx,poly_idx;
	map_sound_type			*sound;
	
		// get sound
		
	select_get(0,&type,&main_idx,&poly_idx);
	sound=&map.sounds[main_idx];
	
		// get controls
		
	dialog_special_combo_fill_sound(palette_sound_wind,kSoundName,0,sound->name);
	dialog_set_float(palette_sound_wind,kSoundPitch,0,sound->pitch);
	dialog_set_boolean(palette_sound_wind,kSoundOn,0,sound->on);
	
	DrawControls(palette_sound_wind);
}

void palette_sound_save(void)
{
	int						type,main_idx,poly_idx;
	map_sound_type			*sound;
	
		// get sound
		
	select_get(0,&type,&main_idx,&poly_idx);
	sound=&map.sounds[main_idx];
	
		// set controls
	
	dialog_special_combo_get_sound(palette_sound_wind,kSoundName,0,sound->name,name_str_len);
	sound->pitch=dialog_get_float(palette_sound_wind,kSoundPitch,0);
	sound->on=dialog_get_boolean(palette_sound_wind,kSoundOn,0);
	
	main_wind_draw();
}

static pascal OSStatus palette_sound_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	palette_sound_save();
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Sound Open/Close
      
======================================================= */

void palette_sound_open(int x,int y)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassControl,kEventControlHit},
										  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_sound_wind,"SoundPalette");
	MoveWindow(palette_sound_wind,x,y,FALSE);

		// show palette
		
	ShowWindow(palette_sound_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(palette_sound_event_proc);
	InstallWindowEventHandler(palette_sound_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
}

void palette_sound_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_sound_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_sound_wind);
}



