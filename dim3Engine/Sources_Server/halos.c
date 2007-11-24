/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Halo Setup

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

extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Initialize Halos
      
======================================================= */

void halo_initialize(void)
{
	server.count.halo=0;
}

/* =======================================================

      Find Halos
      
======================================================= */

int halo_find(char *name)
{
	int			n;
	halo_type	*halo;

	halo=server.halos;
	
	for (n=0;n!=server.count.halo;n++) {
		if (strcmp(halo->name,name)==0)  return(n);
		halo++;
	}
	
	return(-1);
}

