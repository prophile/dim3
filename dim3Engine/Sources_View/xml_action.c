/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Action XML

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

#include "xmls.h"

extern char						control_names_str[][32];

extern action_display_type		action_display[ncontrol];
extern setup_type				setup;

/* =======================================================

      Read Action XML
      
======================================================= */

void read_settings_action(void)
{
	int					n,actions_head_tag,action_tag,idx;
	char				name[name_str_len],path[1024];
						
		// default actions
		
	for (n=0;n!=ncontrol;n++) {
		strcpy(action_display[n].display_name,control_names_str[n]);
		action_display[n].show=TRUE;
	}
	
		// read in actions from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Actions","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    actions_head_tag=xml_findrootchild("Actions");
    if (actions_head_tag!=-1) {
	
		action_tag=xml_findfirstchild("Action",actions_head_tag);
		
		while (action_tag!=-1) {
			xml_get_attribute_text(action_tag,"name",name,name_str_len);
			
			idx=setup_find_control_for_name(name);
			if (idx!=-1) {
				xml_get_attribute_text(action_tag,"display",action_display[idx].display_name,name_str_len);
				action_display[idx].show=xml_get_attribute_boolean(action_tag,"show");
			}
			
			action_tag=xml_findnextchild(action_tag);
		}
	}
	
	xml_close_file();
}

