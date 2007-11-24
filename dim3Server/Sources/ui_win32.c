/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Win32 UI Code

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

#ifdef D3_PCH
	#include "dim3server.h"
#endif

#ifdef D3_OS_WINDOWS

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

#define UI_WIN_X			10
#define UI_WIN_Y			40
#define UI_WIN_WIDTH		640
#define UI_WIN_HEIGHT		480

#define UI_TEXT_WIDTH		8
#define UI_TEXT_HEIGHT		12

#define UI_TEXT_FACE		"Courier"
#define UI_TEXT_SIZE		9

extern bool				quit;

ATOM					wnd_rg_class;
HFONT					fnt;
HWND					wnd;

/* =======================================================

      UI Window Procedure
      
======================================================= */

LRESULT CALLBACK ui_wnd_proc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int				x,y;
	PAINTSTRUCT		ps;
	HDC				dc;

	switch (msg) {

		case WM_PAINT:
			dc=BeginPaint(hWnd,&ps);
			status_draw();
			EndPaint(hWnd,&ps);
			break;

		case WM_LBUTTONDOWN:
			x=LOWORD(lParam)/UI_TEXT_WIDTH;
			y=HIWORD(lParam)/UI_TEXT_HEIGHT;
			status_click(x,y);
			break;

		case WM_CLOSE:
			quit=TRUE;
			break;

		default:
			return(DefWindowProc(hWnd,msg,wParam,lParam));

	}

	return(0);
}

/* =======================================================

      Start/Stop UI
      
======================================================= */

void ui_start(void)
{
	RECT			wbox;
	WNDCLASSEX		wcx;
	HINSTANCE		hInst;

		// create window

	hInst=GetModuleHandle(NULL);
 
    wcx.cbSize=sizeof(wcx);
    wcx.style=0;
    wcx.lpfnWndProc=ui_wnd_proc;
    wcx.cbClsExtra=0;
    wcx.cbWndExtra=0;
    wcx.hInstance=hInst;
    wcx.hIcon=NULL;
    wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
    wcx.hbrBackground=GetStockObject( WHITE_BRUSH);
    wcx.lpszMenuName= NULL;
    wcx.lpszClassName="dim3ServerWindowClass";
    wcx.hIconSm=NULL; 

    RegisterClassEx(&wcx); 

	wbox.left=UI_WIN_X;
	wbox.right=UI_WIN_X+UI_WIN_WIDTH;
	wbox.top=UI_WIN_Y;
	wbox.bottom=UI_WIN_Y+UI_WIN_HEIGHT;

	AdjustWindowRect(&wbox,WS_OVERLAPPEDWINDOW,FALSE);

    wnd=CreateWindow("dim3ServerWindowClass","dim3 Server",WS_OVERLAPPEDWINDOW,wbox.left,wbox.top,(wbox.right-wbox.left),(wbox.bottom-wbox.top),NULL,NULL,hInst,NULL);

		// create font for window

	fnt=CreateFont(-UI_TEXT_SIZE,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,(DEFAULT_PITCH|FF_DONTCARE),UI_TEXT_FACE);

		// show window

    ShowWindow(wnd,SW_SHOW); 
}

void ui_end(void)
{
	DeleteObject(fnt);
	DestroyWindow(wnd);
	UnregisterClass("dim3ServerWindowClass",GetModuleHandle(NULL));
}

/* =======================================================

      UI Message Pump
      
======================================================= */

void ui_pump(void)
{
	MSG			msg;

	while (!quit) {
		if (GetMessage(&msg,NULL,0,0)>0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void ui_quit(void)
{
	quit=TRUE;
}

/* =======================================================

      UI Drawing
      
======================================================= */

void ui_draw_text(int x,int y,int wid,char *str,float *col,bool invert)
{
	RECT		box;
	HBRUSH		brsh;
	HGDIOBJ		*old_fnt;
	HDC			dc;

	dc=GetDC(wnd);

	SetMapMode(dc,MM_TEXT);
	SetBkMode(dc,TRANSPARENT);

	old_fnt=SelectObject(dc,fnt);

		// to screen size

	x*=UI_TEXT_WIDTH;
	y*=UI_TEXT_HEIGHT;

		// erase area

	box.left=x;
	box.right=x+(wid*UI_TEXT_WIDTH);
	box.top=y;
	box.bottom=y+UI_TEXT_HEIGHT;

	if (!invert) {
		brsh=CreateSolidBrush(RGB(0xFF,0xFF,0xFF));
	}
	else {
		brsh=CreateSolidBrush(RGB(0x0,0x0,0x0));
	}

	FillRect(dc,&box,brsh);

	DeleteObject(brsh);

		// draw text

	if (str!=NULL) {

		if (col==NULL) {
			if (invert) {
				SetTextColor(dc,RGB(0xFF,0xFF,0xFF));
			}
			else {
				SetTextColor(dc,RGB(0x0,0x0,0x0));
			}
		}
		else {
			SetTextColor(dc,RGB((int)(((float)0xFF)*col[0]),(int)(((float)0xFF)*col[1]),(int)(((float)0xFF)*col[2])));
		}

		DrawText(dc,str,strlen(str),&box,(DT_SINGLELINE|DT_BOTTOM|DT_LEFT));
	}

		// finish drawing

	SelectObject(dc,old_fnt);
	ReleaseDC(wnd,dc);
}

void ui_draw_section(int x,int y,int wid,int high,char *title)
{
	int			txt_wid,lft,rgt,top,bot;
	HPEN		pen;
	HGDIOBJ		*old_pen;
	HDC			dc;

	dc=GetDC(wnd);

	SetMapMode(dc,MM_TEXT);
	SetBkMode(dc,TRANSPARENT);

		// draw rectangle

	lft=(x*UI_TEXT_WIDTH);
	top=y*UI_TEXT_HEIGHT;
	rgt=lft+(wid*UI_TEXT_WIDTH);
	bot=top+(high*UI_TEXT_HEIGHT);

		// get title break

	txt_wid=strlen(title)*UI_TEXT_WIDTH;

		// draw line

	pen=CreatePen(PS_SOLID,2,RGB(0x0,0x0,0xFF));
	old_pen=SelectObject(dc,pen);

	lft+=(UI_TEXT_WIDTH/2);
	rgt-=(UI_TEXT_WIDTH/2);
	top+=(UI_TEXT_HEIGHT/2);
	bot-=(UI_TEXT_HEIGHT/2);

	MoveToEx(dc,(lft+UI_TEXT_WIDTH),top,NULL);
	LineTo(dc,lft,top);
	LineTo(dc,lft,bot);
	LineTo(dc,rgt,bot);
	LineTo(dc,rgt,top);
	LineTo(dc,(lft+txt_wid+(UI_TEXT_WIDTH*2)),top);

		// clean up

	SelectObject(dc,old_pen);
	DeleteObject(pen);

	ReleaseDC(wnd,dc);

		// draw text

	ui_draw_text((x+2),y,strlen(title),title,NULL,FALSE);
}

#endif
