/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Shader Reading

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

      Read Map Shaders
      
======================================================= */

bool map_shaders_read(map_type *map)
{
	int					n;
	texture_type		*texture;

	texture=map->textures;
	
	for (n=0;n!=max_map_texture;n++) {
		shader_map_add(&maputility_settings.file_path_setup,&texture->shader);
		texture++;
	}
	
	return(TRUE);
}

/* =======================================================

      Close Map Shaders
      
======================================================= */

void map_shaders_close(map_type *map)
{
	int					n;
	texture_type		*texture;

	texture=map->textures;
	
	for (n=0;n!=max_map_texture;n++) {
		shader_delete(&texture->shader);
		texture++;
	}
}

