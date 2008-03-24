/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Choose Library Object Dialog

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

#define kLibObjectList					FOUR_CHAR_CODE('list')
#define kLibObjectListNameColumn		FOUR_CHAR_CODE('pnme')

int								dialog_choose_lib_object_count,dialog_choose_lib_object_idx;
char							*dialog_choose_lib_object_data;
bool							dialog_choose_lib_object_cancel;
WindowRef						dialog_choose_lib_object_wind;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Choose Primitive Event Handlers
      
======================================================= */

static pascal OSStatus choose_lib_object_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOpen:
					QuitAppModalLoopForWindow(dialog_choose_lib_object_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_choose_lib_object_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_choose_lib_object_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

void choose_lib_object_enable_open_button(bool enable)
{
	ControlRef						ctrl;
	ControlID						ctrl_id;

	ctrl_id.signature=FOUR_CHAR_CODE('d3ET');
	ctrl_id.id=1;
	GetControlByID(dialog_choose_lib_object_wind,&ctrl_id,&ctrl);
	
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

static pascal OSStatus choose_lib_object_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	char			txt[file_str_len];
	CFStringRef		cfstr;
	
	switch (property) {
		
		case kLibObjectListNameColumn:
			idx=itemID-1;
			strncpy(txt,(dialog_choose_lib_object_data+(file_str_len*idx)),file_str_len);
			txt[file_str_len-1]=0x0;
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void choose_lib_object_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	UInt32		count;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			dialog_choose_lib_object_idx=itemID-1;
			QuitAppModalLoopForWindow(dialog_choose_lib_object_wind);
			break;

		case kDataBrowserItemSelected:
			dialog_choose_lib_object_idx=itemID-1;
			break;
	}

	GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,FALSE,kDataBrowserItemIsSelected,&count);
	choose_lib_object_enable_open_button(count!=0);
}

/* =======================================================

      Scan Directory for Files
      
======================================================= */

void choose_lib_object_create_list(void)
{
	int							n,sz;
	file_path_directory_type	*fpd;
	
		// read the directory
		
	fpd=file_paths_read_directory(&file_path_setup,"Library","obj",FALSE);

		// get memory for list
		
	dialog_choose_lib_object_count=fpd->nfile;
	if (dialog_choose_lib_object_count==0) {
		dialog_choose_lib_object_data=NULL;
		file_paths_close_directory(fpd);
		return;
	}
	
	sz=dialog_choose_lib_object_count*file_str_len;
	
	dialog_choose_lib_object_data=valloc(sz);
	memset(dialog_choose_lib_object_data,0x0,sz);
	
	for (n=0;n!=dialog_choose_lib_object_count;n++) {
		strcpy((char*)&dialog_choose_lib_object_data[n*file_str_len],fpd->files[n].file_name);
	}
	
	file_paths_close_directory(fpd);
}

void choose_lib_object_close_list(void)
{
	if (dialog_choose_lib_object_data!=NULL) free(dialog_choose_lib_object_data);
}

/* =======================================================

      Run Choose Primitive
      
======================================================= */

bool dialog_choose_library_object_run(char *name)
{
	ControlRef						ctrl;
	ControlID						ctrl_id;
	DataBrowserCallbacks			dbcall;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_choose_lib_object_wind,"ChooseLibraryObject");
	
		// disable open
		
	choose_lib_object_enable_open_button(false);
		
		// show window
	
	ShowWindow(dialog_choose_lib_object_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(choose_lib_object_event_proc);
	InstallWindowEventHandler(dialog_choose_lib_object_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// create primitive list
		
	choose_lib_object_create_list();
	
		// setup the list
		
	ctrl_id.signature=FOUR_CHAR_CODE('d3ET');
	ctrl_id.id=0;
	GetControlByID(dialog_choose_lib_object_wind,&ctrl_id,&ctrl);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&choose_lib_object_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&choose_lib_object_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);
	
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,dialog_choose_lib_object_count,NULL,kDataBrowserItemNoProperty);
	Draw1Control(ctrl);
	
		// modal window
		
	dialog_choose_lib_object_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_choose_lib_object_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_choose_lib_object_wind);
	
		// get object name
		
	if (!dialog_choose_lib_object_cancel) {
		strncpy(name,(char*)&dialog_choose_lib_object_data[dialog_choose_lib_object_idx*file_str_len],file_str_len);
		name[file_str_len-1]=0x0;
	}
	
		// clear up memory
		
	choose_lib_object_close_list();
	
	return(!dialog_choose_lib_object_cancel);
}

