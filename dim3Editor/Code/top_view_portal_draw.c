/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Top View Portal Draw Routines

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

#include "top_view.h"

extern int					cr,cx,cz,cy;
extern float				walk_view_y_angle;

extern map_type				map;

/* =======================================================

      Draw Portal Borders
      
======================================================= */

void top_view_portal_draw_outline(Rect *box)
{
	Pattern		gray;
	
	box->left--;
	box->top--;
	box->right+=2;
	box->bottom+=2;
	GetQDGlobalsGray(&gray);
	PenPat(&gray);
	FrameRect(box);
	PenNormal();
}

void top_view_portal_selection_draw(void)
{
	int			x,z,ex,ez;
	Rect		box;
	Pattern		gray;
	RGBColor	blackcolor={0x0,0x0,0x0},purplecolor={0x9999,0x0,0x6666};

	x=map.portals[cr].x;
	z=map.portals[cr].z;
	top_view_map_to_pane(&x,&z);
	
	ex=map.portals[cr].ex;
	ez=map.portals[cr].ez;
	top_view_map_to_pane(&ex,&ez);
			
	SetRect(&box,(x-8),(z-8),(ex+8),(ez+8));
	
	RGBForeColor(&purplecolor);
	PenSize(3,3);
	GetQDGlobalsGray(&gray);
	PenPat(&gray);
	FrameRect(&box);
	PenNormal();
    
	RGBForeColor(&blackcolor);
}

/* =======================================================

      Draw Portal portals
            
======================================================= */

void top_view_portal_draw(Rect *clipbox)
{
    int			i,x,z,ex,ez;
    Rect		box;
    
	for ((i=0);(i!=map.nportal);i++) {
		x=map.portals[i].x;
		z=map.portals[i].z;
		top_view_map_to_pane(&x,&z);
	
		ex=map.portals[i].ex;
		ez=map.portals[i].ez;
		top_view_map_to_pane(&ex,&ez);
        
        if (x>clipbox->right) continue;
        if (ex<clipbox->left) continue;
        if (z>clipbox->bottom) continue;
        if (ez<clipbox->top) continue;

		top_view_piece_draw(i,clipbox);

		SetRect(&box,x,z,ex,ez);
		top_view_portal_draw_outline(&box);
	}
}

/* =======================================================

      Draw Current Position
      
======================================================= */

void top_view_portal_position_draw(void)
{
    int			x,z,px[3],pz[3];
 	RGBColor	blackcolor={0x0,0x0,0x0};
   
    x=cx/map_enlarge;
    z=cz/map_enlarge;
	top_view_map_to_pane(&x,&z);
	
	px[0]=x-5;
	pz[0]=z+5;
	
	px[1]=x;
	pz[1]=z-5;
	
	px[2]=x+5;
	pz[2]=z+5;
	
	rotate_2D_polygon(3,px,pz,x,z,walk_view_y_angle);
    
	RGBForeColor(&blackcolor);
    PenSize(2,2);
    
	MoveTo(px[0],pz[0]);
	LineTo(px[1],pz[1]);
	LineTo(px[2],pz[2]);

    PenSize(1,1);
}

