/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Vertex Tab

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

bool							vertex_list_notify_ignore;
ControlRef						vertex_list;

DataBrowserItemDataUPP			vertex_list_setitem_upp;
DataBrowserItemNotificationUPP	vertex_list_notify_upp;

extern int						cur_mesh,cur_pose;
extern bool						model_view_reset;
extern model_type				model;

extern WindowRef				model_wind;

/* =======================================================

      Hiliting Vertexes
      
======================================================= */

void hilite_vertex_rows(void)
{
	register int			n,nt,nitem;
	DataBrowserItemID		*itemID,vertex_sel_itemIDs[max_model_vertex];
	
	vertex_list_notify_ignore=TRUE;
	
	nt=model.meshes[cur_mesh].nvertex;
	nitem=0;
	
	itemID=vertex_sel_itemIDs;
	
	for (n=0;n!=nt;n++) {
		if (vertex_check_sel_mask(cur_mesh,n)) {
			*itemID=n+1;
			itemID++;
			nitem++;
		}
	}
	
	SetDataBrowserSelectedItems(vertex_list,nitem,vertex_sel_itemIDs,kDataBrowserItemsAssign);
	
	vertex_list_notify_ignore=FALSE;
}

/* =======================================================

      Set Vertex List
      
======================================================= */

void reset_vertex_tab(void)
{
	vertex_list_notify_ignore=TRUE;
	
		// setup vertexes
		
	RemoveDataBrowserItems(vertex_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(vertex_list,kDataBrowserNoItem,model.meshes[cur_mesh].nvertex,NULL,kDataBrowserItemNoProperty);

		// select current bone
	
	hilite_vertex_rows();
	
	vertex_list_notify_ignore=FALSE;
}

/* =======================================================

      Vertex List Callbacks
      
======================================================= */

static pascal OSStatus vertex_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				i,maj_idx,min_idx;
	char			txt[256],tag[32];
	model_mesh_type	*mesh;
	CFStringRef		cfstr;
	
	if (changeValue) return(errDataBrowserPropertyNotSupported);
	
	mesh=&model.meshes[cur_mesh];
	
	switch (property) {
		
		case kVertexPosDBColumn:
			i=itemID-1;
			sprintf(txt,"(%d,%d,%d)",mesh->vertexes[i].pnt.x,mesh->vertexes[i].pnt.y,mesh->vertexes[i].pnt.z);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kVertexBoneDBColumn:
			i=itemID-1;
			maj_idx=mesh->vertexes[i].major_bone_idx;
			min_idx=mesh->vertexes[i].minor_bone_idx;
			
			txt[0]=0x0;
			
			if (maj_idx!=-1) {
					
					// major bone
					
				memmove(tag,&model.bones[maj_idx].tag,4);
				tag[4]=0x0;
				strcat(txt,tag);
				
				if (model.bones[maj_idx].name[0]!=0x0) {
					strcat(txt," (");
					strcat(txt,model.bones[maj_idx].name);
					strcat(txt,")");
				}
				
					// minor bone
				
				if (min_idx!=-1) {
					sprintf(tag," @ %d%%",(int)(mesh->vertexes[i].bone_factor*100));
					strcat(txt,tag);
				
					strcat(txt," / ");
				
					memmove(tag,&model.bones[min_idx].tag,4);
					tag[4]=0x0;
					strcat(txt,tag);
					
					if (model.bones[min_idx].name[0]!=0x0) {
						strcat(txt," (");
						strcat(txt,model.bones[min_idx].name);
						strcat(txt,")");
					}
					
					sprintf(tag," @ %d%%",(100-(int)(mesh->vertexes[i].bone_factor*100)));
					strcat(txt,tag);
				}
			}
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,txt,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
	}

	return(errDataBrowserPropertyNotSupported);
}
  
static pascal void vertex_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				i;
	
	if (vertex_list_notify_ignore) return;
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			i=itemID-1;
			if (dialog_vertex_settings_run(&model.meshes[cur_mesh].vertexes[i])) {
				model_calculate_parents(&model);
				reset_vertex_tab();
				model_view_reset=TRUE;
			}
			break;

		case kDataBrowserItemSelected:
			vertex_set_sel_mask(cur_mesh,(itemID-1),TRUE);
			model_view_reset=TRUE;
			break;
			
		case kDataBrowserItemDeselected:
			vertex_set_sel_mask(cur_mesh,(itemID-1),FALSE);
			model_view_reset=TRUE;
			break;
	}
}

/* =======================================================

      Start and End Vertex Controls
      
======================================================= */

void start_vertex_controls(WindowRef wind,Rect *box)
{
    bool							framefocus;
	Rect							cbox;
	ControlFontStyleRec				fontstyle;
	DataBrowserCallbacks			dbcall;
	
		// vertex data browser
		
	cbox.left=box->left+440;
	cbox.right=box->right;
	
	cbox.top=(box->bottom-box->top)/2;
	cbox.bottom=box->bottom;

	CreateDataBrowserControl(wind,&cbox,kDataBrowserListView,&vertex_list);
    
	framefocus=FALSE;
	SetControlData(vertex_list,kControlNoPart,kControlDataBrowserIncludesFrameAndFocusTag,sizeof(framefocus),&framefocus);

	SetDataBrowserTableViewRowHeight(vertex_list,9);
	
	fontstyle.flags=kControlUseFontMask|kControlUseSizeMask|kControlUseJustMask;
	fontstyle.font=FMGetFontFamilyFromName("\pMonaco");
	fontstyle.size=9;
	fontstyle.just=teFlushLeft;
	SetControlFontStyle(vertex_list,&fontstyle);
	
		// data browser callbacks
		
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	vertex_list_setitem_upp=NewDataBrowserItemDataUPP(&vertex_list_item_proc);
	dbcall.u.v1.itemDataCallback=vertex_list_setitem_upp;
	
	vertex_list_notify_upp=NewDataBrowserItemNotificationUPP(&vertex_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=vertex_list_notify_upp;
	
	SetDataBrowserCallbacks(vertex_list,&dbcall);

		// columns

	add_db_column(vertex_list,"Vertexes",kVertexPosDBColumn,kDataBrowserTextType,150,0);
	add_db_column(vertex_list,"Bones",kVertexBoneDBColumn,kDataBrowserTextType,300,1);
	
	vertex_list_notify_ignore=FALSE;
}

void end_vertex_controls(void)
{
	DataBrowserCallbacks			dbcall;
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	SetDataBrowserCallbacks(vertex_list,&dbcall);

	DisposeDataBrowserItemDataUPP(vertex_list_setitem_upp);
	DisposeDataBrowserItemNotificationUPP(vertex_list_notify_upp);

	DisposeControl(vertex_list);
}

void resize_vertex_controls(Rect *box)
{
	Rect		cbox;
	
	cbox.left=box->left+440;
	cbox.right=box->right;
	
	cbox.top=(box->bottom-box->top)/2;
	cbox.bottom=box->bottom;

	MoveControl(vertex_list,cbox.left,cbox.top);
	SizeControl(vertex_list,(cbox.right-cbox.left),(cbox.bottom-cbox.top));
}

