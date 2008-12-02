/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Area Routines

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

      2D Triangle Area
      
======================================================= */

int area_2D_trig(int *px,int *py)
{
	int			area;

	area=(px[0]*(py[1]-py[2]))+(px[1]*(py[2]-py[0]))+(px[2]*(py[0]-py[1]));
	return(abs(area/2));
}

/* =======================================================

      2D Polygon Area
      
======================================================= */

int area_2D_polygon(int ptsz,int *px,int *py)
{
	int				n,k,ntrig,area,
					kx[3],ky[3];
	
	if (ptsz==3) return(area_2D_trig(px,py));
	
	area=0;
	ntrig=ptsz-2;
	
	kx[0]=px[0];
	ky[0]=py[0];
	
	for (n=0;n!=ntrig;n++) {
	
		k=n+1;
		kx[1]=px[k];
		ky[1]=py[k];
		
		k=n+2;
		kx[2]=px[k];
		ky[2]=py[k];

		area+=area_2D_trig(kx,ky);
	}
	
	return(area);
}
