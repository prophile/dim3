/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Site Path View Drawing Routines

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

#include "site_path_view.h"

extern map_type				map;

extern int					cx,cz;
extern float				walk_view_y_angle;
extern Rect					site_path_view_box;

/* =======================================================

      Portal Sizes
      
======================================================= */

void site_path_view_get_portal(int rn,int *x,int *z,int *ex,int *ez)
{
	portal_type		*portal;
	
	portal=&map.portals[rn];
	
	*x=portal->x;
	*z=portal->z;
	site_path_view_map_to_pane(x,z);
	
	*ex=portal->ex;
	*ez=portal->ez;
	site_path_view_map_to_pane(ex,ez);
}

/* =======================================================

      Draw Portal View Walls
      
======================================================= */

void site_path_view_draw_portal_walls(int rn)
{
	register int				i,x,z;
	int							lx,rx,lz,rz;
	register segment_type		*seg;
	register wall_segment_data  *wall;
	RGBColor					blackcolor={0x0,0x0,0x0},bluecolor={0x0,0x0,0xFFFF};
	
		// room x,z
		
	x=map.portals[rn].x;
	z=map.portals[rn].z;
	
		// draw walls
	
	RGBForeColor(&bluecolor);
	
	seg=map.segments;
		
	for (i=0;i!=map.nsegment;i++) {
	
		if ((seg->type==sg_wall) && (seg->rn==rn)) {
		
			wall=&seg->data.wall;
			
			lx=wall->lx+x;
			lz=wall->lz+z;
			site_path_view_map_to_pane(&lx,&lz);
			rx=wall->rx+x;
			rz=wall->rz+z;
			site_path_view_map_to_pane(&rx,&rz);
			
			MoveTo(lx,lz);	
			LineTo(rx,rz);
		}
		
		seg++;
	}
	
	RGBForeColor(&blackcolor);
}

/* =======================================================

      Sight Path Utilities
      
======================================================= */

void site_path_view_get_site_path_box(int rn,Rect *box)
{
	int			x,z;
	portal_type *portal;
	
	portal=&map.portals[rn];
	
	x=(portal->x+portal->ex)/2;
	z=(portal->z+portal->ez)/2;
	site_path_view_map_to_pane(&x,&z);
	
	SetRect(box,(x-6),(z-6),(x+6),(z+6));
}

bool site_path_portal_in_path(int rn,int in_rn)
{
	int			i;
	portal_type *portal;
	
	portal=&map.portals[rn];

	for (i=0;i!=max_sight_list;i++) {
        if (portal->sight[i].rn==in_rn) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Draw Sight Path
      
======================================================= */

void site_path_view_draw_single_path(int r1,int r2)
{
	Rect		box1,box2;
	
	site_path_view_get_site_path_box(r1,&box1);
	site_path_view_get_site_path_box(r2,&box2);
	
		// the link
		
	glBegin(GL_LINES);
	glVertex2i(((box1.left+box1.right)/2),((box1.top+box1.bottom)/2));
	glVertex2i(((box2.left+box2.right)/2),((box2.top+box2.bottom)/2));
	glEnd();
	
		// the boxes
		
	glBegin(GL_QUADS);
	
	glVertex2i(box1.left,box1.top);
	glVertex2i(box1.right,box1.top);
	glVertex2i(box1.right,box1.bottom);
	glVertex2i(box1.left,box1.bottom);
	
	glVertex2i(box2.left,box2.top);
	glVertex2i(box2.right,box2.top);
	glVertex2i(box2.right,box2.bottom);
	glVertex2i(box2.left,box2.bottom);
	
	glEnd();
}

void site_path_view_draw_path_list(int rn,int idx)
{
	int				i,t,nr;
	portal_type		*portal;
	
	portal=&map.portals[rn];
	
	nr=portal->sight[idx].rn;

	for (i=0;i!=max_sight_link;i++) {
		if (portal->sight[idx].link[i]==nopath) continue;
        
        t=(int)portal->sight[idx].link[i];
        site_path_view_draw_single_path(nr,(int)portal->sight[t].rn);
        site_path_view_draw_path_list(rn,t);
	}
}

void site_path_view_draw_portal_sight_path(int rn)
{
	int				i;
	Rect			box;
	portal_type		*portal;
	
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	portal=&map.portals[rn];
    
	for (i=0;i!=max_sight_list;i++) {
        if (portal->sight[i].root) {
			site_path_view_draw_single_path(rn,(int)portal->sight[i].rn);
			site_path_view_draw_path_list(rn,i);
        }
	}

		// root box
		
	site_path_view_get_site_path_box(rn,&box);
	
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_QUADS);
	glVertex2i(box.left,box.top);
	glVertex2i(box.right,box.top);
	glVertex2i(box.right,box.bottom);
	glVertex2i(box.left,box.bottom);
	glEnd();
}

