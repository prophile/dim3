/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Mesh Tab

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

#include "model.h"
#include "tab.h"
#include "dialog.h"
#include "window.h"

ControlRef						mesh_list;

DataBrowserItemDataUPP			mesh_list_setitem_upp;
DataBrowserItemNotificationUPP	mesh_list_notify_upp;

extern int						cur_mesh,cur_pose;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;
extern ControlRef				tab_list;

/* =======================================================

      Redraw a Row
      
======================================================= */

void redraw_mesh_row(int itemID)
{
    DataBrowserItemID		dbitem;
    
	dbitem=itemID;
	UpdateDataBrowserItems(mesh_list,kDataBrowserNoItem,1,&dbitem,kDataBrowserItemNoProperty,kDataBrowserNoItem);
    
    Draw1Control(mesh_list);			// this shouldn't be necessary, but it seems to be
}

/* =======================================================

      Mesh List Callbacks
      
======================================================= */

static pascal OSStatus mesh_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	CFStringRef		cfstr;

	if (changeValue) return(errDataBrowserPropertyNotSupported);
	
	switch (property) {
		
		case kMeshNameDBColumn:
			if (itemID>=max_model_vertex) return(noErr);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.meshes[itemID-1].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
	}

	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void mesh_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	switch (message) {

		case kDataBrowserItemDoubleClicked:
			cur_mesh=itemID-1;
			dialog_mesh_info_run(&model.meshes[cur_mesh]);
			redraw_mesh_row(itemID);
			reset_vertex_tab();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			info_palette_draw();
			break;
			
		case kDataBrowserItemSelected:
			cur_mesh=itemID-1;
			reset_vertex_tab();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			info_palette_draw();
			break;
			
	}
}

/* =======================================================

      Set Mesh List
      
======================================================= */

void reset_mesh_tab(int cmesh)
{
	DataBrowserItemID		itemID;

	RemoveDataBrowserItems(mesh_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	
		// add meshes

	AddDataBrowserItems(mesh_list,kDataBrowserNoItem,model.nmesh,NULL,kDataBrowserItemNoProperty);

		// select current mesh
		
	cur_mesh=cmesh;
	
	itemID=cur_mesh+1;
	SetDataBrowserSelectedItems(mesh_list,1,&itemID,kDataBrowserItemsAssign);
}

/* =======================================================

      Start and End Mesh Controls
      
======================================================= */

void start_mesh_controls(WindowRef wind,Rect *box)
{
	int							yadd;
    bool						framefocus;
	Rect						cbox;
	ControlFontStyleRec			fontstyle;
	DataBrowserCallbacks		dbcall;
	
		// mesh data browser

	cbox.left=box->left;
	cbox.right=box->left+((box->right-box->left)/2);
	
	yadd=(box->bottom-box->top)/5;
	cbox.top=box->top;
	cbox.bottom=cbox.top+yadd;
	
	CreateDataBrowserControl(wind,&cbox,kDataBrowserListView,&mesh_list);
    
	framefocus=FALSE;
	SetControlData(mesh_list,kControlNoPart,kControlDataBrowserIncludesFrameAndFocusTag,sizeof(framefocus),&framefocus);

	SetDataBrowserSelectionFlags(mesh_list,kDataBrowserSelectOnlyOne);
	SetDataBrowserTableViewRowHeight(mesh_list,10);
	
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

	add_db_column(mesh_list,"Meshes",kMeshNameDBColumn,kDataBrowserTextType,150,0);
	
	EmbedControl(mesh_list,tab_list);
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
	int					yadd;
	Rect				cbox;
	
	cbox.left=box->left;
	cbox.right=box->left+((box->right-box->left)/2);
	
	yadd=(box->bottom-box->top)/5;
	cbox.top=box->top;
	cbox.bottom=cbox.top+yadd;

	MoveControl(mesh_list,cbox.left,cbox.top);
	SizeControl(mesh_list,(cbox.right-cbox.left),(cbox.bottom-cbox.top));
}

/* =======================================================

      Hide and Show Mesh Controls
      
======================================================= */

void show_mesh_controls(void)
{
//	ShowControl(mesh_list);
//	SetKeyboardFocus(model_wind,mesh_list,1);
//	Draw1Control(mesh_list);
}

void hide_mesh_controls(void)
{
//	HideControl(mesh_list);
}

