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

bool map_textures_read(map_type *map,bool in_engine)
{
	int					i,k,n;
	char				path[1024];
	bool				txt_ok[max_map_texture];
	texture_type		*texture;
	bitmap_type			*bitmap,*bumpmap,*specularmap,*glowmap;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;
						
		// if in engine, then only load textures
		// directly hooked up to elements
		
	if (in_engine) {
	
		for (n=0;n!=max_map_texture;n++) {
			txt_ok[n]=FALSE;
		}
		
		mesh=map->mesh.meshes;
		
		for (n=0;n!=map->mesh.nmesh;n++) {
			
			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				txt_ok[poly->txt_idx]=TRUE;
				poly++;
			}
			
			mesh++;
		}
		
		liq=map->liquid.liquids;
		
		for (n=0;n!=map->liquid.nliquid;n++) {
			txt_ok[liq->txt_idx]=TRUE;
			liq++;
		}
		
		if (map->background.fill!=-1) txt_ok[map->background.fill]=TRUE;
		if (map->sky.fill!=-1) txt_ok[map->sky.fill]=TRUE;
		if (map->sky.bottom_fill!=-1) txt_ok[map->sky.bottom_fill]=TRUE;
		if (map->sky.north_fill!=-1) txt_ok[map->sky.north_fill]=TRUE;
		if (map->sky.south_fill!=-1) txt_ok[map->sky.south_fill]=TRUE;
		if (map->sky.east_fill!=-1) txt_ok[map->sky.east_fill]=TRUE;
		if (map->sky.west_fill!=-1) txt_ok[map->sky.west_fill]=TRUE;
	
	}
	
		// load textures
	
	for (i=0;i!=max_map_texture;i++) {
	
		texture=&map->textures[i];
	
		if ((in_engine) && (!txt_ok[i])) continue;
			
		bitmap=texture->bitmaps;
		bumpmap=texture->bumpmaps;
		specularmap=texture->specularmaps;
		glowmap=texture->glowmaps;
		
		for (k=0;k!=max_texture_frame;k++) {
		
			if (bitmap->name[0]!=0x0) {
			
					// bitmap
					
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",bitmap->name,"png");
				bitmap_open(bitmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression,texture->pixelated,FALSE);
			
					// bumpmap

				switch (texture->bump_mode) {
				
					case bump_mode_auto_generate:
						file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",bitmap->name,"png");
						bitmap_open_normal_from_bitmap(bumpmap,path,anisotropic_mode_none,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression,texture->pixelated);
						break;
						
					case bump_mode_height_map:
						file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_Height",bumpmap->name,"png");
						bitmap_open_normal_from_height(bumpmap,path,anisotropic_mode_none,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression,texture->pixelated);
						break;
						
					case bump_mode_normal_map:
						file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_dot3",bumpmap->name,"png");
						bitmap_open(bumpmap,path,anisotropic_mode_none,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression,texture->pixelated,FALSE);
						break;
						
				}
				
					// specular map
					
				if (specularmap->name[0]!=0x0) {
					file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_Specular",specularmap->name,"png");
					bitmap_open(specularmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression,texture->pixelated,FALSE);
				}
				
					// glow map
					
				if (glowmap->name[0]!=0x0) {
					file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures_Glow",glowmap->name,"png");
					bitmap_open(glowmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,maputility_settings.card_generated_mipmaps,maputility_settings.compression,texture->pixelated,TRUE);
				}
			}
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
		}
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
			bitmap_close(bitmap);
			bitmap_close(bumpmap);
			bitmap_close(specularmap);
			bitmap_close(glowmap);
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
		}
		
		texture++;
	}
}

