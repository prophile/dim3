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

#include "interface.h"
#include "top_view.h"

extern int					cr,cx,cz,cy,magnify_factor;
extern float				walk_view_y_angle;
extern Rect					top_view_box;

extern map_type				map;

/* =======================================================

      Portal In View
      
======================================================= */

bool top_view_portal_in_view(int rn)
{
	int			x,z,ex,ez;

	x=map.portals[rn].x;
	z=map.portals[rn].z;
	top_view_map_to_pane(&x,&z);
	
	if (x>top_view_box.right) return(FALSE);
	if (z>top_view_box.bottom) return(FALSE);

	ex=map.portals[rn].ex;
	ez=map.portals[rn].ez;
	top_view_map_to_pane(&ex,&ez);
	
	if (ex<top_view_box.left) return(FALSE);
	if (ez<top_view_box.top) return(FALSE);

	return(TRUE);
}

/* =======================================================

      Draw Portal Borders
      
======================================================= */

void top_view_portal_draw_block(int rn)
{
	int			x,z,ex,ez;

	x=map.portals[rn].x;
	z=map.portals[rn].z;
	top_view_map_to_pane(&x,&z);
	
	ex=map.portals[rn].ex;
	ez=map.portals[rn].ez;
	top_view_map_to_pane(&ex,&ez);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glBegin(GL_QUADS);
	glVertex2i(x,z);
	glVertex2i(ex,z);
	glVertex2i(ex,ez);
	glVertex2i(x,ez);
	glEnd();
}

void top_view_portal_draw_outline(int rn)
{
	int			x,z,ex,ez;

	x=map.portals[rn].x;
	z=map.portals[rn].z;
	top_view_map_to_pane(&x,&z);
	
	ex=map.portals[rn].ex;
	ez=map.portals[rn].ez;
	top_view_map_to_pane(&ex,&ez);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(x,z);
	glVertex2i(ex,z);
	glVertex2i(ex,ez);
	glVertex2i(x,ez);
	glEnd();
}

void top_view_portal_selection_draw(void)
{
	int			x,z,ex,ez;

	x=map.portals[cr].x;
	z=map.portals[cr].z;
	top_view_map_to_pane(&x,&z);
	
	x-=6;
	z-=6;
	
	ex=map.portals[cr].ex;
	ez=map.portals[cr].ez;
	top_view_map_to_pane(&ex,&ez);
	
	ex+=6;
	ez+=6;
	
	glLineWidth(3.0f);
	
	glColor4f(0.6f,0.0f,0.3f,0.5f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(x,z);
	glVertex2i(ex,z);
	glVertex2i(ex,ez);
	glVertex2i(x,ez);
	glEnd();
	
	glLineWidth(1.0f);
}

/* =======================================================

      Draw Portal portals
            
======================================================= */

void top_view_portal_draw(void)
{
    int			n;
	
	for (n=0;n!=map.nportal;n++) {
		if (top_view_portal_in_view(n)) top_view_portal_draw_block(n);
	}
	
	for (n=0;n!=map.nportal;n++) {
		if (top_view_portal_in_view(n)) top_view_portal_draw_outline(n);
	}
	
	for (n=0;n!=map.nportal;n++) {
		if (top_view_portal_in_view(n)) top_view_piece_draw(n);
	}
}

/* =======================================================

      Draw Current Position
      
======================================================= */

void top_view_portal_position_draw(void)
{
    int			x,z,k,px[4],pz[4];
   
    x=cx;
    z=cz;
	top_view_map_to_pane(&x,&z);
	
	k=(1000*magnify_factor)/magnify_size;
	
	px[0]=x-k;
	pz[0]=z+k;
	
	px[1]=x;
	pz[1]=z-k;
	
	px[2]=x+k;
	pz[2]=z+k;
	
	px[3]=x;
	pz[3]=z+(k>>1);
	
	rotate_2D_polygon(4,px,pz,x,z,walk_view_y_angle);
	
	glColor4f(0.0f,0.0f,0.0f,0.75f);
	
	glBegin(GL_QUADS);
	glVertex2i(px[0],pz[0]);
	glVertex2i(px[1],pz[1]);
	glVertex2i(px[2],pz[2]);
	glVertex2i(px[3],pz[3]);
	glEnd();
}

