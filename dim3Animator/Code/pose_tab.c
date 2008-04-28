/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Pose Tab

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

ControlRef						pose_list;

DataBrowserItemDataUPP			pose_list_setitem_upp;
DataBrowserItemNotificationUPP	pose_list_notify_upp;

extern int						cur_mesh,cur_pose,cur_bone,cur_bone_move;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;
extern ControlRef				tab_list;

/* =======================================================

      Redraw a Row
      
======================================================= */

void redraw_pose_row(void)
{
    DataBrowserItemID		dbitem;
    
	if (cur_pose==-1) return;

    if (cur_bone_move==-1) {
        dbitem=cur_pose+2;
        UpdateDataBrowserItems(pose_list,kDataBrowserNoItem,1,&dbitem,kDataBrowserItemNoProperty,kDataBrowserNoItem);
    }
    else {
        dbitem=((cur_pose+2)*1000)+cur_bone_move;
        UpdateDataBrowserItems(pose_list,(cur_pose+2),1,&dbitem,kDataBrowserItemNoProperty,kDataBrowserNoItem);
    }
    
    Draw1Control(pose_list);			// this shouldn't be necessary, but it seems to be
}

/* =======================================================

      Create Bone Children ID
      
======================================================= */

void create_db_bone_list(DataBrowserItemID itemID,DataBrowserItemID *boneID)
{
    int				i;
    
	for ((i=0);(i!=model.nbone);i++) {
		boneID[i]=(itemID*1000)+i;
	}
}

/* =======================================================

      Pose List Callbacks
      
======================================================= */

static pascal OSStatus pose_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int						k,i;
	char					txt[256],s_mov_x[32],s_mov_y[32],s_mov_z[32],s_mov_acc[32];
	model_bone_move_type	*bone_move;
	CFStringRef				cfstr;
	
	if (changeValue) {
		return(errDataBrowserPropertyNotSupported);
	}
	
	switch (property) {
		
		case kPoseNameDBColumn:
			if (itemID>=1000) return(noErr);

			if (itemID==1) {
				cfstr=CFStringCreateWithCString(kCFAllocatorDefault,"Neutral",kCFStringEncodingMacRoman);
			}
			else {
				cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.poses[itemID-2].name,kCFStringEncodingMacRoman);
			}
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kPoseBoneDBColumn:
			if (itemID<1000) return(noErr);

			k=itemID/1000;
			i=itemID-(k*1000);
			k-=2;
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

		case kPoseBoneMoveDBColumn:
			if (itemID<1000) return(noErr);

			k=itemID/1000;
			i=itemID-(k*1000);
			k=k-2;
			
			bone_move=&model.poses[k].bone_moves[i];
			
			string_convert_float(s_mov_x,bone_move->mov.x);
			string_convert_float(s_mov_y,bone_move->mov.y);
			string_convert_float(s_mov_z,bone_move->mov.z);
			
			sprintf(txt,"(%d,%d,%d) / (%s,%s,%s)",(int)bone_move->rot.x,(int)bone_move->rot.y,(int)bone_move->rot.z,s_mov_x,s_mov_y,s_mov_z);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kPoseBoneAccDBColumn:
			if (itemID<1000) return(noErr);

			k=itemID/1000;
			i=itemID-(k*1000);
			k=k-2;
			
			bone_move=&model.poses[k].bone_moves[i];
			
			string_convert_float(s_mov_acc,bone_move->acceleration);
			
			sprintf(txt,"%s",s_mov_acc);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
		
		case kPoseBoneBlendDBColumn:
			if (itemID<1000) return(noErr);

			k=itemID/1000;
			i=itemID-(k*1000);
			k=k-2;
			
			bone_move=&model.poses[k].bone_moves[i];
			
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
			SetDataBrowserItemDataBooleanValue(itemData,((itemID>1)&&(itemID<1000)));
			return(noErr);
			
	}
	
	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void pose_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int						k,i;
	DataBrowserItemID		boneID[max_model_bone];
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			if (itemID>=1000) {
				k=itemID/1000;
				i=itemID-(k*1000);
				k=k-2;
				dialog_bone_move_settings_run(&model.poses[k].bone_moves[i]);
				redraw_pose_row();
				model_view_reset=TRUE;
				break;
			}
			k=itemID-2;
			if (k>=0) {
				dialog_pose_settings_run(&model.poses[k]);
                redraw_pose_row();
			}
			break;
	
		case kDataBrowserItemSelected:
			if (itemID>=1000) {
				k=itemID/1000;
				i=itemID-(k*1000);
				k=k-2;
				cur_pose=k;
				cur_bone=cur_bone_move=i;
			}
			else {
				cur_pose=itemID-2;
				cur_bone=cur_bone_move=-1;
			}
			model_view_reset=TRUE;
			break;
			
		case kDataBrowserContainerOpened:
			create_db_bone_list(itemID,boneID);
			AddDataBrowserItems(pose_list,itemID,model.nbone,boneID,kDataBrowserItemNoProperty);
			break;
	}
}

/* =======================================================

      Set Pose List
      
======================================================= */

void reset_pose_tab(int cpose,int cbonemove)
{
	DataBrowserItemID		itemID;
	
	RemoveDataBrowserItems(pose_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	
		// add poses
	
	AddDataBrowserItems(pose_list,kDataBrowserNoItem,(model.npose+1),NULL,kDataBrowserItemNoProperty);
			
		// select current pose/bone move
				
	cur_pose=cpose;
	cur_bone=cur_bone_move=cbonemove;
	
	if (cur_pose==-1) {
		itemID=1;
	}
	else {
		itemID=cur_pose+2;
		OpenDataBrowserContainer(pose_list,itemID);
		
		if (cur_bone_move!=-1) {
			itemID=(1000*itemID)+cur_bone_move;
		}
	}
	
	SetDataBrowserSelectedItems(pose_list,1,&itemID,kDataBrowserItemsAssign);
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
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;

		// pose data browser

	CreateDataBrowserControl(wind,box,kDataBrowserListView,&pose_list);
    
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

	add_db_column(pose_list,"Name",kPoseNameDBColumn,kDataBrowserTextType,100,0);
	add_db_column(pose_list,"Bone",kPoseBoneDBColumn,kDataBrowserTextType,200,1);
	add_db_column(pose_list,"Rot & Move",kPoseBoneMoveDBColumn,kDataBrowserTextType,250,2);
	add_db_column(pose_list,"Accel",kPoseBoneAccDBColumn,kDataBrowserTextType,60,3);
	add_db_column(pose_list,"Skip Blend",kPoseBoneBlendDBColumn,kDataBrowserTextType,80,4);

	SetDataBrowserListViewDisclosureColumn(pose_list,kPoseNameDBColumn,FALSE);
	
	EmbedControl(pose_list,tab_list);
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
	SizeControl(pose_list,(box->right-box->left),(box->bottom-box->top));
}

/* =======================================================

      Hide and Show Pose Controls
      
======================================================= */

void show_pose_controls(void)
{
    ShowControl(pose_list);
	SetKeyboardFocus(model_wind,pose_list,1);
	Draw1Control(pose_list);
}

void hide_pose_controls(void)
{
    HideControl(pose_list);
}

