/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shaders XML

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

extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Convert Value to Raw Type
      
======================================================= */

void read_settings_shader_string_to_custom_variable(view_shader_custom_var_type *cvar,char *vtype,char *str)
{
	char			*c,*c2;
	
	memset(&cvar->value,0x0,sizeof(view_shader_custom_var_type));
	
		// int
		
	if (strcasecmp(vtype,"int")==0) {
		cvar->var_type=shader_var_type_int;
		cvar->value.i=atoi(str);
		return;
	}
		
		// float
		
	if (strcasecmp(vtype,"float")==0) {
		cvar->var_type=shader_var_type_float;
		cvar->value.f=strtof(str,NULL);
		return;
	}
	
		// vec3
		
	if (strcasecmp(vtype,"vec3")==0) {
		cvar->var_type=shader_var_type_vec3;

		c2=strchr(str,',');
		if (c2==NULL) return;
		
		*c2=0x0;
		cvar->value.vec3[0]=strtof(str,NULL);
		
		c=c2+1;
		c2=strchr(c,',');
		if (c2==NULL) return;
		
		*c2=0x0;
		cvar->value.vec3[1]=strtof(c,NULL);
		
		cvar->value.vec3[2]=strtof(c2+1,NULL);
		return;
	}
	
		// vec4
		
	if (strcasecmp(vtype,"vec4")==0) {
		cvar->var_type=shader_var_type_vec4;

		c2=strchr(str,',');
		if (c2==NULL) return;
		
		*c2=0x0;
		cvar->value.vec4[0]=strtof(str,NULL);
		
		c=c2+1;
		c2=strchr(c,',');
		if (c2==NULL) return;
		
		*c2=0x0;
		cvar->value.vec4[1]=strtof(c,NULL);
		
		c=c2+1;
		c2=strchr(c,',');
		if (c2==NULL) return;
		
		*c2=0x0;
		cvar->value.vec4[2]=strtof(c,NULL);
		
		cvar->value.vec4[3]=strtof(c2+1,NULL);
		return;
	}
}

/* =======================================================

      Read Shader XML
      
======================================================= */

void read_settings_shader(void)
{
	int					nshader,nparam,shaders_head_tag,shader_tag,param_tag,tag;
	char				vtype[32],value[64],path[1024];
	view_shader_type	*shader;

		// no marks yet

	view.count.shader=0;
	
		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Shaders","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    shaders_head_tag=xml_findrootchild("Shaders");
    if (shaders_head_tag==-1) {
		xml_close_file();
		return;
	}

	nshader=xml_countchildren(shaders_head_tag);

	if (nshader==0) {
		xml_close_file();
		return;
	}

		// read the shaders
	
	shader_tag=xml_findfirstchild("Shader",shaders_head_tag);
	
	while (shader_tag!=-1) {
	
		shader=&view.shaders[view.count.shader];
		bzero(shader,sizeof(view_shader_type));
		
		xml_get_attribute_text(shader_tag,"name",shader->name,name_str_len);
		
		tag=xml_findfirstchild("Code",shader_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"vert",shader->vertex_name,file_str_len);
			xml_get_attribute_text(tag,"frag",shader->fragment_name,file_str_len);
		}
		
		param_tag=xml_findfirstchild("Params",shader_tag);
		if (tag!=-1) {
			
			tag=xml_findfirstchild("Param",param_tag);
			
			nparam=0;
		
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",shader->custom_vars[nparam].name,name_str_len);
				
				xml_get_attribute_text(tag,"type",vtype,32);
				xml_get_attribute_text(tag,"value",value,64);
				
				read_settings_shader_string_to_custom_variable(&shader->custom_vars[nparam],vtype,value);

				nparam++;
				if (nparam==max_view_shader_custom_vars) break;
			}
		}
		
			// move on to next shader
			
		view.count.shader++;
		
		shader_tag=xml_findnextchild(shader_tag);
	}
	
	xml_close_file();
}
