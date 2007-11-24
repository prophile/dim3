/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Sound XML

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "sounds.h"
#include "xmls.h"

extern setup_type				setup;

/* =======================================================

      Read Sound XML
      
======================================================= */

void read_settings_sound(void)
{
	int					sounds_head_tag,sound_tag,tag,
						min_dist,max_dist;
	char				name[name_str_len],filename[file_str_len],
						path[1024],wave_path[1024];
	
		// read in sound from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Sounds","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    sounds_head_tag=xml_findrootchild("Sounds");
    if (sounds_head_tag!=-1) {
	
		sound_tag=xml_findfirstchild("Sound",sounds_head_tag);
		
		while (sound_tag!=-1) {
			xml_get_attribute_text(sound_tag,"name",name,name_str_len);
			
			filename[0]=0x0;
			min_dist=max_dist=0;
			
			tag=xml_findfirstchild("Wave",sound_tag);
			if (tag!=-1) {
				xml_get_attribute_text(tag,"file",filename,file_str_len);
			}
			
			tag=xml_findfirstchild("Distance",sound_tag);
			if (tag!=-1) {
				min_dist=xml_get_attribute_int(tag,"min");
				max_dist=xml_get_attribute_int(tag,"max");
			}
			
			file_paths_data(&setup.file_path_setup,wave_path,"Sounds",filename,"wav");
			
			al_open_buffer(name,wave_path,min_dist,max_dist);

			sound_tag=xml_findnextchild(sound_tag);
		}
	}
	
	xml_close_file();
}

