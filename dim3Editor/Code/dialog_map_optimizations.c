/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Optimizations Dialog

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
#include "dialog.h"

extern map_type				map;

#define kLODLight									FOUR_CHAR_CODE('llig')
#define kLODBump									FOUR_CHAR_CODE('lbmp')
#define kLODSpecular								FOUR_CHAR_CODE('lspl')
#define kLODGlow									FOUR_CHAR_CODE('lglw')
#define kLODModel									FOUR_CHAR_CODE('lmdl')
#define kLODShadow									FOUR_CHAR_CODE('lshd')
#define kLODEffect									FOUR_CHAR_CODE('leff')

bool						dialog_map_optimizations_cancel;
WindowRef					dialog_map_optimizations_wind;

/* =======================================================

      Map Optimization Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_optimization_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_map_optimizations_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_optimizations_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_optimizations_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Optimizations
      
======================================================= */

bool dialog_map_optimizations_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_optimizations_wind,"MapOptimizations");

		// set controls

	dialog_set_int(dialog_map_optimizations_wind,kLODLight,0,map.optimizations.lod_light_distance);
	dialog_set_int(dialog_map_optimizations_wind,kLODBump,0,map.optimizations.lod_bump_distance);
	dialog_set_int(dialog_map_optimizations_wind,kLODSpecular,0,map.optimizations.lod_specular_distance);
	dialog_set_int(dialog_map_optimizations_wind,kLODGlow,0,map.optimizations.lod_glow_distance);
	dialog_set_int(dialog_map_optimizations_wind,kLODModel,0,map.optimizations.lod_model_distance);
	dialog_set_int(dialog_map_optimizations_wind,kLODShadow,0,map.optimizations.lod_shadow_distance);
	dialog_set_int(dialog_map_optimizations_wind,kLODEffect,0,map.optimizations.lod_effect_distance);
	
		// show window
	
	ShowWindow(dialog_map_optimizations_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_optimization_event_proc);
	InstallWindowEventHandler(dialog_map_optimizations_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_optimizations_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_optimizations_wind);
	
		// dialog to data
		
	if (!dialog_map_optimizations_cancel) {
		map.optimizations.lod_light_distance=dialog_get_int(dialog_map_optimizations_wind,kLODLight,0);
		map.optimizations.lod_bump_distance=dialog_get_int(dialog_map_optimizations_wind,kLODBump,0);
		map.optimizations.lod_specular_distance=dialog_get_int(dialog_map_optimizations_wind,kLODSpecular,0);
		map.optimizations.lod_glow_distance=dialog_get_int(dialog_map_optimizations_wind,kLODGlow,0);
		map.optimizations.lod_model_distance=dialog_get_int(dialog_map_optimizations_wind,kLODModel,0);
		map.optimizations.lod_shadow_distance=dialog_get_int(dialog_map_optimizations_wind,kLODShadow,0);
		map.optimizations.lod_effect_distance=dialog_get_int(dialog_map_optimizations_wind,kLODEffect,0);
	}

		// close window
		
	DisposeWindow(dialog_map_optimizations_wind);
	
	return(!dialog_map_optimizations_cancel);
}

