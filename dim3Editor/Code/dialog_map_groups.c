/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Groups Dialog

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

extern map_type				map;

#define kMapGroupList						FOUR_CHAR_CODE('list')
#define kMapGroupListNameColumn				FOUR_CHAR_CODE('name')
#define kMapGroupListCountColumn			FOUR_CHAR_CODE('cont')

#define kMapGroupAddButton					FOUR_CHAR_CODE('agrp')
#define kMapGroupClearButton				FOUR_CHAR_CODE('cgrp')
#define kMapGroupDeleteButton				FOUR_CHAR_CODE('dgrp')

int							dialog_map_groups_current_idx,
							dialog_map_groups_move_current_idx;
bool						dialog_map_groups_ignore_deselect;
WindowRef					dialog_map_groups_wind;

/* =======================================================

      Reset List/Buttons
      
======================================================= */

ControlRef map_groups_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMapGroupList;
	ctrl_id.id=0;
	GetControlByID(dialog_map_groups_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

void map_groups_reset_list(int group_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_map_groups_current_idx=group_idx;
	
	ctrl=map_groups_list_get_ctrl();

		// delete old items
		
	dialog_map_groups_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_map_groups_ignore_deselect=FALSE;
	
		// add items
	
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.ngroup,NULL,kDataBrowserItemNoProperty);
	
		// select items
		
	if (dialog_map_groups_current_idx==-1) return;
		
	itemID=dialog_map_groups_current_idx+1;
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
}

void map_group_reset_buttons(void)
{
	ControlRef		add_ctrl,clear_ctrl,delete_ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMapGroupAddButton;
	ctrl_id.id=0;
	GetControlByID(dialog_map_groups_wind,&ctrl_id,&add_ctrl);
	
	ctrl_id.signature=kMapGroupClearButton;
	ctrl_id.id=0;
	GetControlByID(dialog_map_groups_wind,&ctrl_id,&clear_ctrl);
	
	ctrl_id.signature=kMapGroupDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_map_groups_wind,&ctrl_id,&delete_ctrl);
	
	if (map.ngroup>=max_group) {
		DisableControl(add_ctrl);
	}
	else {
		EnableControl(add_ctrl);
	}
	
	if (dialog_map_groups_current_idx==-1) {
		DisableControl(clear_ctrl);
		DisableControl(delete_ctrl);
	}
	else {
		EnableControl(clear_ctrl);
		EnableControl(delete_ctrl);
	}
}

/* =======================================================

      Map Groups Event Handlers
      
======================================================= */

static pascal OSStatus map_groups_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int					n;
	group_type			*group;
	HICommand			cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kMapGroupAddButton:
					if (map.ngroup>=max_group) {
						StandardAlert(kAlertCautionAlert,"\pCan Not Create Group","\pYou've reached the maximum number of groups.",NULL,NULL);
						return(noErr);
					}
	
					group=&map.groups[map.ngroup];
					group->name[0]=0x0;
					
					if (dialog_group_settings_run(group)) {
						map.ngroup++;
					}
		
					map_groups_reset_list(dialog_map_groups_current_idx);
					Draw1Control(map_groups_list_get_ctrl());
					map_group_reset_buttons();
					
					main_wind_tool_fill_group_combo();
					return(noErr);
					
				case kMapGroupClearButton:
					group_clear(dialog_map_groups_current_idx,FALSE);

					map_groups_reset_list(dialog_map_groups_current_idx);
					Draw1Control(map_groups_list_get_ctrl());
					map_group_reset_buttons();
					
					main_wind_tool_fill_group_combo();
					return(noErr);
					
				case kMapGroupDeleteButton:
				
						// can't delete if attached to movement
						
					for (n=0;n!=map.nmovement;n++) {
						if ((map.movements[n].group_idx==dialog_map_groups_current_idx) || (map.movements[n].reverse_group_idx==dialog_map_groups_current_idx)) {
							StandardAlert(kAlertCautionAlert,"\pCan Not Delete Group","\pThis group is used in a group movement, you will need to delete or edit the group movement first.",NULL,NULL);
							return(noErr);
						}
					}
						
						// clear before deleting
						
					group_clear(dialog_map_groups_current_idx,TRUE);

						// delete
						
					for (n=dialog_map_groups_current_idx;n<map.ngroup;n++) {
						map.groups[n]=map.groups[n+1];
					}
					map.ngroup--;
					
					map_groups_reset_list(-1);
					Draw1Control(map_groups_list_get_ctrl());
					map_group_reset_buttons();
					
					main_wind_tool_fill_group_combo();
					return(noErr);
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_groups_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Group List Event Handlers
      
======================================================= */

static pascal OSStatus map_groups_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int					group_idx;
	char				str[256];
	CFStringRef			cfstr;

	switch (property) {
		
		case kMapGroupListNameColumn:
			group_idx=itemID-1;
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,map.groups[group_idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMapGroupListCountColumn:
			group_idx=itemID-1;

			sprintf(str,"%d",group_count(group_idx));
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
									
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void map_groups_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				group_idx;
	UInt32			count;
	
	switch (message) {

		case kDataBrowserItemDoubleClicked:
			group_idx=itemID-1;
			
			if (dialog_group_settings_run(&map.groups[group_idx])) {
				UpdateDataBrowserItems(ctrl,kDataBrowserNoItem,1,&itemID,kDataBrowserItemNoProperty,kDataBrowserNoItem);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			dialog_map_groups_current_idx=itemID-1;
			map_group_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_map_groups_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_map_groups_current_idx=-1;
			map_group_reset_buttons();
			break;
			
	}
}

/* =======================================================

      Run Map Groups
      
======================================================= */

bool dialog_map_groups_run(void)
{
	ControlRef						ctrl;
	DataBrowserCallbacks			dbcall;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_groups_wind,"MapGroups");
	
		// setup the list
		
	ctrl=map_groups_list_get_ctrl();
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&map_groups_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&map_groups_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);
	
	SetDataBrowserListViewDisclosureColumn(ctrl,kMapGroupListNameColumn,FALSE);
	
	map_groups_reset_list(-1);
	map_group_reset_buttons();

		// show window
	
	ShowWindow(dialog_map_groups_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_groups_event_proc);
	InstallWindowEventHandler(dialog_map_groups_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_map_groups_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_map_groups_wind);
	
	return(TRUE);
}



