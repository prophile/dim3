/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Create Normal Maps

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

      Create Normal Map Utility
      
======================================================= */

unsigned char bitmap_normal_pack_float(float f)
{
	return((unsigned char)(((f+1.0f)/2.0f)*255.0f));
}

float bitmap_normal_get_gray_level(float *gs_data,int x,int y,int wid)
{
	float			*f;
	
	f=gs_data+((y*wid)+x);
	return(*f);
}

void bitmap_normal_data_create_from_gs_data(ptr data,float *gs_data,int wid,int high)
{
	int			x,y,ty,by,lx,rx;
	float		dx,dy,nx,ny,nz,vlen;
	ptr			p_data;
	
	p_data=data;
	
	for (y=0;y!=high;y++) {
	
		for (x=0;x!=wid;x++) {
		
				// get pixel offsets
				
			lx=x-1;
			if (lx<0) lx+=wid;
			
			rx=(x+1)%wid;
			
			ty=y-1;
			if (ty<0) ty+=high;
			
			by=(y+1)%high;

				// x sobel filter
				
			dx=(bitmap_normal_get_gray_level(gs_data,lx,ty,wid)*-1.0f);
			dx+=(bitmap_normal_get_gray_level(gs_data,lx,y,wid)*-2.0f);
			dx+=(bitmap_normal_get_gray_level(gs_data,lx,by,wid)*-1.0f);
			dx+=(bitmap_normal_get_gray_level(gs_data,rx,ty,wid));		// * 1
			dx+=(bitmap_normal_get_gray_level(gs_data,rx,y,wid)*2.0f);
			dx+=(bitmap_normal_get_gray_level(gs_data,rx,by,wid));		// * 1

				// y sobel filter
				
			dy=(bitmap_normal_get_gray_level(gs_data,lx,ty,wid));		// * 1
			dy+=(bitmap_normal_get_gray_level(gs_data,x,ty,wid)*2.0f);
			dy+=(bitmap_normal_get_gray_level(gs_data,rx,ty,wid));		// * 1
			dy+=(bitmap_normal_get_gray_level(gs_data,lx,by,wid)*-1.0f);
			dy+=(bitmap_normal_get_gray_level(gs_data,x,by,wid)*-2.0f);
			dy+=(bitmap_normal_get_gray_level(gs_data,rx,by,wid)*-1.0f);

				// cross-product and normal the vector

			nx=-dx;
			ny=-dy;
			nz=1;
			
			vlen=1.0f/(float)sqrt((nx*nx)+(ny*ny)+(nz*nz));
			nx*=vlen;
			ny*=vlen;
			nz*=vlen;

			*p_data++=bitmap_normal_pack_float(nx);
			*p_data++=bitmap_normal_pack_float(ny);
			*p_data++=bitmap_normal_pack_float(nz);
			*p_data++=0xFF;
		}
	}
}

/* =======================================================

      Create Normal Map from Height Map
      
======================================================= */

bool bitmap_open_normal_from_height(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,bool use_compression,bool pixelated)
{
	int					x,y,wid,high;
	char				*c;
	unsigned char		*png_data,*data,*p_data;
	float				*gs_data,*p_gs_data;
	bool				ok;
	
		// get name
		
	c=strrchr(path,'/');
	if (c!=NULL) strcpy(bitmap->name,(c+1));
	
	c=strrchr(bitmap->name,'.');
	if (c!=NULL) *c=0x0;

	strcat(bitmap->name,"_normal");
	
		// read bitmap
	
	png_data=png_utility_read(path,&bitmap->wid,&bitmap->high);
	if (png_data==NULL) return(FALSE);
		
	bitmap->alpha_mode=alpha_mode_none;
	
	bitmap->alpha_mode=alpha_mode_none;
	
		// new data
		
	wid=bitmap->wid;
	high=bitmap->high;
		
	data=valloc((wid<<2)*high);
	if (data==NULL) {
		free(png_data);
		return(FALSE);
	}

		// gray scale map
		
	gs_data=(float*)valloc((wid*high)*sizeof(float));
	if (gs_data==NULL) {
		free(png_data);
		free(data);
		return(FALSE);
	}
	
		// create the gray scale
		
	p_data=png_data;
	p_gs_data=gs_data;
	
	for (y=0;y!=high;y++) {
		for (x=0;x!=wid;x++) {
			*p_gs_data++=1.0f-((float)*p_data)/255.0f;		// assume r=g=b
			p_data+=4;
		}
	}
	
		// create the normal
	
	bitmap_normal_data_create_from_gs_data(data,gs_data,wid,high);

		// open the texture
		
	ok=bitmap_texture_open(bitmap,data,anisotropic_mode,mipmap_mode,use_compression,pixelated);

	free(png_data);
	free(gs_data);
	free(data);

	return(ok);
}

/* =======================================================

      Create Normal Map from BitMap
      
======================================================= */

bool bitmap_open_normal_from_bitmap(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,bool use_compression,bool pixelated)
{
	int					x,y,wid,high;
	char				*c;
	unsigned char		*png_data,*data,*p_data;
	float				r,g,b;
	float				*gs_data,*p_gs_data;
	bool				ok;
	
	bitmap_new(bitmap);
	
		// get name
		
	c=strrchr(path,'/');
	if (c!=NULL) strcpy(bitmap->name,(c+1));
	
	c=strrchr(bitmap->name,'.');
	if (c!=NULL) *c=0x0;

	strcat(bitmap->name,"_normal");
	
		// read bitmap
	
	png_data=png_utility_read(path,&bitmap->wid,&bitmap->high);
	if (png_data==NULL) return(FALSE);
		
	bitmap->alpha_mode=alpha_mode_none;
	
		// translate the data
		
	wid=bitmap->wid;
	high=bitmap->high;
		
	data=valloc((wid<<2)*high);
	if (data==NULL) {
		free(png_data);
		return(FALSE);
	}

		// gray scale map
		
	gs_data=(float*)valloc((wid*high)*sizeof(float));
	if (gs_data==NULL) {
		free(png_data);
		free(data);
		return(FALSE);
	}
	
		// create the gray scale
		
	p_data=png_data;
	p_gs_data=gs_data;
	
	for (y=0;y!=high;y++) {
		for (x=0;x!=wid;x++) {
			r=((float)(*p_data++))/255.0f;
			g=((float)(*p_data++))/255.0f;
			b=((float)(*p_data++))/255.0f;
			*p_data++;
			
			*p_gs_data++=(r+g+b)/3.0f;
		}
	}
	
		// create the normal
		
	bitmap_normal_data_create_from_gs_data(data,gs_data,wid,high);

		// open the texture

	ok=bitmap_texture_open(bitmap,data,anisotropic_mode,mipmap_mode,use_compression,pixelated);
	
	free(png_data);
	free(gs_data);
	free(data);
	
	return(ok);
}


