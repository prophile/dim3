/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Ring Setting Dialog

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

#include "window.h"
#include "dialog.h"
#include "model.h"

#define kAnimationRingName					FOUR_CHAR_CODE('rign')
#define kAnimationRingBone					FOUR_CHAR_CODE('rbne')
#define kAnimationRingAngle					FOUR_CHAR_CODE('riga')
#define kAnimationRingSlopX					FOUR_CHAR_CODE('rslx')
#define kAnimationRingSlopY					FOUR_CHAR_CODE('rsly')
#define kAnimationRingSlopZ					FOUR_CHAR_CODE('rslz')

bool						dialog_ring_settings_cancel;
WindowRef					dialog_ring_settings_wind;

/* =======================================================

      Ring Setting Event Handlers
      
======================================================= */

static pascal OSStatus ring_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_ring_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_ring_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_ring_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Ring Setting
      
======================================================= */

bool dialog_ring_settings_run(model_ring_type *ring)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_ring_settings_wind,"RingSettings");
	
		// set controls
		
	dialog_special_combo_fill_ring(dialog_ring_settings_wind,kAnimationRingName,0,ring->name);
	dialog_set_bone_combo(dialog_ring_settings_wind,kAnimationRingBone,0,ring->bone_idx);
	dialog_set_boolean(dialog_ring_settings_wind,kAnimationRingAngle,0,ring->angle);
	dialog_set_int(dialog_ring_settings_wind,kAnimationRingSlopX,0,ring->slop.x);
	dialog_set_int(dialog_ring_settings_wind,kAnimationRingSlopY,0,ring->slop.y);
	dialog_set_int(dialog_ring_settings_wind,kAnimationRingSlopZ,0,ring->slop.z);

		// show window
	
	ShowWindow(dialog_ring_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(ring_settings_event_proc);
	InstallWindowEventHandler(dialog_ring_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_ring_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_ring_settings_wind);
	
		// dialog to data
		
	if (!dialog_ring_settings_cancel) {
		dialog_special_combo_get_ring(dialog_ring_settings_wind,kAnimationRingName,0,ring->name,name_str_len);
		ring->bone_idx=dialog_get_bone_combo(dialog_ring_settings_wind,kAnimationRingBone,0);
		ring->angle=dialog_get_boolean(dialog_ring_settings_wind,kAnimationRingAngle,0);
		ring->slop.x=dialog_get_int(dialog_ring_settings_wind,kAnimationRingSlopX,0);
		ring->slop.y=dialog_get_int(dialog_ring_settings_wind,kAnimationRingSlopY,0);
		ring->slop.z=dialog_get_int(dialog_ring_settings_wind,kAnimationRingSlopZ,0);
	}
	
		// close window
		
	DisposeWindow(dialog_ring_settings_wind);
	
	return(!dialog_ring_settings_cancel);
}

