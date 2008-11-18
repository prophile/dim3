/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Halo XML

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "xmls.h"

extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Read Halos XML
      
======================================================= */

void read_settings_halo(void)
{
	int					nhalo,halos_head_tag,halo_tag,tag;
	char				path[1024];
	halo_type			*halo;

		// no halos yet

	server.halos=NULL;
	server.count.halo=0;

		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Halos","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    halos_head_tag=xml_findrootchild("Halos");
    if (halos_head_tag==-1) {
		xml_close_file();
		return;
	}

	nhalo=xml_countchildren(halos_head_tag);

	if (nhalo==0) {
		xml_close_file();
		return;
	}

	server.halos=(halo_type*)valloc(sizeof(halo_type)*nhalo);
	if (server.halos==NULL) {
		xml_close_file();
		return;
	}

		// read the halos

	halo_tag=xml_findfirstchild("Halo",halos_head_tag);
	
	while (halo_tag!=-1) {
	
			// create a new halo
			
		halo=&server.halos[server.count.halo];
		
		xml_get_attribute_text(halo_tag,"name",halo->name,name_str_len);
		
		tag=xml_findfirstchild("Image",halo_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",halo->bitmap_name,file_str_len);
		}
		
			// move on to next halo
			
		server.count.halo++;

		halo_tag=xml_findnextchild(halo_tag);
	}
	
	xml_close_file();
}
