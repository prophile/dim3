/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Hit Box Setting Dialog

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

extern model_type		model;

#define kHitBoxName							FOUR_CHAR_CODE('name')
#define kHitBoxSizeX						FOUR_CHAR_CODE('sizx')
#define kHitBoxSizeY						FOUR_CHAR_CODE('sizy')
#define kHitBoxSizeZ						FOUR_CHAR_CODE('sizz')
#define kHitBoxOffsetX						FOUR_CHAR_CODE('offx')
#define kHitBoxOffsetY						FOUR_CHAR_CODE('offy')
#define kHitBoxOffsetZ						FOUR_CHAR_CODE('offz')

bool						dialog_hit_box_settings_cancel;
WindowRef					dialog_hit_box_settings_wind;

/* =======================================================

      Hit Box Setting Event Handlers
      
======================================================= */

static pascal OSStatus hit_box_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_hit_box_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_hit_box_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_hit_box_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Hit Box Setting
      
======================================================= */

bool dialog_hit_box_settings_run(model_hit_box_type *hit_box)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_hit_box_settings_wind,"HitBoxSettings");

		// set controls

	dialog_set_text(dialog_hit_box_settings_wind,kHitBoxName,0,hit_box->name);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeX,0,hit_box->box.size.x);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeY,0,hit_box->box.size.y);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeZ,0,hit_box->box.size.z);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetX,0,hit_box->box.offset.x);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetY,0,hit_box->box.offset.y);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetZ,0,hit_box->box.offset.z);
	
	dialog_set_focus(dialog_hit_box_settings_wind,kHitBoxName,0);
	
		// show window
	
	ShowWindow(dialog_hit_box_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(hit_box_settings_event_proc);
	InstallWindowEventHandler(dialog_hit_box_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_hit_box_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_hit_box_settings_wind);
	
		// dialog to data
		
	if (!dialog_hit_box_settings_cancel) {
		dialog_get_text(dialog_hit_box_settings_wind,kHitBoxName,0,hit_box->name,name_str_len);
		hit_box->box.size.x=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxSizeX,0);
		hit_box->box.size.y=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxSizeY,0);
		hit_box->box.size.z=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxSizeZ,0);
		hit_box->box.offset.x=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxOffsetX,0);
		hit_box->box.offset.y=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxOffsetY,0);
		hit_box->box.offset.z=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxOffsetZ,0);
	}

		// close window
		
	DisposeWindow(dialog_hit_box_settings_wind);
	
	return(!dialog_hit_box_settings_cancel);
}

