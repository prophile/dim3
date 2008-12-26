/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Bone List

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

#include "model.h"
#include "dialog.h"

bool							bone_list_notify_ignore;
ControlRef						bone_list;

DataBrowserItemDataUPP			bone_list_setitem_upp;
DataBrowserItemNotificationUPP	bone_list_notify_upp;
DataBrowserAddDragItemUPP		bone_list_drag_item_upp;
DataBrowserAcceptDragUPP		bone_list_drag_accept_upp;
DataBrowserReceiveDragUPP		bone_list_drag_receive_upp;

extern int						cur_mesh,cur_pose,cur_bone;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;

/* =======================================================

      Create Bone Children ID
      
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

int bone_create_child_list(int parent_idx,DataBrowserItemID *boneID)
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

      Set Pose List
      
======================================================= */

void reset_bone_list(void)
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
	
		// add bones
		
	bone_list_notify_ignore=TRUE;
	
	RemoveDataBrowserItems(bone_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(bone_list,kDataBrowserNoItem,cnt,itemList,kDataBrowserItemNoProperty);
	
	bone_list_notify_ignore=FALSE;
	
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

		// select item
		
	if (cur_bone!=-1) {
		bone_list_notify_ignore=TRUE;
	
		itemID=cur_bone+1;
		SetDataBrowserSelectedItems(bone_list,1,&itemID,kDataBrowserItemsAssign);
		
		bone_list_notify_ignore=FALSE;
	}
}

void redraw_bone_list(void)
{
	Draw1Control(bone_list);
}

/* =======================================================

      Pose Bone Move List Callbacks
      
======================================================= */

static pascal OSStatus pose_bone_move_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int						idx;
	char					txt[256],s_mov_x[32],s_mov_y[32],s_mov_z[32],s_mov_acc[32];
	model_bone_move_type	*bone_move;
	CFStringRef				cfstr;
	
	switch (property) {
		
		case kPoseBoneDBColumn:
			idx=itemID-1;

			memmove(txt,&model.bones[idx].tag,4);
			txt[4]=0x0;
			if (model.bones[idx].name[0]!=0x0) {
				strcat(txt," (");
				strcat(txt,model.bones[idx].name);
				strcat(txt,")");
			}
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);

		case kPoseBoneMoveDBColumn:
			if (cur_pose==-1) return(noErr);

			idx=itemID-1;
			bone_move=&model.poses[cur_pose].bone_moves[idx];
			
			string_convert_float(s_mov_x,bone_move->mov.x);
			string_convert_float(s_mov_y,bone_move->mov.y);
			string_convert_float(s_mov_z,bone_move->mov.z);
			
			sprintf(txt,"(%d,%d,%d) / (%s,%s,%s)",(int)bone_move->rot.x,(int)bone_move->rot.y,(int)bone_move->rot.z,s_mov_x,s_mov_y,s_mov_z);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kPoseBoneAccDBColumn:
			if (cur_pose==-1) return(noErr);
			
			idx=itemID-1;
			bone_move=&model.poses[cur_pose].bone_moves[idx];
			
			string_convert_float(s_mov_acc,bone_move->acceleration);
			
			sprintf(txt,"%s",s_mov_acc);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
		
		case kPoseBoneBlendDBColumn:
			if (cur_pose==-1) return(noErr);
			
			idx=itemID-1;
			bone_move=&model.poses[cur_pose].bone_moves[idx];
			
			if (bone_move->skip_blended) {
				strcpy(txt,"Y");
			}
			else {
				strcpy(txt,"N");
			}
			
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
	int						idx,cnt;
	ItemCount				count;
	DataBrowserItemID		boneID[max_model_bone];
	
	if (bone_list_notify_ignore) return;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			idx=itemID-1;
			if (cur_pose==-1) {
				if (dialog_bone_settings_run(&model.bones[idx])) {
					model_calculate_parents(&model);
				}
			}
			else {
				dialog_bone_move_settings_run(&model.poses[cur_pose].bone_moves[idx]);
			}
			reset_bone_list();
			model_view_reset=TRUE;
			break;
	
		case kDataBrowserItemSelected:
			cur_bone=itemID-1;
			vertex_set_sel_mask_bone(cur_mesh,cur_bone);
			hilite_vertex_rows();
			model_view_reset=TRUE;
			break;
			
		case kDataBrowserSelectionSetChanged:
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count==0) {
				cur_bone=-1;
				vertex_set_sel_mask_no_bone(cur_mesh);
				hilite_vertex_rows();
				model_view_reset=TRUE;
			}
			break;
			
		case kDataBrowserContainerOpened:
			cnt=bone_create_child_list((itemID-1),boneID);
			AddDataBrowserItems(bone_list,itemID,cnt,boneID,kDataBrowserItemNoProperty);
			break;

	}
}

static pascal Boolean bone_list_drag_accept_proc(ControlRef browser,DragReference theDrag,DataBrowserItemID item)
{
	int					idx;
	ItemReference		theItemRef;
	Size				sz;
	
	if(item==kDataBrowserNoItem) return(FALSE);
	
	item--;
	
	GetDragItemReferenceNumber(theDrag,1,&theItemRef);
	
	sz=sizeof(int);
	GetFlavorData(theDrag,theItemRef,typeUInt32,&idx,&sz,0);
		
	return(item!=idx);
}

static pascal Boolean bone_list_drag_item_proc(ControlRef browser,DragReference theDrag,DataBrowserItemID item,ItemReference *itemRef)
{
	item--;
	
	AddDragItemFlavor(theDrag,*itemRef,typeUInt32,&item,sizeof(DataBrowserItemID),0);
	
	return(TRUE);
}

static pascal Boolean bone_list_drag_receive_proc(ControlRef browser,DragReference theDrag,DataBrowserItemID item)
{
	int				idx,old_parent_idx;
	ItemReference	theItemRef;
	Size			sz;

	if(item==kDataBrowserNoItem) return(FALSE);
		
	item--;
	
	GetDragItemReferenceNumber(theDrag,1,&theItemRef);
	
	sz=sizeof(int);
	GetFlavorData(theDrag,theItemRef,typeUInt32,&idx,&sz,0);
	
	old_parent_idx=model.bones[idx].parent_idx;
	model.bones[idx].parent_idx=item;
	
	if (model_check_bone_circular(&model,&model.bones[idx])) {
		model.bones[idx].parent_idx=old_parent_idx;
		StandardAlert(0,"\pBad Bone Parenting","\pDragging a bone to this location will make a set of bones with circular references.",NULL,NULL);
	}
	
	model_calculate_parents(&model);
	reset_bone_list();
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
	
	return(TRUE);
}

/* =======================================================

      Start and End Bone Controls
      
======================================================= */

void start_bone_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	Rect							cbox;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;

		// pose bone move data browser
		
	cbox.left=box->left+220;
	cbox.right=box->right;
	
	cbox.top=box->top;
	cbox.bottom=(box->bottom-box->top)/2;

	CreateDataBrowserControl(wind,&cbox,kDataBrowserListView,&bone_list);
    
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
	
	bone_list_setitem_upp=NewDataBrowserItemDataUPP(&pose_bone_move_item_proc);
	dbcall.u.v1.itemDataCallback=bone_list_setitem_upp;
	
	bone_list_notify_upp=NewDataBrowserItemNotificationUPP(&bone_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=bone_list_notify_upp;
	
	bone_list_drag_accept_upp=NewDataBrowserAcceptDragUPP(&bone_list_drag_accept_proc);
	dbcall.u.v1.acceptDragCallback=bone_list_drag_accept_upp;
	
	bone_list_drag_item_upp=NewDataBrowserAddDragItemUPP(&bone_list_drag_item_proc);
	dbcall.u.v1.addDragItemCallback=bone_list_drag_item_upp;

	bone_list_drag_receive_upp=NewDataBrowserReceiveDragUPP(&bone_list_drag_receive_proc);
	dbcall.u.v1.receiveDragCallback=bone_list_drag_receive_upp;
	
	SetDataBrowserCallbacks(bone_list,&dbcall);

		// columns

	add_db_column(bone_list,"Bone",kPoseBoneDBColumn,kDataBrowserTextType,200,0);
	add_db_column(bone_list,"Rot & Move",kPoseBoneMoveDBColumn,kDataBrowserTextType,250,1);
	add_db_column(bone_list,"Accel",kPoseBoneAccDBColumn,kDataBrowserTextType,60,2);
	add_db_column(bone_list,"Skip Blend",kPoseBoneBlendDBColumn,kDataBrowserTextType,80,3);
	
		// container
		
	SetDataBrowserListViewDisclosureColumn(bone_list,kPoseBoneDBColumn,FALSE);
	
	bone_list_notify_ignore=FALSE;
}

void end_bone_controls(void)
{
	DataBrowserCallbacks			dbcall;
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	SetDataBrowserCallbacks(bone_list,&dbcall);

	DisposeDataBrowserItemDataUPP(bone_list_setitem_upp);
	DisposeDataBrowserItemNotificationUPP(bone_list_notify_upp);
	DisposeDataBrowserAcceptDragUPP(&bone_list_drag_accept_proc);
	DisposeDataBrowserAddDragItemUPP(&bone_list_drag_item_proc);
	DisposeDataBrowserReceiveDragUPP(&bone_list_drag_receive_proc);
	
	DisposeControl(bone_list);
}

void resize_bone_controls(Rect *box)
{
	Rect		cbox;
	
	cbox.left=box->left+220;
	cbox.right=box->right;
	
	cbox.top=box->top;
	cbox.bottom=(box->bottom-box->top)/2;

	MoveControl(bone_list,cbox.left,cbox.top);
	SizeControl(bone_list,(cbox.right-cbox.left),(cbox.bottom-cbox.top));
}

