/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Mac UI Code

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3server.h"
#endif

#ifdef D3_OS_MAC

#define UI_WIN_X			10
#define UI_WIN_Y			50
#define UI_WIN_WIDTH		640
#define UI_WIN_HEIGHT		480

#define UI_TEXT_WIDTH		8
#define UI_TEXT_HEIGHT		12

#define UI_TEXT_FACE		"\pCourier"
#define UI_TEXT_SIZE		12

extern bool					quit;

WindowRef					wind;
EventHandlerRef				wind_event;
EventHandlerUPP				wind_upp;

/* =======================================================

      Window Events
      
======================================================= */

OSStatus ui_wind_event_handler(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	Point				pt;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					status_draw();
					return(noErr);
					
				case kEventWindowClose:
					QuitApplicationEventLoop();
					return(noErr);
					
				case kEventWindowClickContentRgn:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					
					SetPort(GetWindowPort(wind));
					GlobalToLocal(&pt);
					
					status_click((pt.h/UI_TEXT_WIDTH),(pt.v/UI_TEXT_HEIGHT));
					return(noErr);
					
			}
			break;
									
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Start/Stop UI
      
======================================================= */

void ui_start(void)
{
	Rect			box;
	EventTypeSpec	wind_events[]={	{kEventClassWindow,kEventWindowDrawContent},
									{kEventClassWindow,kEventWindowClose},
									{kEventClassWindow,kEventWindowClickContentRgn}};
	
		// create window
		
	SetRect(&box,UI_WIN_X,UI_WIN_Y,(UI_WIN_X+UI_WIN_WIDTH),(UI_WIN_Y+UI_WIN_HEIGHT));
	CreateNewWindow(kDocumentWindowClass, kWindowCloseBoxAttribute|kWindowCollapseBoxAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&box,&wind);

	SetWTitle(wind,"\pdim3 Server");
	ShowWindow(wind);
	
		// set font
		
	SetPort(GetWindowPort(wind));
	TextFont(FMGetFontFamilyFromName(UI_TEXT_FACE));
	TextSize(UI_TEXT_SIZE);

		// events
		
	wind_upp=NewEventHandlerUPP(ui_wind_event_handler);
	InstallEventHandler(GetWindowEventTarget(wind),wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&wind_event);
}

void ui_end(void)
{
	RemoveEventHandler(wind_event);
	DisposeEventHandlerUPP(wind_upp);
	
	DisposeWindow(wind);
}

/* =======================================================

      UI Pump Events
      
======================================================= */

void ui_pump(void)
{
	RunApplicationEventLoop();

	quit=TRUE;			// once loop returns we are in quit
}

void ui_quit(void)
{
	QuitApplicationEventLoop();
}

/* =======================================================

      UI Drawing
      
======================================================= */

void ui_draw_text(int x,int y,int wid,char *str,float *col,bool invert)
{
	Rect				box;
	RGBColor			blackcolor={0x0,0x0,0x0},whitecolor={0xFFFF,0xFFFF,0xFFFF},txtcolor;
	GrafPtr				saveport;

	GetPort(&saveport);
	SetPort(GetWindowPort(wind));

		// to screen size

	x*=UI_TEXT_WIDTH;
	y*=UI_TEXT_HEIGHT;

		// erase area

	box.left=x;
	box.right=x+(wid*UI_TEXT_WIDTH);
	box.top=y;
	box.bottom=y+UI_TEXT_HEIGHT;

	if (!invert) {
		RGBForeColor(&whitecolor);
	}
	else {
		RGBForeColor(&blackcolor);
	}

	PaintRect(&box);

		// draw text

	if (str!=NULL) {

		if (col==NULL) {
			if (invert) {
				RGBForeColor(&whitecolor);
			}
			else {
				RGBForeColor(&blackcolor);
			}
		}
		else {
			txtcolor.red=(int)(((float)0xFFFF)*col[0]);
			txtcolor.green=(int)(((float)0xFFFF)*col[1]);
			txtcolor.blue=(int)(((float)0xFFFF)*col[2]);
			RGBForeColor(&txtcolor);
		}

		y+=((UI_TEXT_HEIGHT*3)>>2);
		MoveTo(x,y);

		DrawText(str,0,strlen(str));
	}

		// finish drawing

	SetPort(saveport);
	
	if (QDIsPortBuffered(GetWindowPort(wind))) QDFlushPortBuffer(GetWindowPort(wind),NULL);
}

void ui_draw_section(int x,int y,int wid,int high,char *title)
{
	int					txt_wid,lft,rgt,top,bot;
	RGBColor			bluecolor={0x0,0x0,0xFFFF};
	GrafPtr				saveport;

	GetPort(&saveport);
	SetPort(GetWindowPort(wind));
	
	RGBForeColor(&bluecolor);

		// draw rectangle

	lft=(x*UI_TEXT_WIDTH);
	top=y*UI_TEXT_HEIGHT;
	rgt=lft+(wid*UI_TEXT_WIDTH);
	bot=top+(high*UI_TEXT_HEIGHT);

		// get title break

	txt_wid=strlen(title)*UI_TEXT_WIDTH;

		// draw line

	PenSize(2,2);
	
	lft+=(UI_TEXT_WIDTH/2);
	rgt-=(UI_TEXT_WIDTH/2);
	top+=(UI_TEXT_HEIGHT/2);
	bot-=(UI_TEXT_HEIGHT/2);
	
	MoveTo((lft+UI_TEXT_WIDTH),top);
	LineTo(lft,top);
	LineTo(lft,bot);
	LineTo(rgt,bot);
	LineTo(rgt,top);
	LineTo((lft+txt_wid+UI_TEXT_WIDTH),top);
	
	PenNormal();

		// finish drawing

	SetPort(saveport);

		// draw text

	ui_draw_text((x+2),y,strlen(title),title,NULL,FALSE);
}

#endif
