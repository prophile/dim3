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

int							gl_shader_current_idx;

extern int game_time_get(void);
extern float game_time_fequency_second_get(void);

/* =======================================================

      Shader Start/Stop
      
======================================================= */

void gl_shader_start(void)
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

void gl_shader_end(void)
{
		// deactivate any current shader
		
	if (gl_shader_current_prog_obj!=0) glUseProgramObjectARB(0);
	
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
	int							n;
	GLint						var;
	view_shader_custom_var_type	*cvar;

		// time/frequency variables

	var=glGetUniformLocationARB(shader->prog_obj,"dim3TimeMillisec");
	if (var!=-1) glUniform1iARB(var,game_time_get());
		
	var=glGetUniformLocationARB(shader->prog_obj,"dim3FrequencySecond");
	if (var!=-1) glUniform1fARB(var,game_time_fequency_second_get());

		// camera position
		
	var=glGetUniformLocationARB(shader->prog_obj,"dim3CameraPosition");
	if (var!=-1) glUniform3fARB(var,(float)view.camera.pnt.x,(float)view.camera.pnt.y,(float)view.camera.pnt.z);
		
		// textures
		
	var=glGetUniformLocationARB(shader->prog_obj,"dim3Tex");
	if (var!=-1) glUniform1iARB(var,0);

	var=glGetUniformLocationARB(shader->prog_obj,"dim3TexBump");
	if (var!=-1) glUniform1iARB(var,1);
	
	var=glGetUniformLocationARB(shader->prog_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1iARB(var,2);

		// custom variables
		
	for (n=0;n!=max_view_shader_custom_vars;n++) {
		cvar=&shader->custom_vars[n];

		if (cvar->name[0]==0x0) continue;
		
		var=glGetUniformLocationARB(shader->prog_obj,cvar->name);
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

void gl_shader_set_draw_variables(view_shader_type *shader,texture_type *texture,d3pnt *pnt,int nlight,float dark_factor)
{
	GLint					var;

	var=glGetUniformLocationARB(shader->prog_obj,"dim3RenderPosition");
	if (var!=-1) glUniform3fARB(var,(float)pnt->x,(float)pnt->y,(float)pnt->z);

	var=glGetUniformLocationARB(shader->prog_obj,"dim3LightCount");
	if (var!=-1) glUniform1iARB(var,nlight);
	
	var=glGetUniformLocationARB(shader->prog_obj,"dim3DarkFactor");
	if (var!=-1) glUniform1fARB(var,dark_factor);
		
	var=glGetUniformLocationARB(shader->prog_obj,"dim3BumpFactor");
	if (var!=-1) glUniform1fARB(var,texture->bump_factor);
	
	var=glGetUniformLocationARB(shader->prog_obj,"dim3SpecularFactor");
	if (var!=-1) glUniform1fARB(var,texture->specular_factor);
		
	var=glGetUniformLocationARB(shader->prog_obj,"dim3TexColor");
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

void gl_shader_execute(texture_type *texture,int frame,d3pnt *pnt,int nlight,float dark_factor)
{
	view_shader_type				*shader;
	
		// get shader
		
	shader=&view.shaders[texture->shader_idx];
	
		// if we are not in this shader, then
		// change over
		
	if (shader_idx!=gl_shader_current_idx) {
	
			// set in the new program
			
		gl_shader_current_idx=shader_idx;
		glUseProgramObjectARB(shader->prog_obj);
			
			// set per-texture variables, only do this once
			// as they don't change per scene
		
		if (!shader->per_scene_vars_set) {
			shader->per_scene_vars_set=TRUE;
			gl_shader_set_scene_variables(shader);
		}
	}
	
		// per draw variables
		
	gl_shader_set_draw_variables(shader,texture,pnt,nlight,dark_factor);
	
		// textures
		
	gl_shader_texture_set(texture,frame);
}

