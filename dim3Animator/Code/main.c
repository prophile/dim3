/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Main

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

#include "menu.h"
#include "dialog.h"

bool					fileopen;

file_path_setup_type	file_path_setup;

/* =======================================================

      Main Event Loop
      
======================================================= */

void doloop(void)
{
	EventHandlerRef		menu_event;
	EventHandlerUPP		menu_upp;
	EventTypeSpec		app_menu_events[]={{kEventClassCommand,kEventProcessCommand}};
	
	menu_upp=NewEventHandlerUPP(app_event_menu);						   
	InstallEventHandler(GetApplicationEventTarget(),menu_upp,GetEventTypeCount(app_menu_events),app_menu_events,NULL,&menu_event);
	
	RunApplicationEventLoop();

	RemoveEventHandler(menu_event);
	DisposeEventHandlerUPP(menu_upp);
}

/* =======================================================

      Initialization Code
      
======================================================= */

void app_init(void)
{
	RegisterAppearanceClient();

	InitCursor();
	
	MoreMasterPointers(128);
	FlushEvents(everyEvent,0);

	HMSetTagDelay(250);
	
		// setup paths

	file_paths_setup(&file_path_setup,TRUE);
	
		// no open files
		
	fileopen=FALSE;
}

void app_shutdown(void)
{
	UnregisterAppearanceClient();
}

/* =======================================================

      Infamous Main Code
      
======================================================= */

int main(int argc,char *argv[])
{
	app_init();
	
	menu_start();
    open_model_xml();
	
	doloop();

	app_shutdown();
    
    return(0);
}
