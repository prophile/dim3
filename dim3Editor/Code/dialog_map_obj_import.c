/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map OBJ Import Setting Dialogs

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

#define kMapObjImportTabCount						4

#define kMapObjImportTab							FOUR_CHAR_CODE('tabb')

#define kMapObjImportFileName						FOUR_CHAR_CODE('file')
#define kMapObjImportFileButton						FOUR_CHAR_CODE('chfl')

#define kMapObjImportXSize							FOUR_CHAR_CODE('xsiz')
#define kMapObjImportZSize							FOUR_CHAR_CODE('zsiz')
#define kMapObjImportXCount							FOUR_CHAR_CODE('xcnt')
#define kMapObjImportZCount							FOUR_CHAR_CODE('zcnt')

#define kMapObjImportTexturePortalWall				FOUR_CHAR_CODE('pwtx')
#define kMapObjImportTexturePortalFloor				FOUR_CHAR_CODE('pftx')
#define kMapObjImportTexturePortalCeiling			FOUR_CHAR_CODE('pctx')

#define kMapObjImportFlipX							FOUR_CHAR_CODE('flpx')
#define kMapObjImportFlipZ							FOUR_CHAR_CODE('flpz')
#define kMapObjImportIncludeLights					FOUR_CHAR_CODE('ilgt')
#define kMapObjImportIncludeSightPath				FOUR_CHAR_CODE('isit')

extern bool import_load_file(char *path,char *ext);
extern void dialog_fill_texture_combo(WindowRef wind,unsigned long sig,int id,bool none,int idx);
extern int dialog_get_texture_combo(WindowRef wind,unsigned long sig,int id,bool none);

bool						dialog_map_obj_import_cancel;
WindowRef					dialog_map_obj_import_wind;

/* =======================================================

      Map OBJ Import Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_obj_import_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	char			path[1024];
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kMapObjImportFileButton:
					if (import_load_file(path,"obj")) {
						dialog_set_text(dialog_map_obj_import_wind,kMapObjImportFileName,0,path);
					}
					return(noErr);
			
				case kHICommandCancel:
					dialog_map_obj_import_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_obj_import_wind);
					return(noErr);
					
				case kHICommandOK:
					dialog_get_text(dialog_map_obj_import_wind,kMapObjImportFileName,0,path,1024);
					if (path[0]==0x0) {
						StandardAlert(0,"\pNo OBJ selected","\pYou need to select a OBJ file before importing.",NULL,NULL);
						return(noErr);
					}
					QuitAppModalLoopForWindow(dialog_map_obj_import_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus map_obj_import_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	dialog_switch_tab(dialog_map_obj_import_wind,kMapObjImportTab,0,kMapObjImportTabCount);
	return(noErr);
}

/* =======================================================

      Run Map OBJ Import Setting
      
======================================================= */

bool dialog_map_obj_import_setting_run(import_obj_settings_type *oi_settings,bool first)
{
	char					path[1024];
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_map_obj_import_wind,"MapObjImport");
	
		// tab
		
	dialog_set_tab(dialog_map_obj_import_wind,kMapObjImportTab,0,0,kMapObjImportTabCount);
	
	ctrl_id.signature=kMapObjImportTab;
	ctrl_id.id=0;
	GetControlByID(dialog_map_obj_import_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(map_obj_import_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_map_obj_import_wind,NULL);
	
		// old settings
		
	if (!first) {
		dialog_set_text(dialog_map_obj_import_wind,kMapObjImportFileName,0,oi_settings->path);
		
		dialog_set_int(dialog_map_obj_import_wind,kMapObjImportXSize,0,oi_settings->portal_x_sz);
		dialog_set_int(dialog_map_obj_import_wind,kMapObjImportZSize,0,oi_settings->portal_z_sz);
		dialog_set_int(dialog_map_obj_import_wind,kMapObjImportXCount,0,oi_settings->portal_x_cnt);
		dialog_set_int(dialog_map_obj_import_wind,kMapObjImportZCount,0,oi_settings->portal_z_cnt);

		dialog_fill_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalWall,0,FALSE,oi_settings->texture_portal_wall);
		dialog_fill_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalFloor,0,FALSE,oi_settings->texture_portal_floor);
		dialog_fill_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalCeiling,0,FALSE,oi_settings->texture_portal_ceiling);

		dialog_set_boolean(dialog_map_obj_import_wind,kMapObjImportIncludeLights,0,oi_settings->lights);
		dialog_set_boolean(dialog_map_obj_import_wind,kMapObjImportIncludeSightPath,0,oi_settings->sight_path);
		dialog_set_boolean(dialog_map_obj_import_wind,kMapObjImportFlipX,0,oi_settings->flip_x);
		dialog_set_boolean(dialog_map_obj_import_wind,kMapObjImportFlipZ,0,oi_settings->flip_z);
	}
	else {
	
			// always need to set textures
			
		dialog_fill_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalWall,0,FALSE,0);
		dialog_fill_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalFloor,0,FALSE,1);
		dialog_fill_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalCeiling,0,FALSE,2);
		
			// load the first OBJ
			
		if (import_load_file(path,"obj")) {
			dialog_set_text(dialog_map_obj_import_wind,kMapObjImportFileName,0,path);
		}
	}
	
		// show window
	
	ShowWindow(dialog_map_obj_import_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_obj_import_setting_event_proc);
	InstallWindowEventHandler(dialog_map_obj_import_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_obj_import_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_obj_import_wind);
	
		// dialog to data

	dialog_get_text(dialog_map_obj_import_wind,kMapObjImportFileName,0,oi_settings->path,1024);

	oi_settings->portal_x_sz=dialog_get_int(dialog_map_obj_import_wind,kMapObjImportXSize,0);
	oi_settings->portal_z_sz=dialog_get_int(dialog_map_obj_import_wind,kMapObjImportZSize,0);
	oi_settings->portal_x_cnt=dialog_get_int(dialog_map_obj_import_wind,kMapObjImportXCount,0);
	oi_settings->portal_z_cnt=dialog_get_int(dialog_map_obj_import_wind,kMapObjImportZCount,0);
		
	oi_settings->texture_portal_wall=dialog_get_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalWall,0,FALSE);
	oi_settings->texture_portal_floor=dialog_get_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalFloor,0,FALSE);
	oi_settings->texture_portal_ceiling=dialog_get_texture_combo(dialog_map_obj_import_wind,kMapObjImportTexturePortalCeiling,0,FALSE);
	
	oi_settings->lights=dialog_get_boolean(dialog_map_obj_import_wind,kMapObjImportIncludeLights,0);
	oi_settings->sight_path=dialog_get_boolean(dialog_map_obj_import_wind,kMapObjImportIncludeSightPath,0);
	oi_settings->flip_x=dialog_get_boolean(dialog_map_obj_import_wind,kMapObjImportFlipX,0);
	oi_settings->flip_z=dialog_get_boolean(dialog_map_obj_import_wind,kMapObjImportFlipZ,0);
	
		// close window
		
	DisposeWindow(dialog_map_obj_import_wind);
	
	return(!dialog_map_obj_import_cancel);
}

