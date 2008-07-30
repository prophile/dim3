/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Scene Lights

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "lights.h"
#include "effects.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Effect Lights
      
======================================================= */

void effect_add_single_light(int tick,effect_type *effect)
{
	int					intensity,count,mid_tick,fade_tick;
	flash_effect_data	*flash;
	
	if (effect->effecttype!=ef_flash) return;

		// calculate flash
		
	flash=&effect->data.flash;

	intensity=flash->intensity;
	mid_tick=flash->mid_tick;
	
	count=tick-effect->start_tick;

	if (count<mid_tick) {										// start of flash
		intensity=(intensity*count)/mid_tick;
	}
	else {
		if (count>mid_tick) {									// fade of flash
			fade_tick=(effect->life_tick-flash->mid_tick);
			count=fade_tick-(count-mid_tick);
	
			intensity=(intensity*count)/fade_tick;
		}
	}
	
	light_add(&effect->pnt,lt_normal,intensity,&flash->col);
}

void effect_add_lights(int tick)
{
	int					i;
	effect_type			*effect;
	
	effect=server.effects;
		
	for ((i=0);(i!=server.count.effect);i++) {
		effect_add_single_light(tick,effect);		
		effect++;
	}
}

