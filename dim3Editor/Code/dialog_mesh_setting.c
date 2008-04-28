/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Mesh Dialog

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

extern map_type				map;

#define kMeshSettingOn							FOUR_CHAR_CODE('fson')
#define kMeshSettingPassThrough					FOUR_CHAR_CODE('fpth')
#define kMeshSettingMovable						FOUR_CHAR_CODE('fmov')
#define kMeshSettingShiftable					FOUR_CHAR_CODE('fsht')
#define kMeshSettingClimbable					FOUR_CHAR_CODE('fclb')

#define kMeshPolySettingAlpha					FOUR_CHAR_CODE('alph')
#define kMeshPolySettingDark					FOUR_CHAR_CODE('dark')
#define kMeshPolySettingShiftX					FOUR_CHAR_CODE('sftx')
#define kMeshPolySettingShiftY					FOUR_CHAR_CODE('sfty')

#define kMeshPolyXCoord							FOUR_CHAR_CODE('ucrd')
#define kMeshPolyYCoord							FOUR_CHAR_CODE('vcrd')

bool						dialog_mesh_setting_cancel;
WindowRef					dialog_mesh_setting_wind;

/* =======================================================

      Mesh Setting Event Handlers
      
======================================================= */

static pascal OSStatus mesh_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_mesh_setting_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_mesh_setting_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_mesh_setting_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Mesh Setting
      
======================================================= */

bool dialog_mesh_setting_run(map_mesh_type *mesh,int poly_idx)
{
	int						n;
	map_mesh_poly_type		*mesh_poly;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_mesh_setting_wind,"MeshSetting");

		// set controls
		
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingOn,0,mesh->flag.on);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingPassThrough,0,mesh->flag.pass_through);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingMovable,0,mesh->flag.moveable);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingClimbable,0,mesh->flag.climbable);
	
	mesh_poly=&mesh->polys[poly_idx];
	
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingAlpha,0,mesh_poly->alpha);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingDark,0,mesh_poly->dark_factor);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingShiftX,0,mesh_poly->x_shift);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingShiftY,0,mesh_poly->y_shift);
	
	for (n=0;n!=8;n++) {
		if (n>=mesh_poly->ptsz) {
			dialog_enable(dialog_mesh_setting_wind,kMeshPolyXCoord,n,FALSE);
			dialog_enable(dialog_mesh_setting_wind,kMeshPolyYCoord,n,FALSE);
		}
		else {
			dialog_set_float(dialog_mesh_setting_wind,kMeshPolyXCoord,n,mesh_poly->gx[n]);
			dialog_set_float(dialog_mesh_setting_wind,kMeshPolyYCoord,n,mesh_poly->gy[n]);
		}
	}
	
		// show window
	
	ShowWindow(dialog_mesh_setting_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(mesh_setting_event_proc);
	InstallWindowEventHandler(dialog_mesh_setting_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_mesh_setting_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_mesh_setting_wind);
	
		// dialog to data
		
	if (!dialog_mesh_setting_cancel) {
		mesh->flag.on=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingOn,0);
		mesh->flag.pass_through=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingPassThrough,0);
		mesh->flag.moveable=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingMovable,0);
		mesh->flag.climbable=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingClimbable,0);

		mesh_poly->dark_factor=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingDark,0);
		mesh_poly->alpha=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingAlpha,0);
		mesh_poly->x_shift=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingShiftX,0);
		mesh_poly->y_shift=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingShiftY,0);
		
		for (n=0;n!=8;n++) {
			if (n<mesh_poly->ptsz) {
				mesh_poly->gx[n]=dialog_get_float(dialog_mesh_setting_wind,kMeshPolyXCoord,n);
				mesh_poly->gy[n]=dialog_get_float(dialog_mesh_setting_wind,kMeshPolyYCoord,n);
			}
		}
	}
	
		// close window
		
	DisposeWindow(dialog_mesh_setting_wind);
	
	return(!dialog_mesh_setting_cancel);
}

