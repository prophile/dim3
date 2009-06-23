/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: File Pathing

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

      Set File Paths
            
======================================================= */

unsigned char* zip_compress(unsigned char *data,int data_sz,int *p_compress_sz,char *err_str)
{
	int				err,compress_sz;
	unsigned char	*compress_data;

	compress_sz=data_sz+(int)(((double)data_sz)*0.1)+12;
	compress_data=malloc(compress_sz);
	if (compress_data==NULL) {
		strcpy(err_str,"Out of memory");
		return(NULL);
	}

	err=compress2(compress_data,&compress_sz,data,data_sz,9);
	if (err!=Z_OK) {
		sprintf(err_str,"Could not compress file (%d: %s)",err,zError(err));
		free(compress_data);
		return(NULL);
	}

	*p_compress_sz=compress_sz;

	return(compress_data);
}

unsigned char* zip_expand(unsigned char *compress_data,int compress_data_sz,int *p_data_sz,char *err_str)
{
	int						err,data_sz;
	unsigned char			*data;

	data_sz=*p_data_sz;

	data=malloc(data_sz);
	if (data==NULL) {
		strcpy(err_str,"Out of memory");
		return(NULL);
	}
	
	err=uncompress(data,&data_sz,compress_data,compress_data_sz);
	if (err!=Z_OK) {
		sprintf(err_str,"Could not uncompress file (%d: %s)",err,zError(err));
		free(data);
		return(NULL);
	}

	*p_data_sz=data_sz;

	return(data);
}


