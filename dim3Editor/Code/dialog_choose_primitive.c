/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Choose Primitive Dialog

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

#include "dialog.h"

#define kPrimitiveList					FOUR_CHAR_CODE('list')
#define kPrimitiveListNameColumn		FOUR_CHAR_CODE('pnme')
#define kPrimitiveListTypeColumn		FOUR_CHAR_CODE('ptyp')

extern map_type			primitive_map_editor,primitive_map_data;

int						dialog_choose_primitive_count,dialog_choose_primitive_idx;
char					*dialog_choose_primitive_data;
bool					dialog_choose_primitive_cancel;
WindowRef				dialog_choose_primitive_wind;

/* =======================================================

      Choose Primitive Event Handlers
      
======================================================= */

static pascal OSStatus choose_primitive_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOpen:
					QuitAppModalLoopForWindow(dialog_choose_primitive_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_choose_primitive_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_choose_primitive_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

void choose_primitive_enable_open_button(bool enable)
{
	ControlRef						ctrl;
	ControlID						ctrl_id;

	ctrl_id.signature=FOUR_CHAR_CODE('d3ET');
	ctrl_id.id=1;
	GetControlByID(dialog_choose_primitive_wind,&ctrl_id,&ctrl);
	
	if (enable) {
		EnableControl(ctrl);
	}
	else {
		DisableControl(ctrl);
	}
}

/* =======================================================

      Primitive List Event Handlers
      
======================================================= */

static pascal OSStatus choose_primitive_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	char			txt[name_str_len];
	CFStringRef		cfstr;
	
	switch (property) {
		
		case kPrimitiveListNameColumn:
			idx=itemID-1;
			strncpy(txt,(dialog_choose_primitive_data+(name_str_len*idx)),name_str_len);
			txt[name_str_len-1]=0x0;
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kPrimitiveListTypeColumn:
			idx=itemID-1;
			
			if (idx<primitive_map_data.nportal) {
				strcpy(txt,"Custom");
			}
			else {
				strcpy(txt,"Built-In");
			}
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void choose_primitive_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	UInt32		count;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			dialog_choose_primitive_idx=itemID-1;
			QuitAppModalLoopForWindow(dialog_choose_primitive_wind);
			break;

		case kDataBrowserItemSelected:
			dialog_choose_primitive_idx=itemID-1;
			break;
	}

	GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,FALSE,kDataBrowserItemIsSelected,&count);
	choose_primitive_enable_open_button(count!=0);
}

/* =======================================================

      Scan Directory for Files
      
======================================================= */

void choose_primitive_create_list(void)
{
	int				n,nportal,sz;
	char			*c;
	
		// any portals?
		
	dialog_choose_primitive_count=0;
	dialog_choose_primitive_data=NULL;
	
	nportal=primitive_map_editor.nportal+primitive_map_data.nportal;
	if (nportal==0) return;
	
		// create memory for primitive list
		
	sz=nportal*name_str_len;
	
	dialog_choose_primitive_data=valloc(sz);
	memset(dialog_choose_primitive_data,0x0,sz);
	
	dialog_choose_primitive_count=nportal;
	
		// load the primitive list
		
	c=dialog_choose_primitive_data;
	
	for (n=0;n!=primitive_map_data.nportal;n++) {
		strcpy(c,primitive_map_data.portals[n].name);
		c+=name_str_len;
	}
	
	for (n=0;n!=primitive_map_editor.nportal;n++) {
		strcpy(c,primitive_map_editor.portals[n].name);
		c+=name_str_len;
	}
}

void choose_primitive_close_list(void)
{
	if (dialog_choose_primitive_data!=NULL) free(dialog_choose_primitive_data);
}

/* =======================================================

      Run Choose Primitive
      
======================================================= */

bool dialog_choose_primitive_run(int *portal_idx,bool *in_editor)
{
	ControlRef						ctrl;
	ControlID						ctrl_id;
	DataBrowserCallbacks			dbcall;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_choose_primitive_wind,"ChoosePrimitive");
	
		// disable open
		
	choose_primitive_enable_open_button(false);
		
		// show window
	
	ShowWindow(dialog_choose_primitive_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(choose_primitive_event_proc);
	InstallWindowEventHandler(dialog_choose_primitive_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// create primitive list
		
	choose_primitive_create_list();
	
		// setup the list
		
	ctrl_id.signature=FOUR_CHAR_CODE('d3ET');
	ctrl_id.id=0;
	GetControlByID(dialog_choose_primitive_wind,&ctrl_id,&ctrl);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&choose_primitive_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&choose_primitive_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);
	
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,dialog_choose_primitive_count,NULL,kDataBrowserItemNoProperty);
	Draw1Control(ctrl);
	
		// modal window
		
	dialog_choose_primitive_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_choose_primitive_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_choose_primitive_wind);
	
		// clear up memory
		
	choose_primitive_close_list();
	
		// cancelled?
		
	if (dialog_choose_primitive_cancel) return(FALSE);
	
		// which list?
		
	if (dialog_choose_primitive_idx<primitive_map_data.nportal) {
		*in_editor=FALSE;
		*portal_idx=dialog_choose_primitive_idx;
	}
	else {
		*in_editor=TRUE;
		*portal_idx=dialog_choose_primitive_idx-primitive_map_data.nportal;
	}
		
	return(TRUE);
}

