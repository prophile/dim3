/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: GLSL Shaders

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

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Shader Program Files
      
======================================================= */

char* shader_open_file(char *path)
{
	int				sz;
	char			*data;
	FILE			*file;
	struct stat		sb;
	
		// get file size
		
	if (stat(path,&sb)!=0) return(NULL);
	sz=sb.st_size;
	
		// open file
		
	file=fopen(path,"rb");
	if (file==NULL) return(NULL);
    
	data=(char*)malloc(sz+1);
    if (data==NULL) {
        fclose(file);
        return(NULL);
    }
	
	fread(data,1,sz,file);
	fclose(file);
	
	data[sz]=0x0;
    
	return(data);
}

/* =======================================================

      Shader Debugging
      
======================================================= */

bool shader_report_error(char *err_str,char *vertex_name,char *fragment_name,GLhandleARB hand,char *err_type,int check_type)
{
	GLint			result,len;
	char			*str;
	
		// operation OK?
		
	glGetObjectParameterivARB(hand,check_type,&result);
	if (result==1) return(FALSE);
	
		// setup init error

	strcpy(err_str,"GLSL Error [");
	strcat(err_str,err_type);

	if (vertex_name!=NULL) {
		strcat(err_str,"; ");
		strcat(err_str,vertex_name);
	}

	if (fragment_name!=NULL) {
		strcat(err_str,"; ");
		strcat(err_str,fragment_name);
	}

	strcat(err_str,"]\n");
		
	glGetObjectParameterivARB(hand,GL_OBJECT_INFO_LOG_LENGTH_ARB,&len);
	if (len<=0) return(TRUE);
	
	str=malloc(len);
	if (str==NULL) return(TRUE);
	
	glGetInfoLogARB(hand,len,NULL,str);

	strncat(err_str,str,max_shader_error_len);
	err_str[max_shader_error_len-1]=0x0;

	free(str);
	
	return(TRUE);
}

/* =======================================================

      Initialize and Free Shader Structures
      
======================================================= */

void shader_init_objects(shader_type *shader)
{
	shader->vertex_obj=0;
	shader->fragment_obj=0;
	shader->program_obj=0;

	shader->init_error[0]=0x0;
}

void shader_free_objects(shader_type *shader)
{
	if (shader->program_obj==0) return;
	
	if (shader->vertex_obj!=0) {
		glDetachObjectARB(shader->program_obj,shader->vertex_obj);
		glDeleteObjectARB(shader->vertex_obj);
		shader->vertex_obj=0;
	}
	
	if (shader->fragment_obj!=0) {
		glDetachObjectARB(shader->program_obj,shader->fragment_obj);
		glDeleteObjectARB(shader->fragment_obj);
		shader->fragment_obj=0;
	}
	
	glDeleteObjectARB(shader->program_obj);
	shader->program_obj=0;
}

/* =======================================================

      Add and Delete Shaders
      
======================================================= */

bool shader_load(shader_type *shader,char *vert_path,char *frag_path)
{
	char				*vertex_data,*fragment_data;
	const GLcharARB		*vertex_ptr,*fragment_ptr;
		
		// load the shaders
	
	vertex_data=fragment_data=NULL;
	
	if (vert_path[0]!=0x0) {
		vertex_data=shader_open_file(vert_path);
		if (vertex_data==NULL) {
			snprintf(shader->init_error,max_shader_error_len,"GLSL Error:\n Missing vertex shader file named '%s'",vert_path);
			shader->init_error[max_shader_error_len-1]=0x0;
			return(FALSE);
		}
	}
	
	if (frag_path[0]!=0x0) {
		fragment_data=shader_open_file(frag_path);
		if (fragment_data==NULL) {
			snprintf(shader->init_error,max_shader_error_len,"GLSL Error:\n Missing fragment shader file named '%s'",frag_path);
			shader->init_error[max_shader_error_len-1]=0x0;
			return(FALSE);
		}
	}
	
		// create the shader
		
	shader->program_obj=glCreateProgramObjectARB();
		
	if (vertex_data!=NULL) {
		shader->vertex_obj=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		
		vertex_ptr=vertex_data;
		glShaderSourceARB(shader->vertex_obj,1,&vertex_ptr,NULL);
	
		glCompileShaderARB(shader->vertex_obj);
		free(vertex_data);
		
		if (shader_report_error(shader->init_error,shader->vertex_name,NULL,shader->vertex_obj,"Vertex",GL_OBJECT_COMPILE_STATUS_ARB)) {
			shader_free_objects(shader);
			free(fragment_data);
			return(FALSE);
		}
		
		glAttachObjectARB(shader->program_obj,shader->vertex_obj);
	}
	
	if (fragment_data!=NULL) {
		shader->fragment_obj=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		
		fragment_ptr=fragment_data;
		glShaderSourceARB(shader->fragment_obj,1,&fragment_ptr,NULL);
		
		glCompileShaderARB(shader->fragment_obj);
		free(fragment_data);
		
		if (shader_report_error(shader->init_error,NULL,shader->fragment_name,shader->fragment_obj,"Fragment",GL_OBJECT_COMPILE_STATUS_ARB)) {
			shader_free_objects(shader);
			return(FALSE);
		}
		
		glAttachObjectARB(shader->program_obj,shader->fragment_obj);
	}
	
		// link shaders into program
		
	glLinkProgramARB(shader->program_obj);
	if (shader_report_error(shader->init_error,shader->vertex_name,shader->fragment_name,shader->program_obj,"Program",GL_OBJECT_LINK_STATUS_ARB)) {
		shader_free_objects(shader);
		return(FALSE);
	}
	
	return(TRUE);
}

bool shader_map_add(file_path_setup_type *file_path_setup,shader_type *shader)
{
	char		vert_path[1024],frag_path[1024];
	
		// init shaders
		
	shader_init_objects(shader);
	
		// any shaders?
		
	if ((!shader->on) && (shader->vertex_name[0]==0x0) && (shader->fragment_name[0]==0x0)) return(TRUE);
	
		// load shaders
		
	vert_path[0]=0x0;
	if (shader->vertex_name[0]!=0x0) file_paths_data(file_path_setup,vert_path,"Shaders",shader->vertex_name,"vert");
	
	frag_path[0]=0x0;
	if (shader->fragment_name[0]!=0x0) file_paths_data(file_path_setup,frag_path,"Shaders",shader->fragment_name,"frag");
	
	return(shader_load(shader,vert_path,frag_path));
}

bool shader_model_add(file_path_setup_type *file_path_setup,char *model_name,shader_type *shader)
{
	char		sub_path[1024],vert_path[1024],frag_path[1024];
	
		// init shaders
		
	shader_init_objects(shader);
	
		// any shaders?
		
	if ((!shader->on) && (shader->vertex_name[0]==0x0) && (shader->fragment_name[0]==0x0)) return(TRUE);
	
		// load shaders
		
	sprintf(sub_path,"Models/%s/Shaders",model_name);
	
	vert_path[0]=0x0;
	if (shader->vertex_name[0]!=0x0) file_paths_data(file_path_setup,vert_path,sub_path,shader->vertex_name,"vert");

	frag_path[0]=0x0;
	if (shader->fragment_name[0]!=0x0) file_paths_data(file_path_setup,frag_path,sub_path,shader->fragment_name,"frag");
	
	return(shader_load(shader,vert_path,frag_path));
}

void shader_delete(shader_type *shader)
{
	shader_free_objects(shader);
}

