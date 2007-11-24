/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Random Number Functions

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

      Random Numbers
      
======================================================= */

void random_reset(void)
{
	srandom((long)clock());
}

int random_int(int max)
{
	int			r;
	
	if (max==0) return(0);
	
	r=(int)random();
	return(r%max);
}

float random_float(float max)
{
	int			r;
	
	r=(int)random();
	r=r%10240;
	
	return((max*(float)r)/10240);
}

bool random_boolean(void)
{
	int			r;
	
	r=(int)random();
	return((r%100)>50);
}

int random_int_spread(int spread)
{
	int			r,k;
	double		sd,rd;
	
	r=(int)random();
	r=r%10240;

	sd=(double)(spread<<1);
	rd=(double)r;
	k=(int)((sd*rd)/10240.0);

	return(spread-k);
}
