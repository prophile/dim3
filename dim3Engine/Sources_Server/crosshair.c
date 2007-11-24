/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Crosshair Setup/Drawing

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

extern server_type				server;

/* =======================================================

      Initialize Crosshairs
      
======================================================= */

void crosshair_initialize(void)
{
	server.count.crosshair=0;
}

/* =======================================================

      Find Crosshairs
      
======================================================= */

int crosshair_find(char *name)
{
	int				n;
	crosshair_type	*crosshair;

	crosshair=server.crosshairs;
	
	for (n=0;n!=server.count.crosshair;n++) {
		if (strcmp(crosshair->name,name)==0)  return(n);
		crosshair++;
	}
	
	return(-1);
}

