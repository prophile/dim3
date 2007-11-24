/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Angle Functions

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

      Change a factor based on acceleration
      
======================================================= */

float acceleration_calculate(float factor,float acceleration)
{
	float		f;

	if (acceleration==0) return(factor);

	if (acceleration<0) {
		f=1-factor;
		factor=f*(1-f)*(float)exp(-acceleration)+f;
		factor=1-factor;
	}
	else {
		f=factor;
		factor=factor*(1-factor)*(float)exp(acceleration)+factor;
	}

	if (factor<0.0f) return(0.0f);
	if (factor>1.0f) return(1.0f);

	return(factor);
}
