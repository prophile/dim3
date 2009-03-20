/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Spot Palette

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

#define kSpotSettingTabCount					2
#define kSpotSettingTab							FOUR_CHAR_CODE('tabb')

#define kSpotName								FOUR_CHAR_CODE('name')
#define kSpotType								FOUR_CHAR_CODE('type')
#define kSpotScript								FOUR_CHAR_CODE('scpt')
#define kSpotParams								FOUR_CHAR_CODE('parm')
#define kSpotAngle								FOUR_CHAR_CODE('ange')
#define kSpotSkill								FOUR_CHAR_CODE('skil')
#define kSpotSpawn								FOUR_CHAR_CODE('spwn')
#define kSpotDisplayModel						FOUR_CHAR_CODE('dspm')
#define kSpotButtonEdit							FOUR_CHAR_CODE('edit')

extern map_type				map;

WindowRef					palette_spot_wind;
ControlRef					palette_spot_tab;

/* =======================================================

      Break Up/Combine Parameters
      
======================================================= */

void palette_spot_setting_break_params(char *str)
{
	int				n;
	char			*c,*c2,tstr[param_str_len];
	
	c=str;
	
	for (n=0;n!=8;n++) {
		if (c==0x0) break;
		
		c2=strchr(c,'|');
		if (c2==NULL) {
			dialog_set_text(palette_spot_wind,kSpotParams,n,c);
			break;
		}
		
		strcpy(tstr,c);
		c=strchr(tstr,'|');
		*c=0x0;
		
		dialog_set_text(palette_spot_wind,kSpotParams,n,tstr);
		
		c=c2+1;
	}
}

void palette_spot_setting_combine_params(char *str)
{
	int				n,cnt;
	char			tstr[param_str_len];
	
	str[0]=0x0;
	
		// find last non-blank
	
	cnt=-1;
	
	for (n=0;n!=8;n++) {
		dialog_get_text(palette_spot_wind,kSpotParams,n,tstr,param_str_len);
		if (tstr[0]!=0x0) cnt=n;
	}
	
	if (cnt==-1) return;
	
		// fill in params
		
	for (n=0;n<=cnt;n++) {
		if (n!=0) strcat(str,"|");
		dialog_get_text(palette_spot_wind,kSpotParams,n,tstr,param_str_len);
		strcat(str,tstr);
	}
}

/* =======================================================

      Palette Spot Data
      
======================================================= */

void palette_spot_load(void)
{
	int						type,spot_idx,sub_idx;
	spot_type				*spot;

		// get mesh
		
	select_get(0,&type,&spot_idx,&sub_idx);
	spot=&map.spots[spot_idx];

		// set controls
	
	dialog_set_text(palette_spot_wind,kSpotName,0,spot->name);
	dialog_set_text(palette_spot_wind,kSpotType,0,spot->type);
	dialog_special_combo_fill_script(palette_spot_wind,kSpotScript,0,spot->script);
	dialog_set_float(palette_spot_wind,kSpotAngle,0,spot->ang.y);
	dialog_set_combo(palette_spot_wind,kSpotSkill,0,spot->skill);
	dialog_set_combo(palette_spot_wind,kSpotSpawn,0,spot->spawn);
	
	dialog_special_combo_fill_model(palette_spot_wind,kSpotDisplayModel,0,spot->display_model);
	
	palette_spot_setting_break_params(spot->params);

	DrawControls(palette_spot_wind);
}

void palette_spot_save(void)
{
	int						type,spot_idx,sub_idx;
	spot_type				*spot;

		// get mesh
		
	select_get(0,&type,&spot_idx,&sub_idx);
	spot=&map.spots[spot_idx];
		
		// get controls

	dialog_get_text(palette_spot_wind,kSpotName,0,spot->name,name_str_len);
	dialog_get_text(palette_spot_wind,kSpotType,0,spot->type,name_str_len);
	dialog_special_combo_get_script(palette_spot_wind,kSpotScript,0,spot->script,name_str_len);
	spot->ang.y=dialog_get_float(palette_spot_wind,kSpotAngle,0);
	spot->skill=dialog_get_combo(palette_spot_wind,kSpotSkill,0);
	spot->spawn=dialog_get_combo(palette_spot_wind,kSpotSpawn,0);
	
	dialog_special_combo_get_model(palette_spot_wind,kSpotDisplayModel,0,spot->display_model,name_str_len);
	
	palette_spot_setting_combine_params(spot->params);
	
		// need to reset object combo and loaded models
		
	walk_view_models_reset();
	main_wind_tool_fill_object_combo();
}

static pascal OSStatus palette_spot_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				event_class,event_kind;
	char			script_name[name_str_len];
	HICommand		cmd;
	ControlRef		ctrl;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
		// button inside tab
		
	if ((event_class==kEventClassCommand) && (event_kind==kEventProcessCommand)) {
		GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
		switch (cmd.commandID) {
			
			case kSpotButtonEdit:
				palette_spot_save();
				dialog_special_combo_get_script(palette_spot_wind,kSpotScript,0,script_name,name_str_len);
				launch_spot_script_editor(script_name);
				return(noErr);
		}
		
		return(eventNotHandledErr);
	}
	
		// control changes
		
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
	
			// save the changes
			
		palette_spot_save();
		
			// tab change?
			
		GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
		if (ctrl==palette_spot_tab) dialog_switch_tab(palette_spot_wind,kSpotSettingTab,0,kSpotSettingTabCount);
		
		return(eventNotHandledErr);
	}
	
		// keyboard changes
		
	if (event_class==kEventClassKeyboard) {
		palette_spot_save();
		return(eventNotHandledErr);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Spot Open/Close
      
======================================================= */

void palette_spot_open(int x,int y)
{
	ControlID				ctrl_id;
	EventHandlerUPP			tab_event_upp;
	EventTypeSpec			tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit},
											  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_spot_wind,"SpotPalette");
	MoveWindow(palette_spot_wind,x,y,FALSE);
	
		// setup the tabs
		
	dialog_set_tab(palette_spot_wind,kSpotSettingTab,0,0,kSpotSettingTabCount);

	ctrl_id.signature=kSpotSettingTab;
	ctrl_id.id=0;
	GetControlByID(palette_spot_wind,&ctrl_id,&palette_spot_tab);
	
	tab_event_upp=NewEventHandlerUPP(palette_spot_tab_proc);
	InstallControlEventHandler(palette_spot_tab,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,palette_spot_wind,NULL);

		// show palette
		
	ShowWindow(palette_spot_wind);
}

void palette_spot_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_spot_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;
	
	DisposeWindow(palette_spot_wind);
}


