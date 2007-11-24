/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Vector Functions

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

      Vector Routines
      
======================================================= */

void vector_normalize(d3vct *v)
{
	double			dx,dy,dz,d;
	
	dx=(double)v->x;
	dy=(double)v->y;
	dz=(double)v->z;

	d=sqrt((dx*dx)+(dy*dy)+(dz*dz));
	if (d==0) return;
	
	d=1.0/d;
	v->x=(float)(dx*d);
	v->y=(float)(dy*d);
	v->z=(float)(dz*d);
}

void vector_create(d3vct *v,int x1,int y1,int z1,int x2,int y2,int z2)
{
	v->x=(float)(x1-x2);
	v->y=(float)(y1-y2);
	v->z=(float)(z1-z2);
	
	vector_normalize(v);
}

void vector_cross_product(d3vct *v,d3vct *v1,d3vct *v2)
{
	v->x=(v1->y*v2->z)-(v2->y*v1->z);
    v->y=(v1->z*v2->x)-(v2->z*v1->x);
    v->z=(v1->x*v2->y)-(v2->x*v1->y);

	vector_normalize(v);
}

float vector_dot_product(d3vct *v1,d3vct *v2)
{
	return((v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z));
}
