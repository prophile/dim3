/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Timing

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

int							game_tick,game_last_tick;

extern server_type			server;

/* =======================================================

      Start Game Time
      
======================================================= */

void game_time_initialize(void)
{
	time_start();
	
	game_tick=0;
	game_last_tick=time_get();
	
	server.time.paused=FALSE;
}

/* =======================================================

      Set Game Tick
      
======================================================= */

int game_time_calculate(void)
{
	int			current_time;
	
	if (server.time.paused) return(game_tick);
	
	current_time=time_get();
	game_tick+=(current_time-game_last_tick);
	
	game_last_tick=current_time;
	
	return(game_tick);
}

void game_time_reset(void)
{
	game_last_tick=time_get();
}

inline int game_time_get(void)
{
	return(game_tick);
}

void game_time_set(int tick)
{
	game_tick=tick;
}

float game_time_fequency_second_get(void)
{
	int				k;
	
	k=game_tick%2000;
	if (k>=1000) k=2000-k;
	
	return(((float)k)/1000.0f);
}

/* =======================================================

      Start and Stop Pauses
      
======================================================= */

void game_time_pause_start(void)
{
	server.time.paused=TRUE;
}

void game_time_pause_end(void)
{
	if (!server.time.paused) return;
	
	game_time_reset();
	server.time.paused=FALSE;
}

