/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Texture Reading

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

      New Map Textures
      
======================================================= */

void map_textures_new(map_type *map)
{
	int					i,k;
	texture_type		*texture;
	bitmap_type			*bitmap,*bumpmap,*specularmap,*glowmap;
	
	texture=map->textures;
	
	for (i=0;i!=max_map_texture;i++) {
	
		bitmap=texture->bitmaps;
		bumpmap=texture->bumpmaps;
		specularmap=texture->specularmaps;
		glowmap=texture->glowmaps;
		
		for (k=0;k!=max_texture_frame;k++) {
			bitmap_new(bitmap);
			bitmap_new(bumpmap);
			bitmap_new(specularmap);
			bitmap_new(glowmap);
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
		}
		
		texture++;
	}
}

/* =======================================================

      Read Map Textures
      
======================================================= */

bool map_textures_read(map_type *map)
{
	int					i,k;
	char				path[1024];
	texture_type		*texture;
	bitmap_type			*bitmap,*bumpmap,*specularmap,*glowmap,
						height_bitmap;
	
	texture=map->textures;
	
	for (i=0;i!=max_map_texture;i++) {
		
		bitmap=texture->bitmaps;
		bumpmap=texture->bumpmaps;
		specularmap=texture->specularmaps;
		glowmap=texture->glowmaps;
		
		for (k=0;k!=max_texture_frame;k++) {
		
			if (bitmap->name[0]!=0x0) {
			
					// bitmap
					
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",bitmap->name,"png");
				bitmap_open(bitmap,path,maputility_settings.anisotropic_mode,maputility_settings.texture_quality_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression);
			
					// bumpmap
					
				switch (texture->bump_mode) {
				
					case bump_mode_auto_generate:
						bitmap_create_normal_from_bitmap(bumpmap,bitmap,anisotropic_mode_none,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression);
						break;
						
					case bump_mode_height_map:
						bitmap_new(&height_bitmap);
						file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_Height",bumpmap->name,"png");
						bitmap_open(&height_bitmap,path,anisotropic_mode_none,maputility_settings.texture_quality_mode,mipmap_mode_none,FALSE,FALSE);
						bitmap_create_normal_from_height_bitmap(bumpmap,&height_bitmap,anisotropic_mode_none,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression);
						bitmap_close(&height_bitmap);
						break;
						
					case bump_mode_normal_map:
						file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_dot3",bumpmap->name,"png");
						bitmap_open(bumpmap,path,anisotropic_mode_none,maputility_settings.texture_quality_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression);
						break;
						
				}
				
					// specular map
					
				if (specularmap->name[0]!=0x0) {
					file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_Specular",specularmap->name,"png");
					bitmap_open(specularmap,path,maputility_settings.anisotropic_mode,maputility_settings.texture_quality_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression);
				}
				
					// glow map
					
				if (glowmap->name[0]!=0x0) {
					file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_Glow",glowmap->name,"png");
					bitmap_open(glowmap,path,maputility_settings.anisotropic_mode,maputility_settings.texture_quality_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression);
				}
				
			}
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
		}
		
		texture++;
	}
	
	return(TRUE);
}

/* =======================================================

      Convert Glow Maps
      
======================================================= */

bool map_textures_setup_glowmaps(map_type *map)
{
	int					i,k;
	texture_type		*texture;
	bitmap_type			*bitmap,*glowmap;
	
	texture=map->textures;
	
	for (i=0;i!=max_map_texture;i++) {
		bitmap=texture->bitmaps;
		glowmap=texture->glowmaps;
		
		for (k=0;k!=max_texture_frame;k++) {
			if ((bitmap->data!=NULL) && (glowmap->data!=NULL)) {
				if (!bitmap_setup_render_glowmap(glowmap,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression)) return(FALSE);
			}
			bitmap++;
			glowmap++;
		}
		
		texture++;
	}
	
	return(TRUE);
}

/* =======================================================

      Close Map Textures
      
======================================================= */

void map_textures_close(map_type *map)
{
	int						i,k;
	texture_type			*texture;
	bitmap_type				*bitmap,*bumpmap,*specularmap,*glowmap;
	
	texture=map->textures;
	
	for (i=0;i!=max_map_texture;i++) {
		
		bitmap=texture->bitmaps;
		bumpmap=texture->bumpmaps;
		specularmap=texture->specularmaps;
		glowmap=texture->glowmaps;
		
		for (k=0;k!=max_texture_frame;k++) {
			if (bitmap->data!=NULL) bitmap_close(bitmap);
			if (bumpmap->data!=NULL) bitmap_close(bumpmap);
			if (specularmap->data!=NULL) bitmap_close(specularmap);
			if (glowmap->data!=NULL) bitmap_close(glowmap);
			
			bitmap->data=NULL;
			bumpmap->data=NULL;
			specularmap->data=NULL;
			glowmap->data=NULL;
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
		}
		
		texture++;
	}
}

