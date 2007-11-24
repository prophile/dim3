/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Timers

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

#include "scripts.h"

extern js_type				js;

/* =======================================================

      Initialize Timers
      
======================================================= */

void timers_initialize(void)
{
	js.count.timer=0;
}

/* =======================================================

      Find Timer
      
======================================================= */

int timers_find(attach_type *attach,int mode)
{
	int				i;
	timer_type		*timer;
	
	timer=js.timers;
	
	for (i=0;i!=js.count.timer;i++) {
		if (bcmp(&timer->attach,attach,sizeof(attach_type))==0) {
			if (timer->mode==mode) return(i);
		}
		timer++;
	}
	
	return(-1);
}

/* =======================================================

      Add a Timer
      
======================================================= */

bool timers_add(attach_type *attach,int freq,int user_id,char *chain_func_name,int mode)
{
	int				k,script_idx;
	script_type		*script;
	timer_type		*timer;
	
		// if already timer for this script, mark it as disposed
		
	k=timers_find(attach,mode);
	if (k!=-1) {
		js.timers[k].mode=timer_mode_dispose;
	}
	
		// create a new timer
		
	if (js.count.timer>=max_timers) return(FALSE);
	
		// script
	
	script_idx=scripts_find_uid(attach->script_uid);
	script=&js.scripts[script_idx];
	
		// setup timer
	
	timer=&js.timers[js.count.timer];
	
	memmove(&timer->attach,attach,sizeof(attach_type));
	timer->bind=script->bind;
	
	timer->mode=mode;
	
    timer->freq=freq;
    if (timer->freq<1) timer->freq=1;

    timer->count=timer->freq;
    timer->user_id=user_id;
	if (chain_func_name!=NULL) strcpy(timer->chain_func_name,chain_func_name);
	
	js.count.timer++;
	
	return(TRUE);
}

/* =======================================================

      Remove a Timers
      
======================================================= */

void timers_remove(int idx)
{
	if (idx<(js.count.timer-1)) {
		memmove(&js.timers[idx],&js.timers[idx+1],(sizeof(timer_type)*((js.count.timer-idx)-1)));
	}
	
	js.count.timer--;
}

/* =======================================================

      Timer Clear
      
======================================================= */

void timers_clear(attach_type *attach,int mode)
{
	int				k;
	
	k=timers_find(attach,mode);
	if (k==-1) return;
	
	js.timers[k].mode=timer_mode_dispose;
}

/* =======================================================

      Dispose of Bound Timers
      
======================================================= */

void timers_dispose(int bind)
{
	int				i;
	
	i=0;
	
	while (i<js.count.timer) {
	
		if (js.timers[i].bind==bind) {
			timers_remove(i);
			if (js.count.timer==0) break;
		}
		else {
			i++;
		}
		
	}
}

/* =======================================================

      Run Timer
      
======================================================= */

void timers_run(void)
{
	int				i,ntimer;
	timer_type		*timer;
	
		// only run current timers
		// don't run new timers created inside these timer calls
		
	ntimer=js.count.timer;
	
		// run timers
	
	for (i=0;i<ntimer;i++) {
		timer=&js.timers[i];
		
			// skip timers in dispose
			
		if (timer->mode==timer_mode_dispose) continue;
	
			// time to fire?
			
		timer->count--;
		if (timer->count>0) continue;
		
			// fire timer
			
		switch (timer->mode) {
		
			case timer_mode_single:
				scripts_post_event_console(&timer->attach,sd_event_wait,0,timer->user_id);
				timer->mode=timer_mode_dispose;		// auto-dispose waits
				break;
				
			case timer_mode_chain:
				scripts_chain_console(&timer->attach,timer->chain_func_name);
				timer->mode=timer_mode_dispose;		// auto-dispose chains
				break;

			case timer_mode_repeat:
				scripts_post_event_console(&timer->attach,sd_event_timer,0,timer->user_id);
				break;
		}
		
			// repeat timer
			
		timer->count=timer->freq;
	}
	
		// remove timers
		
	i=0;
	
	while (i<js.count.timer) {
		timer=&js.timers[i];
	
		if (timer->mode==timer_mode_dispose) {
			timers_remove(i);
			if (js.count.timer==0) break;
		}
		else {
			i++;
		}
	}
}
	
