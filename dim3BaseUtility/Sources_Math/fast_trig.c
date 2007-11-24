/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Fast Trig Functions

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

float		fast_sin_array[1024],fast_cos_array[1024];

/* =======================================================

      Fast Setup
      
======================================================= */

void fast_trig_setup(void)
{
	int			i;
	double		d,mult;
	
	mult=(2*TRIG_PI)/1024.0;
	
	for (i=0;i!=1000;i++) {
		d=((double)i)*mult;
		
		fast_sin_array[i]=(float)sin(d);
		fast_cos_array[i]=(float)cos(d);
	}
}

/* =======================================================

      Fast Trig Functions
      
======================================================= */

float fast_atan2f(float y,float x)
{
	float			atan,z;
	
	if (x==0.0f) {
		if (y>0.0f) return(TRIG_PI/2.0f);
		if (y==0.0f) return(0.0f);
		return(-TRIG_PI/2.0f);
	}
	
	z=y/x;
	
	if (fabs(z)<1.0f) {
		atan=z/(1.0f+(0.28f*z*z));
		if (x<0.0f) {
			if (y<0.0f) return(atan-TRIG_PI);
			return(atan+TRIG_PI);
		}
	}
	else {
		atan=(TRIG_PI/2.0f)-(z/((z*z)+0.28f));
		if (y<0.0f) return(atan-TRIG_PI);
	}
	
	return(atan);
}

float fast_sinf(float x)
{
	return(fast_sin_array[(int)(x*(1000.0f/(2.0f*TRIG_PI)))]);
}

float fast_cosf(float x)
{
	return(fast_cos_array[(int)(x*(1000.0f/(2.0f*TRIG_PI)))]);
}

