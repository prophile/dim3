/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Create Glow Maps

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

      Create Glow Map For Rendering
      
======================================================= */

bool bitmap_setup_render_glowmap(bitmap_type *glowmap,int anisotropic_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression)
{
	int				x,y,wid,high;
	unsigned char	gr,gg,gb;
	ptr				data;
	
		// set all the blacks to alphas

	wid=glowmap->wid;
	high=glowmap->high;

	data=glowmap->data;
	
	for (y=0;y!=high;y++) {
		for (x=0;x!=wid;x++) {

			gr=*data++;
			gg=*data++;
			gb=*data++;
			
			if ((gr==0) && (gg==0) && (gb==0)) *data=0;
			
			data++;
		}
	}
	
		// fix GL texture
		
	if (glowmap->gl_id!=-1) bitmap_texture_close(glowmap);
		
	if (!bitmap_texture_open(glowmap,anisotropic_mode,mipmap_mode,use_card_generated_mipmaps,use_compression)) {
		free(glowmap->data);
		return(FALSE);
	}

	return(TRUE);
}
