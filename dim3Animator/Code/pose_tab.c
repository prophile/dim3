/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Pose List

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
#include "dialog.h"

bool							pose_list_notify_ignore;
ControlRef						pose_list;

DataBrowserItemDataUPP			pose_list_setitem_upp;
DataBrowserItemNotificationUPP	pose_list_notify_upp;

extern int						cur_mesh,cur_pose,cur_bone;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;

/* =======================================================

      Set Pose List
      
======================================================= */

void reset_pose_list(void)
{
	DataBrowserItemID		itemID;
	
	pose_list_notify_ignore=TRUE;
	
		// setup pose list
		
	RemoveDataBrowserItems(pose_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(pose_list,kDataBrowserNoItem,(model.npose+1),NULL,kDataBrowserItemNoProperty);
			
		// select current pose/bone move
				
	if (cur_pose==-1) {
		itemID=1;
	}
	else {
		itemID=cur_pose+2;
	}
	
	SetDataBrowserSelectedItems(pose_list,1,&itemID,kDataBrowserItemsAssign);
	
	pose_list_notify_ignore=FALSE;
}

/* =======================================================

      Pose List Callbacks
      
======================================================= */

static pascal OSStatus pose_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	CFStringRef				cfstr;
	
	if (changeValue) return(errDataBrowserPropertyNotSupported);
	
	switch (property) {
		
		case kPoseNameDBColumn:
			if (itemID==1) {
				cfstr=CFStringCreateWithCString(kCFAllocatorDefault,"Neutral",kCFStringEncodingMacRoman);
			}
			else {
				cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.poses[itemID-2].name,kCFStringEncodingMacRoman);
			}
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);

	}
	
	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void pose_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int						idx;
	ItemCount				count;
	
	if (pose_list_notify_ignore) return;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			idx=itemID-2;
			if (idx>=0) {
				dialog_pose_settings_run(&model.poses[idx]);
                reset_pose_list();
			}
			break;

		case kDataBrowserItemSelected:
			idx=itemID-2;
			cur_pose=itemID-2;
			reset_bone_list();
			model_view_reset=TRUE;
			break;
			
		case kDataBrowserSelectionSetChanged:
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,FALSE,kDataBrowserItemIsSelected,&count);
			if (count==0) {
				reset_pose_list();
				reset_bone_list();
				model_view_reset=TRUE;
			}
			break;

	}
}

/* =======================================================

      Browser Columns
      
======================================================= */

void add_db_column(ControlRef ctrl,char *name,int idx,int type,int sz,int spot)
{
	DataBrowserListViewColumnDesc	dbcol;
	
	dbcol.propertyDesc.propertyID=idx;
	dbcol.propertyDesc.propertyType=type;
	dbcol.propertyDesc.propertyFlags=kDataBrowserListViewSelectionColumn|kDataBrowserDoNotTruncateText;

	dbcol.headerBtnDesc.version=kDataBrowserListViewLatestHeaderDesc;
	dbcol.headerBtnDesc.initialOrder=kDataBrowserOrderIncreasing;
	dbcol.headerBtnDesc.minimumWidth=dbcol.headerBtnDesc.maximumWidth=sz;
	
	dbcol.headerBtnDesc.btnFontStyle.flags=kControlUseJustMask;
	dbcol.headerBtnDesc.btnFontStyle.just=teFlushLeft;
	
	dbcol.headerBtnDesc.btnFontStyle.flags=kControlUseFontMask|kControlUseSizeMask|kControlUseJustMask;
	dbcol.headerBtnDesc.btnFontStyle.font=FMGetFontFamilyFromName("\pMonaco");
	dbcol.headerBtnDesc.btnFontStyle.size=9;
	dbcol.headerBtnDesc.btnFontStyle.just=teFlushLeft;
	
	dbcol.headerBtnDesc.btnContentInfo.contentType=kControlContentTextOnly;

	dbcol.headerBtnDesc.titleOffset=0;
	dbcol.headerBtnDesc.titleString=CFStringCreateWithCString(kCFAllocatorDefault,name,kCFStringEncodingMacRoman);

	AddDataBrowserListViewColumn(ctrl,&dbcol,spot);
}

/* =======================================================

      Start and End Pose Controls
      
======================================================= */

void start_pose_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	Rect							cbox;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;

		// pose data browser
		
	cbox.left=box->left;
	cbox.right=box->left+220;
	
	cbox.top=box->top;
	cbox.bottom=(box->bottom-box->top)/2;

	CreateDataBrowserControl(wind,&cbox,kDataBrowserListView,&pose_list);
    
	framefocus=FALSE;
	SetControlData(pose_list,kControlNoPart,kControlDataBrowserIncludesFrameAndFocusTag,sizeof(framefocus),&framefocus);

	SetDataBrowserSelectionFlags(pose_list,kDataBrowserSelectOnlyOne);
	SetDataBrowserTableViewRowHeight(pose_list,10);
	
	fontstyle.flags=kControlUseFontMask|kControlUseSizeMask|kControlUseJustMask;
	fontstyle.font=FMGetFontFamilyFromName("\pMonaco");
	fontstyle.size=9;
	fontstyle.just=teFlushLeft;
	SetControlFontStyle(pose_list,&fontstyle);
	
		// data browser callbacks
		
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	pose_list_setitem_upp=NewDataBrowserItemDataUPP(&pose_list_item_proc);
	dbcall.u.v1.itemDataCallback=pose_list_setitem_upp;
	
	pose_list_notify_upp=NewDataBrowserItemNotificationUPP(&pose_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=pose_list_notify_upp;
	
	SetDataBrowserCallbacks(pose_list,&dbcall);

		// columns

	add_db_column(pose_list,"Poses",kPoseNameDBColumn,kDataBrowserTextType,200,0);
	
	pose_list_notify_ignore=FALSE;
}

void end_pose_controls(void)
{
	DataBrowserCallbacks			dbcall;
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	SetDataBrowserCallbacks(pose_list,&dbcall);

	DisposeDataBrowserItemDataUPP(pose_list_setitem_upp);
	DisposeDataBrowserItemNotificationUPP(pose_list_notify_upp);
	
	DisposeControl(pose_list);
}

void resize_pose_controls(Rect *box)
{
	Rect		cbox;
	
	cbox.left=box->left;
	cbox.right=box->left+220;
	
	cbox.top=box->top;
	cbox.bottom=(box->bottom-box->top)/2;

	MoveControl(pose_list,cbox.left,cbox.top);
	SizeControl(pose_list,(cbox.right-cbox.left),(cbox.bottom-cbox.top));
}

