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
	bitmap->gl_id=-1;
}

/* =======================================================

      Open Bitmaps
      
======================================================= */

bool bitmap_open(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,bool use_compression,bool pixelated,bool scrub_black_to_alpha)
{
	int					n,psz;
	char				*c;
	unsigned char		*png_data,*data;
	bool				ok;

	bitmap_new(bitmap);
	
		// get name
		
	c=strrchr(path,'/');
	if (c!=NULL) strcpy(bitmap->name,(c+1));
	
	c=strrchr(bitmap->name,'.');
	if (c!=NULL) *c=0x0;
	
		// read bitmap
	
	png_data=png_utility_read(path,&bitmap->wid,&bitmap->high);
	if (png_data==NULL) return(FALSE);

	psz=(bitmap->wid<<2)*bitmap->high;

		// scrub blacks to alpahs
		// this is mostly used for glow maps

	if (scrub_black_to_alpha) {

		data=png_data;

		for (n=0;n<psz;n+=4) {
			*(data+3)=*data;
			data+=4;
		}
	}
	
		// find if bitmap has transparencies
		
	bitmap->alpha_mode=alpha_mode_none;
		
	data=png_data+3;
	
	for (n=0;n<psz;n+=4) {
	
		if (*data!=0xFF) {
			if (*data==0x0) {
				bitmap->alpha_mode=alpha_mode_cut_out;			// possibly a cut out
			}
			else {
				bitmap->alpha_mode=alpha_mode_transparent;		// and single non-0xFF and non-0x00 means transparency
				break;
			}
		}

		data+=4;
	}
	
		// get the texture
		
	ok=bitmap_texture_open(bitmap,png_data,anisotropic_mode,mipmap_mode,use_compression,pixelated);

	free(png_data);
	
	return(ok);
}

/* =======================================================

      Color Bitmap
      
======================================================= */

bool bitmap_color(bitmap_type *bitmap,char *name,d3col *col)
{
	int				i;
	long			collong,*dptr;
	unsigned char	*png_data;
	bool			ok;
	
	strcpy(bitmap->name,name);
	bitmap->wid=bitmap->high=32;
	
	png_data=malloc(4096);
	if (png_data==NULL) return(FALSE);
	
	collong=((int)(0xFF*col->r)<<24)|((int)(0xFF*col->g)<<16)|((int)(0xFF*col->b)<<8)|0x000000FF;
	
	dptr=(long*)png_data;

	for (i=0;i!=1024;i++) {
		*dptr++=collong;
	}
	
	ok=bitmap_texture_open(bitmap,png_data,anisotropic_mode_none,mipmap_mode_none,FALSE,TRUE);

	free(png_data);
	
	return(ok);
}

/* =======================================================

      Data Bitmap
      
======================================================= */

bool bitmap_data(bitmap_type *bitmap,char *name,unsigned char *data,int wid,int high,int anisotropic_mode,int mipmap_mode,bool use_compression)
{
	int				n,psz;
	unsigned char	*ptr;
	
	strcpy(bitmap->name,name);
	bitmap->wid=wid;
	bitmap->high=high;
	
		// find if bitmap has transparencies
		
	bitmap->alpha_mode=alpha_mode_none;
	ptr=data+3;

	psz=(wid*4)*high;
	
	for (n=0;n<psz;n+=4) {
	
		if (*ptr!=0xFF) {
			if (*ptr==0x0) {
				bitmap->alpha_mode=alpha_mode_cut_out;			// possibly a cut out
			}
			else {
				bitmap->alpha_mode=alpha_mode_transparent;		// and single non-0xFF and non-0x00 means transparency
				break;
			}
		}

		ptr+=4;
	}

		// get the texture
		
	return(bitmap_texture_open(bitmap,data,anisotropic_mode,mipmap_mode,use_compression,FALSE));
}

/* =======================================================

      Close Bitmaps
      
======================================================= */

void bitmap_close(bitmap_type *bitmap)
{
	if (bitmap->gl_id!=-1) bitmap_texture_close(bitmap);
	
	bitmap->gl_id=-1;
}

/* =======================================================

      Misc Bitmap Functions
      
======================================================= */

unsigned char* bitmap_load_png_data(char *path,int *p_wid,int *p_high)
{
	return(png_utility_read(path,p_wid,p_high));
}

bool bitmap_write_png_data(unsigned char *data,int wid,int high,char *path)
{
	return(png_utility_write(data,wid,high,path));
}

bool bitmap_check(char *path,char *err_str)
{
	return(png_utility_check(path,err_str));
}

bool bitmap_copy(char *srce_path,char *dest_path)
{
	return(png_utility_copy(srce_path,dest_path));
}



