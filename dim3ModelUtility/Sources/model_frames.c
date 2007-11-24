/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Frame Routines

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
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Count Texture Frames
      
======================================================= */

int model_count_texture_frames(model_type *model,int txt)
{
	int					i;
	texture_type		*texture;
	bitmap_type			*bitmap;
	
	texture=&model->textures[txt];
	bitmap=texture->bitmaps;
	
	for (i=0;i!=max_texture_frame;i++) {
		if (bitmap->name[0]==0x0) return(i);
		bitmap++;
	}
	
	return(max_texture_frame);
}

/* =======================================================

      Set Current Frames
      
======================================================= */

void model_setup_animated_textures(model_type *model,unsigned char *user_frames,int tick)
{
	bitmap_texture_setup_animation(model->textures,max_model_texture,user_frames,tick);
}

/* =======================================================

      Texture Changing
      
======================================================= */

bool model_add_texture_frame(model_type *model,int txt,char *bitmap_name,char *bumpmap_name,char *specularmap_name,char *glowmap_name)
{
	int			count;
	
	count=model_count_texture_frames(model,txt);
	if (count==max_texture_frame) return(FALSE);
	
	model_textures_close(model);
	
	strcpy(model->textures[txt].bitmaps[count].name,bitmap_name);
	strcpy(model->textures[txt].bumpmaps[count].name,bumpmap_name);
	strcpy(model->textures[txt].specularmaps[count].name,specularmap_name);
	strcpy(model->textures[txt].glowmaps[count].name,glowmap_name);
	
	model_textures_read(model);
	
	return(TRUE);
}

bool model_delete_texture_frame(model_type *model,int txt)
{
	int			count;
	
	count=model_count_texture_frames(model,txt);
	if (count==0) return(FALSE);
	
	model_textures_close(model);
	
	count--;
	model->textures[txt].bitmaps[count].name[0]=0x0;
	model->textures[txt].bitmaps[count].data=NULL;
	model->textures[txt].bumpmaps[count].name[0]=0x0;
	model->textures[txt].bumpmaps[count].data=NULL;
	model->textures[txt].specularmaps[count].name[0]=0x0;
	model->textures[txt].specularmaps[count].data=NULL;
	model->textures[txt].glowmaps[count].name[0]=0x0;
	model->textures[txt].glowmaps[count].data=NULL;
	
	model_textures_read(model);
	
	return(TRUE);
}



