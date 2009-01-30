/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Mesh List

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "window.h"
#include "dialog.h"

bool							mesh_list_notify_ignore;
ControlRef						mesh_list;

DataBrowserItemDataUPP			mesh_list_setitem_upp;
DataBrowserItemNotificationUPP	mesh_list_notify_upp;

extern int						cur_mesh;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;

/* =======================================================

      Set Mesh List
      
======================================================= */

void reset_mesh_list(void)
{
	DataBrowserItemID		itemID;

	mesh_list_notify_ignore=TRUE;
	
		// setup meshes
		
	RemoveDataBrowserItems(mesh_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(mesh_list,kDataBrowserNoItem,model.nmesh,NULL,kDataBrowserItemNoProperty);

		// select animation
				
	if (cur_mesh!=-1) {
		itemID=cur_mesh+1;
		SetDataBrowserSelectedItems(mesh_list,1,&itemID,kDataBrowserItemsAssign);
	}
	
	mesh_list_notify_ignore=FALSE;
}

/* =======================================================

      Mesh List Callbacks
      
======================================================= */

static pascal OSStatus mesh_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	CFStringRef		cfstr;
	
	if (changeValue) return(errDataBrowserPropertyNotSupported);
	
	switch (property) {
		
		case kMeshNameDBColumn:
			idx=itemID-1;
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.meshes[idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void mesh_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				idx;
	ItemCount		count;
	
	if (mesh_list_notify_ignore) return;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			idx=itemID-1;
			if (dialog_mesh_info_run(&model.meshes[idx])) {
				reset_mesh_list();
			}
			break;

		case kDataBrowserItemSelected:
			cur_mesh=itemID-1;
			model_view_reset=TRUE;
			break;
			
		case kDataBrowserSelectionSetChanged:
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,FALSE,kDataBrowserItemIsSelected,&count);
			if (count==0) {
				reset_mesh_list();
				model_view_reset=TRUE;
			}
			break;
	}
}

/* =======================================================

      Start and End Mesh Controls
      
======================================================= */

void start_mesh_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	Rect							cbox;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;
	
		// mesh data browser
		
	cbox.left=box->left+220;
	cbox.right=cbox.left+220;
	
	cbox.top=(box->bottom-box->top)/2;
	cbox.bottom=box->bottom;

	CreateDataBrowserControl(wind,&cbox,kDataBrowserListView,&mesh_list);
    
	framefocus=FALSE;
	SetControlData(mesh_list,kControlNoPart,kControlDataBrowserIncludesFrameAndFocusTag,sizeof(framefocus),&framefocus);

	SetDataBrowserTableViewRowHeight(mesh_list,9);
	
	fontstyle.flags=kControlUseFontMask|kControlUseSizeMask|kControlUseJustMask;
	fontstyle.font=FMGetFontFamilyFromName("\pMonaco");
	fontstyle.size=9;
	fontstyle.just=teFlushLeft;
	SetControlFontStyle(mesh_list,&fontstyle);
	
		// data browser callbacks
		
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	mesh_list_setitem_upp=NewDataBrowserItemDataUPP(&mesh_list_item_proc);
	dbcall.u.v1.itemDataCallback=mesh_list_setitem_upp;
	
	mesh_list_notify_upp=NewDataBrowserItemNotificationUPP(&mesh_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=mesh_list_notify_upp;
	
	SetDataBrowserCallbacks(mesh_list,&dbcall);

		// columns

	add_db_column(mesh_list,"Meshes",kMeshNameDBColumn,kDataBrowserTextType,200,0);
	
	mesh_list_notify_ignore=FALSE;
}

void end_mesh_controls(void)
{
	DataBrowserCallbacks			dbcall;
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	SetDataBrowserCallbacks(mesh_list,&dbcall);

	DisposeDataBrowserItemDataUPP(mesh_list_setitem_upp);
	DisposeDataBrowserItemNotificationUPP(mesh_list_notify_upp);

	DisposeControl(mesh_list);
}

void resize_mesh_controls(Rect *box)
{
	Rect		cbox;
	
	cbox.left=box->left+220;
	cbox.right=cbox.left+220;
	
	cbox.top=(box->bottom-box->top)/2;
	cbox.bottom=box->bottom;

	MoveControl(mesh_list,cbox.left,cbox.top);
	SizeControl(mesh_list,(cbox.right-cbox.left),(cbox.bottom-cbox.top));
}

