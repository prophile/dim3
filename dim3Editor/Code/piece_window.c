/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Window

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "portal_view.h"

#define maxpiece		15

extern int				cr;

WindowRef				piecewind;
EventHandlerRef			piece_wind_event;
EventHandlerUPP			piece_wind_upp;
ControlRef				piece_ctrl[maxpiece];

char					piece_tooltip_str[maxpiece][64]={
								"Add Portal","Add Script Spot",
								"Add Light","Add Sound",
								"Add Particle","Add Scenery","Add Node",
								"Add Wall","Add Liquid Volume",
								"Add Floor","Add Ceiling",
								"Add Wall Ambient","Add Floor-Ceiling Ambient",
								"Add Primitive","Replace Primitive with New Primitive"
														};

/* =======================================================

      Piece Window Events
      
======================================================= */

OSStatus piece_wind_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	int				i,k,x,y,z;
	ControlRef		ctrl;

	GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);

	k=-1;
	
	for (i=0;i!=maxpiece;i++) {
		if (ctrl==piece_ctrl[i]) {
			k=i;
			break;
		}
	}
	
	if (k==-1) return(noErr);
	
	switch (k) {
		case 0:
			main_wind_set_view_piece_portal();
			portal_new();
			break;
		case 1:
			main_wind_set_view_piece_segment();
			piece_create_spot();
			break;
		case 2:
			piece_create_light();
			break;
		case 3:
			piece_create_sound();
			break;
		case 4:
			piece_create_particle();
			break;
		case 5:
			main_wind_set_view_piece_segment();
			piece_create_scenery();
			break;
		case 6:
			main_wind_set_view_piece_segment();
			piece_create_node();
			break;
		case 7:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,8);
			segment_add_wall(x,z,(x+8),z,(y-8),y,wc_none,wc_none,-1,TRUE);
			break;
		case 8:
			main_wind_set_view_piece_segment();
			segment_add_liquid(TRUE);
			break;
		case 9:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,1);
			segment_add_floor_box(x,z,(x+8),(z+8),(y-1),-1,TRUE);
			break;
		case 10:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,0);
			segment_add_ceiling_box(x,z,(x+8),(z+8),(y-1),-1,TRUE);
			break;
		case 11:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,4);
			segment_add_ambient_wall(x,(x+4),z,(z+4),(y-4),y,TRUE);
			break;
		case 12:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,4);
			segment_add_ambient_fc(x,z,(x+4),(z+4),y,TRUE);
			break;
		case 13:
			main_wind_set_view_piece_segment();
			primitive_add();
			break;
		case 14:
			main_wind_set_view_piece_segment();
			primitive_replace();
			break;
	}
	
	SetControlValue(piece_ctrl[k],0);
			
	return(noErr);
}

/* =======================================================

      Piece Window
      
======================================================= */

void piece_wind_open(void)
{
	int							t;
	Rect						wbox,box;
	ControlButtonContentInfo	icon_info;
	HMHelpContentRec			tag;
	EventTypeSpec				wind_events[]={{kEventClassControl,kEventControlHit}};

    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
    
	SetRect(&box,(wbox.right-46),(wbox.top+49),(wbox.right-3),((wbox.top+49)+((maxpiece*36)+2)));
	CreateNewWindow(kFloatingWindowClass,kWindowStandardFloatingAttributes|kWindowStandardHandlerAttribute,&box,&piecewind);
	ChangeWindowAttributes(piecewind,0,kWindowCloseBoxAttribute|kWindowFullZoomAttribute|kWindowCollapseBoxAttribute|kWindowResizableAttribute);
	SetWTitle(piecewind,"\pPieces");

	SetPort(GetWindowPort(piecewind));

	box.left=1;
	box.right=33;
	
	SetRect(&box,5,1,37,33);
	/*
	for (t=0;t!=maxpiece;t++) {
			
			// create button
			
		icon_info.contentType=kControlContentCIconRes;
		icon_info.u.resID=600+t;
		CreateIconControl(piecewind,&box,&icon_info,FALSE,&piece_ctrl[t]);
		
			// create tooltip
			
		tag.version=kMacHelpVersion;
		tag.tagSide=kHMOutsideLeftCenterAligned;
		SetRect(&tag.absHotRect,0,0,0,0);
		tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
		tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,piece_tooltip_str[t],kCFStringEncodingMacRoman);
		tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
		HMSetControlHelpContent(piece_ctrl[t],&tag);
		
		OffsetRect(&box,0,36);
    }
	*/
	piece_wind_upp=NewEventHandlerUPP(piece_wind_event_callback);
	InstallEventHandler(GetWindowEventTarget(piecewind),piece_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&piece_wind_event);

	ShowWindow(piecewind);
}

void piece_wind_close(void)
{
	int			t;
	
	for (t=0;t!=maxpiece;t++) {
	//	DisposeControl(piece_ctrl[t]);
	}
	
	RemoveEventHandler(piece_wind_event);
	DisposeEventHandlerUPP(piece_wind_upp);
	
	DisposeWindow(piecewind);
}

