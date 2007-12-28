/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Auto-Generate Setting Dialogs

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
#include "import.h"

#define kMapGenerateTabCount						7

#define kMapGenerateTab								FOUR_CHAR_CODE('tabb')

#define kMapGenerateSeed							FOUR_CHAR_CODE('seed')

#define kMapGenerateMapXSize						FOUR_CHAR_CODE('xsiz')
#define kMapGenerateMapZSize						FOUR_CHAR_CODE('zsiz')
#define kMapGenerateSplitFactor						FOUR_CHAR_CODE('splt')

#define kMapGenerateInitialPortalCount				FOUR_CHAR_CODE('ipct')
#define kMapGenerateMinPortalSize					FOUR_CHAR_CODE('mnps')
#define kMapGenerateMaxPortalSize					FOUR_CHAR_CODE('mxps')
#define kMapGeneratePortalBottomY					FOUR_CHAR_CODE('boty')
#define kMapGeneratePortalTopY						FOUR_CHAR_CODE('topy')
#define kMapGenerateMinPortalExtraY					FOUR_CHAR_CODE('mnxy')
#define kMapGenerateMaxPortalExtraY					FOUR_CHAR_CODE('mxxy')
#define kMapGenerateOpenCeilingExtraY				FOUR_CHAR_CODE('ocxy')

#define kMapGenerateMaxMergeDistance				FOUR_CHAR_CODE('mxmd')
#define kMapGenerateMinConnectDistance				FOUR_CHAR_CODE('mncd')
#define kMapGenerateMaxConnectDistance				FOUR_CHAR_CODE('mxcd')
#define kMapGenerateMinConnectSize					FOUR_CHAR_CODE('mncs')
#define kMapGenerateMaxConnectSize					FOUR_CHAR_CODE('mxcs')
#define kMapGenerateCeilingSlantExtraY				FOUR_CHAR_CODE('snty')
#define kMapGenerateCorridorType					FOUR_CHAR_CODE('crty')
#define kMapGenerateStepSize						FOUR_CHAR_CODE('stsz')
#define kMapGenerateStepHigh						FOUR_CHAR_CODE('sthg')

#define kMapGenerateTexturePortalWall				FOUR_CHAR_CODE('pwtx')
#define kMapGenerateTexturePortalFloor				FOUR_CHAR_CODE('pftx')
#define kMapGenerateTexturePortalCeiling			FOUR_CHAR_CODE('pctx')
#define kMapGenerateTextureCorridorWall				FOUR_CHAR_CODE('cwtx')
#define kMapGenerateTextureCorridorFloor			FOUR_CHAR_CODE('cftx')
#define kMapGenerateTextureCorridorCeiling			FOUR_CHAR_CODE('cctx')
#define kMapGenerateTextureDoor						FOUR_CHAR_CODE('drtx')
#define kMapGenerateTextureStep						FOUR_CHAR_CODE('sttx')

#define kMapGenerateBlock							FOUR_CHAR_CODE('bloc')

#define kMapGenerateIncludeWalls					FOUR_CHAR_CODE('iwal')
#define kMapGenerateIncludeFloors					FOUR_CHAR_CODE('iflr')
#define kMapGenerateRoughPortalFloors				FOUR_CHAR_CODE('rgfr')
#define kMapGenerateRoughCorridorFloors				FOUR_CHAR_CODE('rcfr')
#define kMapGenerateRoughFloorFactor				FOUR_CHAR_CODE('rfff')
#define kMapGenerateIncludeCeilings					FOUR_CHAR_CODE('iclg')
#define kMapGenerateOpenHoleCeilings				FOUR_CHAR_CODE('oclg')
#define kMapGenerateOpenHoldePercentage				FOUR_CHAR_CODE('cprc')
#define kMapGenerateIncludeDoors					FOUR_CHAR_CODE('idor')
#define kMapGenerateDoorPercentage					FOUR_CHAR_CODE('dprc')
#define kMapGenerateDoorSound						FOUR_CHAR_CODE('dsnd')

#define kMapGenerateIncludeLights					FOUR_CHAR_CODE('ilgt')
#define kMapGenerateLightFudgeFactor				FOUR_CHAR_CODE('lfdg')
#define kMapGenerateLightFillPercentage				FOUR_CHAR_CODE('lfpr')
#define kMapGenerateLightColorPercentage			FOUR_CHAR_CODE('lcol')
#define kMapGenerateLightFlickerPercentage			FOUR_CHAR_CODE('lflk')
#define kMapGenerateIncludeSightPath				FOUR_CHAR_CODE('isit')
#define kMapGenerateIncludeSpots					FOUR_CHAR_CODE('ispt')
#define kMapGenerateSpotCount						FOUR_CHAR_CODE('cspt')

#define kMapGenerateBlockCombo						FOUR_CHAR_CODE('bcmb')
#define kMapGenerateBlockPresetEmpty				FOUR_CHAR_CODE('blk0')
#define kMapGenerateBlockPresetCircle				FOUR_CHAR_CODE('blk1')
#define kMapGenerateBlockPresetTopU					FOUR_CHAR_CODE('blk2')
#define kMapGenerateBlockPresetBottomU				FOUR_CHAR_CODE('blk3')
#define kMapGenerateBlockPresetLeftU				FOUR_CHAR_CODE('blk4')
#define kMapGenerateBlockPresetRightU				FOUR_CHAR_CODE('blk5')
#define kMapGenerateBlockPresetVerticalH			FOUR_CHAR_CODE('blk6')
#define kMapGenerateBlockPresetHorizontalH			FOUR_CHAR_CODE('blk7')

extern void map_auto_generate_block_preset(auto_generate_settings_type *ag_settings,int block);

extern auto_generate_settings_type			ag_settings;

bool						dialog_map_generate_cancel;
WindowRef					dialog_map_generate_wind;

/* =======================================================

      Map Generate Setting Utilities
      
======================================================= */

void map_generate_setting_fill_block_buttons(void)
{
	int			x,z;
	
	for (z=0;z!=max_ag_block_sz;z++) {
		for (x=0;x!=max_ag_block_sz;x++) {
			dialog_set_boolean(dialog_map_generate_wind,kMapGenerateBlock,((z*10)+x),(ag_settings.block[z][x]!=0));
		}
	}
}

/* =======================================================

      Map Generate Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_generate_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand					cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
					// blocking combo
					
				case kMapGenerateBlockPresetEmpty:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_empty);
					map_generate_setting_fill_block_buttons();
					return(noErr);
					
				case kMapGenerateBlockPresetCircle:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_circle);
					map_generate_setting_fill_block_buttons();
					return(noErr);
					
				case kMapGenerateBlockPresetTopU:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_top_u);
					map_generate_setting_fill_block_buttons();
					return(noErr);
					
				case kMapGenerateBlockPresetBottomU:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_bottom_u);
					map_generate_setting_fill_block_buttons();
					return(noErr);
					
				case kMapGenerateBlockPresetLeftU:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_left_u);
					map_generate_setting_fill_block_buttons();
					return(noErr);
					
				case kMapGenerateBlockPresetRightU:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_right_u);
					map_generate_setting_fill_block_buttons();
					return(noErr);

				case kMapGenerateBlockPresetVerticalH:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_vertical_h);
					map_generate_setting_fill_block_buttons();
					return(noErr);
					
				case kMapGenerateBlockPresetHorizontalH:
					map_auto_generate_block_preset(&ag_settings,ag_block_preset_horizontal_h);
					map_generate_setting_fill_block_buttons();
					return(noErr);

					// buttons
					
				case kHICommandCancel:
					dialog_map_generate_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_generate_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_generate_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus map_generate_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	dialog_switch_tab(dialog_map_generate_wind,kMapGenerateTab,0,kMapGenerateTabCount);
	return(noErr);
}

/* =======================================================

      Run Map Auto-Generate Setting
      
======================================================= */

bool dialog_map_auto_generate_setting_run(bool first)
{
	int						x,z;
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_map_generate_wind,"MapGenerate");
	
		// tab
		
	dialog_set_tab(dialog_map_generate_wind,kMapGenerateTab,0,0,kMapGenerateTabCount);
	
	ctrl_id.signature=kMapGenerateTab;
	ctrl_id.id=0;
	GetControlByID(dialog_map_generate_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(map_generate_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_map_generate_wind,NULL);
	
		// set random seed
		
	dialog_set_int(dialog_map_generate_wind,kMapGenerateSeed,0,(int)time(NULL));
	
		// old settings
		
	if (!first) {
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMapXSize,0,ag_settings.max_map_x_size);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMapZSize,0,ag_settings.max_map_z_size);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateSplitFactor,0,ag_settings.split_factor);
		
		dialog_set_int(dialog_map_generate_wind,kMapGenerateInitialPortalCount,0,ag_settings.initial_portal_count);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMinPortalSize,0,ag_settings.min_portal_sz);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMaxPortalSize,0,ag_settings.max_portal_sz);
		dialog_set_int(dialog_map_generate_wind,kMapGeneratePortalBottomY,0,ag_settings.portal_by);
		dialog_set_int(dialog_map_generate_wind,kMapGeneratePortalTopY,0,ag_settings.portal_ty);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMinPortalExtraY,0,ag_settings.min_portal_extra_y);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMaxPortalExtraY,0,ag_settings.max_portal_extra_y);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateOpenCeilingExtraY,0,ag_settings.open_ceiling_extra_y);
		
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMaxMergeDistance,0,ag_settings.max_portal_merge_distance);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMinConnectDistance,0,ag_settings.min_portal_connect_distance);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMaxConnectDistance,0,ag_settings.max_portal_connect_distance);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMinConnectSize,0,ag_settings.min_portal_connect_sz);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateMaxConnectSize,0,ag_settings.max_portal_connect_sz);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateCeilingSlantExtraY,0,ag_settings.ceiling_slant_extra_y);
		dialog_set_combo(dialog_map_generate_wind,kMapGenerateCorridorType,0,ag_settings.corridor_type);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateStepSize,0,ag_settings.steps.sz);
		dialog_set_int(dialog_map_generate_wind,kMapGenerateStepHigh,0,ag_settings.steps.high);
		
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE,ag_settings.texture.portal_wall);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE,ag_settings.texture.portal_floor);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling,0,FALSE,ag_settings.texture.portal_ceiling);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWall,0,FALSE,ag_settings.texture.corridor_wall);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE,ag_settings.texture.corridor_floor);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorCeiling,0,FALSE,ag_settings.texture.corridor_ceiling);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE,ag_settings.texture.door);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE,ag_settings.texture.steps);
		
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeWalls,0,ag_settings.walls);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeFloors,0,ag_settings.floors);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateRoughPortalFloors,0,ag_settings.rough_portal_floors);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateRoughCorridorFloors,0,ag_settings.rough_corridor_floors);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateRoughFloorFactor,0,ag_settings.rough_floor_factor);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeCeilings,0,ag_settings.ceilings);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateOpenHoleCeilings,0,ag_settings.open_hole_ceilings);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateOpenHoldePercentage,0,ag_settings.open_hole_percentage);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeDoors,0,ag_settings.doors);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateDoorPercentage,0,ag_settings.door_percentage);
		dialog_set_text(dialog_map_generate_wind,kMapGenerateDoorSound,0,ag_settings.door_sound);
		
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeLights,0,ag_settings.lights);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightFudgeFactor,0,ag_settings.light_fudge_factor);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightFillPercentage,0,ag_settings.light_fill_percentage);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightColorPercentage,0,ag_settings.light_color_percentage);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightFlickerPercentage,0,ag_settings.light_flicker_percentage);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeSightPath,0,ag_settings.sight_path);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateIncludeSpots,0,ag_settings.spots);
		dialog_set_value(dialog_map_generate_wind,kMapGenerateSpotCount,0,ag_settings.spot_count);
		
		map_generate_setting_fill_block_buttons();
	}
	else {
	
			// always need to set textures
			
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE,0);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE,1);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling,0,FALSE,2);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWall,0,FALSE,3);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE,4);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorCeiling,0,FALSE,5);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE,6);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE,7);
	}
	
		// show window
	
	ShowWindow(dialog_map_generate_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_generate_setting_event_proc);
	InstallWindowEventHandler(dialog_map_generate_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_generate_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_generate_wind);
	
		// dialog to data
		
	ag_settings.seed=dialog_get_int(dialog_map_generate_wind,kMapGenerateSeed,0);
	
	ag_settings.max_map_x_size=dialog_get_int(dialog_map_generate_wind,kMapGenerateMapXSize,0);
	ag_settings.max_map_z_size=dialog_get_int(dialog_map_generate_wind,kMapGenerateMapZSize,0);
	ag_settings.split_factor=dialog_get_int(dialog_map_generate_wind,kMapGenerateSplitFactor,0);
	
	ag_settings.initial_portal_count=dialog_get_int(dialog_map_generate_wind,kMapGenerateInitialPortalCount,0);
	ag_settings.min_portal_sz=dialog_get_int(dialog_map_generate_wind,kMapGenerateMinPortalSize,0);
	ag_settings.max_portal_sz=dialog_get_int(dialog_map_generate_wind,kMapGenerateMaxPortalSize,0);
	ag_settings.portal_by=dialog_get_int(dialog_map_generate_wind,kMapGeneratePortalBottomY,0);
	ag_settings.portal_ty=dialog_get_int(dialog_map_generate_wind,kMapGeneratePortalTopY,0);
	ag_settings.min_portal_extra_y=dialog_get_int(dialog_map_generate_wind,kMapGenerateMinPortalExtraY,0);
	ag_settings.max_portal_extra_y=dialog_get_int(dialog_map_generate_wind,kMapGenerateMaxPortalExtraY,0);
	ag_settings.open_ceiling_extra_y=dialog_get_int(dialog_map_generate_wind,kMapGenerateOpenCeilingExtraY,0);
	
	ag_settings.max_portal_merge_distance=dialog_get_int(dialog_map_generate_wind,kMapGenerateMaxMergeDistance,0);
	ag_settings.min_portal_connect_distance=dialog_get_int(dialog_map_generate_wind,kMapGenerateMinConnectDistance,0);
	ag_settings.max_portal_connect_distance=dialog_get_int(dialog_map_generate_wind,kMapGenerateMaxConnectDistance,0);
	ag_settings.min_portal_connect_sz=dialog_get_int(dialog_map_generate_wind,kMapGenerateMinConnectSize,0);
	ag_settings.max_portal_connect_sz=dialog_get_int(dialog_map_generate_wind,kMapGenerateMaxConnectSize,0);
	ag_settings.ceiling_slant_extra_y=dialog_get_int(dialog_map_generate_wind,kMapGenerateCeilingSlantExtraY,0);
	ag_settings.corridor_type=dialog_get_combo(dialog_map_generate_wind,kMapGenerateCorridorType,0);
	ag_settings.steps.sz=dialog_get_int(dialog_map_generate_wind,kMapGenerateStepSize,0);
	ag_settings.steps.high=dialog_get_int(dialog_map_generate_wind,kMapGenerateStepHigh,0);
	
	ag_settings.texture.portal_wall=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE);
	ag_settings.texture.portal_floor=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE);
	ag_settings.texture.portal_ceiling=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling,0,FALSE);
	ag_settings.texture.corridor_wall=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWall,0,FALSE);
	ag_settings.texture.corridor_floor=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE);
	ag_settings.texture.corridor_ceiling=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorCeiling,0,FALSE);
	ag_settings.texture.door=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE);
	ag_settings.texture.steps=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE);
	
	ag_settings.walls=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeWalls,0);
	ag_settings.floors=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeFloors,0);
	ag_settings.rough_portal_floors=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateRoughPortalFloors,0);
	ag_settings.rough_corridor_floors=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateRoughCorridorFloors,0);
	ag_settings.rough_floor_factor=dialog_get_value(dialog_map_generate_wind,kMapGenerateRoughFloorFactor,0);
	ag_settings.ceilings=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeCeilings,0);
	ag_settings.open_hole_ceilings=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateOpenHoleCeilings,0);
	ag_settings.open_hole_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateOpenHoldePercentage,0);
	ag_settings.doors=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeDoors,0);
	ag_settings.door_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateDoorPercentage,0);
	dialog_get_text(dialog_map_generate_wind,kMapGenerateDoorSound,0,ag_settings.door_sound,name_str_len);

	ag_settings.lights=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeLights,0);
	ag_settings.light_fudge_factor=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightFudgeFactor,0);
	ag_settings.light_fill_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightFillPercentage,0);
	ag_settings.light_color_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightColorPercentage,0);
	ag_settings.light_flicker_percentage=dialog_get_value(dialog_map_generate_wind,kMapGenerateLightFlickerPercentage,0);
	ag_settings.sight_path=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeSightPath,0);
	ag_settings.spots=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateIncludeSpots,0);
	ag_settings.spot_count=dialog_get_value(dialog_map_generate_wind,kMapGenerateSpotCount,0);
	
	for (z=0;z!=max_ag_block_sz;z++) {
		for (x=0;x!=max_ag_block_sz;x++) {
			ag_settings.block[z][x]=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateBlock,((z*10)+x))?1:0;
		}
	}
	
		// close window
		
	DisposeWindow(dialog_map_generate_wind);
	
	return(!dialog_map_generate_cancel);
}

