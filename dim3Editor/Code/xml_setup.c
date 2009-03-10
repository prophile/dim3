/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Setup XML

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "common_view.h"

extern file_path_setup_type		file_path_setup;

setup_type						setup;

/* =======================================================

      Default Preferences
      
======================================================= */

void setup_xml_default(void)
{
	setup.mipmap_mode=mipmap_mode_bilinear;
	
	setup.auto_texture=TRUE;
	setup.duplicate_offset=5;
	
	strcpy(setup.engine_name,"dim3 Engine");
	
	setup.col.mesh_line.r=0.5f;
	setup.col.mesh_line.g=0.5f;
	setup.col.mesh_line.b=1.0f;
	
	setup.col.mesh_sel.r=1.0f;
	setup.col.mesh_sel.g=1.0f;
	setup.col.mesh_sel.b=0.0f;
	
	setup.col.poly_sel.r=1.0f;
	setup.col.poly_sel.g=0.0f;
	setup.col.poly_sel.b=0.0f;
	
	setup.col.background.r=0.75f;
	setup.col.background.g=0.75f;
	setup.col.background.b=0.75f;
}

/* =======================================================

      Read Setup
      
======================================================= */

bool setup_xml_read(void)
{
	int							setup_tag;
	char						path[1024];
	
		// default prefs
		
	setup_xml_default();
    
        // find the setup.xml file
		
	file_paths_preferences(path,"dim3 Editor Setup","xml");
	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
       
    setup_tag=xml_findrootchild("Setup");
    if (setup_tag==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// keys

    xml_key_read_int(setup_tag,"Mipmap_Mode",&setup.mipmap_mode);
 	xml_key_read_boolean(setup_tag,"Auto_Texture",&setup.auto_texture);
	xml_key_read_int(setup_tag,"Duplicate_Offset",&setup.duplicate_offset);
	xml_key_read_text(setup_tag,"Engine_Name",setup.engine_name,256);
	xml_key_read_color(setup_tag,"Color_Mesh_Line",&setup.col.mesh_line);
	xml_key_read_color(setup_tag,"Color_Mesh_Sel",&setup.col.mesh_sel);
	xml_key_read_color(setup_tag,"Color_Poly_Sel",&setup.col.poly_sel);
	xml_key_read_color(setup_tag,"Color_Background",&setup.col.background);
  
	xml_close_file();
	
	return(TRUE);
}

/* =======================================================

      Write Key
      
======================================================= */

bool setup_xml_write(void)
{
	char						path[1024];
	bool						ok;
	
		// start the setup file
		
    xml_new_file();
    
    xml_add_tagstart("Setup");
    xml_add_tagend(FALSE);
	
		// keys
		
    xml_key_write_int("Mipmap_Mode",setup.mipmap_mode);
 	xml_key_write_boolean("Auto_Texture",setup.auto_texture);
	xml_key_write_int("Duplicate_Offset",setup.duplicate_offset);
	xml_key_write_text("Engine_Name",setup.engine_name);
	xml_key_write_color("Color_Mesh_Line",&setup.col.mesh_line);
	xml_key_write_color("Color_Mesh_Sel",&setup.col.mesh_sel);
	xml_key_write_color("Color_Poly_Sel",&setup.col.poly_sel);
	xml_key_write_color("Color_Background",&setup.col.background);

        // save the setup
		// always save to user specific data
		
	file_paths_preferences(path,"dim3 Editor Setup","xml");
	ok=xml_save_file(path);
    xml_close_file();
	
	return(ok);
}
