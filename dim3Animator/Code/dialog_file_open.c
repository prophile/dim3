/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: File Open Dialogs

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

#define kFileOpenList				FOUR_CHAR_CODE('list')
#define kFileOpenListDirColumn		FOUR_CHAR_CODE('pdir')
#define kFileOpenListNameColumn		FOUR_CHAR_CODE('pnme')

char							fp_file_name[256];
bool							fp_cancel;
file_path_directory_type		*fpd;
WindowRef						dialog_file_open_wind;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      File Open Event Handlers
      
======================================================= */

static pascal OSStatus file_open_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOpen:
					QuitAppModalLoopForWindow(dialog_file_open_wind);
					return(noErr);
					
				case kHICommandCancel:
					fp_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_file_open_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

void file_open_enable_open_button(bool enable)
{
	ControlRef						ctrl;
	ControlID						ctrl_id;

	ctrl_id.signature=FOUR_CHAR_CODE('d3ET');
	ctrl_id.id=1;
	GetControlByID(dialog_file_open_wind,&ctrl_id,&ctrl);
	
	if (enable) {
		EnableControl(ctrl);
	}
	else {
		DisableControl(ctrl);
	}
}

/* =======================================================

      File List Event Handlers
      
======================================================= */

static pascal OSStatus file_open_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	char			txt[file_str_len];
	CFStringRef		cfstr;
	
	switch (property) {
	
		case kFileOpenListDirColumn:
			idx=itemID-1;
			file_paths_descript_directory_file(txt,fpd->files[idx].dir_type);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
		
		case kFileOpenListNameColumn:
			idx=itemID-1;
			strncpy(txt,fpd->files[idx].file_name,file_str_len);
			txt[file_str_len-1]=0x0;
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void file_open_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int			idx;
	UInt32		count;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			idx=itemID-1;
			strncpy(fp_file_name,fpd->files[idx].file_name,file_str_len);
			fp_file_name[file_str_len-1]=0x0;
			QuitAppModalLoopForWindow(dialog_file_open_wind);
			break;

		case kDataBrowserItemSelected:
			idx=itemID-1;
			strncpy(fp_file_name,fpd->files[idx].file_name,file_str_len);
			fp_file_name[file_str_len-1]=0x0;
			break;
	}

	GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,FALSE,kDataBrowserItemIsSelected,&count);
	file_open_enable_open_button(count!=0);
}

/* =======================================================

      Run File Open
      
======================================================= */

bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *file_name)
{
	ControlRef						ctrl;
	ControlID						ctrl_id;
	DataBrowserCallbacks			dbcall;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_file_open_wind,dialog_name);
	
		// disable open
		
	file_open_enable_open_button(false);
		
		// show window
	
	ShowWindow(dialog_file_open_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(file_open_event_proc);
	InstallWindowEventHandler(dialog_file_open_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// scan for files
		
	fpd=file_paths_read_directory(&file_path_setup,search_path,extension,FALSE);
	
		// setup the list
		
	ctrl_id.signature=FOUR_CHAR_CODE('d3ET');
	ctrl_id.id=0;
	GetControlByID(dialog_file_open_wind,&ctrl_id,&ctrl);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&file_open_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&file_open_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);
	
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,fpd->nfile,NULL,kDataBrowserItemNoProperty);
	Draw1Control(ctrl);
	
		// modal window
		
	fp_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_file_open_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_file_open_wind);
	
		// clear up memory
		
	file_paths_close_directory(fpd);
	
		// cancelled?
		
	if (fp_cancel) return(FALSE);
	
		// copy file
		
	strcpy(file_name,fp_file_name);
	return(TRUE);
}

