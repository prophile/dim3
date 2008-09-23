/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Texture Frames

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
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

/* =======================================================

      Count Texture Frames
      
======================================================= */

int map_count_texture_frames(map_type *map,int txt)
{
	int					i;
	texture_type		*texture;
	bitmap_type			*bitmap;
		
	texture=&map->textures[txt];
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

void map_setup_animated_textures(map_type *map,int tick)
{
	bitmap_texture_setup_animation(map->textures,max_map_texture,NULL,tick);
}

/* =======================================================

      Texture Changing
      
======================================================= */

bool map_add_texture_frame(map_type *map,int txt,char *bitmap_name,char *bumpmap_name,char *specularmap_name,char *glowmap_name)
{
	int			count;
	
	count=map_count_texture_frames(map,txt);
	if (count==max_texture_frame) return(FALSE);
	
	map_textures_close(map);
	
	strcpy(map->textures[txt].bitmaps[count].name,bitmap_name);
	strcpy(map->textures[txt].bumpmaps[count].name,bumpmap_name);
	strcpy(map->textures[txt].specularmaps[count].name,specularmap_name);
	strcpy(map->textures[txt].glowmaps[count].name,glowmap_name);
	
	return(map_textures_read(map,FALSE));
}

bool map_delete_texture_frame(map_type *map,int txt)
{
	int			count;
	
	count=map_count_texture_frames(map,txt);
	if (count==0) return(FALSE);
	
	map_textures_close(map);
	
	count--;
	map->textures[txt].bitmaps[count].name[0]=0x0;
	map->textures[txt].bitmaps[count].gl_id=-1;
	map->textures[txt].bumpmaps[count].name[0]=0x0;
	map->textures[txt].bumpmaps[count].gl_id=-1;
	map->textures[txt].specularmaps[count].name[0]=0x0;
	map->textures[txt].specularmaps[count].gl_id=-1;
	map->textures[txt].glowmaps[count].name[0]=0x0;
	map->textures[txt].glowmaps[count].gl_id=-1;
	
	return(map_textures_read(map,FALSE));
}


