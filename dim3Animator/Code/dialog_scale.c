/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Scale Dialog

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

#define kXValue						FOUR_CHAR_CODE('xxxx')
#define kZValue						FOUR_CHAR_CODE('zzzz')
#define kYValue						FOUR_CHAR_CODE('yyyy')
#define kScaleBy					FOUR_CHAR_CODE('scby')

float					dialog_scale_x,dialog_scale_y,dialog_scale_z;
bool					dialog_scale_cancel;
WindowRef				dialog_scale_wind;

/* =======================================================

      Scale Event Handlers
      
======================================================= */

static pascal OSStatus scale_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kScaleBy:
					switch (dialog_get_combo(dialog_scale_wind,kScaleBy,0)) {
						case 0:
							dialog_set_float(dialog_scale_wind,kXValue,0,100.0f);
							dialog_set_float(dialog_scale_wind,kZValue,0,100.0f);
							dialog_set_float(dialog_scale_wind,kYValue,0,100.0f);
							break;
						case 1:
							dialog_set_float(dialog_scale_wind,kXValue,0,dialog_scale_x);
							dialog_set_float(dialog_scale_wind,kZValue,0,dialog_scale_z);
							dialog_set_float(dialog_scale_wind,kYValue,0,dialog_scale_y);
							break;
						case 2:
							dialog_set_float(dialog_scale_wind,kXValue,0,(dialog_scale_x/(float)map_enlarge));
							dialog_set_float(dialog_scale_wind,kZValue,0,(dialog_scale_z/(float)map_enlarge));
							dialog_set_float(dialog_scale_wind,kYValue,0,(dialog_scale_y/(float)map_enlarge));
							break;
					}
					DrawControls(dialog_scale_wind);
					return(noErr);
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_scale_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_scale_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_scale_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Scale Dialog
      
======================================================= */

bool dialog_scale_run(model_type *mdl,float *x,float *z,float *y)
{
	int								minx,maxx,minz,maxz,miny,maxy;
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// find total size of model
		
	model_get_vertex_extent_all(mdl,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	dialog_scale_x=(float)abs(maxx-minx);
	dialog_scale_y=(float)abs(maxy-miny);
	dialog_scale_z=(float)abs(maxz-minz);

		// open the dialog
		
	dialog_open(&dialog_scale_wind,"ScalePick");
	
		// setup the controls
		
	dialog_set_float(dialog_scale_wind,kXValue,0,100.0f);
	dialog_set_float(dialog_scale_wind,kZValue,0,100.0f);
	dialog_set_float(dialog_scale_wind,kYValue,0,100.0f);
	
	dialog_set_combo(dialog_scale_wind,kScaleBy,0,0);
	
		// show window
	
	ShowWindow(dialog_scale_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(scale_event_proc);
	InstallWindowEventHandler(dialog_scale_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_scale_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_scale_wind);
	
	if (!dialog_scale_cancel) {
	
		switch (dialog_get_combo(dialog_scale_wind,kScaleBy,0)) {
			case 0:
				*x=dialog_get_float(dialog_scale_wind,kXValue,0)/100.0f;
				*z=dialog_get_float(dialog_scale_wind,kZValue,0)/100.0f;
				*y=dialog_get_float(dialog_scale_wind,kYValue,0)/100.0f;
				break;
			case 1:
				*x=dialog_get_float(dialog_scale_wind,kXValue,0)/dialog_scale_x;
				*z=dialog_get_float(dialog_scale_wind,kZValue,0)/dialog_scale_z;
				*y=dialog_get_float(dialog_scale_wind,kYValue,0)/dialog_scale_y;
				break;
			case 2:
				*x=(dialog_get_float(dialog_scale_wind,kXValue,0)*(float)map_enlarge)/dialog_scale_x;
				*z=(dialog_get_float(dialog_scale_wind,kZValue,0)*(float)map_enlarge)/dialog_scale_z;
				*y=(dialog_get_float(dialog_scale_wind,kYValue,0)*(float)map_enlarge)/dialog_scale_y;
				break;
		}
	}
	
		// close window

	DisposeWindow(dialog_scale_wind);
	
	return(!dialog_scale_cancel);
}

