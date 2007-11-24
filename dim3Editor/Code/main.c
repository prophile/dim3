/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Line

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
#include "common_view.h"
#include "dialog.h"

int						cr;
bool					done,map_opened;

file_path_setup_type	file_path_setup;

CCrsrHandle				handcur,dragcur,cutcur,rotatecur,towardcur,forwardcur,resizecur,addcur;
CIconHandle				maplight_icon,mapsound_icon,mapparticle_icon;

extern bool setup_xml_read(void);

/* =======================================================

      Main Event Loop
      
======================================================= */

void doloop(void)
{
	EventHandlerRef		menu_event;
	EventHandlerUPP		menu_upp;
	EventTypeSpec		app_menu_events[]=	{{kEventClassCommand,kEventProcessCommand}};
	
	menu_upp=NewEventHandlerUPP(menu_event_callback);						   
	InstallEventHandler(GetApplicationEventTarget(),menu_upp,GetEventTypeCount(app_menu_events),app_menu_events,NULL,&menu_event);
	
	RunApplicationEventLoop();

	RemoveEventHandler(menu_event);
	DisposeEventHandlerUPP(menu_upp);
}

/* =======================================================

      Initialization Code
      
======================================================= */

RGBColor make_rgb(unsigned short r,unsigned short g,unsigned short b)
{
	RGBColor	acol;
	
	acol.red=r;
	acol.green=g;
	acol.blue=b;
	
	return(acol);
}

void doinit(void)
{
	RegisterAppearanceClient();
	
	InitCursor();
	
	MoreMasterPointers(128);
	FlushEvents(everyEvent,0);

	HMSetTagDelay(250);

	handcur=GetCCursor(128);
	dragcur=GetCCursor(129);
	cutcur=GetCCursor(130);
	rotatecur=GetCCursor(131);
	towardcur=GetCCursor(132);
	forwardcur=GetCCursor(133);
	resizecur=GetCCursor(134);
	addcur=GetCCursor(135);

	maplight_icon=GetCIcon(650);
	mapsound_icon=GetCIcon(651);
	mapparticle_icon=GetCIcon(652);
    
	cr=-1;
	map_opened=FALSE;
	
		// setup paths

	file_paths_setup(&file_path_setup,TRUE);
	
		// setup XML
		
	setup_xml_read();
}

void doshutdown(void)
{
	DisposeCCursor(handcur);
	DisposeCCursor(dragcur);
	DisposeCCursor(cutcur);
	DisposeCCursor(rotatecur);
	DisposeCCursor(towardcur);
	DisposeCCursor(forwardcur);
	DisposeCCursor(resizecur);
	DisposeCCursor(addcur);
	
	DisposeCIcon(maplight_icon);
	DisposeCIcon(mapsound_icon);
	DisposeCIcon(mapparticle_icon);

	UnregisterAppearanceClient();
}

/* =======================================================

      Infamous Main Code
      
======================================================= */

int main(int argc,char *argv[])
{
	doinit();
	
	menu_start();
	
	file_open_map();
	doloop();
	file_close_map();
	
	doshutdown();
	
    return(0);
}
