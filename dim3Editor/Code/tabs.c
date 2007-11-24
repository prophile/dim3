/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Window Tabs

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

#include "interface.h"

extern WindowRef		mainwind;

extern int				main_wind_view;
extern map_type			map;

char					tabs_title[5][32]={"Top Down and 3D Edit","Portals Edit","Sight Path Edit","Top Down Edit","3D Edit"};
Rect					tabs_box;

/* =======================================================

      Setup Tabs
      
======================================================= */

void tabs_setup(void)
{
	return;
	
 	GetWindowPortBounds(mainwind,&tabs_box);
	
	tabs_box.top=toolbar_high;
	tabs_box.bottom=tabs_box.top+tab_high;
	tabs_box.right-=piece_wid;
}

/* =======================================================

      Draw Tabs
      
======================================================= */

void tabs_draw_single(Rect *box,char *str,bool hilite)
{
	int				y_add;
	PolyHandle		poly;
	RGBColor		blackcolor={0x0,0x0,0x0},ltgraycolor={0xAAAA,0xAAAA,0xAAAA},graycolor={0x7FFF,0x7FFF,0x7FFF};
		
	y_add=0;
	if (!hilite) y_add=3;
	
	poly=OpenPoly();
	MoveTo(box->left,(box->top+y_add));
	LineTo((box->right-5),(box->top+y_add));
	LineTo(box->right,(box->bottom-1));
	LineTo(box->left,(box->bottom-1));
	LineTo(box->left,(box->top+y_add));
	ClosePoly();
	
	if (hilite) {
		RGBForeColor(&ltgraycolor);
		TextSize(14);
	}
	else {
		RGBForeColor(&graycolor);
		TextSize(12);
	}
	PaintPoly(poly);
	
	RGBForeColor(&blackcolor);
	FramePoly(poly);
	
	KillPoly(poly);
	
	MoveTo((box->left+4),(box->bottom+(y_add-5)));
	DrawText(str,0,strlen(str));
}

void tabs_draw(void)
{
	int				n,y,xadd;
	Rect			tbox;
	RGBColor		blackcolor={0x0,0x0,0x0},ltgraycolor={0xAAAA,0xAAAA,0xAAAA};
	
	return;
	
		// draw the line
		
	RGBForeColor(&blackcolor);
	
	y=tabs_box.bottom-1;
	MoveTo(tabs_box.left,y);
	LineTo(tabs_box.right,y);
	
		// erase
		
	tbox=tabs_box;
	tbox.bottom-=2;
	
	RGBForeColor(&ltgraycolor);
	PaintRect(&tbox);
	
		// draw tabs
		
	xadd=(tabs_box.right-tabs_box.left)/5;
	
	for (n=0;n!=5;n++) {
		tbox=tabs_box;
		tbox.left=tabs_box.left+(n*xadd);
		tbox.right=tbox.left+xadd;
		
		tabs_draw_single(&tbox,tabs_title[n],(main_wind_view==n));
	}
}

/* =======================================================

      Click Tabs
      
======================================================= */

void tabs_click(Point pt)
{
	int				view;
	
	return;
	
	view=(pt.h-tabs_box.left)/((tabs_box.right-tabs_box.left)/5);
	main_wind_set_view(view);
	main_wind_draw();
}
