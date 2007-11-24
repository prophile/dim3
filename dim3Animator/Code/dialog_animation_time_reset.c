/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Animation Time Reset

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dialog.h"

extern int					dialog_animate_idx;
extern model_type			model;

#define kAnimateResetTime					FOUR_CHAR_CODE('time')

bool						dialog_animate_time_reset_cancel;
WindowRef					dialog_animate_time_reset_wind;

/* =======================================================

      Animate Time Reset Event Handlers
      
======================================================= */

static pascal OSStatus animate_time_reset_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_animate_time_reset_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_animate_time_reset_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_animate_time_reset_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Animate Time Reset Utility
	        
======================================================= */

int dialog_animation_reset_time_get_total_time(void)
{
	int						n,tot_time;
	model_animate_type		*animate;
	
	animate=&model.animates[dialog_animate_idx];
	
	tot_time=0;
	
	for (n=0;n!=animate->npose_move;n++) {
		tot_time+=animate->pose_moves[n].msec;
	}
	
	return(tot_time);
}

void dialog_animate_reset_change(int tot_time)
{
	int						n,org_tot_time;
	model_animate_type		*animate;

	org_tot_time=dialog_animation_reset_time_get_total_time();
	if (org_tot_time==tot_time) return;
	
	animate=&model.animates[dialog_animate_idx];
	
	for (n=0;n!=animate->npose_move;n++) {
		animate->pose_moves[n].msec=(animate->pose_moves[n].msec*tot_time)/org_tot_time;
	}
}

/* =======================================================

      Animate Time Reset
	        
======================================================= */

bool dialog_animation_reset_time_run(int animate_idx)
{
	int						tot_time;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
	dialog_animate_idx=animate_idx;
	
		// open the dialog
		
	dialog_open(&dialog_animate_time_reset_wind,"AnimationTimeReset");

		// set controls
		
	dialog_set_int(dialog_animate_time_reset_wind,kAnimateResetTime,0,dialog_animation_reset_time_get_total_time());
	dialog_set_focus(dialog_animate_time_reset_wind,kAnimateResetTime,0);
	
		// show window
	
	ShowWindow(dialog_animate_time_reset_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(animate_time_reset_event_proc);
	InstallWindowEventHandler(dialog_animate_time_reset_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_animate_time_reset_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_animate_time_reset_wind);
	
		// dialog to data
		
	if (!dialog_animate_time_reset_cancel) {
		tot_time=dialog_get_int(dialog_animate_time_reset_wind,kAnimateResetTime,0);
		dialog_animate_reset_change(tot_time);
	}

		// close window
		
	DisposeWindow(dialog_animate_time_reset_wind);
	
	return(!dialog_animate_time_reset_cancel);
}

