/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Clipping Functions

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Clip Lines to Horizontal/Vertical Lines
      
======================================================= */

void clip_line_to_2D_horizontal_line(int clip_z,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2)
{
	int			dx,dy,dz,zchg;

	dx=x2-x1;
	dy=y2-y1;
	dz=z2-z1;

	zchg=clip_z-z1;

	if (dz==0) {
		*x=x1;
		*y=y1;
	}
	else {
		*x=x1+((dx*zchg)/dz);
		*y=y1+((dy*zchg)/dz);
	}
	
	*z=clip_z;
}

void clip_line_to_2D_vertical_line(int clip_x,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2)
{
	int			dx,dy,dz,xchg;

	dx=x2-x1;
	dy=y2-y1;
	dz=z2-z1;

	xchg=clip_x-x1;

	*x=clip_x;
	
	if (dx==0) {
		*y=y1;
		*z=z1;
	}
	else {
		*y=y1+((dy*xchg)/dx);
		*z=z1+((dz*xchg)/dx);
	}
}

void clip_extend_line_to_2D_horizontal_line(int clip_z,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2)
{
	int			cx,cy,cz;
	
	cx=(*x)*100;
	cy=(*y)*100;
	cz=(*z)*100;
	
	clip_line_to_2D_horizontal_line((clip_z*100),&cx,&cy,&cz,(x1*100),(y1*100),(z1*100),(x2*100),(y2*100),(z2*100));

	*x=cx/100;
	*y=cy/100;
	*z=cz/100;
}

void clip_extend_line_to_2D_vertical_line(int clip_x,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2)
{
	int			cx,cy,cz;
	
	cx=(*x)*100;
	cy=(*y)*100;
	cz=(*z)*100;
	
	clip_line_to_2D_vertical_line((clip_x*100),&cx,&cy,&cz,(x1*100),(y1*100),(z1*100),(x2*100),(y2*100),(z2*100));

	*x=cx/100;
	*y=cy/100;
	*z=cz/100;
}

/* =======================================================

      Clip Polygons To 2D Rect
      
======================================================= */

int clip_polygon_to_2D_line_top_z(int ptsz,int *x,int *y,int *z,int clip_z)
{
	int			n,neg_idx,pos_idx,cx,cy,cz,
				nx[32],ny[32],nz[32],nptsz;

	nptsz=0;

	n=0;

	while (TRUE) {

			// outside of clipping line?

		if (z[n]>=clip_z) {

			nx[nptsz]=x[n];
			ny[nptsz]=y[n];
			nz[nptsz]=z[n];
			nptsz++;

			n++;
			if (n>=ptsz) break;
			continue;
		}

			// get vertexes on either side

		neg_idx=n-1;
		if (neg_idx<0) neg_idx=ptsz-1;
				
		pos_idx=n+1;
		if (pos_idx>=ptsz) pos_idx=0;

			// if both vertexes are outside, remove vertex

		if ((z[neg_idx]<=clip_z) && (z[pos_idx]<=clip_z)) {
			n++;
			if (n>=ptsz) break;
			continue;
		}

			// clip each line

		if (z[neg_idx]>clip_z) {
			clip_line_to_2D_horizontal_line(clip_z,&cx,&cy,&cz,x[neg_idx],y[neg_idx],z[neg_idx],x[n],y[n],z[n]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		if (z[pos_idx]>clip_z) {
			clip_line_to_2D_horizontal_line(clip_z,&cx,&cy,&cz,x[n],y[n],z[n],x[pos_idx],y[pos_idx],z[pos_idx]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		n++;
		if (n>=ptsz) break;
	}

		// only allow polygons up to 8 vertexes

	if (nptsz>8) nptsz=8;

	for (n=0;n!=nptsz;n++) {
		x[n]=nx[n];
		y[n]=ny[n];
		z[n]=nz[n];
	}

	return(nptsz);
}

int clip_polygon_to_2D_line_bottom_z(int ptsz,int *x,int *y,int *z,int clip_z)
{
	int			n,neg_idx,pos_idx,cx,cy,cz,
				nx[32],ny[32],nz[32],nptsz;

	nptsz=0;

	n=0;

	while (TRUE) {

			// outside of clipping line?

		if (z[n]<=clip_z) {

			nx[nptsz]=x[n];
			ny[nptsz]=y[n];
			nz[nptsz]=z[n];
			nptsz++;

			n++;
			if (n>=ptsz) break;
			continue;
		}

			// get vertexes on either side

		neg_idx=n-1;
		if (neg_idx<0) neg_idx=ptsz-1;
				
		pos_idx=n+1;
		if (pos_idx>=ptsz) pos_idx=0;

			// if both vertexes are outside, remove vertex

		if ((z[neg_idx]>=clip_z) && (z[pos_idx]>=clip_z)) {
			n++;
			if (n>=ptsz) break;
			continue;
		}

			// clip each line

		if (z[neg_idx]<clip_z) {
			clip_line_to_2D_horizontal_line(clip_z,&cx,&cy,&cz,x[neg_idx],y[neg_idx],z[neg_idx],x[n],y[n],z[n]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		if (z[pos_idx]<clip_z) {
			clip_line_to_2D_horizontal_line(clip_z,&cx,&cy,&cz,x[n],y[n],z[n],x[pos_idx],y[pos_idx],z[pos_idx]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		n++;
		if (n>=ptsz) break;
	}

		// only allow polygons up to 8 vertexes

	if (nptsz>8) nptsz=8;

	for (n=0;n!=nptsz;n++) {
		x[n]=nx[n];
		y[n]=ny[n];
		z[n]=nz[n];
	}

	return(nptsz);
}

int clip_polygon_to_2D_line_left_x(int ptsz,int *x,int *y,int *z,int clip_x)
{
	int			n,neg_idx,pos_idx,cx,cy,cz,
				nx[32],ny[32],nz[32],nptsz;

	nptsz=0;

	n=0;

	while (TRUE) {

			// outside of clipping line?

		if (x[n]>=clip_x) {

			nx[nptsz]=x[n];
			ny[nptsz]=y[n];
			nz[nptsz]=z[n];
			nptsz++;

			n++;
			if (n>=ptsz) break;
			continue;
		}

			// get vertexes on either side

		neg_idx=n-1;
		if (neg_idx<0) neg_idx=ptsz-1;
				
		pos_idx=n+1;
		if (pos_idx>=ptsz) pos_idx=0;

			// if both vertexes are outside, remove vertex

		if ((x[neg_idx]<=clip_x) && (x[pos_idx]<=clip_x)) {
			n++;
			if (n>=ptsz) break;
			continue;
		}

			// clip each line

		if (x[neg_idx]>clip_x) {
			clip_line_to_2D_vertical_line(clip_x,&cx,&cy,&cz,x[neg_idx],y[neg_idx],z[neg_idx],x[n],y[n],z[n]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		if (x[pos_idx]>clip_x) {
			clip_line_to_2D_vertical_line(clip_x,&cx,&cy,&cz,x[n],y[n],z[n],x[pos_idx],y[pos_idx],z[pos_idx]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		n++;
		if (n>=ptsz) break;
	}

		// only allow polygons up to 8 vertexes

	if (nptsz>8) nptsz=8;

	for (n=0;n!=nptsz;n++) {
		x[n]=nx[n];
		y[n]=ny[n];
		z[n]=nz[n];
	}

	return(nptsz);
}

int clip_polygon_to_2D_line_right_x(int ptsz,int *x,int *y,int *z,int clip_x)
{
	int			n,neg_idx,pos_idx,cx,cy,cz,
				nx[32],ny[32],nz[32],nptsz;

	nptsz=0;

	n=0;

	while (TRUE) {

			// outside of clipping line?

		if (x[n]<=clip_x) {

			nx[nptsz]=x[n];
			ny[nptsz]=y[n];
			nz[nptsz]=z[n];
			nptsz++;

			n++;
			if (n>=ptsz) break;
			continue;
		}

			// get vertexes on either side

		neg_idx=n-1;
		if (neg_idx<0) neg_idx=ptsz-1;
				
		pos_idx=n+1;
		if (pos_idx>=ptsz) pos_idx=0;

			// if both vertexes are outside, remove vertex

		if ((x[neg_idx]>=clip_x) && (x[pos_idx]>=clip_x)) {
			n++;
			if (n>=ptsz) break;
			continue;
		}

			// clip each line

		if (x[neg_idx]<clip_x) {
			clip_line_to_2D_vertical_line(clip_x,&cx,&cy,&cz,x[neg_idx],y[neg_idx],z[neg_idx],x[n],y[n],z[n]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		if (x[pos_idx]<clip_x) {
			clip_line_to_2D_vertical_line(clip_x,&cx,&cy,&cz,x[n],y[n],z[n],x[pos_idx],y[pos_idx],z[pos_idx]);
			nx[nptsz]=cx;
			ny[nptsz]=cy;
			nz[nptsz]=cz;
			nptsz++;
		}

		n++;
		if (n>=ptsz) break;
	}

		// only allow polygons up to 8 vertexes

	if (nptsz>8) nptsz=8;

	for (n=0;n!=nptsz;n++) {
		x[n]=nx[n];
		y[n]=ny[n];
		z[n]=nz[n];
	}

	return(nptsz);
}

int clip_polygon_to_2D_rect(int ptsz,int *x,int *y,int *z,int lx,int rx,int tz,int bz)
{
	ptsz=clip_polygon_to_2D_line_top_z(ptsz,x,y,z,tz);
	ptsz=clip_polygon_to_2D_line_bottom_z(ptsz,x,y,z,bz);
	ptsz=clip_polygon_to_2D_line_left_x(ptsz,x,y,z,lx);
	return(clip_polygon_to_2D_line_right_x(ptsz,x,y,z,rx));
}

int clip_extend_polygon_to_2D_rect(int ptsz,int *x,int *y,int *z,int lx,int rx,int tz,int bz)
{
	int			n;
	
		// extend polygons to eliminate integer rounding errors
		
	for (n=0;n!=ptsz;n++) {
		x[n]*=100;
		y[n]*=100;
		z[n]*=100;
	}
	
		// clip
		
	ptsz=clip_polygon_to_2D_rect(ptsz,x,y,z,(lx*100),(rx*100),(tz*100),(bz*100));

		// return to normal size
		
	for (n=0;n!=ptsz;n++) {
		x[n]/=100;
		y[n]/=100;
		z[n]/=100;
	}

	return(ptsz);
}
