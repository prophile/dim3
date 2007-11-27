/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Blended Animation Playing Dialog

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dialog.h"

extern int				play_animate_blend_idx[max_model_blend_animation];
extern model_type		model;

#define kBlendAnimate1						FOUR_CHAR_CODE('ani1')
#define kBlendAnimate2						FOUR_CHAR_CODE('ani2')
#define kBlendAnimate3						FOUR_CHAR_CODE('ani3')
#define kBlendAnimate4						FOUR_CHAR_CODE('ani4')

bool						dialog_play_blend_animation_cancel;
WindowRef					dialog_play_blend_animation_wind;

/* =======================================================

      Play Blend Animation Event Handlers
      
======================================================= */

static pascal OSStatus play_blend_animation_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_play_blend_animation_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_play_blend_animation_wind);
					return(noErr);
					
				case kHICommandOK:

					QuitAppModalLoopForWindow(dialog_play_blend_animation_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Fill Animation Combo
      
======================================================= */

void dialog_set_animate_combo(WindowRef wind,unsigned long sig,int id,int sel_idx,bool use_none)
{
	int				n;
	
	dialog_clear_combo(wind,sig,id);

	if (use_none) {
		dialog_add_combo_item(wind,sig,id,"None",0);
		dialog_add_combo_item(wind,sig,id,"-",0);
	}
	
	for (n=0;n!=model.nanimate;n++) {
		dialog_add_combo_item(wind,sig,id,model.animates[n].name,0);
	}
	
	if (!use_none) {
		if (sel_idx<0) sel_idx=0;
		dialog_set_combo(wind,sig,id,sel_idx);
	}
	else {
		if (sel_idx==-1) {
			dialog_set_combo(wind,sig,id,0);
		}
		else {
			dialog_set_combo(wind,sig,id,(sel_idx+2));
		}
	}
}

int dialog_get_animate_combo(WindowRef wind,unsigned long sig,int id,bool use_none)
{
	int				sel_idx;
	
	sel_idx=dialog_get_combo(wind,sig,id);
	if (!use_none) return(sel_idx);
	
	if (sel_idx==0) return(-1);
	
	return(sel_idx-2);
}

/* =======================================================

      Play Blend Animation
      
======================================================= */

bool dialog_play_blend_animation_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_play_blend_animation_wind,"BlendAnimation");

		// set controls
		
	dialog_set_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate1,0,play_animate_blend_idx[0],FALSE);
	dialog_set_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate2,0,play_animate_blend_idx[1],TRUE);
	dialog_set_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate3,0,play_animate_blend_idx[2],TRUE);
	dialog_set_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate4,0,play_animate_blend_idx[3],TRUE);
	
		// show window
	
	ShowWindow(dialog_play_blend_animation_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(play_blend_animation_event_proc);
	InstallWindowEventHandler(dialog_play_blend_animation_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_play_blend_animation_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_play_blend_animation_wind);
	
		// dialog to data
		
	if (!dialog_play_blend_animation_cancel) {

			// get play animations
			
		play_animate_blend_idx[0]=dialog_get_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate1,0,FALSE);
		play_animate_blend_idx[1]=dialog_get_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate2,0,TRUE);
		play_animate_blend_idx[2]=dialog_get_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate3,0,TRUE);
		play_animate_blend_idx[3]=dialog_get_animate_combo(dialog_play_blend_animation_wind,kBlendAnimate4,0,TRUE);
	}

		// close window
		
	DisposeWindow(dialog_play_blend_animation_wind);
	
	return(!dialog_play_blend_animation_cancel);
}

