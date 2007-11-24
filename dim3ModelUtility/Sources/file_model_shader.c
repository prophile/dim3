/****************************** File *********************************

Module: dim3 Model Utility
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
	#include "dim3modelutility.h"
#endif

extern modelutility_settings_type		modelutility_settings;

/* =======================================================

      Read Model Shaders
      
======================================================= */

bool model_shaders_read(model_type *model)
{
	int					n;
	texture_type		*texture;

	texture=model->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		shader_model_add(&modelutility_settings.file_path_setup,model->name,&texture->shader);
		texture++;
	}
	
	return(TRUE);
}

/* =======================================================

      Close Model Shaders
      
======================================================= */

void model_shaders_close(model_type *model)
{
	int					n;
	texture_type		*texture;

	texture=model->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		shader_delete(&texture->shader);
		texture++;
	}
}

