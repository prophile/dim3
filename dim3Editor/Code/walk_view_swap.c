/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Swap Control Routines

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
#include "walk_view.h"

#define view_swap_x_lft_offset			35
#define view_swap_x_rgt_offset			15
#define view_swap_y_offset				10
#define view_swap_triangle_size			8

extern int				edit_view;
extern float			walk_view_y_angle,walk_view_x_angle;

extern map_type			map;

/* =======================================================

      Draw Walk View Swap Control
	        
======================================================= */

void walk_view_swap_draw(editor_3D_view_setup *view_setup)
{
	int				lx,rx,mx,ty,by,wid,high;
	
	wid=view_setup->box.right-view_setup->box.left;
	high=view_setup->box.bottom-view_setup->box.top;
	
		// 2D drawing
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,(GLdouble)wid,(GLdouble)high,0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	
		// no depth buffer
		
	glDisable(GL_DEPTH_TEST);
		
		// swap left arrow
		
	lx=(wid-view_swap_x_lft_offset)-view_swap_triangle_size;
	rx=(wid-view_swap_x_lft_offset)+view_swap_triangle_size;
	mx=(lx+rx)>>1;
	
	ty=view_swap_y_offset;
	by=ty+(view_swap_triangle_size*2);
	
	glColor4f(1,(view_setup->swap_on?1:0),0,1);
	
	glBegin(GL_TRIANGLES);
	glVertex2i(lx,by);
	glVertex2i(mx,ty);
	glVertex2i(rx,by);
	glEnd();
	
		// swap right arrow

	lx=(wid-view_swap_x_rgt_offset)-view_swap_triangle_size;
	rx=(wid-view_swap_x_rgt_offset)+view_swap_triangle_size;
	mx=(lx+rx)>>1;

	glColor4f(1,(view_setup->swap_on?0:1),0,1);
	
	glBegin(GL_TRIANGLES);
	glVertex2i(lx,ty);
	glVertex2i(mx,by);
	glVertex2i(rx,ty);
	glEnd();
}

/* =======================================================

      Click Walk View Swap Control
	        
======================================================= */

bool walk_view_swap_click(editor_3D_view_setup *view_setup,d3pnt *pt,bool *swap_on)
{
	int				box_left,box_top,
					lx,rx,ty,by,wid,high;
	Rect			click_box;
	
	box_left=view_setup->box.left;
	box_top=view_setup->box.top;
	
	wid=view_setup->box.right-view_setup->box.left;
	high=view_setup->box.bottom-view_setup->box.top;
		
	
		// click left triangle
		
	lx=(wid-view_swap_x_lft_offset)-view_swap_triangle_size;
	rx=(wid-view_swap_x_lft_offset)+view_swap_triangle_size;
	
	ty=view_swap_y_offset;
	by=ty+(view_swap_triangle_size*2);
	
	if (view_setup->swap_on) {
		SetRect(&click_box,lx,ty,rx,by);
		OffsetRect(&click_box,box_left,box_top);
		if (main_wind_click_check_box(pt,&click_box)) {
			*swap_on=FALSE;
			main_wind_draw();
			return(TRUE);
		}
	}
	
		// click right triangle

	lx=(wid-view_swap_x_rgt_offset)-view_swap_triangle_size;
	rx=(wid-view_swap_x_rgt_offset)+view_swap_triangle_size;
	
	if (!view_setup->swap_on) {
		SetRect(&click_box,lx,ty,rx,by);
		OffsetRect(&click_box,box_left,box_top);
		if (main_wind_click_check_box(pt,&click_box)) {
			*swap_on=TRUE;
			main_wind_draw();
			return(TRUE);
		}
	}
	
	return(FALSE);
}