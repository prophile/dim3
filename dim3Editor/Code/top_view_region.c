/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Top View Regions Routines

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

extern map_type				map;


// supergumba -- can probably delete a lot of this

/* =======================================================

      Polygon Routines
      
======================================================= */

void top_view_make_rect(int rn,int lft,int rgt,int top,int bot,Rect *box)
{
	portal_type			*portal;
	
	portal=&map.portals[rn];
	
	lft+=portal->x;
	top+=portal->z;
	top_view_map_to_pane(&lft,&top);
	
	rgt+=portal->x;
	bot+=portal->z;
	top_view_map_to_pane(&rgt,&bot);
	
	box->left=lft;
	box->right=rgt;
	box->top=top;
	box->bottom=bot;
}

void top_view_make_rect_by_pos(d3pos *pos,int sz,Rect *box)
{
	top_view_make_rect(pos->rn,(pos->x-sz),(pos->x+sz),(pos->z-sz),(pos->z+sz),box);
}

void top_view_make_poly(int rn,int ptsz,int *x,int *z,PolyHandle *tply)
{
	int				i,dx,dz;
	portal_type		*portal;
	
	portal=&map.portals[rn];
	
	*tply=OpenPoly();
	
	dx=x[ptsz-1]+portal->x;
	dz=z[ptsz-1]+portal->z;
	top_view_map_to_pane(&dx,&dz);
	MoveTo(dx,dz);
	
	for (i=0;i!=ptsz;i++) {
		dx=x[i]+portal->x;
		dz=z[i]+portal->z;
		top_view_map_to_pane(&dx,&dz);
		LineTo(dx,dz);
	}
	
	ClosePoly();
}

bool top_view_line_click(int rn,int lx,int lz,int rx,int rz,int tx,int tz)
{
	int				dx,dz;
	int				px[4],pz[4];
	portal_type		*portal;
	
	portal=&map.portals[rn];
			
	lx+=portal->x;
	lz+=portal->z;
	top_view_map_to_pane(&lx,&lz);
			
	rx+=portal->x;
	rz+=portal->z;
	top_view_map_to_pane(&rx,&rz);
			
	dx=rx-lx;
	if (dx<0) dx=-dx;
	dz=rz-lz;
	if (dz<0) dz=-dz;

	if ((dx==0) && (dz==0)) {
		px[0]=px[3]=lx-4;
		px[1]=px[2]=lx+4;
		pz[0]=pz[1]=lz-4;
		pz[2]=pz[3]=lz+4;
	}
	else {
		if (dz>dx) {
			px[0]=lx-4;
			px[1]=rx-4;
			px[2]=rx+4;
			px[3]=lx+4;
			pz[0]=pz[3]=lz;
			pz[1]=pz[2]=rz;
		}
		else {
			px[0]=px[3]=lx;
			px[1]=px[2]=rx;
			pz[0]=lz-4;
			pz[1]=rz-4;
			pz[2]=rz+4;
			pz[3]=lz+4;
		}
	}

	return(polygon_2D_point_inside(4,px,pz,tx,tz));
}

bool top_view_polygon_click(int rn,int ptsz,int *x,int *z,int tx,int tz)
{
	int				i,px[8],pz[8];
	portal_type		*portal;
	
	portal=&map.portals[rn];
		
	for (i=0;i!=ptsz;i++) {
		px[i]=x[i]+portal->x;
		pz[i]=z[i]+portal->z;
		top_view_map_to_pane(&px[i],&pz[i]);
	}
	
	return(polygon_2D_point_inside(ptsz,px,pz,tx,tz));
}

bool top_view_pos_rect_click(d3pos *pos,int sz,int tx,int tz)
{
	int					x,z;
	portal_type			*portal;
	
	portal=&map.portals[pos->rn];
	
	x=pos->x+portal->x;
	z=pos->z+portal->z;
	top_view_map_to_pane(&x,&z);
		
	if (tx<(x-sz)) return(FALSE);
	if (tx>(x+sz)) return(FALSE);
	if (tz<(z-sz)) return(FALSE);
	if (tz>(z+sz)) return(FALSE);
	
	return(TRUE);
}

bool top_view_rect_click(int rn,int lft,int rgt,int top,int bot,int tx,int tz)
{
	portal_type			*portal;
	
	portal=&map.portals[rn];
	
	lft+=portal->x;
	top+=portal->z;
	top_view_map_to_pane(&lft,&top);

	if (tx<lft) return(FALSE);
	if (tz<top) return(FALSE);
	
	rgt+=portal->x;
	bot+=portal->z;
	top_view_map_to_pane(&rgt,&bot);
	
	if (tx>rgt) return(FALSE);
	if (tz>bot) return(FALSE);
	
	return(TRUE);
}


