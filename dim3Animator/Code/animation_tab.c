/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Animation Tab

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

#include "window.h"
#include "dialog.h"

bool							animate_list_notify_ignore;
ControlRef						animate_list;

DataBrowserItemDataUPP			animate_list_setitem_upp;
DataBrowserItemNotificationUPP	animate_list_notify_upp;

extern int						cur_animate,cur_pose;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;
extern ControlRef				tab_list;

/* =======================================================

      Set Animate List
      
======================================================= */

void reset_animate_list(void)
{
	DataBrowserItemID		itemID;
	
	animate_list_notify_ignore=TRUE;
	
		// reset list
		
	RemoveDataBrowserItems(animate_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(animate_list,kDataBrowserNoItem,model.nanimate,NULL,kDataBrowserItemNoProperty);
	
		// select animation
				
	if (cur_animate!=-1) {
		itemID=cur_animate+1;
		SetDataBrowserSelectedItems(animate_list,1,&itemID,kDataBrowserItemsAssign);
	}
	
	animate_list_notify_ignore=FALSE;
}

/* =======================================================

      Animate List Callbacks
      
======================================================= */

static pascal OSStatus animate_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	CFStringRef				cfstr;
	
	if (changeValue) {
		return(errDataBrowserPropertyNotSupported);
	}

	switch (property) {
		
		case kAnimateNameDBColumn:
			if (itemID>=1000) return(noErr);

			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.animates[itemID-1].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);

	}

	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void animate_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	ItemCount				count;
	
	if (animate_list_notify_ignore) return;
	
	switch (message) {

		case kDataBrowserItemDoubleClicked:
			dialog_animation_settings_run(cur_animate);
			reset_animate_list();
			break;
			
		case kDataBrowserItemSelected:
			cur_animate=itemID-1;
			model_view_reset=TRUE;
			break;
			
		case kDataBrowserSelectionSetChanged:
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,FALSE,kDataBrowserItemIsSelected,&count);
			if (count==0) {
				reset_animate_list();
				model_view_reset=TRUE;
			}
			break;

	}
}

/* =======================================================

      Start and End Animation Controls
      
======================================================= */

void start_animate_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	Rect							cbox;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;
    
		// setup data browser for animations

	cbox.left=box->left;
	cbox.right=box->left+220;
	
	cbox.top=(box->bottom-box->top)/2;
	cbox.bottom=box->bottom;

	CreateDataBrowserControl(wind,&cbox,kDataBrowserListView,&animate_list);

	framefocus=FALSE;
	SetControlData(animate_list,kControlNoPart,kControlDataBrowserIncludesFrameAndFocusTag,sizeof(framefocus),&framefocus);

	SetDataBrowserSelectionFlags(animate_list,kDataBrowserSelectOnlyOne);
	SetDataBrowserTableViewRowHeight(animate_list,10);

	fontstyle.flags=kControlUseFontMask|kControlUseSizeMask|kControlUseJustMask;
	fontstyle.font=FMGetFontFamilyFromName("\pMonaco");
	fontstyle.size=9;
	fontstyle.just=teFlushLeft;
	SetControlFontStyle(animate_list,&fontstyle);

		// data browser callbacks
		
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	animate_list_setitem_upp=NewDataBrowserItemDataUPP(&animate_list_item_proc);
	dbcall.u.v1.itemDataCallback=animate_list_setitem_upp;
    
	animate_list_notify_upp=NewDataBrowserItemNotificationUPP(&animate_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=animate_list_notify_upp;
	
	SetDataBrowserCallbacks(animate_list,&dbcall);

		// columns

	add_db_column(animate_list,"Animations",kAnimateNameDBColumn,kDataBrowserTextType,200,0);
	
	animate_list_notify_ignore=FALSE;
}

void end_animate_controls(void)
{
	DataBrowserCallbacks			dbcall;
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	SetDataBrowserCallbacks(animate_list,&dbcall);

	DisposeDataBrowserItemDataUPP(animate_list_setitem_upp);
	DisposeDataBrowserItemNotificationUPP(animate_list_notify_upp);
	
	DisposeControl(animate_list);
}

void resize_animate_controls(Rect *box)
{
	Rect		cbox;
	
	cbox.left=box->left;
	cbox.right=box->left+220;
	
	cbox.top=(box->bottom-box->top)/2;
	cbox.bottom=box->bottom;

	MoveControl(animate_list,cbox.left,cbox.top);
	SizeControl(animate_list,(cbox.right-cbox.left),(cbox.bottom-cbox.top));
}

