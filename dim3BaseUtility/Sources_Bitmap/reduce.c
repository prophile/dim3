/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Bitmap Quality Reductions

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

      Bitmap Quality Reductions
      
======================================================= */

void bitmap_reduce(bitmap_type *bitmap,int max_pixel_sz)
{
	int				x,y,x_skip,y_skip,
					wid,high;
	ptr				data,srce_data,dest_data;
	
		// find reduction
		
	x_skip=y_skip=0;
	
	if (bitmap->wid>max_pixel_sz) x_skip=bitmap->wid/max_pixel_sz;
	if (bitmap->high>max_pixel_sz) y_skip=bitmap->high/max_pixel_sz;
	
	if ((x_skip==0) && (y_skip==0)) return;
	
		// new data
		
	wid=bitmap->wid;
	high=bitmap->high;

	if (x_skip!=0) wid=bitmap->wid/x_skip;
	if (y_skip!=0) high=bitmap->high/y_skip;
	
	data=(ptr)valloc((wid<<2)*high);
	if (data==NULL) return;
	
		// reduce
		
	srce_data=bitmap->data;
	dest_data=data;
	
	for (y=0;y!=high;y++) {
	
		for (x=0;x!=wid;x++) {
			*dest_data++=*srce_data++;
			*dest_data++=*srce_data++;
			*dest_data++=*srce_data++;
			*dest_data++=*srce_data++;
			if (x_skip!=0) srce_data+=((x_skip-1)<<2);
		}
		
		if (y_skip!=0) srce_data+=((bitmap->wid<<2)*(y_skip-1));
	}
	
		// replace
		
	free(bitmap->data);
	bitmap->data=data;
	
	bitmap->wid=wid;
	bitmap->high=high;
}
