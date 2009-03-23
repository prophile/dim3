/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Particle Palette

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

#define kParticleName								FOUR_CHAR_CODE('pnam')
#define kParticleSpawnTick							FOUR_CHAR_CODE('sptk')
#define kParticleSlopTick							FOUR_CHAR_CODE('sltk')
#define kParticleOn									FOUR_CHAR_CODE('pton')

extern map_type				map;

WindowRef					palette_particle_wind;

/* =======================================================

      Particle Poly Data
      
======================================================= */

void palette_particle_load(void)
{
	int						type,main_idx,poly_idx;
	map_particle_type		*particle;
	
		// get particle
		
	select_get(0,&type,&main_idx,&poly_idx);
	particle=&map.particles[main_idx];
	
		// get controls
		
	dialog_special_combo_fill_particle(palette_particle_wind,kParticleName,0,particle->name);
	dialog_set_int(palette_particle_wind,kParticleSpawnTick,0,particle->spawn_tick);
	dialog_set_int(palette_particle_wind,kParticleSlopTick,0,particle->slop_tick);
	dialog_set_boolean(palette_particle_wind,kParticleOn,0,particle->on);
	
	DrawControls(palette_particle_wind);
}

void palette_particle_save(void)
{
	int						type,main_idx,poly_idx;
	map_particle_type		*particle;
	
		// get particle
		
	select_get(0,&type,&main_idx,&poly_idx);
	particle=&map.particles[main_idx];
	
		// set controls
	
	dialog_special_combo_get_particle(palette_particle_wind,kParticleName,0,particle->name,name_str_len);
	particle->spawn_tick=dialog_get_int(palette_particle_wind,kParticleSpawnTick,0);
	particle->slop_tick=dialog_get_int(palette_particle_wind,kParticleSlopTick,0);
	particle->on=dialog_get_boolean(palette_particle_wind,kParticleOn,0);
	
		// need to reset node combo
		
	main_wind_tool_fill_node_combo();
	
	main_wind_draw();
}

static pascal OSStatus palette_particle_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	palette_particle_save();
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Particle Open/Close
      
======================================================= */

void palette_particle_open(int x,int y)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassControl,kEventControlHit},
										  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_particle_wind,"ParticlePalette");
	MoveWindow(palette_particle_wind,x,y,FALSE);

		// show palette
		
	ShowWindow(palette_particle_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(palette_particle_event_proc);
	InstallWindowEventHandler(palette_particle_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
}

void palette_particle_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_particle_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_particle_wind);
}



