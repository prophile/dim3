/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effects Main Line

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

#include "effects.h"

extern map_type		map;
extern server_type  server;

extern void console_add_error(char *txt);
extern int game_time_get(void);

/* =======================================================

      Initialize Effects
      
======================================================= */

void effect_start(void)
{
	server.count.effect=0;
}

/* =======================================================

      Create a New Effect
      
======================================================= */

effect_type* effect_spawn(int effecttype,d3pnt *pt,int life_tick)
{
	effect_type		*effect;
	
	if (server.count.effect>=max_effect) {
		console_add_error("No more spots to spawn effect");
		return(NULL);
	}
	
		// create effect
	
	effect=&server.effects[server.count.effect];
	server.count.effect++;
	
	effect->effecttype=effecttype;
	
	memmove(&effect->pnt,pt,sizeof(d3pnt));

	effect->mesh_idx=map_find_mesh(&map,pt);

	effect->start_tick=game_time_get();
	effect->life_tick=life_tick;
	
	return(effect);
}

/* =======================================================

      Delete Effects
      
======================================================= */

void effect_dispose(void)
{
	int					i,tick;
	effect_type			*effect;
	
	tick=game_time_get();
	
		// delete all effects that have timed out
	
	i=0;
	
	while (i<server.count.effect) {
		effect=&server.effects[i];
		
		if ((tick-effect->start_tick)<effect->life_tick) {
			i++;
			continue;
		}
	
		if (i<(server.count.effect-1)) {
			memmove(&server.effects[i],&server.effects[i+1],(sizeof(effect_type)*((server.count.effect-i)-1)));
		}
			
		server.count.effect--;
		if (server.count.effect==0) break;
	}
}

