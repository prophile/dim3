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
#define kMeshSettingLockUV						FOUR_CHAR_CODE('lkuv')

#define kMeshSettingRotX						FOUR_CHAR_CODE('rotx')
#define kMeshSettingRotY						FOUR_CHAR_CODE('roty')
#define kMeshSettingRotZ						FOUR_CHAR_CODE('rotz')

#define kMeshPolySettingOffX					FOUR_CHAR_CODE('offx')
#define kMeshPolySettingOffY					FOUR_CHAR_CODE('offy')
#define kMeshPolySettingSizeX					FOUR_CHAR_CODE('sizx')
#define kMeshPolySettingSizeY					FOUR_CHAR_CODE('sizy')
#define kMeshPolySettingAlpha					FOUR_CHAR_CODE('alph')
#define kMeshPolySettingDark					FOUR_CHAR_CODE('dark')
#define kMeshPolySettingShiftX					FOUR_CHAR_CODE('sftx')
#define kMeshPolySettingShiftY					FOUR_CHAR_CODE('sfty')

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

bool dialog_mesh_setting_run(int portal_idx,int mesh_idx,int poly_idx)
{
	float					x_txtoff,y_txtoff,x_txtfact,y_txtfact;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_mesh_setting_wind,"MeshSetting");

		// set controls
	
	mesh=&map.portals[portal_idx].mesh.meshes[mesh_idx];
		
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingOn,0,mesh->flag.on);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingPassThrough,0,mesh->flag.pass_through);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingMovable,0,mesh->flag.moveable);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingClimbable,0,mesh->flag.climbable);
	dialog_set_boolean(dialog_mesh_setting_wind,kMeshSettingLockUV,0,mesh->flag.lock_uv);

	dialog_set_int(dialog_mesh_setting_wind,kMeshSettingRotX,0,mesh->rot_off.x);
	dialog_set_int(dialog_mesh_setting_wind,kMeshSettingRotY,0,mesh->rot_off.y);
	dialog_set_int(dialog_mesh_setting_wind,kMeshSettingRotZ,0,mesh->rot_off.z);
	
	mesh_poly=&mesh->polys[poly_idx];
	
	map_portal_mesh_get_poly_uv_as_box(&map,portal_idx,mesh_idx,poly_idx,&x_txtoff,&y_txtoff,&x_txtfact,&y_txtfact);
	
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingOffX,0,x_txtoff);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingOffY,0,y_txtoff);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingSizeX,0,x_txtfact);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingSizeY,0,y_txtfact);
	
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingAlpha,0,mesh_poly->alpha);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingDark,0,mesh_poly->dark_factor);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingShiftX,0,mesh_poly->x_shift);
	dialog_set_float(dialog_mesh_setting_wind,kMeshPolySettingShiftY,0,mesh_poly->y_shift);
	
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
		mesh->flag.lock_uv=dialog_get_boolean(dialog_mesh_setting_wind,kMeshSettingLockUV,0);
		
		mesh->rot_off.x=dialog_get_int(dialog_mesh_setting_wind,kMeshSettingRotX,0);
		mesh->rot_off.y=dialog_get_int(dialog_mesh_setting_wind,kMeshSettingRotY,0);
		mesh->rot_off.z=dialog_get_int(dialog_mesh_setting_wind,kMeshSettingRotZ,0);
	
		x_txtoff=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingOffX,0);
		y_txtoff=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingOffY,0);
		x_txtfact=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingSizeX,0);
		y_txtfact=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingSizeY,0);

		map_portal_mesh_set_poly_uv_as_box(&map,portal_idx,mesh_idx,poly_idx,x_txtoff,y_txtoff,x_txtfact,y_txtfact);

		mesh_poly->dark_factor=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingDark,0);
		mesh_poly->alpha=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingAlpha,0);
		mesh_poly->x_shift=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingShiftX,0);
		mesh_poly->y_shift=dialog_get_float(dialog_mesh_setting_wind,kMeshPolySettingShiftY,0);
	}
	
		// close window
		
	DisposeWindow(dialog_mesh_setting_wind);
	
	return(!dialog_mesh_setting_cancel);
}

