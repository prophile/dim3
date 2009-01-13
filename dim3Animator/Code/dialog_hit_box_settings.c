/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Hit Box Setting Dialog

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

#include "dialog.h"

extern int					cur_mesh,cur_pose;
extern model_type			model;

#define kHitBoxList							FOUR_CHAR_CODE('list')
#define kHitBoxListNameColumn				FOUR_CHAR_CODE('hnme')

#define kHitBoxAdd							FOUR_CHAR_CODE('addp')
#define kHitBoxSub							FOUR_CHAR_CODE('subp')

#define kHitBoxName							FOUR_CHAR_CODE('name')
#define kHitBoxSizeX						FOUR_CHAR_CODE('sizx')
#define kHitBoxSizeY						FOUR_CHAR_CODE('sizy')
#define kHitBoxSizeZ						FOUR_CHAR_CODE('sizz')
#define kHitBoxOffsetX						FOUR_CHAR_CODE('offx')
#define kHitBoxOffsetY						FOUR_CHAR_CODE('offy')
#define kHitBoxOffsetZ						FOUR_CHAR_CODE('offz')

int							dialog_hit_box_idx;
bool						dialog_hit_box_change_ok;
ControlRef					dialog_hit_box_list;
WindowRef					dialog_hit_box_settings_wind;

/* =======================================================

      Load and Save Hit Box Data
      
======================================================= */

void dialog_hit_box_settings_load(void)
{
	if (dialog_hit_box_idx==-1) {
		dialog_set_text(dialog_hit_box_settings_wind,kHitBoxName,0,"");
		dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeX,0,0);
		dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeY,0,0);
		dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeZ,0,0);
		dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetX,0,0);
		dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetY,0,0);
		dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetZ,0,0);
		return;
	}
	
	dialog_set_text(dialog_hit_box_settings_wind,kHitBoxName,0,model.hit_boxes[dialog_hit_box_idx].name);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeX,0,model.hit_boxes[dialog_hit_box_idx].box.size.x);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeY,0,model.hit_boxes[dialog_hit_box_idx].box.size.y);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxSizeZ,0,model.hit_boxes[dialog_hit_box_idx].box.size.z);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetX,0,model.hit_boxes[dialog_hit_box_idx].box.offset.x);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetY,0,model.hit_boxes[dialog_hit_box_idx].box.offset.y);
	dialog_set_int(dialog_hit_box_settings_wind,kHitBoxOffsetZ,0,model.hit_boxes[dialog_hit_box_idx].box.offset.z);
	
	DrawControls(dialog_hit_box_settings_wind);
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
}

void dialog_hit_box_settings_save(void)
{
	if (dialog_hit_box_idx==-1) return;
	
	dialog_get_text(dialog_hit_box_settings_wind,kHitBoxName,0,model.hit_boxes[dialog_hit_box_idx].name,name_str_len);
	model.hit_boxes[dialog_hit_box_idx].box.size.x=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxSizeX,0);
	model.hit_boxes[dialog_hit_box_idx].box.size.y=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxSizeY,0);
	model.hit_boxes[dialog_hit_box_idx].box.size.z=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxSizeZ,0);
	model.hit_boxes[dialog_hit_box_idx].box.offset.x=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxOffsetX,0);
	model.hit_boxes[dialog_hit_box_idx].box.offset.y=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxOffsetY,0);
	model.hit_boxes[dialog_hit_box_idx].box.offset.z=dialog_get_int(dialog_hit_box_settings_wind,kHitBoxOffsetZ,0);
}

/* =======================================================

      Hit Box Setting Event Handlers
      
======================================================= */

void dialog_hit_box_list_reset(void)
{
	DataBrowserItemID				itemID;

	RemoveDataBrowserItems(dialog_hit_box_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(dialog_hit_box_list,kDataBrowserNoItem,model.nhit_box,NULL,kDataBrowserItemNoProperty);

	itemID=dialog_hit_box_idx+1;
	SetDataBrowserSelectedItems(dialog_hit_box_list,1,&itemID,kDataBrowserItemsAssign);

	Draw1Control(dialog_hit_box_list);
}

static pascal OSStatus hit_box_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHitBoxAdd:
					dialog_hit_box_idx=model_hit_box_add(&model);
					dialog_hit_box_settings_load();
					dialog_hit_box_list_reset();
					return(noErr);
					
				case kHitBoxSub:
					if (model.nhit_box==0) return(noErr);
					model_hit_box_delete(&model,dialog_hit_box_idx);
					dialog_hit_box_idx=0;
					if (dialog_hit_box_idx>=model.nhit_box) dialog_hit_box_idx=-1;
					dialog_hit_box_settings_load();
					dialog_hit_box_list_reset();
					return(noErr);
				
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_hit_box_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Hit Box List Event Handlers
      
======================================================= */

static pascal OSStatus hit_box_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	CFStringRef		cfstr;
	
	switch (property) {
	
		case kHitBoxListNameColumn:
			idx=itemID-1;
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.hit_boxes[idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);

	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void hit_box_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	switch (message) {

		case kDataBrowserItemSelected:
			if ((!dialog_hit_box_change_ok) || (itemID==0)) return;
			
			dialog_hit_box_settings_save();
			dialog_hit_box_idx=itemID-1;
			dialog_hit_box_settings_load();
			
			break;

	}
}

/* =======================================================

      Run Hit Box Setting
      
======================================================= */

bool dialog_hit_box_settings_run(void)
{
	ControlID						ctrl_id;
	DataBrowserItemID				itemID;
	DataBrowserCallbacks			dbcall;
	DataBrowserItemDataUPP			hit_box_list_item_upp;
	DataBrowserItemNotificationUPP	hit_box_list_notify_upp;
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_hit_box_settings_wind,"HitBoxSettings");
	
		// setup hit box list
		
	ctrl_id.signature=kHitBoxList;
	ctrl_id.id=0;
	GetControlByID(dialog_hit_box_settings_wind,&ctrl_id,&dialog_hit_box_list);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	hit_box_list_item_upp=NewDataBrowserItemDataUPP(&hit_box_list_item_proc);
	dbcall.u.v1.itemDataCallback=hit_box_list_item_upp;

	hit_box_list_notify_upp=NewDataBrowserItemNotificationUPP(&hit_box_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=hit_box_list_notify_upp;
	
	SetDataBrowserCallbacks(dialog_hit_box_list,&dbcall);
	
	AddDataBrowserItems(dialog_hit_box_list,kDataBrowserNoItem,model.nhit_box,NULL,kDataBrowserItemNoProperty);
	
	dialog_hit_box_change_ok=FALSE;
	
	itemID=1;
	SetDataBrowserSelectedItems(dialog_hit_box_list,1,&itemID,kDataBrowserItemsAssign);
	
	dialog_hit_box_change_ok=TRUE;

		// set controls

	dialog_hit_box_idx=-1;
	if (model.nhit_box!=0) dialog_hit_box_idx=0;
	
	dialog_hit_box_settings_load();
	
		// show window
	
	ShowWindow(dialog_hit_box_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(hit_box_settings_event_proc);
	InstallWindowEventHandler(dialog_hit_box_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_hit_box_settings_wind);
	
		// dialog to data
		
	dialog_hit_box_settings_save();

		// close window
		
	DisposeWindow(dialog_hit_box_settings_wind);
	
	return(TRUE);
}

