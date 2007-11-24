/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Move Dialog

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

extern int					cr;
extern map_type				map;

#define kPieceMovePortal						FOUR_CHAR_CODE('port')

bool						dialog_piece_move_cancel;
WindowRef					dialog_piece_move_wind;

/* =======================================================

      Piece Move To Portal Event Handlers
      
======================================================= */

static pascal OSStatus piece_move_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_piece_move_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_piece_move_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_piece_move_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Piece Move To Portal Dialog
      
======================================================= */

int dialog_piece_move_to_portal_run(void)
{
	int						n,portal_idx;
	char					str[256];
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_piece_move_wind,"MovePiece");

		// set controls
		
	dialog_clear_combo(dialog_piece_move_wind,kPieceMovePortal,0);
	
	for (n=0;n!=map.nportal;n++) {
		if (map.portals[n].name[0]!=0x0) {
			sprintf(str,"%d (%s)",n,map.portals[n].name);
		}
		else {
			sprintf(str,"%d",n);
		}
		dialog_add_combo_item(dialog_piece_move_wind,kPieceMovePortal,0,str,0);
	}
	
	dialog_set_combo(dialog_piece_move_wind,kPieceMovePortal,0,cr);
	
		// show window
	
	ShowWindow(dialog_piece_move_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(piece_move_event_proc);
	InstallWindowEventHandler(dialog_piece_move_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_piece_move_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_piece_move_wind);
	
		// dialog to data
		
	portal_idx=-1;
	
	if (!dialog_piece_move_cancel) {
		portal_idx=dialog_get_combo(dialog_piece_move_wind,kPieceMovePortal,0);
	}

		// close window
		
	DisposeWindow(dialog_piece_move_wind);
	
	return(portal_idx);
}

