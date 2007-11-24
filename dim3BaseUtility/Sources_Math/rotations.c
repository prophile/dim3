/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Rotation Functions

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Rotating Around Point
      
======================================================= */

void rotate_point(int *x,int *y,int *z,int cx,int cy,int cz,float ang_x,float ang_y,float ang_z)
{
	float				fx,fy,fz;
	matrix_type			mat;
	
	matrix_rotate_xyz(&mat,ang_x,ang_y,ang_z);
	
	fx=(float)((*x)-cx);
	fy=(float)((*y)-cy);
	fz=(float)((*z)-cz);
	
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	*x=(int)fx+cx;
	*y=(int)fy+cy;
	*z=(int)fz+cz;
}

void rotate_polygon(int ptsz,int *x,int *y,int *z,int cx,int cy,int cz,float ang_x,float ang_y,float ang_z)
{
	int					n;
	int					*px,*py,*pz;
	float				fx,fy,fz;
	matrix_type			mat;
	
	matrix_rotate_xyz(&mat,ang_x,ang_y,ang_z);
	
	px=x;
	py=y;
	pz=z;
	
	for (n=0;n<ptsz;n++) {
		fx=(float)((*px)-cx);
		fy=(float)((*py)-cy);
		fz=(float)((*pz)-cz);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		*px++=(int)fx+cx;
		*py++=(int)fy+cy;
		*pz++=(int)fz+cz;
	}
}

/* =======================================================

      Rotate Around Center
      
======================================================= */

void rotate_point_center(int *x,int *y,int *z,float ang_x,float ang_y,float ang_z)
{
	float				fx,fy,fz;
	matrix_type			mat;
	
	matrix_rotate_xyz(&mat,ang_x,ang_y,ang_z);
	
	fx=(float)*x;
	fy=(float)*y;
	fz=(float)*z;
	
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	*x=(int)fx;
	*y=(int)fy;
	*z=(int)fz;
}

void rotate_polygon_center(int ptsz,int *x,int *y,int *z,float ang_x,float ang_y,float ang_z)
{
	int					n;
	int					*px,*py,*pz;
	float				fx,fy,fz;
	matrix_type			mat;
	
	matrix_rotate_xyz(&mat,ang_x,ang_y,ang_z);
	
	px=x;
	py=y;
	pz=z;
	
	for (n=0;n<ptsz;n++) {
		fx=(float)*px;
		fy=(float)*py;
		fz=(float)*pz;
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		*px++=(int)fx;
		*py++=(int)fy;
		*pz++=(int)fz;
	}
}

/* =======================================================

      Rotate 2D Around Point
      
======================================================= */

void rotate_2D_point(int *x,int *z,int cx,int cz,float ang_y)
{
	float			fx,fy,fz;
	matrix_type		mat;
	
	matrix_rotate_y(&mat,ang_y);
	
	fx=(float)((*x)-cx);
	fy=0;
	fz=(float)((*z)-cz);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	*x=(int)fx+cx;
	*z=(int)fz+cz;
}

/* =======================================================

      Rotate 2D Around Center
      
======================================================= */

void rotate_2D_point_center(int *x,int *z,float ang_y)
{
	float			fx,fy,fz;
	matrix_type		mat;
	
	matrix_rotate_y(&mat,ang_y);
	
	fx=(float)*x;
	fy=0;
	fz=(float)*z;
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	*x=(int)fx;
	*z=(int)fz;
}

/* =======================================================

      Rotate 2D Line Around Center
      
======================================================= */

void rotate_2D_line(int *lx,int *lz,int *rx,int *rz,int cx,int cz,float ang_y)
{
	float			fx,fy,fz;
	matrix_type		mat;
	
	matrix_rotate_y(&mat,ang_y);
	
	fx=(float)((*lx)-cx);
	fy=0;
	fz=(float)((*lz)-cz);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	*lx=(int)fx+cx;
	*lz=(int)fz+cz;
	
	fx=(float)((*rx)-cx);
	fy=0;
	fz=(float)((*rz)-cz);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	*rx=(int)fx+cx;
	*rz=(int)fz+cz;
}

/* =======================================================

      Rotate 2D Polygon
      
======================================================= */

void rotate_2D_polygon(int ptsz,int *x,int *z,int cx,int cz,float ang_y)
{
	int					n;
	int					*px,*pz;
	float				fx,fy,fz;
	matrix_type			mat;
	
	matrix_rotate_y(&mat,ang_y);
	
	px=x;
	pz=z;
	
	for (n=0;n<ptsz;n++) {
		fx=(float)((*px)-cx);
		fy=0;
		fz=(float)((*pz)-cz);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		*px++=(int)fx+cx;
		*pz++=(int)fz+cz;
	}
}

