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

#include "tab.h"
#include "dialog.h"

ControlRef						animate_list;

DataBrowserItemDataUPP			animate_list_setitem_upp;
DataBrowserItemNotificationUPP	animate_list_notify_upp;

extern int						cur_animate,cur_animate_pose,cur_pose;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;
extern ControlRef				tab_list;

/* =======================================================

      Redraw a Row
      
======================================================= */

void redraw_animate_row(void)
{
    DataBrowserItemID		dbitem;
    
	if (cur_animate==-1) return;

    if (cur_animate_pose==-1) {
        dbitem=cur_animate+1;
        UpdateDataBrowserItems(animate_list,kDataBrowserNoItem,1,&dbitem,kDataBrowserItemNoProperty,kDataBrowserNoItem);
    }
    else {
        dbitem=((cur_animate+1)*1000)+cur_animate_pose;
        UpdateDataBrowserItems(animate_list,(cur_animate+1),1,&dbitem,kDataBrowserItemNoProperty,kDataBrowserNoItem);
    }
    
    Draw1Control(animate_list);			// this shouldn't be necessary, but it seems to be
}

/* =======================================================

      Create Pose Children ID
      
======================================================= */

int create_db_pose_list(int canimate,DataBrowserItemID itemID,DataBrowserItemID *poseID)
{
	int					i,npose;
	model_animate_type	*animate;
	
	animate=&model.animates[canimate];
	
	npose=animate->npose_move;
	for ((i=0);(i!=npose);i++) {
		poseID[i]=(itemID*1000)+i;
	}
	
	return(npose);
}

/* =======================================================

      Animate List Callbacks
      
======================================================= */

static pascal OSStatus animate_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int						k,i,n;
	char					txt[1024];
	model_pose_move_type	*pose_move;
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
			
		case kAnimatePoseDBColumn:
			if (itemID<1000) return(noErr);

			k=itemID/1000;
			i=itemID-(k*1000);
			k--;
			
			n=model.animates[k].pose_moves[i].pose_idx;
			sprintf(txt,"  %s (%d)",model.poses[n].name,model.animates[k].pose_moves[i].msec);

			if ((i>=model.animates[k].loop_start) && (i<=model.animates[k].loop_end)) txt[0]='¥';

			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);

		case kAnimateSwayMoveDBColumn:
			if (itemID<1000) return(noErr);

			k=itemID/1000;
			i=itemID-(k*1000);
			k--;
			
			pose_move=&model.animates[k].pose_moves[i];
			
			sprintf(txt,"(%.1f,%.1f,%.1f) / (%.1f,%.1f,%.1f)",pose_move->sway.x,pose_move->sway.y,pose_move->sway.z,pose_move->mov.x,pose_move->mov.y,pose_move->mov.z);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kAnimateOtherDBColumn:
			if (itemID<1000) return(noErr);

			k=itemID/1000;
			i=itemID-(k*1000);
			k--;
			
			pose_move=&model.animates[k].pose_moves[i];
			
			txt[0]=0x0;
			
			if (pose_move->sound.name[0]!=0x0) {
				if (txt[0]!=0x0) strcat(txt,", ");
				strcat(txt,pose_move->sound.name);
			}
			
			for (n=0;n!=pose_move->particle.count;n++) {
				if (txt[0]!=0x0) strcat(txt,", ");
				strcat(txt,pose_move->particle.particles[n].name);
			}
			
			for (n=0;n!=pose_move->ring.count;n++) {
				if (txt[0]!=0x0) strcat(txt,", ");
				strcat(txt,pose_move->ring.rings[n].name);
			}
			
			if (pose_move->flash.intensity!=0) {
				if (txt[0]!=0x0) strcat(txt,", ");
				strcat(txt,"Flash");
			}
			
			if (pose_move->mesh_fade.name[0]!=0x0) {
				if (txt[0]!=0x0) strcat(txt,", ");
				strcat(txt,"Fade");
			}

			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kDataBrowserItemIsContainerProperty:
			SetDataBrowserItemDataBooleanValue(itemData,(itemID<1000));
			return(noErr);
	}

	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void animate_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int						k,i,npose;
	DataBrowserItemID		poseID[max_model_animate_pose];
	
	switch (message) {

		case kDataBrowserItemDoubleClicked:
			if (itemID>=1000) {
				k=itemID/1000;
				i=itemID-(k*1000);
				k--;
				dialog_animation_settings_run(k,i);
				redraw_animate_row();
				break;
			}
			k=itemID-1;
			dialog_animation_settings_run(k,-1);
            redraw_animate_row();
			break;
			
		case kDataBrowserItemSelected:
			if (itemID>=1000) {
				k=itemID/1000;
				i=itemID-(k*1000);
				k--;
				cur_animate=k;
				cur_animate_pose=i;
				cur_pose=model.animates[k].pose_moves[i].pose_idx;
			}
			else {
				cur_animate=itemID-1;
				cur_animate_pose=-1;
			}
			model_view_reset=TRUE;
			break;

		case kDataBrowserContainerOpened:
			npose=create_db_pose_list((itemID-1),itemID,poseID);
			AddDataBrowserItems(animate_list,itemID,npose,poseID,kDataBrowserItemNoProperty);
			break;
	}
}

/* =======================================================

      Set Animate List
      
======================================================= */

void reset_animate_tab(int canimate,int canimatepose)
{
	DataBrowserItemID		itemID;

	RemoveDataBrowserItems(animate_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	
		// add poses
	
	AddDataBrowserItems(animate_list,kDataBrowserNoItem,model.nanimate,NULL,kDataBrowserItemNoProperty);
			
		// select neutral pose
				
	cur_animate=canimate;
	cur_animate_pose=canimatepose;
	
	if (cur_animate!=-1) {

		itemID=cur_animate+1;
		OpenDataBrowserContainer(animate_list,itemID);
		
		if (cur_animate_pose!=-1) {
			itemID=(1000*itemID)+cur_animate_pose;
		}
	}
	
	SetDataBrowserSelectedItems(animate_list,1,&itemID,kDataBrowserItemsAssign);
}

/* =======================================================

      Start and End Animation Controls
      
======================================================= */

void start_animate_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;
    
		// setup data browser for animations

	CreateDataBrowserControl(wind,box,kDataBrowserListView,&animate_list);
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

	add_db_column(animate_list,"Name",kAnimateNameDBColumn,kDataBrowserTextType,100,0);
	add_db_column(animate_list,"Pose",kAnimatePoseDBColumn,kDataBrowserTextType,150,1);
	add_db_column(animate_list,"Sway & Move",kAnimateSwayMoveDBColumn,kDataBrowserTextType,190,2);
	add_db_column(animate_list,"Other",kAnimateOtherDBColumn,kDataBrowserTextType,300,3);

	SetDataBrowserListViewDisclosureColumn(animate_list,kAnimateNameDBColumn,FALSE);
	
	EmbedControl(animate_list,tab_list);
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
	SizeControl(animate_list,(box->right-box->left),(box->bottom-box->top));
}

/* =======================================================

      Show and Hide Animation Controls
      
======================================================= */

void show_animate_controls(void)
{
	ShowControl(animate_list);
	
	SetKeyboardFocus(model_wind,animate_list,1);
	Draw1Control(animate_list);
}

void hide_animate_controls(void)
{
	HideControl(animate_list);
}

