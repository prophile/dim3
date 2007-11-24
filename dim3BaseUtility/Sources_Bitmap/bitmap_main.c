/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Main Bitmap Functions
 
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

      New Bitmaps
      
======================================================= */

void bitmap_new(bitmap_type *bitmap)
{
	bitmap->name[0]=0x0;
	bitmap->wid=bitmap->high=0;
	bitmap->data=NULL;
	bitmap->gl_id=-1;
}

/* =======================================================

      Open Bitmaps
      
======================================================= */

bool bitmap_open(bitmap_type *bitmap,char *path,int anisotropic_mode,int texture_quality_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression)
{
	char		*c;
	
	bitmap_new(bitmap);
	
		// get name
		
	c=strrchr(path,'/');
	if (c!=NULL) strcpy(bitmap->name,(c+1));
	
	c=strrchr(bitmap->name,'.');
	if (c!=NULL) *c=0x0;
	
		// read bitmap
	
	if (!bitmap_png_read(bitmap,path)) return(FALSE);
	
		// quality changes
		
	switch (texture_quality_mode) {
		
		case texture_quality_mode_low:
			bitmap_reduce(bitmap,128);
			break;
			
		case texture_quality_mode_medium:
			bitmap_reduce(bitmap,256);
			break;
			
	}
	
		// get the texture
		
	if (!bitmap_texture_open(bitmap,anisotropic_mode,mipmap_mode,use_card_generated_mipmaps,use_compression)) {
		free(bitmap->data);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Color Bitmap
      
======================================================= */

bool bitmap_color(bitmap_type *bitmap,char *name,d3col *col,int anisotropic_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression)
{
	int			i;
	long		collong,*dptr;
	
	strcpy(bitmap->name,name);
	bitmap->wid=bitmap->high=32;
	
	bitmap->data=valloc(4096);
	if (bitmap->data==NULL) return(FALSE);
	
	collong=((int)(0xFF*col->r)<<24)|((int)(0xFF*col->g)<<16)|((int)(0xFF*col->b)<<8)|0x000000FF;
	
	dptr=(long*)bitmap->data;
	for (i=0;i!=1024;i++) {
		*dptr++=collong;
	}
	
	if (!bitmap_texture_open(bitmap,anisotropic_mode,mipmap_mode,use_card_generated_mipmaps,use_compression)) {
		free(bitmap->data);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Save Bitmaps
      
======================================================= */

bool bitmap_save(bitmap_type *bitmap,char *path)
{
	return(bitmap_png_write(bitmap,path));
}

/* =======================================================

      Close Bitmaps
      
======================================================= */

void bitmap_close(bitmap_type *bitmap)
{
	if (bitmap->gl_id!=-1) bitmap_texture_close(bitmap);
	if (bitmap->data!=NULL) free(bitmap->data);
	
	bitmap->gl_id=-1;
	bitmap->data=NULL;
}

/* =======================================================

      Misc Bitmap Functions
      
======================================================= */

bool bitmap_check(char *path,char *err_str)
{
	return(bitmap_png_check(path,err_str));
}

bool bitmap_copy(char *srce_path,char *dest_path)
{
	return(bitmap_png_copy(srce_path,dest_path));
}



