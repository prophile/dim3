/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Hit Box Tab

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

#include "tab.h"
#include "dialog.h"

ControlRef						hit_box_list;

DataBrowserItemDataUPP			hit_box_list_setitem_upp;
DataBrowserItemNotificationUPP	hit_box_list_notify_upp;

extern int						cur_hit_box;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;
extern ControlRef				tab_list;

/* =======================================================

      Redraw a Row
      
======================================================= */

void redraw_hit_box_row(int row)
{
    DataBrowserItemID		dbitem;
    
	dbitem=row+1;
	UpdateDataBrowserItems(hit_box_list,kDataBrowserNoItem,1,&dbitem,kDataBrowserItemNoProperty,kDataBrowserNoItem);
    
    Draw1Control(hit_box_list);			// this shouldn't be necessary, but it seems to be
}

/* =======================================================

      Hit box List Callbacks
      
======================================================= */

static pascal OSStatus hit_box_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				i;
	CFStringRef		cfstr;
	
	if (changeValue) return(errDataBrowserPropertyNotSupported);
	
	switch (property) {
		
		case kHitBoxNameDBColumn:
			i=itemID-1;
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.hit_boxes[i].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
	}

	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void hit_box_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				i;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			i=itemID-1;
			if (dialog_hit_box_settings_run(&model.hit_boxes[i])) {
				redraw_hit_box_row(i);
				model_view_reset=TRUE;
			}
			break;
			
		case kDataBrowserItemSelected:
			cur_hit_box=itemID-1;
			if (cur_hit_box!=-1) {
				model_view_reset=TRUE;
			}
			break;

	}
}

/* =======================================================

      Set Hit Box List
      
======================================================= */

void reset_hit_box_tab(int chit_box)
{
	DataBrowserItemID		itemID;
	
	RemoveDataBrowserItems(hit_box_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(hit_box_list,kDataBrowserNoItem,model.nhit_box,NULL,kDataBrowserItemNoProperty);
	
		// select current hit box
				
	cur_hit_box=chit_box;
	
	if (cur_hit_box!=-1) {
		itemID=cur_hit_box+1;
		SetDataBrowserSelectedItems(hit_box_list,1,&itemID,kDataBrowserItemsAssign);
	}
}

/* =======================================================

      Start and End Hit Box Controls
      
======================================================= */

void start_hit_box_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;
	
		// hit_box data browser

	CreateDataBrowserControl(wind,box,kDataBrowserListView,&hit_box_list);
    
	framefocus=FALSE;
	SetControlData(hit_box_list,kControlNoPart,kControlDataBrowserIncludesFrameAndFocusTag,sizeof(framefocus),&framefocus);

	SetDataBrowserSelectionFlags(hit_box_list,kDataBrowserSelectOnlyOne);
	SetDataBrowserTableViewRowHeight(hit_box_list,10);
	
	fontstyle.flags=kControlUseFontMask|kControlUseSizeMask|kControlUseJustMask;
	fontstyle.font=FMGetFontFamilyFromName("\pMonaco");
	fontstyle.size=9;
	fontstyle.just=teFlushLeft;
	SetControlFontStyle(hit_box_list,&fontstyle);
	
		// data browser callbacks
		
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	hit_box_list_setitem_upp=NewDataBrowserItemDataUPP(&hit_box_list_item_proc);
	dbcall.u.v1.itemDataCallback=hit_box_list_setitem_upp;
	
	hit_box_list_notify_upp=NewDataBrowserItemNotificationUPP(&hit_box_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=hit_box_list_notify_upp;
	
	SetDataBrowserCallbacks(hit_box_list,&dbcall);

		// columns

	add_db_column(hit_box_list,"Name",kHitBoxNameDBColumn,kDataBrowserTextType,200,0);
	
	EmbedControl(hit_box_list,tab_list);
}

void end_hit_box_controls(void)
{
	DataBrowserCallbacks			dbcall;
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	SetDataBrowserCallbacks(hit_box_list,&dbcall);
	
	DisposeDataBrowserItemDataUPP(hit_box_list_setitem_upp);
	DisposeDataBrowserItemNotificationUPP(hit_box_list_notify_upp);
	
	DisposeControl(hit_box_list);
}

void resize_hit_box_controls(Rect *box)
{
	SizeControl(hit_box_list,(box->right-box->left),(box->bottom-box->top));
}

/* =======================================================

      Hide and Show Hit Box Controls
      
======================================================= */

void show_hit_box_controls(void)
{
    ShowControl(hit_box_list);
	SetKeyboardFocus(model_wind,hit_box_list,1);
	Draw1Control(hit_box_list);
}

void hide_hit_box_controls(void)
{
    HideControl(hit_box_list);
}

