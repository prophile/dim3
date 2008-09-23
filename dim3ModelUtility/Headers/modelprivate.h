/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Private Header

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

//
// Setting Structure
//

typedef struct		{
						int						anisotropic_mode,mipmap_mode;
						bool					card_generated_mipmaps,compression;
						file_path_setup_type	file_path_setup;
                    } modelutility_settings_type;

//
// Functions
//

extern int model_find_bone(model_type *mdl,model_tag tag);
extern int model_find_pose(model_type *mdl,char *posename);
extern bool xml_add_attribute_model_tag(char *name,model_tag tag);

extern model_tag xml_get_attribute_model_tag(int xml_tag,char *name);
extern void decode_mesh_v1_xml(model_type *model,int model_head);
extern void decode_mesh_v2_xml(model_type *model,int model_head);
extern void encode_mesh_v2_xml(model_type *model);
extern bool read_mesh_xml(model_type *model);
extern bool write_mesh_xml(model_type *model);

extern bool read_pose_xml(model_type *model);
extern bool write_pose_xml(model_type *model);
extern bool read_animate_xml(model_type *model);
extern bool write_animate_xml(model_type *model);

extern void model_textures_clear(model_type *model);
extern void model_textures_read(model_type *model);
extern void model_textures_close(model_type *model);
extern bool model_shaders_read(model_type *model);
extern void model_shaders_close(model_type *model);
extern void model_calculate_parents(model_type *model);
