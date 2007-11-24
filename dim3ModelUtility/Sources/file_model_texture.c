/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Texture File Loading

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

extern modelutility_settings_type		modelutility_settings;

/* =======================================================

      Clear Textures
      
======================================================= */

void model_textures_clear(model_type *model)
{
	int						i,k;
    texture_type			*texture;
	bitmap_type				*bitmap,*bumpmap,*specularmap,*glowmap;

    texture=model->textures;
    
	for (i=0;i!=max_model_texture;i++) {
    
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

      Load Textures
      
======================================================= */

void model_textures_read(model_type *model)
{
    int						i,k;
    char					sub_path[1024],path[1024];
	texture_type			*texture;
	bitmap_type				*bitmap,*bumpmap,*specularmap,*glowmap,
							height_bitmap;
    
        // load the fills
        
    texture=model->textures;
    
	for (i=0;i!=max_model_texture;i++) {
    
		bitmap=texture->bitmaps;
		bumpmap=texture->bumpmaps;
		specularmap=texture->specularmaps;
		glowmap=texture->glowmaps;
        
        for (k=0;k!=max_texture_frame;k++) {
		
			if (bitmap->name[0]!=0x0) {
				sprintf(sub_path,"Models/%s/Bitmaps/Textures",model->name);
				file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,bitmap->name,"png");
				bitmap_open(bitmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.texture_quality_mode,modelutility_settings.mipmap_mode,modelutility_settings.card_generated_mipmaps,modelutility_settings.compression);

					// bumpmap
					
				switch (texture->bump_mode) {
				
					case bump_mode_auto_generate:
						bitmap_create_normal_from_bitmap(bumpmap,bitmap,modelutility_settings.anisotropic_mode,modelutility_settings.mipmap_mode,modelutility_settings.card_generated_mipmaps,modelutility_settings.compression);
						break;
						
					case bump_mode_height_map:
						bitmap_new(&height_bitmap);
						sprintf(sub_path,"Models/%s/Bitmaps/Textures_Height",model->name);
						file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,bumpmap->name,"png");
						bitmap_open(&height_bitmap,path,anisotropic_mode_none,modelutility_settings.texture_quality_mode,mipmap_mode_none,FALSE,FALSE);
						bitmap_create_normal_from_height_bitmap(bumpmap,&height_bitmap,modelutility_settings.anisotropic_mode,modelutility_settings.mipmap_mode,modelutility_settings.card_generated_mipmaps,modelutility_settings.compression);
						bitmap_close(&height_bitmap);
						break;
						
					case bump_mode_normal_map:
						sprintf(sub_path,"Models/%s/Bitmaps/Textures_dot3",model->name);
						file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,bumpmap->name,"png");
						bitmap_open(bumpmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.texture_quality_mode,modelutility_settings.mipmap_mode,modelutility_settings.card_generated_mipmaps,modelutility_settings.compression);
						break;
						
				}
				
					// specular map

				if (specularmap->name[0]!=0x0) {
					sprintf(sub_path,"Models/%s/Bitmaps/Textures_Specular",model->name);
					file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,specularmap->name,"png");
					bitmap_open(specularmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.texture_quality_mode,modelutility_settings.mipmap_mode,modelutility_settings.card_generated_mipmaps,modelutility_settings.compression);
				}
				
					// glow map

				if (glowmap->name[0]!=0x0) {
					sprintf(sub_path,"Models/%s/Bitmaps/Textures_Glow",model->name);
					file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,glowmap->name,"png");
					bitmap_open(glowmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.texture_quality_mode,modelutility_settings.mipmap_mode,modelutility_settings.card_generated_mipmaps,modelutility_settings.compression);
				}
			}
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
        }
        
        texture++;
	}
}

/* =======================================================

      Convert Glow Maps
      
======================================================= */

bool model_textures_setup_glowmaps(model_type *model)
{
	int					i,k;
	texture_type		*texture;
	bitmap_type			*bitmap,*glowmap;
	
    texture=model->textures;
    
	for (i=0;i!=max_model_texture;i++) {
		bitmap=texture->bitmaps;
		glowmap=texture->glowmaps;
		
		for (k=0;k!=max_texture_frame;k++) {
			if ((bitmap->data!=NULL) && (glowmap->data!=NULL)) {
				if (!bitmap_setup_render_glowmap(glowmap,modelutility_settings.anisotropic_mode,modelutility_settings.mipmap_mode,modelutility_settings.card_generated_mipmaps,modelutility_settings.compression)) return(FALSE);
			}
			bitmap++;
			glowmap++;
		}
		
		texture++;
	}
	
	return(TRUE);
}

/* =======================================================

      Write Textures
      
======================================================= */

void model_textures_write(model_type *model)
{
    int						i,k;
    char					path[1024];
    texture_type			*texture;
	bitmap_type				*bitmap,*bumpmap,*specularmap,*glowmap;
	struct stat				sb;

    texture=model->textures;
    
	for (i=0;i!=max_model_texture;i++) {
    
		bitmap=texture->bitmaps;
		bumpmap=texture->bumpmaps;
 		specularmap=texture->specularmaps;
		glowmap=texture->glowmaps;
   
        for (k=0;k!=max_texture_frame;k++) {
		
			if (bitmap->data!=NULL) {
			
				sprintf(path,"%s/Bitmaps/Textures/%s.png",model->load_base_path,bitmap->name);
				if (stat(path,&sb)!=0) bitmap_save(bitmap,path);		// only save if bitmap doesn't exist
				
				switch (texture->bump_mode) {
					case bump_mode_height_map:
					case bump_mode_normal_map:
						if (bumpmap->data!=NULL) {
							sprintf(path,"%s/Bitmaps/Textures_dot3/%s.png",model->load_base_path,bumpmap->name);
							if (stat(path,&sb)!=0) bitmap_save(bumpmap,path);
						}
						break;
				}
				
				if (specularmap->name[0]!=0x0) {
					sprintf(path,"%s/Bitmaps/Textures_Specular/%s.png",model->load_base_path,specularmap->name);
					if (stat(path,&sb)!=0) bitmap_save(specularmap,path);
				}
				
				if (glowmap->name[0]!=0x0) {
					sprintf(path,"%s/Bitmaps/Textures_Glow/%s.png",model->load_base_path,glowmap->name);
					if (stat(path,&sb)!=0) bitmap_save(glowmap,path);
				}
			}
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
        }
        
        texture++;
	}
}

/* =======================================================

      Close Textures
      
======================================================= */

void model_textures_close(model_type *model)
{
    int						i,k;
    texture_type			*texture;
	bitmap_type				*bitmap,*bumpmap,*specularmap,*glowmap;

    texture=model->textures;
    
	for (i=0;i!=max_model_texture;i++) {
    
		bitmap=texture->bitmaps;
		bumpmap=texture->bumpmaps;
  		specularmap=texture->specularmaps;
		glowmap=texture->glowmaps;
  
        for (k=0;k!=max_texture_frame;k++) {
			if (bitmap->data!=NULL) bitmap_close(bitmap);
			if (bumpmap->data!=NULL) bitmap_close(bumpmap);
			if (specularmap->data!=NULL) bitmap_close(specularmap);
			if (glowmap->data!=NULL) bitmap_close(glowmap);
			
			bitmap++;
			bumpmap++;
			specularmap++;
			glowmap++;
        }
        
        texture++;
	}
}


