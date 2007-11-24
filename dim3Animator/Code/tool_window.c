/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Tool Window

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "window.h"
#include "model.h"

#define maxtool			10

WindowRef				tool_wind;
EventHandlerRef			tool_wind_event;
EventHandlerUPP			tool_wind_upp;
ControlRef				tool_ctrl[maxtool];

extern int				draw_type,cur_mesh,cur_pose,drag_bone_mode;
extern bool				fileopen,model_show_first_mesh,model_box_on,model_bump_on,model_normal_on;
extern model_type		model;

char					tool_tooltip_str[maxtool][64]={
								"Show Textured Model","Show Mesh Only","Show Bones Only","Show Mesh and Bones Only",
								"Always Show First Mesh","Show Model, Shadow, and Hit Boxes","Show Bump-Mapping","Show Normals",
								"Rotate Bones Mode","Stretch Bones Mode"};

/* =======================================================

      Tool Window Events
      
======================================================= */

OSStatus app_event_tool_wind(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	int				i,k;
	ControlRef		ctrl;

	GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
		
		// buttons
		
	k=-1;
	for (i=0;i!=maxtool;i++) {
		if (ctrl==tool_ctrl[i]) {
			k=i;
			break;
		}
	}
	
	switch (k) {
		case 0:
			SetControlValue(tool_ctrl[0],1);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			draw_type=dt_model;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
		case 1:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],1);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			draw_type=dt_mesh;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
		case 2:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],1);
			SetControlValue(tool_ctrl[3],0);
			draw_type=dt_bones;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
		case 3:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],1);
			draw_type=dt_mesh_bones;
			break;
		case 4:
			model_show_first_mesh=!model_show_first_mesh;
			break;
		case 5:
			model_box_on=!model_box_on;
			break;
		case 6:
			model_bump_on=!model_bump_on;
			break;
		case 7:
			model_normal_on=!model_normal_on;
			break;
		case 8:
			drag_bone_mode=drag_bone_mode_rotate;
			SetControlValue(tool_ctrl[8],1);
			SetControlValue(tool_ctrl[9],0);
			break;
		case 9:
			drag_bone_mode=drag_bone_mode_stretch;
			SetControlValue(tool_ctrl[8],0);
			SetControlValue(tool_ctrl[9],1);
			break;
	}
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
			
	return(noErr);
}

/* =======================================================

      Toolbar Window
      
======================================================= */

void open_tool_wind(void)
{
	return;
	
	int							t;
	Rect						wbox,box;
	ControlButtonContentInfo	icon_info;
	HMHelpContentRec			tag;
	EventTypeSpec				wind_events[]={{kEventClassControl,kEventControlHit}};

    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
	
	SetRect(&box,wbox.left,wbox.top,wbox.right,(wbox.top+28));
	CreateNewWindow(kFloatingWindowClass,kWindowStandardFloatingAttributes|kWindowStandardHandlerAttribute,&box,&tool_wind);
	ChangeWindowAttributes(tool_wind,kWindowNoConstrainAttribute,kWindowCloseBoxAttribute|kWindowFullZoomAttribute|kWindowCollapseBoxAttribute|kWindowResizableAttribute);
	
    SetRect(&box,2,2,26,26);
   
	for (t=0;t!=maxtool;t++) {
	
			// create button
			
		icon_info.contentType=kControlContentCIconRes;
		icon_info.u.resID=500+t;
		CreateBevelButtonControl(tool_wind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorToggles,&icon_info,0,0,0,&tool_ctrl[t]);
	
			// create tooltip
			
		tag.version=kMacHelpVersion;
		tag.tagSide=kHMDefaultSide;
		SetRect(&tag.absHotRect,0,0,0,0);
		tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
		tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,tool_tooltip_str[t],kCFStringEncodingMacRoman);
		tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
		HMSetControlHelpContent(tool_ctrl[t],&tag);

			// next button position
			
		OffsetRect(&box,24,0);
		if ((t==3) || (t==7)) OffsetRect(&box,5,0);
	}
	
		// tool window events
		
	tool_wind_upp=NewEventHandlerUPP(app_event_tool_wind);
	InstallEventHandler(GetWindowEventTarget(tool_wind),tool_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&tool_wind_event);

	ShowWindow(tool_wind);
}

void close_tool_wind(void)
{
	return;
	
	int			t;
	
	for (t=0;t!=maxtool;t++) {
		DisposeControl(tool_ctrl[t]);
	}
	
	RemoveEventHandler(tool_wind_event);
	DisposeEventHandlerUPP(tool_wind_upp);
	
	DisposeWindow(tool_wind);
}

/* =======================================================

      Toolbar Setup
      
======================================================= */

void tool_wind_reset(void)
{
	return;
	
	SetControlValue(tool_ctrl[0],(draw_type==dt_model)?1:0);
	SetControlValue(tool_ctrl[1],(draw_type==dt_mesh)?1:0);
	SetControlValue(tool_ctrl[2],(draw_type==dt_bones)?1:0);
	SetControlValue(tool_ctrl[3],(draw_type==dt_mesh_bones)?1:0);
    
	SetControlValue(tool_ctrl[4],model_show_first_mesh?1:0);
	SetControlValue(tool_ctrl[5],model_box_on?1:0);
	SetControlValue(tool_ctrl[6],model_bump_on?1:0);
	SetControlValue(tool_ctrl[7],model_normal_on?1:0);
	
	SetControlValue(tool_ctrl[8],(drag_bone_mode==drag_bone_mode_rotate)?1:0);
	SetControlValue(tool_ctrl[9],(drag_bone_mode==drag_bone_mode_stretch)?1:0);
}

void tool_wind_switch_mesh_mode(void)
{
	return;
	
	draw_type++;
	if (draw_type>dt_mesh_bones) draw_type=dt_model;
	
	SetControlValue(tool_ctrl[0],(draw_type==dt_model)?1:0);
	SetControlValue(tool_ctrl[1],(draw_type==dt_mesh)?1:0);
	SetControlValue(tool_ctrl[2],(draw_type==dt_bones)?1:0);
	SetControlValue(tool_ctrl[3],(draw_type==dt_mesh_bones)?1:0);
}
