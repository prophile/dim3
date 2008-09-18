/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Read/Write/Check PNG Files

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

      Read PNGs
      
======================================================= */

unsigned char* png_utility_read(char *path,int *p_wid,int *p_high)
{
	int						x,y,
							psz,rowbytes,channels,wid,high;
	unsigned char			header[8];
	unsigned char			*data,*ptr,*rp;
	FILE					*file;
	png_structp				png_ptr;
	png_infop				info_ptr;
	png_bytep				*rptrs;
	
		// open file
		
	file=fopen(path,"rb");
	if (file==NULL) return(NULL);
	
	fread(header,1,8,file);
	if (png_sig_cmp(header,0,8)) return(NULL);
	
		// setup read
		
	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (png_ptr==NULL) {
		fclose(file);
		return(NULL);
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if (info_ptr==NULL) {
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(file);
		return(NULL);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(NULL);
	}
	
	png_init_io(png_ptr,file);
	png_set_sig_bytes(png_ptr,8);
	
	png_read_info(png_ptr,info_ptr);
	
	wid=info_ptr->width;
	high=info_ptr->height;
	rowbytes=info_ptr->rowbytes;
	
	channels=png_get_channels(png_ptr,info_ptr);
	
	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);
	
		// create the bitmap
		
	psz=(wid<<2)*high;
	
	data=valloc(psz);
	if (data==NULL) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(NULL);
	}
	
	if (p_wid!=NULL) {
		*p_wid=wid;
		*p_high=high;
	}

		// read the file
		
	rptrs=(png_bytep*)malloc(sizeof(png_bytep)*high);
	
	for (y=0;y!=high;y++) {
		rptrs[y]=(png_byte*)malloc(rowbytes);
	}
	
	png_read_image(png_ptr,rptrs);
	
		// translate
		
	ptr=data;
	
	if (channels==4) {
	
			// RGBA
			
		for (y=0;y!=high;y++) {
			memmove(ptr,rptrs[y],rowbytes);
			ptr+=rowbytes;
		}
	}
	else {
	
			// RGB
			
		for (y=0;y!=high;y++) {
			rp=rptrs[y];
			for (x=0;x!=wid;x++) {
				*ptr++=*rp++;
				*ptr++=*rp++;
				*ptr++=*rp++;
				*ptr++=0xFF;
			}
		}
	}
	
		// clean up
		
	for (y=0;y!=high;y++) {
		free(rptrs[y]);
	}
	
	free(rptrs);
		
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	fclose(file);
	
	return(data);
}

/* =======================================================

      Write PNGs
      
======================================================= */

bool png_utility_write(unsigned char *data,int wid,int high,char *path)
{
	int						y,rowbytes;
	unsigned char			*ptr;
	FILE					*file;
	png_structp				png_ptr;
	png_infop				info_ptr;
	png_bytep				*rptrs;
	
		// open file
		
	file=fopen(path,"wb");
	if (file==NULL) return(FALSE);
	
		// setup write
		
	png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (png_ptr==NULL) {
		fclose(file);
		return(FALSE);
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if (info_ptr==NULL) {
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(file);
		return(FALSE);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(FALSE);
	}
	
	png_init_io(png_ptr,file);
	
		// write the header
		
	png_set_IHDR(png_ptr,info_ptr,wid,high,8,PNG_COLOR_TYPE_RGB_ALPHA,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);
	
		// setup row pointers
		
	rptrs=(png_bytep*)malloc(sizeof(png_bytep)*high);
	
	rowbytes=(wid<<2);
	ptr=data;
	
	for (y=0;y!=high;y++) {
		rptrs[y]=(png_byte*)malloc(rowbytes);
		memmove(rptrs[y],ptr,rowbytes);
		ptr+=rowbytes;
	}
		
		// write the image
		
	png_write_image(png_ptr,rptrs);
	
		// write the end
		
	png_write_end(png_ptr,NULL);
	
		// clean up
		
	for (y=0;y!=high;y++) {
		free(rptrs[y]);
	}
	
	free(rptrs);
		
	png_destroy_write_struct(&png_ptr,&info_ptr);
	fclose(file);
	
	return(TRUE);
}

/* =======================================================

      Check PNGs
      
======================================================= */

bool png_utility_check(char *path,char *err_str)
{
	int						x,y,bit_depth,channels,
							*v,values[]={2,4,8,16,32,64,128,256,512,1024,2048,4096,-1};
	bool					x_ok,y_ok;
	unsigned char			header[8];
	FILE					*file;
	png_structp				png_ptr;
	png_infop				info_ptr;
	
		// open file
		
	file=fopen(path,"rb");
	if (file==NULL) {
		strcpy(err_str,"Could not open PNG.");
		return(FALSE);
	}
	
	fread(header,1,8,file);
	if (png_sig_cmp(header,0,8)) {
		strcpy(err_str,"Not a proper PNG file.");
		return(FALSE);
	}
	
		// setup read
		
	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (png_ptr==NULL) {
		fclose(file);
		strcpy(err_str,"Could not read PNG.");
		return(FALSE);
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if (info_ptr==NULL) {
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(file);
		strcpy(err_str,"Could not read PNG.");
		return(FALSE);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		strcpy(err_str,"Could not read PNG.");
		return(FALSE);
	}
	
	png_init_io(png_ptr,file);
	png_set_sig_bytes(png_ptr,8);
	
	png_read_info(png_ptr,info_ptr);
	
	x=info_ptr->width;
	y=info_ptr->height;
	bit_depth=info_ptr->bit_depth;
	
	channels=png_get_channels(png_ptr,info_ptr);
	
	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);
	
		// clean up		

	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	fclose(file);
	
		// correct bit depth?
		
	if (bit_depth!=8) {
		strcpy(err_str,"PNG color components need to have a bit depth of 8.");
		return(FALSE);
	}
	
		// correct channels
		
	if ((channels!=3) && (channels!=4)) {
		strcpy(err_str,"PNG must be either 3-component (RGB) or 4-component (RGBA).");
		return(FALSE);
	}
	
		// is it not a power of 2?
		
	x_ok=y_ok=FALSE;
	
	v=values;
	while (TRUE) {
		if (*v==-1) break;
		x_ok=x_ok||(x==*v);
		y_ok=y_ok||(y==*v);
		v++;
	}
	
	if ((!x_ok) || (!y_ok)) {
		strcpy(err_str,"Both the width and height of the texture must be a power of 2 (2, 4, 8, 16, 32, 64, 128, 256, etc).");
		return(FALSE);
	}

		// bitmap is OK
		
	return(TRUE);
}

/* =======================================================

      Copy PNGs
      
======================================================= */

bool png_utility_copy(char *srce_path,char *dest_path)
{
	int				sz;
	unsigned char	c[1024];
	FILE			*srce,*dest;
	
		// open files
		
	srce=fopen(srce_path,"rb");
	if (srce==NULL) return(FALSE);
	
	dest=fopen(dest_path,"wb");
	if (dest==NULL) {
		fclose(srce);
		return(FALSE);
	}
	
		// copy
		
	while (TRUE) {
		sz=(int)fread(c,1,1024,srce);
		if (sz<=0) break;
		fwrite(c,1,sz,dest);
	}

		// close
		
	fclose(srce);
	fclose(dest);
	
	return(TRUE);
}

