/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: NIB Dialog Utilities

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

#include "dialog.h"

/* =======================================================

      Open Dialogs
      
======================================================= */

void dialog_open(WindowRef *wind,char *name)
{
	IBNibRef				nib;
	CFStringRef				cf_str;
	
	InitCursor();
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Animator",kCFStringEncodingMacRoman);
	CreateNibReference(cf_str,&nib);
	CFRelease(cf_str);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,name,kCFStringEncodingMacRoman);
	CreateWindowFromNib(nib,cf_str,wind);
	CFRelease(cf_str);
	
	DisposeNibReference(nib);
}

/* =======================================================

      Text and Number Utilities
      
======================================================= */

void dialog_set_text(WindowRef wind,unsigned long sig,int id,char *str)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	if (str!=NULL) {
		SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,strlen(str),str);
	}
	else {
		SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,0,NULL);
	}
}

void dialog_get_text(WindowRef wind,unsigned long sig,int id,char *str,int len)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	memset(str,0x0,len);
	GetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,len,str,NULL);
	str[len-1]=0x0;
}

void dialog_set_int(WindowRef wind,unsigned long sig,int id,int value)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	sprintf(str,"%d",value);
	SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,strlen(str),str);
}

int dialog_get_int(WindowRef wind,unsigned long sig,int id)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	memset(str,0x0,32);
	GetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,32,str,NULL);
	str[31]=0x0;
	
	return(atoi(str));
}

void dialog_set_float(WindowRef wind,unsigned long sig,int id,float value)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	string_convert_float(str,value);
	SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,strlen(str),str);
}

float dialog_get_float(WindowRef wind,unsigned long sig,int id)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	memset(str,0x0,32);
	GetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,32,str,NULL);
	str[31]=0x0;
	
	return(atof(str));
}

/* =======================================================

      Boolean Utilities
      
======================================================= */

void dialog_set_boolean(WindowRef wind,unsigned long sig,int id,bool b)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,(b?1:0));
}

bool dialog_get_boolean(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	return(GetControl32BitValue(ctrl)!=0);
}

/* =======================================================

      Combo Utilities
      
======================================================= */

void dialog_clear_combo(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	MenuRef			menu;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	menu=GetControlPopupMenuHandle(ctrl);
	DeleteMenuItems(menu,1,CountMenuItems(menu));
	
	SetControl32BitMaximum(ctrl,0);
}

void dialog_add_combo_item(WindowRef wind,unsigned long sig,int id,char *str,unsigned long cmd)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	MenuRef			menu;
	CFStringRef		cf_str;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	menu=GetControlPopupMenuHandle(ctrl);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
	AppendMenuItemTextWithCFString(menu,cf_str,0,cmd,NULL);
	CFRelease(cf_str);
	
	SetControl32BitMaximum(ctrl,CountMenuItems(menu));
}

void dialog_set_combo(WindowRef wind,unsigned long sig,int id,int idx)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,(idx+1));
}

int dialog_get_combo(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	return(GetControl32BitValue(ctrl)-1);
}

/* =======================================================

      Radio Button Utilities
      
======================================================= */

void dialog_set_radio_buttons(WindowRef wind,unsigned long sig,int value,int count)
{
	int				n;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	for (n=0;n!=count;n++) {
		ctrl_id.signature=sig;
		ctrl_id.id=n;
		GetControlByID(wind,&ctrl_id,&ctrl);
		SetControl32BitValue(ctrl,(value==n)?1:0);
	}
}

int dialog_get_radio_buttons(WindowRef wind,unsigned long sig,int count)
{
	int				n;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	for (n=0;n!=count;n++) {
		ctrl_id.signature=sig;
		ctrl_id.id=n;
		GetControlByID(wind,&ctrl_id,&ctrl);
		if (GetControl32BitValue(ctrl)!=0) return(n);
	}
	
	return(0);
}

/* =======================================================

      Value Utilities
      
======================================================= */

void dialog_set_value(WindowRef wind,unsigned long sig,int id,int value)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,value);
}

int dialog_get_value(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	return(GetControl32BitValue(ctrl));
}

/* =======================================================

      Tab Utilities
      
======================================================= */

void dialog_switch_tab(WindowRef wind,unsigned long sig,int id,int npane)
{
	int				n,index;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
		// get tab
		
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	index=GetControl32BitValue(ctrl)-1;
	
		// hide all other panes
		
	for (n=0;n!=npane;n++) {
		if (index!=n) {
			ctrl_id.signature='tabt';
			ctrl_id.id=n;
			GetControlByID(wind,&ctrl_id,&ctrl);
			SetControlVisibility(ctrl,FALSE,FALSE);
		}
	}
	
		// show selected pane
		
	ctrl_id.signature='tabt';
	ctrl_id.id=index;
	GetControlByID(wind,&ctrl_id,&ctrl);
	SetControlVisibility(ctrl,TRUE,TRUE);
}

void dialog_set_tab(WindowRef wind,unsigned long sig,int id,int index,int npane)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
		// set tab
		
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,(index+1));
	
		// hide show panes
		
	dialog_switch_tab(wind,sig,id,npane);
}

/* =======================================================

      Color Utilities
      
======================================================= */

void dialog_set_color_icon(ControlRef ctrl,d3col *col)
{
	ControlButtonContentInfo	info;
	Rect						box;
	RGBColor					rgb,black={0x0,0x0,0x0};
 	PicHandle					pic;

		// draw the color
		
	SetRect(&box,0,0,24,12);
	
	pic=OpenPicture(&box);
	rgb.red=(int)(col->r*(float)0xFFFF);
	rgb.green=(int)(col->g*(float)0xFFFF);
	rgb.blue=(int)(col->b*(float)0xFFFF);
	RGBForeColor(&rgb);
	PaintRect(&box);
	RGBForeColor(&black);
	FrameRect(&box);
	ClosePicture();
	
		// set it to button
		
	info.contentType=kControlContentPictHandle;
	info.u.picture=pic;

	SetBevelButtonContentInfo(ctrl,&info);
	
		// set the internal data
		
	SetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),col);
}

static pascal OSStatus dialog_set_color_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	d3col			col;
	RGBColor		color;
	Point			pt;
	WindowRef		wind;
	ControlRef		ctrl;
	
	GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
	wind=GetControlOwner(ctrl);
	
	GetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),NULL,&col);

	color.red=(int)(col.r*(float)0xFFFF);
	color.green=(int)(col.g*(float)0xFFFF);
	color.blue=(int)(col.b*(float)0xFFFF);
	
	pt.h=pt.v=-1;
	
	if (GetColor(pt,"\pChoose the Light Color:",&color,&color)) {
		col.r=((float)color.red/(float)0xFFFF);
		col.g=((float)color.green/(float)0xFFFF);
		col.b=((float)color.blue/(float)0xFFFF);
	
		dialog_set_color_icon(ctrl,&col);
		Draw1Control(ctrl);
	}
	
	return(eventNotHandledErr);
}

void dialog_set_color(WindowRef wind,unsigned long sig,int id,d3col *col)
{
	ControlRef					ctrl;
	ControlID					ctrl_id;
	EventHandlerUPP				ctrl_event_upp;
	EventTypeSpec				ctrl_event_list[]={{kEventClassControl,kEventControlHit}};
	d3col						col2;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
		// if no property yet, then we can setup event
		
	if (GetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),NULL,&col2)!=0) {
		ctrl_event_upp=NewEventHandlerUPP(dialog_set_color_proc);
		InstallControlEventHandler(ctrl,ctrl_event_upp,GetEventTypeCount(ctrl_event_list),ctrl_event_list,wind,NULL);
	}
	
		// create color
		
	dialog_set_color_icon(ctrl,col);
	Draw1Control(ctrl);
}

void dialog_get_color(WindowRef wind,unsigned long sig,int id,d3col *col)
{
	ControlRef					ctrl;
	ControlID					ctrl_id;
 	ByteCount					sz;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	GetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),&sz,col);
}

/* =======================================================

      Keyboard Focus
      
======================================================= */

void dialog_set_focus(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	SetKeyboardFocus(wind,ctrl,kControlFocusNextPart);
}

/* =======================================================

      Enabling/Hiding
      
======================================================= */

void dialog_enable(WindowRef wind,unsigned long sig,int id,bool enable)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	if (enable) {
		EnableControl(ctrl);
	}
	else {
		DisableControl(ctrl);
	}
}

void dialog_hide(WindowRef wind,unsigned long sig,int id,bool show)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControlVisibility(ctrl,show,show);
}

