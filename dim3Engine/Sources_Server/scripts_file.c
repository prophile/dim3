/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Loading and Parsing

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

#include "scripts.h"

extern js_type				js;
extern setup_type			setup;

/* =======================================================

      Load/Release Scripts
      
======================================================= */

bool script_load_file(script_type *script,char *script_dir,char *script_name,char *err_str)
{
    int					sz;
    char				file_name[256],sub_path[1024],path[1024];
	char				*data;
	FILE				*file;
	struct stat			sb;
	
	script->data=NULL;
	
		// read file in null terminated string
		
	sprintf(file_name,"%s.js",script_name);
	
	sprintf(sub_path,"Scripts/%s",script_dir);
	file_paths_data(&setup.file_path_setup,path,sub_path,script_name,"js");
		
	file=fopen(path,"rb");
	if (file==NULL) {
		sprintf(err_str,"JavaScript Engine: Could not open script '%s'",path);
		return(FALSE);
	}
	
	stat(path,&sb);
	sz=sb.st_size;
	
	data=valloc(sz+1);
	if (data==NULL) {
		fclose(file);
		sprintf(err_str,"JavaScript Engine: Not enough memory to load script '%s'",path);
		return(FALSE);
	}
	
	fread(data,1,sz,file);
	fclose(file);
	
	data[sz]=0x0;
	
		// parse defines
	
	data=script_parse_defines(data,&sz);
	
		// setup script

	sprintf(script->name,"%s.js",script_name);
	script->data=data;
	script->data_len=sz;
	
	return(TRUE);
}

void script_free_file(script_type *script)
{
	if (script->data!=NULL) free(script->data);
}

