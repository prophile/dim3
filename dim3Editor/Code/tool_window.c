/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Toolbar Window

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
#include "top_view.h"
#include "portal_view.h"
#include "site_path_view.h"

#define maxtool					21
#define group_combo_menu_id		160

extern int				cr,vertex_mode,magnify_factor;
extern bool				dp_primitive,dp_auto_texture,dp_wall,dp_floor,dp_ceiling,dp_liquid,dp_ambient,
						dp_object,dp_lightsoundparticle,dp_node,dp_textured,dp_y_hide;

extern map_type			map;
extern setup_type		setup;

WindowRef				toolwind;
EventHandlerRef			tool_wind_event;
EventHandlerUPP			tool_wind_upp;
ControlRef				tool_ctrl[maxtool],group_combo,magnify_icon,magnify_slider;
ControlActionUPP		magnify_proc;
MenuRef					group_menu;
Rect					group_box;

char					tool_tooltip_str[maxtool][64]={
								"Drag Points Freely","Drag Touching Points Together",
								"Snap To Nearby Points While Dragging",
								"Texture Entire Primitive or Segment Only","Auto-Texture","Rotate Editor Panes",
								"Show/Hide Walls","Show/Hide Floors","Show/Hide Ceilings","Show/Hide Liquids",
								"Show/Hide Ambients","Show/Hide Script Spots/Scenery","Show/Hide Light/Sound/Particles",
								"Show/Hide Nodes","Textured/Simple 3D Drawing","Show Only Segments With Nearby Ys",
								"Subtract Polygon Point","Add Polygon Point","Tesselate Polygon",
								"Edit Map Script","Run Map In Engine",
														};

/* =======================================================

      Tool Window Events
      
======================================================= */

OSStatus tool_wind_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	int				i,k;
	ControlRef		ctrl;

	GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
	
		// group combo
		
	if (ctrl==group_combo) {
		k=GetControl32BitValue(ctrl);
		
			// no group
			
		if (k==1) {
			group_set(-1);
			return(noErr);
		}
		
			// set a group
			
		group_set(k-3);
		return(noErr);
	}
		
		// buttons
		
	k=-1;
	for ((i=0);(i!=maxtool);i++) {
		if (ctrl==tool_ctrl[i]) {
			k=i;
			break;
		}
	}
	
	if (k!=-1) undo_clear();
	
	switch (k) {
		case 0:
			vertex_mode=vm_none;
            SetControlValue(tool_ctrl[0],1);
            SetControlValue(tool_ctrl[1],0);
            SetControlValue(tool_ctrl[2],0);
			break;
		case 1:
			vertex_mode=vm_lock;
            SetControlValue(tool_ctrl[0],0);
            SetControlValue(tool_ctrl[1],1);
            SetControlValue(tool_ctrl[2],0);
			break;
		case 2:
			vertex_mode=vm_snap;
            SetControlValue(tool_ctrl[0],0);
            SetControlValue(tool_ctrl[1],0);
            SetControlValue(tool_ctrl[2],1);
			break;
		case 3:
			dp_primitive=!dp_primitive;
			break;
		case 4:
			dp_auto_texture=!dp_auto_texture;
			break;
		case 5:
			SetControlValue(tool_ctrl[k],0);			// pop back up
			main_wind_rotate_view();
			main_wind_draw();
			break;
		case 6:
            select_clear();
			dp_wall=!dp_wall;
			break;
		case 7:
            select_clear();
			dp_floor=!dp_floor;
			break;
		case 8:
            select_clear();
			dp_ceiling=!dp_ceiling;
			break;
		case 9:
            select_clear();
			dp_liquid=!dp_liquid;
			break;
		case 10:
            select_clear();
			dp_ambient=!dp_ambient;
			break;
		case 11:
            select_clear();
			dp_object=!dp_object;
			break;
		case 12:
            select_clear();
			dp_lightsoundparticle=!dp_lightsoundparticle;
			break;
		case 13:
            select_clear();
            dp_node=!dp_node;
			break;
		case 14:
			dp_textured=!dp_textured;
			break;
		case 15:
            dp_y_hide=!dp_y_hide;
			break;
		case 16:
			SetControlValue(tool_ctrl[k],0);			// pop back up
			segment_sub_point();
			break;
		case 17:
			SetControlValue(tool_ctrl[k],0);			// pop back up
			segment_add_point();
			break;
		case 18:
			SetControlValue(tool_ctrl[k],0);			// pop back up
			segment_tesselate();
			break;
		case 19:
			SetControlValue(tool_ctrl[k],0);			// pop back up
			launch_map_script_editor();
			break;
		case 20:
			SetControlValue(tool_ctrl[k],0);			// pop back up
			launch_engine();
			break;
			
	}
    
    menu_fix_enable();
    
    main_wind_draw();
			
	return(noErr);
}

/* =======================================================

      Magnify Slider Action
      
======================================================= */

void tool_wind_magnify_action(ControlRef ctrl,ControlPartCode code)
{
	int				s;
	
	s=GetControlValue(ctrl);
	if (s==magnify_factor) return;
	
	magnify_factor=s;
	
	top_view_reset();
	portal_view_reset();
	site_path_view_reset();
	
    main_wind_draw();
}

void tool_wind_magnify_scroll(int delta)
{
	magnify_factor-=delta;
	if (magnify_factor<5) magnify_factor=5;
	if (magnify_factor>210) magnify_factor=210;
	
	SetControlValue(magnify_slider,magnify_factor);

	top_view_reset();
    main_wind_draw();
}

/* =======================================================

      Toolbar Window
      
======================================================= */

void tool_wind_open(void)
{
	int							x,t,rspace,wid;
	Rect						wbox,box;
	ControlButtonContentInfo	icon_info;
	HMHelpContentRec			tag;
	EventTypeSpec				wind_events[]={{kEventClassControl,kEventControlHit}};

    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
	
	SetRect(&box,wbox.left,wbox.top,wbox.right,(wbox.top+28));
	CreateNewWindow(kFloatingWindowClass,kWindowStandardFloatingAttributes|kWindowStandardHandlerAttribute,&box,&toolwind);
	ChangeWindowAttributes(toolwind,kWindowNoConstrainAttribute,kWindowCloseBoxAttribute|kWindowFullZoomAttribute|kWindowCollapseBoxAttribute|kWindowResizableAttribute);
	
	/*
    SetRect(&box,2,2,26,26);
    
	for (t=0;t!=maxtool;t++) {
	
			// create button
			
		icon_info.contentType=kControlContentCIconRes;
		icon_info.u.resID=500+t;
		CreateBevelButtonControl(toolwind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorToggles,&icon_info,0,0,0,&tool_ctrl[t]);
	
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
		if ((t==2) || (t==5) || (t==13) || (t==15) || (t==18)) OffsetRect(&box,5,0);
	}
	
		// remaining space
	
	rspace=wbox.right-(box.left+5);
	
		// group combo
		
	CreateNewMenu(group_combo_menu_id,kMenuAttrExcludesMarkColumn,&group_menu);
	InsertMenu(group_menu,kInsertHierarchicalMenu);
		
	wid=(rspace-5)/2;
	if (wid>200) wid=200;
	
	group_box=box;
	group_box.top++;
	group_box.bottom++;
	group_box.left+=5;
	group_box.right=group_box.left+wid;
	
	CreatePopupButtonControl(toolwind,&group_box,NULL,group_combo_menu_id,FALSE,0,0,0,&group_combo);
	
		// magnify slider

	wid=(rspace-5)/2;
	x=wbox.right-wid;
	
	SetRect(&box,x,5,(x+16),21);
	icon_info.contentType=kControlContentCIconRes;
	icon_info.u.resID=130;
	CreateIconControl(toolwind,&box,&icon_info,TRUE,&magnify_icon);
	
	tag.version=kMacHelpVersion;
	tag.tagSide=kHMDefaultSide;
	SetRect(&tag.absHotRect,0,0,0,0);
	tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
	tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,"Magnify",kCFStringEncodingMacRoman);
	tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
	HMSetControlHelpContent(magnify_icon,&tag);
	
	SetRect(&box,(x+20),5,(x+(wid-6)),25);
	magnify_proc=NewControlActionUPP(tool_wind_magnify_action);
	CreateSliderControl(toolwind,&box,(magnify_factor-1),5,210,kControlSliderDoesNotPoint,0,TRUE,magnify_proc,&magnify_slider);
	*/
		// tool window events
		
	tool_wind_upp=NewEventHandlerUPP(tool_wind_event_callback);
	InstallEventHandler(GetWindowEventTarget(toolwind),tool_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&tool_wind_event);

	ShowWindow(toolwind);
}

void tool_wind_close(void)
{
	int			t;
	
	for ((t=0);(t!=maxtool);t++) {
		DisposeControl(tool_ctrl[t]);
	}
	
	DisposeControl(magnify_icon);
	DisposeControl(magnify_slider);
	DisposeControlActionUPP(magnify_proc);
	
	DisposeControl(group_combo);
	DisposeMenu(group_menu);

	RemoveEventHandler(tool_wind_event);
	DisposeEventHandlerUPP(tool_wind_upp);
	
	DisposeWindow(toolwind);
}

/* =======================================================

      Toolbar Values and States
      
======================================================= */

void tool_wind_reset(void)
{
    int			t;
    
	SetControlValue(tool_ctrl[0],(vertex_mode==vm_none)?1:0);
	SetControlValue(tool_ctrl[1],(vertex_mode==vm_lock)?1:0);
	SetControlValue(tool_ctrl[2],(vertex_mode==vm_snap)?1:0);
	
	SetControlValue(tool_ctrl[3],dp_primitive?1:0);
	SetControlValue(tool_ctrl[4],dp_auto_texture?1:0);
    
	SetControlValue(tool_ctrl[6],dp_wall?1:0);
	SetControlValue(tool_ctrl[7],dp_floor?1:0);
	SetControlValue(tool_ctrl[8],dp_ceiling?1:0);
	SetControlValue(tool_ctrl[9],dp_liquid?1:0);
	SetControlValue(tool_ctrl[10],dp_ambient?1:0);
	SetControlValue(tool_ctrl[11],dp_object?1:0);
	SetControlValue(tool_ctrl[12],dp_lightsoundparticle?1:0);
	SetControlValue(tool_ctrl[13],dp_node?1:0);
	
	SetControlValue(tool_ctrl[14],dp_textured?1:0);
	SetControlValue(tool_ctrl[15],dp_y_hide?1:0);
   
	for ((t=16);(t!=maxtool);t++) {
		SetControlValue(tool_ctrl[t],0);
	}
    
	SetControlValue(magnify_slider,(magnify_factor-1));
}

void tool_wind_fill_group_combo(void)
{
	int					n,group_idx;
	char				str[256];
	CFStringRef			cf_str;
	HMHelpContentRec	tag;
	
		// old settings
		
	group_idx=GetControl32BitValue(group_combo);
	
		// delete old control and menu
		
	DisposeControl(group_combo);
	
	DeleteMenu(160);
	DisposeMenu(group_menu);
	
		// recreate the menu
		
	CreateNewMenu(group_combo_menu_id,0,&group_menu);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"No Group",kCFStringEncodingMacRoman);
	AppendMenuItemTextWithCFString(group_menu,cf_str,0,FOUR_CHAR_CODE('gp01'),NULL);
	CFRelease(cf_str);
	
	AppendMenuItemTextWithCFString(group_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
	for (n=0;n<map.ngroup;n++) {
		sprintf(str,"%s (%d)",map.groups[n].name,group_count(n));
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(group_menu,cf_str,0,FOUR_CHAR_CODE('gp03'),NULL);
		CFRelease(cf_str);
	}
	
	InsertMenu(group_menu,kInsertHierarchicalMenu);
	
		// recreate the contorl
		
	CreatePopupButtonControl(toolwind,&group_box,NULL,group_combo_menu_id,FALSE,0,0,0,&group_combo);
	Draw1Control(group_combo);
	
		// build the help
	
	tag.version=kMacHelpVersion;
	tag.tagSide=kHMDefaultSide;
	SetRect(&tag.absHotRect,0,0,0,0);
	tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
	tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,"Segment Groups",kCFStringEncodingMacRoman);
	tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
	HMSetControlHelpContent(group_combo,&tag);
	
		// reset the control
		
	SetControl32BitValue(group_combo,group_idx);
}

void tool_wind_default(void)
{
    vertex_mode=vm_none;
	
	dp_primitive=TRUE;
	dp_auto_texture=setup.auto_texture;
	
	dp_wall=dp_floor=dp_ceiling=dp_liquid=dp_ambient=dp_object=dp_lightsoundparticle=TRUE;
	dp_node=FALSE;
	dp_textured=TRUE;
	dp_y_hide=FALSE;
    
    tool_wind_reset();
}

void tool_wind_fix_enable(void)
{
	int			n,nsel,type,index,seg_type,group_idx;
	bool		has_fc_seg;
	
		// add-sub-tesselate vertex buttons
		
	has_fc_seg=FALSE;
	nsel=select_count();
	
	for (n=0;n!=nsel;n++) {
		select_get(0,&type,&index);
		if (type==segment_piece) {
			seg_type=map.segments[index].type;
			if ((seg_type==sg_floor) || (seg_type==sg_ceiling)) {
				has_fc_seg=TRUE;
				break;
			}
		}
	}
	
	if (!has_fc_seg) {
		HiliteControl(tool_ctrl[16],255);
		HiliteControl(tool_ctrl[17],255);
		HiliteControl(tool_ctrl[18],255);
	}
	else {
		HiliteControl(tool_ctrl[16],0);
		HiliteControl(tool_ctrl[17],0);
		HiliteControl(tool_ctrl[18],0);
	}
	
		// group combo
		
	group_idx=-1;
	
	if (select_count()==1) {
		select_get(0,&type,&index);
		if ((type==segment_piece) || (type==primitive_piece)) group_idx=map.segments[index].group_idx;
	}
	
	if (group_idx==-1) {
		SetControl32BitValue(group_combo,1);
	}
	else {
		SetControl32BitValue(group_combo,(3+group_idx));
	}
}

void tool_wind_switch_vertex_mode(void)
{
	vertex_mode++;
	if (vertex_mode>vm_snap) vertex_mode=vm_none;

	SetControlValue(tool_ctrl[0],(vertex_mode==vm_none)?1:0);
	SetControlValue(tool_ctrl[1],(vertex_mode==vm_lock)?1:0);
	SetControlValue(tool_ctrl[2],(vertex_mode==vm_snap)?1:0);
}

