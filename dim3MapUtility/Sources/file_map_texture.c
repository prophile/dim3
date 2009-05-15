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
	int					n,k;
	texture_type		*texture;
	texture_frame_type	*frame;
	
	texture=map->textures;
	
	for (n=0;n!=max_map_texture;n++) {
	
		frame=texture->frames;
		
		for (k=0;k!=max_texture_frame;k++) {
			bitmap_new(&frame->bitmap);
			bitmap_new(&frame->bumpmap);
			bitmap_new(&frame->specularmap);
			bitmap_new(&frame->glowmap);
			
			frame++;
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
	char				path[1024],name[256];
	bool				txt_ok[max_map_texture];
	texture_type		*texture;
	texture_frame_type	*frame;
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
				txt_ok[poly->uv[0].txt_idx]=TRUE;
				poly++;
			}
			
			mesh++;
		}
		
		liq=map->liquid.liquids;
		
		for (n=0;n!=map->liquid.nliquid;n++) {
			txt_ok[liq->txt_idx]=TRUE;
			liq++;
		}
		
		if (map->fog.texture_idx!=-1) txt_ok[map->fog.texture_idx]=TRUE;
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
			
		frame=texture->frames;
		
		for (k=0;k!=max_texture_frame;k++) {
		
			if (frame->name[0]!=0x0) {
			
					// bitmap
					
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",frame->name,"png");
				bitmap_open(&frame->bitmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,maputility_settings.compression,texture->pixelated,FALSE);
			
					// bumpmap
					
				sprintf(name,"%s_n",frame->name);
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",name,"png");
				bitmap_open(&frame->bumpmap,path,anisotropic_mode_none,maputility_settings.mipmap_mode,maputility_settings.compression,texture->pixelated,FALSE);
								
					// specular map
					
				sprintf(name,"%s_s",frame->name);
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",name,"png");
				bitmap_open(&frame->specularmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,maputility_settings.compression,texture->pixelated,FALSE);
				
					// glow map
					
				sprintf(name,"%s_g",frame->name);
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",name,"png");
				bitmap_open(&frame->glowmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,maputility_settings.compression,texture->pixelated,TRUE);
			}
			
			frame++;
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
	texture_frame_type		*frame;
	
	texture=map->textures;
	
	for (i=0;i!=max_map_texture;i++) {
		
		frame=texture->frames;
		
		for (k=0;k!=max_texture_frame;k++) {
			bitmap_close(&frame->bitmap);
			bitmap_close(&frame->bumpmap);
			bitmap_close(&frame->specularmap);
			bitmap_close(&frame->glowmap);
			
			frame++;
		}
		
		texture++;
	}
}

