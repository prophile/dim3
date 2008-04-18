/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Counts Dialog

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

#define kMapPortalCount								FOUR_CHAR_CODE('poct')
#define kMapSegmentCount							FOUR_CHAR_CODE('sgct')
#define kMapSpotCount								FOUR_CHAR_CODE('spct')
#define kMapSceneryCount							FOUR_CHAR_CODE('ssct')
#define kMapLightCount								FOUR_CHAR_CODE('lgct')
#define kMapSoundCount								FOUR_CHAR_CODE('snct')
#define kMapParticleCount							FOUR_CHAR_CODE('prct')
#define kMapNodeCount								FOUR_CHAR_CODE('ndct')

WindowRef					dialog_map_counts_wind;

/* =======================================================

      Map Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_map_counts_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_counts_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Counts
      
======================================================= */

void dialog_map_counts_run(void)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_counts_wind,"MapCount");

		// set controls
		
	dialog_set_int(dialog_map_counts_wind,kMapPortalCount,0,map.nportal);
	dialog_set_int(dialog_map_counts_wind,kMapSegmentCount,0,0);			// supergumba -- need to fix this
	dialog_set_int(dialog_map_counts_wind,kMapSpotCount,0,map.nspot);
	dialog_set_int(dialog_map_counts_wind,kMapSceneryCount,0,map.nscenery);
	dialog_set_int(dialog_map_counts_wind,kMapLightCount,0,map.nlight);
	dialog_set_int(dialog_map_counts_wind,kMapSoundCount,0,map.nsound);
	dialog_set_int(dialog_map_counts_wind,kMapParticleCount,0,map.nparticle);
	dialog_set_int(dialog_map_counts_wind,kMapNodeCount,0,map.nnode);
	
		// show window
	
	ShowWindow(dialog_map_counts_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_map_counts_event_proc);
	InstallWindowEventHandler(dialog_map_counts_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_map_counts_wind);

		// close window
		
	DisposeWindow(dialog_map_counts_wind);
}

