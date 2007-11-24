/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Compass Routines

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
#include "common_view.h"
#include "walk_view.h"

#define view_compass_radius				30
#define view_compass_x_offset			40
#define view_compass_y_offset			10
#define view_compass_look_x_offset		15
#define view_compass_look_y_offset		10
#define view_compass_triangle_size		8

extern int				edit_view;
extern float			walk_view_y_angle,walk_view_x_angle;
extern Rect				walk_view_box;

extern map_type			map;

/* =======================================================

      Draw Walk View Compass
	        
======================================================= */

void walk_view_compass_draw(void)
{
	int				n,px[3],py[3],lx,rx,mx,ty,by,my,wid,high;
	bool			sel;
	GLUquadricObj	*disk;
	
	wid=walk_view_box.right-walk_view_box.left;
	high=walk_view_box.bottom-walk_view_box.top;
	
		// 2D drawing
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,(GLdouble)wid,(GLdouble)high,0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
		// no depth buffer
		
	glDisable(GL_DEPTH_TEST);
	
		// draw the circle

	disk=gluNewQuadric();
	gluQuadricNormals(disk,GLU_NONE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(((wid-view_compass_radius)-view_compass_x_offset),(view_compass_radius+view_compass_y_offset),0);
	
	glColor4f(1,1,0,0.2);
	gluDisk(disk,(view_compass_radius-5),view_compass_radius,32,32);
	
	glLoadIdentity();

	gluDeleteQuadric(disk);
	
		// Y angle triangles
		
	lx=(wid-(view_compass_radius*2))-view_compass_x_offset;
	rx=wid-view_compass_x_offset;
	mx=(lx+rx)>>1;
	
	ty=view_compass_y_offset;
	by=(view_compass_radius*2)+view_compass_y_offset;
	my=(ty+by)>>1;
	
	for (n=0;n!=360;n+=45) {
		sel=(((int)walk_view_y_angle)==n);
		glColor4f(1,(sel?0:1),0,1);
	
		px[0]=mx;
		py[0]=ty-view_compass_triangle_size;
		px[1]=mx+view_compass_triangle_size;
		py[1]=ty+view_compass_triangle_size;
		px[2]=mx-view_compass_triangle_size;
		py[2]=ty+view_compass_triangle_size;
		rotate_2D_polygon(3,px,py,mx,my,n);
	
		glBegin(GL_TRIANGLES);
		glVertex2i(px[0],py[0]);
		glVertex2i(px[1],py[1]);
		glVertex2i(px[2],py[2]);
		glEnd();
	}
	
		// X angle triangles
		
	lx=(wid-view_compass_look_x_offset)-view_compass_triangle_size;
	rx=(wid-view_compass_look_x_offset)+view_compass_triangle_size;
	mx=(lx+rx)>>1;
	
	ty=view_compass_look_y_offset;									// up arrow
	by=ty+(view_compass_triangle_size*2);
	
	sel=(walk_view_x_angle==30.0f);
	glColor4f(1,(sel?0:1),0,1);
	
	glBegin(GL_TRIANGLES);
	glVertex2i(lx,by);
	glVertex2i(mx,ty);
	glVertex2i(rx,by);
	glEnd();
	
	ty=view_compass_look_y_offset+view_compass_triangle_size+view_compass_radius;				// midddle box
	by=ty-(view_compass_triangle_size*2);
	
	sel=(walk_view_x_angle==0.0f);
	glColor4f(1,(sel?0:1),0,1);
	
	glBegin(GL_POLYGON);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	ty=view_compass_look_y_offset+(view_compass_radius*2);			// down arrow
	by=ty-(view_compass_triangle_size*2);
	
	sel=(walk_view_x_angle==-30.0f);
	glColor4f(1,(sel?0:1),0,1);
	
	glBegin(GL_TRIANGLES);
	glVertex2i(lx,by);
	glVertex2i(mx,ty);
	glVertex2i(rx,by);
	glEnd();
}

/* =======================================================

      Click Walk View Compass
	        
======================================================= */

bool walk_view_compass_click(Point pt)
{
	int				n,px[4],py[4],lft,rgt,top,bot,
					lx,rx,mx,ty,by,my,wid,high;
	Rect			box;
	
	wid=walk_view_box.right-walk_view_box.left;
	high=walk_view_box.bottom-walk_view_box.top;
		
		// click Y angle triangles
		
	lx=(wid-(view_compass_radius*2))-view_compass_x_offset;
	rx=wid-view_compass_x_offset;
	mx=(lx+rx)>>1;
	
	ty=view_compass_y_offset;
	by=(view_compass_radius*2)+view_compass_y_offset;
	my=(ty+by)>>1;
		
	for (n=0;n!=360;n+=45) {
		px[0]=mx-view_compass_triangle_size;
		py[0]=ty-view_compass_triangle_size;
		px[1]=mx+view_compass_triangle_size;
		py[1]=ty-view_compass_triangle_size;
		px[2]=mx+view_compass_triangle_size;
		py[2]=ty+view_compass_triangle_size;
		px[3]=mx-view_compass_triangle_size;
		py[3]=ty+view_compass_triangle_size;
		rotate_2D_polygon(4,px,py,mx,my,(float)n);
		polygon_2D_get_box(4,px,py,&lft,&rgt,&top,&bot);
	
		SetRect(&box,lft,top,rgt,bot);
		OffsetRect(&box,walk_view_box.left,walk_view_box.top);
		if (PtInRect(pt,&box)) {
			walk_view_y_angle=(float)n;
			main_wind_draw();
			return(TRUE);
		}
	}
	
		// click X angle triangles
		
	lx=(wid-view_compass_look_x_offset)-view_compass_triangle_size;
	rx=(wid-view_compass_look_x_offset)+view_compass_triangle_size;
	
	ty=view_compass_look_y_offset;									// up arrow
	by=ty+(view_compass_triangle_size*2);
	
	SetRect(&box,lx,ty,rx,by);
	OffsetRect(&box,walk_view_box.left,walk_view_box.top);
	if (PtInRect(pt,&box)) {
		walk_view_x_angle=30.0f;
		main_wind_draw();
		return(TRUE);
	}
	
	by=view_compass_look_y_offset+view_compass_triangle_size+view_compass_radius;				// midddle box
	ty=by-(view_compass_triangle_size*2);
	
	SetRect(&box,lx,ty,rx,by);
	OffsetRect(&box,walk_view_box.left,walk_view_box.top);
	if (PtInRect(pt,&box)) {
		walk_view_x_angle=0.0f;
		main_wind_draw();
		return(TRUE);
	}

	by=view_compass_look_y_offset+(view_compass_radius*2);			// down arrow
	ty=by-(view_compass_triangle_size*2);
	
	SetRect(&box,lx,ty,rx,by);
	OffsetRect(&box,walk_view_box.left,walk_view_box.top);
	if (PtInRect(pt,&box)) {
		walk_view_x_angle=-30.0f;
		main_wind_draw();
		return(TRUE);
	}
	
	return(FALSE);
}