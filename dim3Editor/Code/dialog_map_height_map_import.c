/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Height Map Import Setting Dialogs

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "dialog.h"

#define kMapHeightMapImportTabCount					4

#define kMapHeightMapImportTab						FOUR_CHAR_CODE('tabb')

#define kMapHeightMapImportFileName					FOUR_CHAR_CODE('file')
#define kMapHeightMapImportFileButton				FOUR_CHAR_CODE('chfl')
#define kMapHeightMapImportType						FOUR_CHAR_CODE('imty')

#define kMapHeightMapImportXSize					FOUR_CHAR_CODE('xsiz')
#define kMapHeightMapImportZSize					FOUR_CHAR_CODE('zsiz')
#define kMapHeightMapImportXCount					FOUR_CHAR_CODE('xcnt')
#define kMapHeightMapImportZCount					FOUR_CHAR_CODE('zcnt')
#define kMapHeightMapImportTopY						FOUR_CHAR_CODE('ytop')
#define kMapHeightMapImportBottomY					FOUR_CHAR_CODE('ybot')

#define kMapHeightMapImportTexturePortalWall		FOUR_CHAR_CODE('pwtx')
#define kMapHeightMapImportTexturePortalHighFloor	FOUR_CHAR_CODE('pft1')
#define kMapHeightMapImportTexturePortalMidFloor	FOUR_CHAR_CODE('pft2')
#define kMapHeightMapImportTexturePortalLowFloor	FOUR_CHAR_CODE('pft3')

#define kMapHeightMapImportIncludeWall				FOUR_CHAR_CODE('iwal')
#define kMapHeightMapImportWallHeight				FOUR_CHAR_CODE('wlhg')
#define kMapHeightMapImportIncludeLights			FOUR_CHAR_CODE('ilgt')
#define kMapHeightMapImportIncludeSightPath			FOUR_CHAR_CODE('isit')
#define kMapHeightMapImportIncludeSightPathAll		FOUR_CHAR_CODE('isal')

extern bool import_load_file(char *path,char *ext);
extern void dialog_fill_texture_combo(WindowRef wind,unsigned long sig,int id,bool none,int idx);
extern int dialog_get_texture_combo(WindowRef wind,unsigned long sig,int id,bool none);

bool						dialog_map_height_import_cancel;
WindowRef					dialog_map_height_import_wind;

/* =======================================================

      Map Height Map Import Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_height_import_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	char			path[1024];
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kMapHeightMapImportFileButton:
					if (import_load_file(path,"png")) {
						dialog_set_text(dialog_map_height_import_wind,kMapHeightMapImportFileName,0,path);
					}
					return(noErr);
			
				case kHICommandCancel:
					dialog_map_height_import_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_height_import_wind);
					return(noErr);
					
				case kHICommandOK:
					dialog_get_text(dialog_map_height_import_wind,kMapHeightMapImportFileName,0,path,1024);
					if (path[0]==0x0) {
						StandardAlert(0,"\pNo PNG selected","\pYou need to select a PNG file before importing.",NULL,NULL);
						return(noErr);
					}
					QuitAppModalLoopForWindow(dialog_map_height_import_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus map_height_import_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	dialog_switch_tab(dialog_map_height_import_wind,kMapHeightMapImportTab,0,kMapHeightMapImportTabCount);
	return(noErr);
}

/* =======================================================

      Run Map Height Map Import Setting
      
======================================================= */

bool dialog_map_height_import_setting_run(import_height_map_settings_type *hmi_settings,bool first)
{
	char					path[1024];
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_map_height_import_wind,"MapHeightMapImport");
	
		// tab
		
	dialog_set_tab(dialog_map_height_import_wind,kMapHeightMapImportTab,0,0,kMapHeightMapImportTabCount);
	
	ctrl_id.signature=kMapHeightMapImportTab;
	ctrl_id.id=0;
	GetControlByID(dialog_map_height_import_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(map_height_import_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_map_height_import_wind,NULL);
	
		// old settings
		
	if (!first) {
		dialog_set_text(dialog_map_height_import_wind,kMapHeightMapImportFileName,0,hmi_settings->path);
		dialog_set_combo(dialog_map_height_import_wind,kMapHeightMapImportType,0,hmi_settings->import_type);
		
		dialog_set_int(dialog_map_height_import_wind,kMapHeightMapImportXSize,0,hmi_settings->portal_x_sz);
		dialog_set_int(dialog_map_height_import_wind,kMapHeightMapImportZSize,0,hmi_settings->portal_z_sz);
		dialog_set_int(dialog_map_height_import_wind,kMapHeightMapImportXCount,0,hmi_settings->portal_x_cnt);
		dialog_set_int(dialog_map_height_import_wind,kMapHeightMapImportZCount,0,hmi_settings->portal_z_cnt);
		dialog_set_int(dialog_map_height_import_wind,kMapHeightMapImportTopY,0,hmi_settings->portal_ty);
		dialog_set_int(dialog_map_height_import_wind,kMapHeightMapImportBottomY,0,hmi_settings->portal_by);

		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalWall,0,FALSE,hmi_settings->texture_portal_wall);
		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalHighFloor,0,FALSE,hmi_settings->texture_portal_high_floor);
		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalMidFloor,0,FALSE,hmi_settings->texture_portal_mid_floor);
		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalLowFloor,0,FALSE,hmi_settings->texture_portal_low_floor);

		dialog_set_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeWall,0,hmi_settings->wall);
		dialog_set_int(dialog_map_height_import_wind,kMapHeightMapImportWallHeight,0,hmi_settings->wall_high);
		dialog_set_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeLights,0,hmi_settings->lights);
		dialog_set_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeSightPath,0,hmi_settings->sight_path);
		dialog_set_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeSightPathAll,0,hmi_settings->sight_path_all);
	}
	else {
	
			// always need to set textures
			
		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalWall,0,FALSE,0);
		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalHighFloor,0,FALSE,1);
		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalMidFloor,0,FALSE,1);
		dialog_fill_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalLowFloor,0,FALSE,1);
		
			// load up height map
			
		if (import_load_file(path,"png")) {
			dialog_set_text(dialog_map_height_import_wind,kMapHeightMapImportFileName,0,path);
		}
	}
	
		// show window
	
	ShowWindow(dialog_map_height_import_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_height_import_setting_event_proc);
	InstallWindowEventHandler(dialog_map_height_import_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_height_import_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_height_import_wind);
	
		// dialog to data

	dialog_get_text(dialog_map_height_import_wind,kMapHeightMapImportFileName,0,hmi_settings->path,1024);
	hmi_settings->import_type=dialog_get_combo(dialog_map_height_import_wind,kMapHeightMapImportType,0);

	hmi_settings->portal_x_sz=dialog_get_int(dialog_map_height_import_wind,kMapHeightMapImportXSize,0);
	hmi_settings->portal_z_sz=dialog_get_int(dialog_map_height_import_wind,kMapHeightMapImportZSize,0);
	hmi_settings->portal_x_cnt=dialog_get_int(dialog_map_height_import_wind,kMapHeightMapImportXCount,0);
	hmi_settings->portal_z_cnt=dialog_get_int(dialog_map_height_import_wind,kMapHeightMapImportZCount,0);
	hmi_settings->portal_ty=dialog_get_int(dialog_map_height_import_wind,kMapHeightMapImportTopY,0);
	hmi_settings->portal_by=dialog_get_int(dialog_map_height_import_wind,kMapHeightMapImportBottomY,0);
		
	hmi_settings->texture_portal_wall=dialog_get_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalWall,0,FALSE);
	hmi_settings->texture_portal_high_floor=dialog_get_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalHighFloor,0,FALSE);
	hmi_settings->texture_portal_mid_floor=dialog_get_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalMidFloor,0,FALSE);
	hmi_settings->texture_portal_low_floor=dialog_get_texture_combo(dialog_map_height_import_wind,kMapHeightMapImportTexturePortalLowFloor,0,FALSE);
	
	hmi_settings->wall=dialog_get_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeWall,0);
	hmi_settings->wall_high=dialog_get_int(dialog_map_height_import_wind,kMapHeightMapImportWallHeight,0);
	hmi_settings->lights=dialog_get_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeLights,0);
	hmi_settings->sight_path=dialog_get_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeSightPath,0);
	hmi_settings->sight_path_all=dialog_get_boolean(dialog_map_height_import_wind,kMapHeightMapImportIncludeSightPathAll,0);
	
		// close window
		
	DisposeWindow(dialog_map_height_import_wind);
	
	return(!dialog_map_height_import_cancel);
}

