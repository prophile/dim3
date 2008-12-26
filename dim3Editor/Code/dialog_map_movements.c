/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Movements Dialog

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

extern map_type				map;

#define kMapMovementList					FOUR_CHAR_CODE('list')
#define kMapMovementListNameColumn			FOUR_CHAR_CODE('name')
#define kMapMovementListGroupColumn			FOUR_CHAR_CODE('grop')
#define kMapMovementListTimeColumn			FOUR_CHAR_CODE('time')
#define kMapMovementListMoveColumn			FOUR_CHAR_CODE('move')
#define kMapMovementListRotateColumn		FOUR_CHAR_CODE('rott')
#define kMapMovementListSoundColumn			FOUR_CHAR_CODE('sond')

#define kMapMovementAddMovementButton		FOUR_CHAR_CODE('amov')
#define kMapMovementAddMovementMoveButton	FOUR_CHAR_CODE('ammv')
#define kMapMovementDeleteButton			FOUR_CHAR_CODE('dele')

int							dialog_map_movements_current_idx,
							dialog_map_movements_move_current_idx;
bool						dialog_map_movements_ignore_deselect;
WindowRef					dialog_map_movements_wind;

/* =======================================================

      Reset List/Buttons
      
======================================================= */

ControlRef map_movements_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMapMovementList;
	ctrl_id.id=0;
	GetControlByID(dialog_map_movements_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

void map_movements_reset_list(int movement_idx,int move_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_map_movements_current_idx=movement_idx;
	dialog_map_movements_move_current_idx=move_idx;
	
	ctrl=map_movements_list_get_ctrl();

		// delete old items
		
	dialog_map_movements_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_map_movements_ignore_deselect=FALSE;
	
		// add items
	
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.nmovement,NULL,kDataBrowserItemNoProperty);
	
		// select items
		
	if (dialog_map_movements_current_idx==-1) return;
		
	itemID=dialog_map_movements_current_idx+1;
	OpenDataBrowserContainer(ctrl,itemID);
		
	if (dialog_map_movements_move_current_idx!=-1) {
		itemID=(1000*(dialog_map_movements_current_idx+1))+dialog_map_movements_move_current_idx;
	}
	
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
}

void map_movements_add_move(DataBrowserItemID itemID)
{
	int					n,idx;
	movement_type		*movement;
	DataBrowserItemID	items[max_movement_move];
	ControlRef			ctrl;
	
	idx=itemID-1;
	movement=&map.movements[idx];
	
	for (n=0;n!=movement->nmove;n++) {
		items[n]=((idx+1)*1000)+n;
	}
	
	ctrl=map_movements_list_get_ctrl();
	AddDataBrowserItems(ctrl,itemID,movement->nmove,items,kDataBrowserItemNoProperty);
}

void map_movement_reset_buttons(void)
{
	ControlRef		add_movement_ctrl,add_move_ctrl,delete_ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMapMovementAddMovementButton;
	ctrl_id.id=0;
	GetControlByID(dialog_map_movements_wind,&ctrl_id,&add_movement_ctrl);
	
	ctrl_id.signature=kMapMovementAddMovementMoveButton;
	ctrl_id.id=0;
	GetControlByID(dialog_map_movements_wind,&ctrl_id,&add_move_ctrl);
	
	ctrl_id.signature=kMapMovementDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_map_movements_wind,&ctrl_id,&delete_ctrl);
	
	if (map.nmovement>=max_movement) {
		DisableControl(add_movement_ctrl);
	}
	else {
		EnableControl(add_movement_ctrl);
	}
	
	if ((dialog_map_movements_current_idx==-1) && (dialog_map_movements_move_current_idx==-1)) {
		DisableControl(add_move_ctrl);
		DisableControl(delete_ctrl);
	}
	else {
		EnableControl(add_move_ctrl);
		EnableControl(delete_ctrl);
	}
}

/* =======================================================

      Map Movements Event Handlers
      
======================================================= */

static pascal OSStatus map_movements_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int					idx;
	movement_type		*movement;
	movement_move_type	*move;
	HICommand			cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kMapMovementAddMovementButton:
					if (map.ngroup==0) {
						StandardAlert(kAlertCautionAlert,"\pCan Not Create Movement","\pYou need at least one segment group to create a movement.",NULL,NULL);
						return(noErr);
					}
					
					idx=map_movement_add(&map);
					if (idx==-1) return(noErr);
					
					movement=&map.movements[idx];
					
					if (!dialog_movement_settings_run(movement)) {
						map_movement_delete(&map,idx);
						return(noErr);
					}
					
					map_movements_reset_list(idx,-1);
					Draw1Control(map_movements_list_get_ctrl());
					map_movement_reset_buttons();

					return(noErr);
					
				case kMapMovementAddMovementMoveButton:
					idx=map_movement_move_add(&map,dialog_map_movements_current_idx);
					if (idx==-1) return(noErr);
					
					movement=&map.movements[dialog_map_movements_current_idx];
					move=&movement->moves[idx];
					
					if (!dialog_movement_move_settings_run(move)) {
						map_movement_move_delete(&map,dialog_map_movements_current_idx,idx);
						return(noErr);
					}
					
					map_movements_reset_list(dialog_map_movements_current_idx,idx);
					Draw1Control(map_movements_list_get_ctrl());
					map_movement_reset_buttons();

					return(noErr);
					
				case kMapMovementDeleteButton:
					if (dialog_map_movements_current_idx==-1) return(noErr);
					if (dialog_map_movements_move_current_idx==-1) {
						map_movement_delete(&map,dialog_map_movements_current_idx);
					}
					else {
						map_movement_move_delete(&map,dialog_map_movements_current_idx,dialog_map_movements_move_current_idx);
					}
					
					map_movements_reset_list(-1,-1);
					Draw1Control(map_movements_list_get_ctrl());
					map_movement_reset_buttons();
					
					return(noErr);
				
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_movements_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Movement List Event Handlers
      
======================================================= */

static pascal OSStatus map_movements_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int					movement_idx,move_idx;
	char				str[256];
	movement_move_type	*move;
	CFStringRef			cfstr;

	switch (property) {
		
		case kMapMovementListNameColumn:
			if (itemID>=1000) return(noErr);
			movement_idx=itemID-1;
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,map.movements[movement_idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMapMovementListGroupColumn:
			if (itemID>=1000) return(noErr);
			movement_idx=itemID-1;
			
			strcpy(str,map.groups[map.movements[movement_idx].group_idx].name);
			if (map.movements[movement_idx].reverse_group_idx!=-1) {
				strcat(str,"/");
				strcat(str,map.groups[map.movements[movement_idx].reverse_group_idx].name);
			}
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMapMovementListTimeColumn:
			if (itemID<1000) return(noErr);
			
			movement_idx=(itemID/1000)-1;
			move_idx=itemID%1000;
			
			move=&map.movements[movement_idx].moves[move_idx];
			sprintf(str,"%d msec",move->msec);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMapMovementListMoveColumn:
			if (itemID<1000) return(noErr);
			
			movement_idx=(itemID/1000)-1;
			move_idx=itemID%1000;
			
			move=&map.movements[movement_idx].moves[move_idx];
			sprintf(str,"%d,%d,%d",move->mov.x,move->mov.y,move->mov.z);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMapMovementListRotateColumn:
			if (itemID<1000) return(noErr);
			
			movement_idx=(itemID/1000)-1;
			move_idx=itemID%1000;
			
			move=&map.movements[movement_idx].moves[move_idx];
			sprintf(str,"%.2f,%.2f,%.2f",move->rot.x,move->rot.y,move->rot.z);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMapMovementListSoundColumn:
			if (itemID<1000) return(noErr);
			
			movement_idx=(itemID/1000)-1;
			move_idx=itemID%1000;
			
			move=&map.movements[movement_idx].moves[move_idx];
			if (move->sound_name[0]==0x0) return(noErr);
			sprintf(str,"%s @ %.2f",move->sound_name,move->sound_pitch);
				
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kDataBrowserItemIsContainerProperty:
			SetDataBrowserItemDataBooleanValue(itemData,(itemID<1000));
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void map_movements_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				movement_idx,move_idx;
	UInt32			count;
	
	switch (message) {

		case kDataBrowserItemDoubleClicked:
			if (itemID<1000) {
				movement_idx=itemID-1;
				if (dialog_movement_settings_run(&map.movements[movement_idx])) {
					UpdateDataBrowserItems(ctrl,kDataBrowserNoItem,1,&itemID,kDataBrowserItemNoProperty,kDataBrowserNoItem);
					Draw1Control(ctrl);
				}
				break;
			}
			
			movement_idx=(itemID/1000)-1;
			move_idx=itemID%1000;
			
			if (dialog_movement_move_settings_run(&map.movements[movement_idx].moves[move_idx])) {
				UpdateDataBrowserItems(ctrl,(movement_idx+1),1,&itemID,kDataBrowserItemNoProperty,kDataBrowserNoItem);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			if (itemID<1000) {
				dialog_map_movements_current_idx=itemID-1;
				dialog_map_movements_move_current_idx=-1;
			}
			else {
				dialog_map_movements_current_idx=(itemID/1000)-1;
				dialog_map_movements_move_current_idx=itemID%1000;
			}
			map_movement_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_map_movements_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_map_movements_current_idx=-1;
			dialog_map_movements_move_current_idx=-1;
			map_movement_reset_buttons();
			break;

		case kDataBrowserContainerOpened:
			if (itemID<1000) map_movements_add_move(itemID);
			break;
			
	}
}

/* =======================================================

      Run Map Movements
      
======================================================= */

bool dialog_map_movements_run(void)
{
	ControlRef						ctrl;
	DataBrowserCallbacks			dbcall;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_map_movements_wind,"MapMovements");
	
		// setup the list
		
	ctrl=map_movements_list_get_ctrl();
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&map_movements_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&map_movements_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);
	
	SetDataBrowserListViewDisclosureColumn(ctrl,kMapMovementListNameColumn,FALSE);
	
	map_movements_reset_list(-1,-1);
	map_movement_reset_buttons();

		// show window
	
	ShowWindow(dialog_map_movements_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_movements_event_proc);
	InstallWindowEventHandler(dialog_map_movements_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_map_movements_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_map_movements_wind);
	
	return(TRUE);
}



