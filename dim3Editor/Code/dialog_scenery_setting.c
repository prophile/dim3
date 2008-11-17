/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Scenery Dialog

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
#include "walk_view.h"

extern map_type				map;

#define kSceneryModelName							FOUR_CHAR_CODE('mnme')
#define kSceneryLightingMode						FOUR_CHAR_CODE('ltmd')
#define kSceneryAnimationName						FOUR_CHAR_CODE('anme')
#define kSceneryAngleX								FOUR_CHAR_CODE('angx')
#define kSceneryAngleY								FOUR_CHAR_CODE('angy')
#define kSceneryAngleZ								FOUR_CHAR_CODE('angz')
#define kSceneryContactObject						FOUR_CHAR_CODE('cnct')
#define kSceneryContactProjectile					FOUR_CHAR_CODE('cnpj')
#define kSceneryContactHitBox						FOUR_CHAR_CODE('hbox')
#define kSceneryFaceFront							FOUR_CHAR_CODE('afft')
#define kSceneryShadow								FOUR_CHAR_CODE('shdw')
#define kSceneryShadowCastDown						FOUR_CHAR_CODE('swcd')
#define kSceneryOverrideSize						FOUR_CHAR_CODE('orsz')
#define kScenerySizeX								FOUR_CHAR_CODE('rszX')
#define kScenerySizeY								FOUR_CHAR_CODE('rszY')
#define kScenerySizeZ								FOUR_CHAR_CODE('rszZ')
#define kSceneryFrame								FOUR_CHAR_CODE('txtf')

bool						dialog_scenery_setting_cancel;
WindowRef					dialog_scenery_setting_wind;

/* =======================================================

      Map Scenery Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_scenery_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_scenery_setting_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_scenery_setting_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_scenery_setting_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Scenery Setting
      
======================================================= */

bool dialog_scenery_setting_run(map_scenery_type *scenery)
{
	int						n;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_scenery_setting_wind,"ScenerySetting");

		// set controls

	dialog_special_combo_fill_model(dialog_scenery_setting_wind,kSceneryModelName,0,scenery->model_name);
	dialog_set_combo(dialog_scenery_setting_wind,kSceneryLightingMode,0,scenery->lighting_mode);
	dialog_set_text(dialog_scenery_setting_wind,kSceneryAnimationName,0,scenery->animation_name);
	dialog_set_float(dialog_scenery_setting_wind,kSceneryAngleX,0,scenery->ang.x);
	dialog_set_float(dialog_scenery_setting_wind,kSceneryAngleY,0,scenery->ang.y);
	dialog_set_float(dialog_scenery_setting_wind,kSceneryAngleZ,0,scenery->ang.z);
	dialog_set_boolean(dialog_scenery_setting_wind,kSceneryContactObject,0,scenery->contact_object_on);
	dialog_set_boolean(dialog_scenery_setting_wind,kSceneryContactProjectile,0,scenery->contact_projectile_on);
	dialog_set_boolean(dialog_scenery_setting_wind,kSceneryContactHitBox,0,scenery->contact_hit_box);
	dialog_set_boolean(dialog_scenery_setting_wind,kSceneryFaceFront,0,scenery->face_forward);
	dialog_set_boolean(dialog_scenery_setting_wind,kSceneryShadow,0,scenery->shadow);
	dialog_set_boolean(dialog_scenery_setting_wind,kSceneryShadowCastDown,0,scenery->shadow_cast_down);
	dialog_set_boolean(dialog_scenery_setting_wind,kSceneryOverrideSize,0,scenery->override_size);
	dialog_set_int(dialog_scenery_setting_wind,kScenerySizeX,0,scenery->size.x);
	dialog_set_int(dialog_scenery_setting_wind,kScenerySizeY,0,scenery->size.y);
	dialog_set_int(dialog_scenery_setting_wind,kScenerySizeZ,0,scenery->size.z);
	
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		dialog_set_int(dialog_scenery_setting_wind,kSceneryFrame,n,scenery->texture_frames[n]);
	}
	
		// show window
	
	ShowWindow(dialog_scenery_setting_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_scenery_setting_event_proc);
	InstallWindowEventHandler(dialog_scenery_setting_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_scenery_setting_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_scenery_setting_wind);
	
		// dialog to data
		
	if (!dialog_scenery_setting_cancel) {
		dialog_special_combo_get_model(dialog_scenery_setting_wind,kSceneryModelName,0,scenery->model_name,name_str_len);
		scenery->lighting_mode=dialog_get_combo(dialog_scenery_setting_wind,kSceneryLightingMode,0);
		dialog_get_text(dialog_scenery_setting_wind,kSceneryAnimationName,0,scenery->animation_name,name_str_len);
		scenery->ang.x=dialog_get_float(dialog_scenery_setting_wind,kSceneryAngleX,0);
		scenery->ang.y=dialog_get_float(dialog_scenery_setting_wind,kSceneryAngleY,0);
		scenery->ang.z=dialog_get_float(dialog_scenery_setting_wind,kSceneryAngleZ,0);
		scenery->contact_object_on=dialog_get_boolean(dialog_scenery_setting_wind,kSceneryContactObject,0);
		scenery->contact_projectile_on=dialog_get_boolean(dialog_scenery_setting_wind,kSceneryContactProjectile,0);
		scenery->contact_hit_box=dialog_get_boolean(dialog_scenery_setting_wind,kSceneryContactHitBox,0);
		scenery->face_forward=dialog_get_boolean(dialog_scenery_setting_wind,kSceneryFaceFront,0);
		scenery->shadow=dialog_get_boolean(dialog_scenery_setting_wind,kSceneryShadow,0);
		scenery->shadow_cast_down=dialog_get_boolean(dialog_scenery_setting_wind,kSceneryShadowCastDown,0);
		scenery->override_size=dialog_get_boolean(dialog_scenery_setting_wind,kSceneryOverrideSize,0);
		scenery->size.x=dialog_get_int(dialog_scenery_setting_wind,kScenerySizeX,0);
		scenery->size.y=dialog_get_int(dialog_scenery_setting_wind,kScenerySizeY,0);
		scenery->size.z=dialog_get_int(dialog_scenery_setting_wind,kScenerySizeZ,0);
		
		for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
			scenery->texture_frames[n]=dialog_get_int(dialog_scenery_setting_wind,kSceneryFrame,n);
		}
	
		main_wind_draw();
	}

		// close window
		
	DisposeWindow(dialog_scenery_setting_wind);
	
		// reset any models
		
	walk_view_models_reset();
	main_wind_draw();
	
	return(!dialog_scenery_setting_cancel);
}

