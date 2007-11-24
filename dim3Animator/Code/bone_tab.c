/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
        Bone Drag/Drop Code by Rocco Bowling
 Usage: Bone Tab

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

ControlRef						bone_list;

DataBrowserItemDataUPP			bone_list_setitem_upp;
DataBrowserItemNotificationUPP	bone_list_notify_upp;

DataBrowserAddDragItemUPP		bone_list_drag_item_upp;
DataBrowserAcceptDragUPP		bone_list_drag_accept_upp;
DataBrowserReceiveDragUPP		bone_list_drag_receive_upp;

extern int						cur_mesh,cur_bone,cur_pose;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;
extern ControlRef				tab_list;

/* =======================================================

      Redraw a Row
      
======================================================= */

void redraw_bone_row(void)
{
    DataBrowserItemID		dbitem;
    
	if (cur_bone==-1) return;

	dbitem=cur_bone+1;
	UpdateDataBrowserItems(bone_list,kDataBrowserNoItem,1,&dbitem,kDataBrowserItemNoProperty,kDataBrowserNoItem);
    
    Draw1Control(bone_list);			// this shouldn't be necessary, but it seems to be
}

/* =======================================================

      Parent/Child Utilities
      
======================================================= */

bool bone_is_parent(int parent_idx)
{
	int				n;
	
	for (n=0;n!=model.nbone;n++) {
		if (n==parent_idx) continue;
		if (model.bones[n].parent_idx==parent_idx) return(TRUE);
	}
	
	return(FALSE);
}

int create_db_bone_child_list(int parent_idx,DataBrowserItemID *boneID)
{
	int				n,cnt;
	
	cnt=0;
	
	for (n=0;n!=model.nbone;n++) {
		if (n==parent_idx) continue;
		if (model.bones[n].parent_idx==parent_idx) {
			boneID[cnt]=n+1;
			cnt++;
		}
	}
	
	return(cnt);
}

/* =======================================================

      Bone List Callbacks
      
======================================================= */

static pascal OSStatus bone_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				i;
	char			txt[64];
	CFStringRef		cfstr;
	
	if (changeValue) {
		return(errDataBrowserPropertyNotSupported);
	}
	
	switch (property) {
		
		case kBoneTagDBColumn:
			i=itemID-1;
			memmove(txt,&model.bones[i].tag,4);
			txt[4]=0x0;
			if (model.bones[i].name[0]!=0x0) {
				strcat(txt," (");
				strcat(txt,model.bones[i].name);
				strcat(txt,")");
			}
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);

		case kBonePosDBColumn:
			i=itemID-1;
			sprintf(txt,"(%d,%d,%d)",model.bones[i].pnt.x,model.bones[i].pnt.y,model.bones[i].pnt.z);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kDataBrowserItemIsContainerProperty:
			SetDataBrowserItemDataBooleanValue(itemData,bone_is_parent(itemID-1));
			return(noErr);
			
	}

	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void bone_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int						k,cnt;
	DataBrowserItemID		boneID[max_model_bone];
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			k=itemID-1;
			if (dialog_bone_settings_run(&model.bones[k])) {
				model_calculate_parents(&model);
				reset_bone_tab(cur_bone);
				model_view_reset=TRUE;
			}
			break;

		case kDataBrowserItemSelected:
			cur_bone=itemID-1;
			if (cur_bone!=-1) {
				model_set_sel_mask_bone(&model,cur_mesh,cur_bone);
				hilite_vertex_rows();
				model_view_reset=TRUE;
			}
			break;
			
		case kDataBrowserContainerOpened:
			cnt=create_db_bone_child_list((itemID-1),boneID);
			AddDataBrowserItems(bone_list,itemID,cnt,boneID,kDataBrowserItemNoProperty);
			break;
	}
}

static pascal Boolean bone_list_drag_accept_proc(ControlRef browser,
												 DragReference theDrag,
												 DataBrowserItemID item)
{
	ItemReference theItemRef;
	unsigned int other_index;
	Size dataSize = sizeof(other_index);
	
	if(item == kDataBrowserNoItem)
	{
		return false;
	}
	
	item--;
	
	GetDragItemReferenceNumber(theDrag, 1, &theItemRef);
	GetFlavorData(theDrag, theItemRef, typeUInt32, &other_index, &dataSize, 0);
		
	if(item != other_index)
	{
		return true;
	}
	
	return false;
}

static pascal Boolean bone_list_drag_item_proc(ControlRef browser,
											   DragReference theDrag,
											   DataBrowserItemID item,
											   ItemReference *itemRef)
{
	item--;
	
	AddDragItemFlavor(theDrag, *itemRef, typeUInt32, &item, sizeof(item), 0);
	
	return true;
}

static pascal Boolean bone_list_drag_receive_proc(ControlRef browser,
												  DragReference theDrag,
												  DataBrowserItemID item)
{
	int				old_parent_idx;
	ItemReference theItemRef;
	unsigned int other_index;
	Size dataSize = sizeof(other_index);
	
	if(item == kDataBrowserNoItem)
	{
		return false;
	}
	
	item--;
	
	GetDragItemReferenceNumber(theDrag, 1, &theItemRef);
	GetFlavorData(theDrag, theItemRef, typeUInt32, &other_index, &dataSize, 0);
	
	old_parent_idx=model.bones[other_index].parent_idx;
	model.bones[other_index].parent_idx = item;
	
	if (model_check_bone_circular(&model,&model.bones[other_index])) {
		model.bones[other_index].parent_idx=old_parent_idx;
		StandardAlert(0,"\pBad Bone Parenting","\pDragging a bone to this location will make a set of bones with circular references.",NULL,NULL);
	}
	
	model_calculate_parents(&model);
	reset_bone_tab(-1);
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
	
	return true;
}

/* =======================================================

      Set Pose List
      
======================================================= */

void reset_bone_tab(int cbone)
{
	int						n,cnt;
	bool					all_open;
	DataBrowserItemID		itemID,itemList[max_model_bone];
	DataBrowserItemState	state;
	
		// find all non-parent bones
		
	cnt=0;
	
	for (n=0;n!=model.nbone;n++) {
		if (model.bones[n].parent_idx==-1) {
			itemList[cnt]=n+1;
			cnt++;
		}
	}
	
		// if all bones have parents, it's a bug, but let's deal with it
		
	if ((cnt==0) && (model.nbone!=0)) {
		for (n=0;n!=model.nbone;n++) {
			itemList[n]=n+1;
		}
		cnt=model.nbone;
	}
	
		// add non-parent bones
	
	RemoveDataBrowserItems(bone_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(bone_list,kDataBrowserNoItem,cnt,itemList,kDataBrowserItemNoProperty);
	
		// force open all items
		// this code is ultra cheesy, but there's no easy API for opening all data browser items
		// so we keep looping until there are no more items to open
		
	while (TRUE) {
		all_open=TRUE;
		for (n=0;n!=model.nbone;n++) {
			if (GetDataBrowserItemState(bone_list,(n+1),&state)!=errDataBrowserItemNotFound) {
				if ((state&kDataBrowserContainerIsOpen)==0) {
					if (OpenDataBrowserContainer(bone_list,(n+1))==noErr) {
						all_open=FALSE;
					}
				}
			}
		}
		if (all_open) break;
	}
	
		// select current bone
				
	cur_bone=cbone;
	
	if (cur_bone!=-1) {
		itemID=cur_bone+1;
		SetDataBrowserSelectedItems(bone_list,1,&itemID,kDataBrowserItemsAssign);
	}
}

/* =======================================================

      Start and End Bone Controls
      
======================================================= */

void start_bone_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;

		// bone data browser

	CreateDataBrowserControl(wind,box,kDataBrowserListView,&bone_list);
    
	framefocus=FALSE;
	SetControlData(bone_list,kControlNoPart,kControlDataBrowserIncludesFrameAndFocusTag,sizeof(framefocus),&framefocus);

	SetDataBrowserSelectionFlags(bone_list,kDataBrowserSelectOnlyOne);
	SetDataBrowserTableViewRowHeight(bone_list,10);
	
	fontstyle.flags=kControlUseFontMask|kControlUseSizeMask|kControlUseJustMask;
	fontstyle.font=FMGetFontFamilyFromName("\pMonaco");
	fontstyle.size=9;
	fontstyle.just=teFlushLeft;
	SetControlFontStyle(bone_list,&fontstyle);
	
		// data browser callbacks
		
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	bone_list_setitem_upp=NewDataBrowserItemDataUPP(&bone_list_item_proc);
	dbcall.u.v1.itemDataCallback=bone_list_setitem_upp;
	
	bone_list_notify_upp=NewDataBrowserItemNotificationUPP(&bone_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=bone_list_notify_upp;
	
	// Drag and drop
	bone_list_drag_accept_upp=NewDataBrowserAcceptDragUPP(&bone_list_drag_accept_proc);
	dbcall.u.v1.acceptDragCallback=bone_list_drag_accept_upp;
	
	bone_list_drag_item_upp=NewDataBrowserAddDragItemUPP(&bone_list_drag_item_proc);
	dbcall.u.v1.addDragItemCallback=bone_list_drag_item_upp;
	
	bone_list_drag_receive_upp=NewDataBrowserReceiveDragUPP(&bone_list_drag_receive_proc);
	dbcall.u.v1.receiveDragCallback=bone_list_drag_receive_upp;
	
	SetDataBrowserCallbacks(bone_list,&dbcall);

		// columns

	add_db_column(bone_list,"Bone",kBoneTagDBColumn,kDataBrowserTextType,150,0);
	add_db_column(bone_list,"Position",kBonePosDBColumn,kDataBrowserTextType,200,1);
	
	SetDataBrowserListViewDisclosureColumn(bone_list,kBoneTagDBColumn,FALSE);
	
	EmbedControl(bone_list,tab_list);
}

void end_bone_controls(void)
{
	DataBrowserCallbacks			dbcall;
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	SetDataBrowserCallbacks(bone_list,&dbcall);
	
	DisposeDataBrowserItemDataUPP(bone_list_setitem_upp);
	DisposeDataBrowserItemNotificationUPP(bone_list_notify_upp);
	
	DisposeControl(bone_list);
}

void resize_bone_controls(Rect *box)
{
	SizeControl(bone_list,(box->right-box->left),(box->bottom-box->top));
}

/* =======================================================

      Hide and Show Bone Controls
      
======================================================= */

void show_bone_controls(void)
{
    ShowControl(bone_list);
	SetKeyboardFocus(model_wind,bone_list,1);
	Draw1Control(bone_list);
	
	SetAutomaticControlDragTrackingEnabledForWindow(model_wind, true);
}

void hide_bone_controls(void)
{
	SetAutomaticControlDragTrackingEnabledForWindow(model_wind, false);
	
    HideControl(bone_list);
}

