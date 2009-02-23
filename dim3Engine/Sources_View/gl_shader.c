/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Routines

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

#include "lights.h"
#include "cameras.h"
#include "consoles.h"
#include "video.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							gl_shader_current_idx;

extern int game_time_get(void);
extern float game_time_fequency_second_get(void);

/* =======================================================

      Shader Loading Utilities
      
======================================================= */

char* gl_shader_open_file(char *path)
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

void gl_shader_set_instance_variables(view_shader_type *shader)
{
	int							n;
	GLint						var;
	view_shader_custom_var_type	*cvar;

		// texture pointers
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3Tex");
	if (var!=-1) glUniform1iARB(var,0);

	var=glGetUniformLocationARB(shader->program_obj,"dim3TexBump");
	if (var!=-1) glUniform1iARB(var,1);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1iARB(var,2);

		// custom variables
		
	for (n=0;n!=max_view_shader_custom_vars;n++) {
		cvar=&shader->custom_vars[n];

		if (cvar->name[0]==0x0) continue;
		
		var=glGetUniformLocationARB(shader->program_obj,cvar->name);
		if (var==-1) continue;
		
		switch (cvar->var_type) {
		
			case shader_var_type_int:
				glUniform1iARB(var,cvar->value.i);
				break;
				
			case shader_var_type_float:
				glUniform1fARB(var,cvar->value.f);
				break;
				
			case shader_var_type_vec3:
				glUniform3fARB(var,cvar->value.vec3[0],cvar->value.vec3[1],cvar->value.vec3[2]);
				break;
				
			case shader_var_type_vec4:
				glUniform4fARB(var,cvar->value.vec4[0],cvar->value.vec4[1],cvar->value.vec4[2],cvar->value.vec4[3]);
				break;
		}
	}
}

/* =======================================================

      Shader Error Reporter
      
======================================================= */

bool gl_shader_report_error(char *err_str,char *vertex_name,char *fragment_name,GLhandleARB hand,char *err_type,char *code,int check_type)
{
	int				line;
	GLint			result,len;
	char			*c,*str,log_line[256];
	FILE			*file;
	struct tm		*tm;
	time_t			curtime;
	
		// operation OK?
		
	glGetObjectParameterivARB(hand,check_type,&result);
	if (result==1) return(FALSE);
	
		// start or append log file
		
	file=fopen("glsl_error_log.txt","a");
	if (file==NULL) return(TRUE);
		
		// header
		
	fwrite("***\n",1,4,file);

	sprintf(log_line,"Error Type: %s\n",err_type);
	fwrite(log_line,1,strlen(log_line),file);
	
	if (vertex_name!=NULL) {
		sprintf(log_line,"Vertex: %s\n",vertex_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	if (fragment_name!=NULL) {
		sprintf(log_line,"Fragment: %s\n",fragment_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	curtime=time(NULL);
	tm=localtime(&curtime);
	
	sprintf(log_line,"Time: %.4d %.2d %.2d %.2d:%.2d.%.2d\n",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	fwrite(log_line,1,strlen(log_line),file);

		// the error
		
	glGetObjectParameterivARB(hand,GL_OBJECT_INFO_LOG_LENGTH_ARB,&len);
	if (len>0) {
		str=malloc(len);
		if (str!=NULL) {
			glGetInfoLogARB(hand,len,NULL,str);

			fwrite(str,1,len,file);
			fwrite("\n",1,1,file);

			free(str);
		}
	}
	
		// the code
		
	if (code!=NULL) {
	
		c=code;
		line=2;
		
		fwrite("001:",1,4,file);
		
		while (*c!=0x0) {
			
			fwrite(c,1,1,file);
			
			if (*c=='\n') {
				sprintf(log_line,"%.3d:",line);
				fwrite(log_line,1,4,file);
				line++;
			}

			c++;
		}
	}
	
	fwrite("\n",1,1,file);
		
		// finish log

	fclose(file);

		// fatal error string

	strcpy(err_str,"GLSL: Could not compile or link code, check out glsl_error_log.txt for more information");
	
	return(TRUE);
}

/* =======================================================

      Shader Init/Shutdown
      
======================================================= */

bool gl_shader_initialize(char *err_str)
{
	int					n;
	bool				ok;
	char				path[1024];
	char				*vertex_data,*fragment_data;
	const GLcharARB		*vertex_ptr,*fragment_ptr;
	view_shader_type	*shader;

		// shaders on?

	if (!gl_check_shader_ok()) return(TRUE);

		// clear the shaders

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {
		shader->vertex_obj=shader->fragment_obj=shader->program_obj=0;
		shader++;
	}
	
		// load the shaders

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {

			// load the shaders

		file_paths_data(&setup.file_path_setup,path,"Shaders",shader->vertex_name,"vert");
		vertex_data=gl_shader_open_file(path);
		if (vertex_data==NULL) {
			sprintf(err_str,"GLSL Error: Can not open shader file '%s'",shader->vertex_name);
			return(FALSE);
		}
		
		file_paths_data(&setup.file_path_setup,path,"Shaders",shader->fragment_name,"frag");
		fragment_data=gl_shader_open_file(path);
		if (fragment_data==NULL) {
			sprintf(err_str,"GLSL Error: Can not open shader file '%s'",shader->fragment_name);
			return(FALSE);
		}
	
			// create the shader
			
		shader->program_obj=glCreateProgramObjectARB();
			
		shader->vertex_obj=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		
		vertex_ptr=vertex_data;
		glShaderSourceARB(shader->vertex_obj,1,&vertex_ptr,NULL);
	
		glCompileShaderARB(shader->vertex_obj);
		ok=gl_shader_report_error(err_str,shader->vertex_name,NULL,shader->vertex_obj,"Vertex",vertex_data,GL_OBJECT_COMPILE_STATUS_ARB);
		free(vertex_data);
		
		if (!ok) {
			gl_shader_shutdown();
			free(fragment_data);
			return(FALSE);
		}
		
		glAttachObjectARB(shader->program_obj,shader->vertex_obj);
		
		shader->fragment_obj=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		
		fragment_ptr=fragment_data;
		glShaderSourceARB(shader->fragment_obj,1,&fragment_ptr,NULL);
		
		glCompileShaderARB(shader->fragment_obj);
		ok=gl_shader_report_error(err_str,NULL,shader->fragment_name,shader->fragment_obj,"Fragment",fragment_data,GL_OBJECT_COMPILE_STATUS_ARB);
		free(fragment_data);
		
		if (!ok) {
			gl_shader_shutdown();
			return(FALSE);
		}
		
		glAttachObjectARB(shader->program_obj,shader->fragment_obj);
		
			// link shaders into program
			
		glLinkProgramARB(shader->program_obj);
		if (gl_shader_report_error(err_str,shader->vertex_name,shader->fragment_name,shader->program_obj,"Program",NULL,GL_OBJECT_LINK_STATUS_ARB)) {
			gl_shader_shutdown();
			return(FALSE);
		}

			// per instance shader variables

		gl_shader_set_instance_variables(shader);

		shader++;
	}

	return(TRUE);
}

void gl_shader_shutdown(void)
{
	int							n;
	view_shader_type			*shader;

	if (!gl_check_shader_ok()) return;

		// shutdown shaders

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {
		if (shader->vertex_obj!=0) {
			glDetachObjectARB(shader->program_obj,shader->vertex_obj);
			glDeleteObjectARB(shader->vertex_obj);
		}

		if (shader->fragment_obj!=0) {
			glDetachObjectARB(shader->program_obj,shader->fragment_obj);
			glDeleteObjectARB(shader->fragment_obj);
		}

		if (shader->program_obj!=0) glDeleteObjectARB(shader->program_obj);

		shader++;
	}
}

/* =======================================================

      Attach Shaders to Textures
      
======================================================= */

// supergumba -- we need to do this

void gl_shader_attach_map(void)
{
	int					n;
	texture_type		*texture;

	texture=map.textures;
	
	for (n=0;n!=max_map_texture;n++) {
		texture->shader_idx=-1;
		texture++;
	}
}

void gl_shader_attach_model(model_type *mdl)
{
	int					n;
	texture_type		*texture;

	texture=mdl->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		texture->shader_idx=-1;
		texture++;
	}
}

/* =======================================================

      Shader Drawing Start/Stop
      
======================================================= */

void gl_shader_draw_start(void)
{
	int					n;
	view_shader_type	*shader;

	gl_shader_current_idx=-1;
	
		// use this flag for variables that only
		// need to be set once per scene

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {
		shader->per_scene_vars_set=FALSE;
		shader++;
	}
}

void gl_shader_draw_end(void)
{
		// deactivate any current shader
		
	if (gl_shader_current_idx!=-1) glUseProgramObjectARB(0);
	
		// turn off any used textures
		
	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Shader Variables
      
======================================================= */

void gl_shader_set_scene_variables(view_shader_type *shader)
{
	GLint					var;

	var=glGetUniformLocationARB(shader->program_obj,"dim3TimeMillisec");
	if (var!=-1) glUniform1iARB(var,game_time_get());
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3FrequencySecond");
	if (var!=-1) glUniform1fARB(var,game_time_fequency_second_get());
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3CameraPosition");
	if (var!=-1) glUniform3fARB(var,(float)view.camera.pnt.x,(float)view.camera.pnt.y,(float)view.camera.pnt.z);
}

void gl_shader_set_draw_variables(view_shader_type *shader,texture_type *texture,int nlight,float dark_factor)
{
	GLint					var;

	var=glGetUniformLocationARB(shader->program_obj,"dim3LightCount");
	if (var!=-1) glUniform1iARB(var,nlight);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3DarkFactor");
	if (var!=-1) glUniform1fARB(var,dark_factor);
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3BumpFactor");
	if (var!=-1) glUniform1fARB(var,texture->bump_factor);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3SpecularFactor");
	if (var!=-1) glUniform1fARB(var,texture->specular_factor);
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexColor");
	if (var!=-1) glUniform4fARB(var,texture->col.r,texture->col.g,texture->col.b,1.0f);
}

/* =======================================================

      Set Shader Textures
      
======================================================= */

void gl_shader_texture_set(texture_type *texture,int frame)
{
		// set textures to all replace
		
	glActiveTexture(GL_TEXTURE3);
	
	if (texture->glowmaps[frame].gl_id!=-1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture->glowmaps[frame].gl_id);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
	
	glActiveTexture(GL_TEXTURE2);
	
	if (texture->specularmaps[frame].gl_id!=-1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture->specularmaps[frame].gl_id);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}

	glActiveTexture(GL_TEXTURE1);
	
	if (texture->bumpmaps[frame].gl_id!=-1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture->bumpmaps[frame].gl_id);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
	
	glActiveTexture(GL_TEXTURE0);
		
	if (texture->bitmaps[frame].gl_id!=-1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture->bitmaps[frame].gl_id);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
}

/* =======================================================

      Execute Shader
      
======================================================= */

void gl_shader_draw_execute(texture_type *texture,int frame,int nlight,float dark_factor)
{
	view_shader_type				*shader;
	
		// get shader
		
	shader=&view.shaders[texture->shader_idx];
	
		// if we are not in this shader, then
		// change over
		
	if (texture->shader_idx!=gl_shader_current_idx) {
	
			// set in the new program
			
		gl_shader_current_idx=texture->shader_idx;
		glUseProgramObjectARB(shader->program_obj);
			
			// set per-texture variables, only do this once
			// as they don't change per scene
		
		if (!shader->per_scene_vars_set) {
			shader->per_scene_vars_set=TRUE;
			gl_shader_set_scene_variables(shader);
		}
	}
	
		// per draw variables
		
	gl_shader_set_draw_variables(shader,texture,nlight,dark_factor);
	
		// textures
		
	gl_shader_texture_set(texture,frame);
}

