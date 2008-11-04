/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Line Functions

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

      Movement on a Line
      
======================================================= */

void line_get_movement(int x1,int y1,int z1,int x2,int y2,int z2,int mv,int *xadd,int *yadd,int *zadd)
{
    int				dx,dz,dy;
    float			f;
    double			kx,kz,ky,k;
    
        // line size
    
    dx=x1-x2;
    dz=z1-z2;
    dy=y1-y2;
    
    kx=(double)dx;
    kz=(double)dz;
    ky=(double)dy;
	k=(kx*kx)+(kz*kz)+(ky*ky);
	
    f=(float)sqrt(k);
    
        // fraction moved
        
    f=((float)mv)/f;
    
        // separate x/z/y moves
        
    *xadd=(int)(((float)dx)*f);
    *zadd=(int)(((float)dz)*f);
    *yadd=(int)(((float)dy)*f);
}

/* =======================================================

      Line Intersections
      
======================================================= */

bool line_2D_get_intersect(int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3,int *x,int *y)
{
	float			fx0,fy0,fx1,fy1,fx2,fy2,fx3,fy3,
					ax,bx,dx,ay,by,dy,r,s;

	fx0=(float)x0;
	fy0=(float)y0;
	fx1=(float)x1;
	fy1=(float)y1;
	fx2=(float)x2;
	fy2=(float)y2;
	fx3=(float)x3;
	fy3=(float)y3;
	
	ax=fx0-fx2;
	bx=fx1-fx0;
	dx=fx3-fx2;
	
	ay=fy0-fy2;
	by=fy1-fy0;
	dy=fy3-fy2;
	
	r=((ay*dx)-(ax*dy))/((bx*dy)-(by*dx));
	if ((r<0) || (r>1)) return(FALSE);
	
	s=((ay*bx)-(ax*by))/((bx*dy)-(by*dx));
	if ((s<0) || (s>1)) return(FALSE);
	
	*x=(int)(fx0+(r*bx));
	*y=(int)(fy0+(r*by));
	
	return(TRUE);
}

bool line_2D_test_intersect(int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3)
{
	float			fx0,fy0,fx1,fy1,fx2,fy2,fx3,fy3,
					ax,bx,dx,ay,by,dy,r,s;

	fx0=(float)x0;
	fy0=(float)y0;
	fx1=(float)x1;
	fy1=(float)y1;
	fx2=(float)x2;
	fy2=(float)y2;
	fx3=(float)x3;
	fy3=(float)y3;
	
	ax=fx0-fx2;
	bx=fx1-fx0;
	dx=fx3-fx2;
	
	ay=fy0-fy2;
	by=fy1-fy0;
	dy=fy3-fy2;
	
	r=((ay*dx)-(ax*dy))/((bx*dy)-(by*dx));
	if ((r<0) || (r>1)) return(FALSE);
	
	s=((ay*bx)-(ax*by))/((bx*dy)-(by*dx));
	if ((s<0) || (s>1)) return(FALSE);
	
	return(TRUE);
}
        
/* =======================================================

      Angle To Line Hit
      
======================================================= */

void line_2D_find_angle_hit(int x,int z,float ang,int lx,int lz,int rx,int rz,int *kx,int *kz)
{
	int			k;
	
		// rotate line so movement angle = 0
	
	if (ang!=0) {
		rotate_2D_line(&lx,&lz,&rx,&rz,x,z,(360-ang));
	}
	
		// adjust line
		
	if (lx>rx) {
		k=lx;
		lx=rx;
		rx=k;
		k=lz;
		lz=rz;
		rz=k;
	}
	
		// where is z in line?
	
	*kx=x;
	
	k=rx-lx;
	if (k==0) {
		*kz=lz;
	}
	else {
		*kz=lz+(((rz-lz)*(x-lx))/k);
	}
	
		// fix rotation
		
	if (ang!=0) {
		rotate_2D_point(kx,kz,x,z,ang);
	}
}

/* =======================================================

      Find Line Segment of Width inside Line
      
======================================================= */

void line_2D_find_inside_infinite(int x,int z,int wid,int lx,int lz,int rx,int rz,int *klx,int *klz,int *krx,int *krz)
{
	int			xadd,zadd,dist;
	double		d,dx,dz;

		// find line size
		
	dx=(double)(rx-lx);
	dz=(double)(rz-lz);
	
	d=(dx*dx)+(dz*dz);
	dist=(int)sqrt(d);
	
		// calc new line from slope
	
	if (dist==0) {
		xadd=0;
		zadd=0;
	}
	else {
		xadd=((rx-lx)*wid)/dist;
		zadd=((rz-lz)*wid)/dist;
	}	

	*klx=x-xadd;
	*krx=x+xadd;
	
	*klz=z-zadd;
	*krz=z+zadd;
}

/* =======================================================

      Check if Points are on a Single Line
      
======================================================= */

bool line_2D_all_points_in_line(int ptsz,int *px,int *pz,float slop)
{
	int				n,x,z,slope_cnt;
	float			slopes[8];
	bool			same_x,same_z,horz,vert;

		// only two points, must be in line

	if (ptsz<=2) return(TRUE);

		// quick check for same x or z coordinates

	same_x=same_z=TRUE;

	for (n=1;n<ptsz;n++) {
		same_x=same_x&&(px[0]==px[n]);
		same_z=same_z&&(pz[0]==pz[n]);
	}

	if ((same_x) || (same_z)) return(TRUE);

		// get the average slopes

	horz=vert=FALSE;
	slope_cnt=0;

	for (n=0;n<(ptsz-1);n++) {

		x=px[0]-px[n+1];
		z=pz[0]-pz[n+1];
		
			// if point is equal, then we ignore
			// the slope and the point
			
		if ((x==0) && (z==0)) continue;

			// special check for horz or vertical lines
			// note vertical lines have undefined slope, so
			// we are setting it to zero and checking for
			// the special case where there are both horz
			// and vertical slopes (now both equal to 0)

		if (x==0) {
			horz=TRUE;
			slopes[slope_cnt]=0.0f;
		}
		else {
			if (z==0) {
				vert=TRUE;
				slopes[slope_cnt]=0.0f;
			}
			else {
				slopes[slope_cnt]=((float)x)/((float)z);
			}
		}
		
		slope_cnt++;
	}

		// if both horz and vertical, then not in line

	if ((horz) && (vert)) return(FALSE);

		// compare all the slopes to each other, if they
		// are within the slope, it's all within a line
		
	for (n=1;n<slope_cnt;n++) {
		if (fabs(slopes[0]-slopes[n])>slop) return(FALSE);
	}
	
	return(TRUE);
}
