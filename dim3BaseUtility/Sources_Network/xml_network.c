/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Setup Network

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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Default Preferences
      
======================================================= */

void network_xml_default(network_setup_type *net_setup)
{
	memset(net_setup,0x0,sizeof(network_setup_type));

	strcpy(net_setup->host.password,"dim3");
}

/* =======================================================

      Read Network XML
      
======================================================= */

bool network_xml_read(file_path_setup_type *file_path_setup,network_setup_type *net_setup)
{
	int						network_head_tag,host_tag,
							games_head_tag,game_tag;
	char					path[1024];
	network_setup_game_type	*game;
	
		// default prefs
		
	network_xml_default(net_setup);
	
		// read network.xml file
		
	file_paths_data(file_path_setup,path,"Settings","Network","xml");
	if (!xml_open_file(path)) return(FALSE);
	
		// decode the file
		
	network_head_tag=xml_findrootchild("Network");
	if (network_head_tag!=-1) {
	
			// query

		host_tag=xml_findfirstchild("Host",network_head_tag);
		if (host_tag!=-1) {
			xml_get_attribute_text(host_tag,"name",net_setup->host.name,name_str_len);
			xml_get_attribute_text(host_tag,"password",net_setup->host.password,32);
		}

			// games
		
		games_head_tag=xml_findfirstchild("Games",network_head_tag);
		if (games_head_tag!=-1) {
		
			game=net_setup->games;
		
			game_tag=xml_findfirstchild("Game",games_head_tag);
			while (game_tag!=-1) {
			
				xml_get_attribute_text(game_tag,"type",game->script_name,name_str_len);
				xml_get_attribute_text(game_tag,"map",game->map_name,name_str_len);
				
				game++;
				
				net_setup->ngame++;
				if (net_setup->ngame==network_setup_max_game) break;

				game_tag=xml_findnextchild(game_tag);
			}
		}
	}
	
	xml_close_file();
	
	return(TRUE);
}
