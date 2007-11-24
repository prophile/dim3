/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Progress Window

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

WindowRef				progress_wind;
ControlRef				progress_ctrl;

/* =======================================================

      Progress Window
      
======================================================= */

void progress_wind_start(char *txt,int max)
{
    int					x,y;
    unsigned char		p_str[256];
    Rect				wbox,box;

    SetCursor(*GetCursor(watchCursor));

    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
    
    x=((wbox.right-wbox.left)/2)-160;
    y=((wbox.bottom-wbox.top)/2)-15;
    
	SetRect(&box,x,y,(x+310),(y+34));
	CreateNewWindow(kUtilityWindowClass /* kDocumentWindowClass */,kWindowStandardHandlerAttribute,&box,&progress_wind);
	
    CopyCStringToPascal(txt,p_str);
    SetWTitle(progress_wind,p_str);
	
	ShowWindow(progress_wind);
	SetPort(GetWindowPort(progress_wind));
    
    SetRect(&box,5,10,305,30);
    CreateProgressBarControl(progress_wind,&box,0,0,1,FALSE,&progress_ctrl);
	
	SetControlMaximum(progress_ctrl,max);

    QDFlushPortBuffer(GetWindowPort(progress_wind),NULL);
}

void progress_wind_update(int v)
{
    IdleControls(progress_wind);
    SetControlValue(progress_ctrl,v);
	
    QDFlushPortBuffer(GetWindowPort(progress_wind),NULL);
}

void progress_wind_close(void)
{
    DisposeControl(progress_ctrl);
	DisposeWindow(progress_wind);
    
    InitCursor();
}

