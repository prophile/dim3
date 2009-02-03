/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Mesh File Loading

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

char						deform_mode_str[][32]=deform_mode_xml_list_str;

model_tag					bone_parent_tag[max_model_bone],
                            major_bone_tag[max_model_vertex],
							minor_bone_tag[max_model_vertex];

/* =======================================================

      Read Mesh XML
      
======================================================= */

bool read_mesh_xml(model_type *model)
{
	int						version,model_head;
	char					sub_path[1024],path[1024];

        // load the model xml
		
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,"mesh","xml");
	
	if (!xml_open_file(path)) return(FALSE);

    model_head=xml_findrootchild("Model");
    if (model_head==-1) {
		xml_close_file();
		return(FALSE);
	}
	
		// get the version
	
	version=xml_get_attribute_int_default(model_head,"version",1);
	
		// decode
		
	switch (version) {
		case 1:
			decode_mesh_v1_xml(model,model_head);
			break;
		case 2:
			decode_mesh_v2_xml(model,model_head);
			break;
	}
	
	xml_close_file(); 
	  
    return(TRUE);
}

/* =======================================================

      Write Mesh XML
      
======================================================= */

bool write_mesh_xml(model_type *model)
{
	char					path[1024];
	bool					ok;
    
		// start
		
    xml_new_file();
    
    xml_add_tagstart("Model");
	xml_add_attribute_int("version",2);
    xml_add_tagend(FALSE);
    
		// encode
		
	encode_mesh_v2_xml(model);

        // save model
        
    xml_add_tagclose("Model");
    
	sprintf(path,"%s/mesh.xml",model->load_base_path);
	ok=xml_save_file(path);
	
    xml_close_file();

	return(ok);
}
