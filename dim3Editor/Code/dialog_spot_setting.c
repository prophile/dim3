/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Spot Dialog

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
#include "walk_view.h"

extern map_type				map;

#define kSpotName									FOUR_CHAR_CODE('name')
#define kSpotType									FOUR_CHAR_CODE('type')
#define kSpotScript									FOUR_CHAR_CODE('scpt')
#define kSpotParams									FOUR_CHAR_CODE('parm')
#define kSpotAngle									FOUR_CHAR_CODE('ange')
#define kSpotSkill									FOUR_CHAR_CODE('skil')
#define kSpotSpawn									FOUR_CHAR_CODE('spwn')
#define kSpotDisplayModel							FOUR_CHAR_CODE('dspm')
#define kSpotButtonEdit								FOUR_CHAR_CODE('edit')

bool						dialog_spot_setting_cancel;
WindowRef					dialog_spot_setting_wind;

/* =======================================================

      Map Spot Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_spot_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	char			script_name[name_str_len];
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kSpotButtonEdit:
					dialog_special_combo_get_script(dialog_spot_setting_wind,kSpotScript,0,script_name,name_str_len);
					launch_spot_script_editor(script_name);
					return(noErr);
					
				case kHICommandCancel:
					dialog_spot_setting_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_spot_setting_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_spot_setting_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Break Up/Combine Parameters
      
======================================================= */

void dialog_spot_setting_break_params(char *str)
{
	int				n;
	char			*c,*c2,tstr[param_str_len];
	
	c=str;
	
	for (n=0;n!=10;n++) {
		if (c==0x0) break;
		
		c2=strchr(c,'|');
		if (c2==NULL) {
			dialog_set_text(dialog_spot_setting_wind,kSpotParams,n,c);
			break;
		}
		
		strcpy(tstr,c);
		c=strchr(tstr,'|');
		*c=0x0;
		
		dialog_set_text(dialog_spot_setting_wind,kSpotParams,n,tstr);
		
		c=c2+1;
	}
}

void dialog_spot_setting_combine_params(char *str)
{
	int				n,cnt;
	char			tstr[param_str_len];
	
	str[0]=0x0;
	
		// find last non-blank
	
	cnt=-1;
	
	for (n=0;n!=10;n++) {
		dialog_get_text(dialog_spot_setting_wind,kSpotParams,n,tstr,param_str_len);
		if (tstr[0]!=0x0) cnt=n;
	}
	
	if (cnt==-1) return;
	
		// fill in params
		
	for (n=0;n<=cnt;n++) {
		if (n!=0) strcat(str,"|");
		dialog_get_text(dialog_spot_setting_wind,kSpotParams,n,tstr,param_str_len);
		strcat(str,tstr);
	}
}

/* =======================================================

      Run Spot Setting
      
======================================================= */

bool dialog_spot_setting_run(spot_type *spot)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_spot_setting_wind,"SpotSetting");

		// set controls

	dialog_set_text(dialog_spot_setting_wind,kSpotName,0,spot->name);
	dialog_set_text(dialog_spot_setting_wind,kSpotType,0,spot->type);
	dialog_special_combo_fill_script(dialog_spot_setting_wind,kSpotScript,0,spot->script);
	dialog_set_float(dialog_spot_setting_wind,kSpotAngle,0,spot->ang.y);
	dialog_set_combo(dialog_spot_setting_wind,kSpotSkill,0,spot->skill);
	dialog_set_combo(dialog_spot_setting_wind,kSpotSpawn,0,spot->spawn);
	
	dialog_special_combo_fill_model(dialog_spot_setting_wind,kSpotDisplayModel,0,spot->display_model);
	
	dialog_spot_setting_break_params(spot->params);
	
	dialog_set_focus(dialog_spot_setting_wind,kSpotName,0);
	
		// show window
	
	ShowWindow(dialog_spot_setting_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_spot_setting_event_proc);
	InstallWindowEventHandler(dialog_spot_setting_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_spot_setting_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_spot_setting_wind);
	
		// dialog to data
		
	if (!dialog_spot_setting_cancel) {
		dialog_get_text(dialog_spot_setting_wind,kSpotName,0,spot->name,name_str_len);
		dialog_get_text(dialog_spot_setting_wind,kSpotType,0,spot->type,name_str_len);
		dialog_special_combo_get_script(dialog_spot_setting_wind,kSpotScript,0,spot->script,name_str_len);
		spot->ang.y=dialog_get_float(dialog_spot_setting_wind,kSpotAngle,0);
		spot->skill=dialog_get_combo(dialog_spot_setting_wind,kSpotSkill,0);
		spot->spawn=dialog_get_combo(dialog_spot_setting_wind,kSpotSpawn,0);
		
		dialog_special_combo_get_model(dialog_spot_setting_wind,kSpotDisplayModel,0,spot->display_model,name_str_len);
		
		dialog_spot_setting_combine_params(spot->params);
	
		main_wind_draw();
	}

		// close window
		
	DisposeWindow(dialog_spot_setting_wind);
	
		// reset any models
		
	walk_view_models_reset();
	main_wind_draw();
	
	return(!dialog_spot_setting_cancel);
}

