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
	setup.anisotropic_mode=anisotropic_mode_none;
	setup.mipmap_mode=mipmap_mode_bilinear;
	setup.mipmap_card_generated=FALSE;
	setup.texture_compression=FALSE;
	
	setup.view_flip=FALSE;
	setup.auto_texture=TRUE;
	setup.duplicate_offset=5;
	
	strcpy(setup.engine_name,"dim3 Engine");
}

/* =======================================================

      Read Setup
      
======================================================= */

void setup_xml_read_key_int(int setup_tag,char *name,int *value)
{
	int			tag;

	tag=xml_findfirstchild(name,setup_tag);
	if (tag!=-1) *value=xml_get_attribute_int(tag,"value");	
}

void setup_xml_read_key_float(int setup_tag,char *name,float *value)
{
	int			tag;

	tag=xml_findfirstchild(name,setup_tag);
	if (tag!=-1) *value=xml_get_attribute_float(tag,"value");	
}

void setup_xml_read_key_text(int setup_tag,char *name,char *value,int value_sz)
{
	int			tag;

	tag=xml_findfirstchild(name,setup_tag);
	if (tag!=-1) xml_get_attribute_text(tag,"value",value,value_sz);	
}

void setup_xml_read_key_boolean(int setup_tag,char *name,bool *value)
{
	int			tag;

	tag=xml_findfirstchild(name,setup_tag);
	if (tag!=-1) *value=xml_get_attribute_boolean(tag,"value");	
}

bool setup_xml_read(void)
{
	int							setup_tag;
	char						path[1024];
	
		// default prefs
		
	setup_xml_default();
    
        // find the setup.xml file
        
	file_paths_app(&file_path_setup,path,"Contents/Resources/Settings","Setup","xml");
	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
       
    setup_tag=xml_findrootchild("Setup");
    if (setup_tag==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// keys

	setup_xml_read_key_boolean(setup_tag,"Texture_Compression",&setup.texture_compression);
	setup_xml_read_key_int(setup_tag,"Anisotropic_Mode",&setup.anisotropic_mode);
    setup_xml_read_key_int(setup_tag,"Mipmap_Mode",&setup.mipmap_mode);
    setup_xml_read_key_boolean(setup_tag,"Mipmap_Card_Generated",&setup.mipmap_card_generated);

    setup_xml_read_key_boolean(setup_tag,"View_Flip",&setup.view_flip);
    setup_xml_read_key_boolean(setup_tag,"Auto_Texture",&setup.auto_texture);
	setup_xml_read_key_int(setup_tag,"Duplicate_Offset",&setup.duplicate_offset);
	
	setup_xml_read_key_text(setup_tag,"Engine_Name",setup.engine_name,256);
   
	xml_close_file();
	
	return(TRUE);
}

