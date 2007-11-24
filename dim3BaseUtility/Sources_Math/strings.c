/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: String Routines

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

      Float To String
      
======================================================= */

void string_convert_float(char *str,float f)
{
	int				lidx,n;
	float			f2;
	char			*c;

		// no fractional component?

	f2=(float)floor(f);
	if (f2==f) {
		sprintf(str,"%d",(int)f2);
		return;
	}

		// has fractional component
	
	sprintf(str,"%f",f);
	
	c=strchr(str,'.');
	if (c==NULL) return;
	
	lidx=((int)(c-str))+1;
	
	n=strlen(str)-1;
	while (n>lidx) {
		if (str[n]!='0') break;
		str[n]=0x0;
		n--;
	}
}

