/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Distance Functions

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

      Get distance
      
======================================================= */

int distance_get(int x,int y,int z,int tox,int toy,int toz)
{
	double		dx,dz,dy;
	
	dx=(double)(x-tox);
	dy=(double)(y-toy);
	dz=(double)(z-toz);
	
	return((int)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

bool distance_check(int x,int y,int z,int tox,int toy,int toz,int max_distance)
{
	double		d,dx,dz,dy,md;
	
	dx=(double)(x-tox);
	dy=(double)(y-toy);
	dz=(double)(z-toz);
	d=(dx*dx)+(dy*dy)+(dz*dz);
	
	md=(double)max_distance;
	if (d>(md*md)) return(FALSE);
	
	return(sqrt(d)<=md);
}

int distance_2D_get(int x,int z,int tox,int toz)
{
	double		dx,dz;
	
	dx=(double)(x-tox);
	dz=(double)(z-toz);
	
	return((int)sqrt((dx*dx)+(dz*dz)));
}
