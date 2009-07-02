/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Scenery Palette

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

#define kSceneryTabCount					3
#define kSceneryTab							FOUR_CHAR_CODE('tabb')

#define kSceneryModelName					FOUR_CHAR_CODE('mnme')
#define kSceneryAnimationName				FOUR_CHAR_CODE('anme')
#define kSceneryAngleX						FOUR_CHAR_CODE('angx')
#define kSceneryAngleY						FOUR_CHAR_CODE('angy')
#define kSceneryAngleZ						FOUR_CHAR_CODE('angz')
#define kSceneryResize						FOUR_CHAR_CODE('resz')
#define kSceneryContactObject				FOUR_CHAR_CODE('cnct')
#define kSceneryContactProjectile			FOUR_CHAR_CODE('cnpj')
#define kSceneryContactHitBox				FOUR_CHAR_CODE('hbox')
#define kSceneryFaceFront					FOUR_CHAR_CODE('afft')
#define kSceneryShadow						FOUR_CHAR_CODE('shdw')
#define kSceneryFrame						FOUR_CHAR_CODE('txtf')

extern map_type				map;

WindowRef					palette_scenery_wind;
ControlRef					palette_scenery_tab;

/* =======================================================

      Scenery Palette Data
      
======================================================= */

void palette_scenery_load(void)
{
	int						n,type,main_idx,sub_idx;
	map_scenery_type		*scenery;

		// get scenery
		
	select_get(0,&type,&main_idx,&sub_idx);
	scenery=&map.sceneries[main_idx];

		// set controls
	
	dialog_special_combo_fill_model(palette_scenery_wind,kSceneryModelName,0,scenery->model_name);
	dialog_set_text(palette_scenery_wind,kSceneryAnimationName,0,scenery->animation_name);
	dialog_set_float(palette_scenery_wind,kSceneryAngleX,0,scenery->ang.x);
	dialog_set_float(palette_scenery_wind,kSceneryAngleY,0,scenery->ang.y);
	dialog_set_float(palette_scenery_wind,kSceneryAngleZ,0,scenery->ang.z);
	dialog_set_float(palette_scenery_wind,kSceneryResize,0,scenery->resize);
	dialog_set_boolean(palette_scenery_wind,kSceneryContactObject,0,scenery->contact_object_on);
	dialog_set_boolean(palette_scenery_wind,kSceneryContactProjectile,0,scenery->contact_projectile_on);
	dialog_set_boolean(palette_scenery_wind,kSceneryContactHitBox,0,scenery->contact_hit_box);
	dialog_set_boolean(palette_scenery_wind,kSceneryFaceFront,0,scenery->face_forward);
	dialog_set_boolean(palette_scenery_wind,kSceneryShadow,0,scenery->shadow);
	
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		dialog_set_int(palette_scenery_wind,kSceneryFrame,n,(int)scenery->texture_frame[n]);
	}

	DrawControls(palette_scenery_wind);
}

void palette_scenery_save(void)
{
	int						n,type,main_idx,sub_idx;
	map_scenery_type		*scenery;

		// get scenery
		
	select_get(0,&type,&main_idx,&sub_idx);
	scenery=&map.sceneries[main_idx];
		
		// get controls

	dialog_special_combo_get_model(palette_scenery_wind,kSceneryModelName,0,scenery->model_name,name_str_len);
	dialog_get_text(palette_scenery_wind,kSceneryAnimationName,0,scenery->animation_name,name_str_len);
	scenery->ang.x=dialog_get_float(palette_scenery_wind,kSceneryAngleX,0);
	scenery->ang.y=dialog_get_float(palette_scenery_wind,kSceneryAngleY,0);
	scenery->ang.z=dialog_get_float(palette_scenery_wind,kSceneryAngleZ,0);
	scenery->resize=dialog_get_float(palette_scenery_wind,kSceneryResize,0);
	scenery->contact_object_on=dialog_get_boolean(palette_scenery_wind,kSceneryContactObject,0);
	scenery->contact_projectile_on=dialog_get_boolean(palette_scenery_wind,kSceneryContactProjectile,0);
	scenery->contact_hit_box=dialog_get_boolean(palette_scenery_wind,kSceneryContactHitBox,0);
	scenery->face_forward=dialog_get_boolean(palette_scenery_wind,kSceneryFaceFront,0);
	scenery->shadow=dialog_get_boolean(palette_scenery_wind,kSceneryShadow,0);
	
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		scenery->texture_frame[n]=(short)dialog_get_int(palette_scenery_wind,kSceneryFrame,n);
	}
	
		// need to reset models
		
	walk_view_models_reset();
}

static pascal OSStatus palette_scenery_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	ControlRef		ctrl;
	
		// save the changes
		
	palette_scenery_save();
	
		// tab change?
		
	GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
	if (ctrl==palette_scenery_tab) dialog_switch_tab(palette_scenery_wind,kSceneryTab,0,kSceneryTabCount);
	
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Scenery Open/Close
      
======================================================= */

void palette_scenery_open(int x,int y)
{
	ControlID				ctrl_id;
	EventHandlerUPP			tab_event_upp;
	EventTypeSpec			tab_event_list[]={{kEventClassControl,kEventControlHit},
											  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_scenery_wind,"SceneryPalette");
	MoveWindow(palette_scenery_wind,x,y,FALSE);
	
		// setup the tabs
		
	dialog_set_tab(palette_scenery_wind,kSceneryTab,0,0,kSceneryTabCount);

	ctrl_id.signature=kSceneryTab;
	ctrl_id.id=0;
	GetControlByID(palette_scenery_wind,&ctrl_id,&palette_scenery_tab);
	
	tab_event_upp=NewEventHandlerUPP(palette_scenery_tab_proc);
	InstallControlEventHandler(palette_scenery_tab,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,palette_scenery_wind,NULL);

		// show palette
		
	ShowWindow(palette_scenery_wind);
}

void palette_scenery_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_scenery_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_scenery_wind);
}



