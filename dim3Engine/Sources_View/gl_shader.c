/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

extern camera_type			camera;

GLhandleARB					gl_shader_current_prog_obj;

extern float game_time_fequency_second_get(void);

/* =======================================================

      Use Shaders
      
======================================================= */

void gl_shader_start(void)
{
	gl_shader_current_prog_obj=0;
}

void gl_shader_end(void)
{
	if (gl_shader_current_prog_obj!=0) glUseProgramObjectARB(0);
}

void gl_shader_set_program(GLhandleARB shader_prog_obj)
{
	if (shader_prog_obj==gl_shader_current_prog_obj) return;
	
		// turning off shader?
		
	if (shader_prog_obj==0) {
		glUseProgramObjectARB(0);
		gl_shader_current_prog_obj=0;
		return;
	}
	
		// use shader
	
	glUseProgramObjectARB(shader_prog_obj);
	
	gl_shader_current_prog_obj=shader_prog_obj;
}

void gl_shader_set_variables(GLhandleARB shader_prog_obj,d3pnt *pnt,texture_type *texture,light_spot_type *lspot)
{
	int						n,nlight;
	float					light_pos[max_light_spot*3],light_col[max_light_spot*4];
	d3vct					vct;
	GLint					var;
	shader_custom_var_type	*cvar;

	if (shader_prog_obj==0) return;

		// frequency variables
		
	var=glGetUniformLocationARB(shader_prog_obj,"dim3FrequencySecond");
	if (var!=-1) glUniform1fARB(var,game_time_fequency_second_get());

		// camera and render position
		
	var=glGetUniformLocationARB(shader_prog_obj,"dim3CameraPosition");
	if (var!=-1) glUniform3fARB(var,(float)camera.pos.x,(float)camera.pos.y,(float)camera.pos.z);
	
	var=glGetUniformLocationARB(shader_prog_obj,"dim3RenderPosition");
	if (var!=-1) glUniform3fARB(var,(float)pnt->x,(float)pnt->y,(float)pnt->z);
	
		// light array

	nlight=light_create_glsl_array(light_pos,light_col);

	var=glGetUniformLocationARB(shader_prog_obj,"dim3lightCount");
	if (var!=-1) glUniform1iARB(var,nlight);

	var=glGetUniformLocationARB(shader_prog_obj,"dim3LightPositions");
	if (var!=-1) glUniform3fvARB(var,max_light_spot,light_pos);

	var=glGetUniformLocationARB(shader_prog_obj,"dim3LightColors");
	if (var!=-1) glUniform4fvARB(var,max_light_spot,light_col);
	
		// closest lights

	var=glGetUniformLocationARB(shader_prog_obj,"dim3HasClosestLight");
	if (var!=-1) glUniform1iARB(var,((lspot==NULL)?0:1));

	var=glGetUniformLocationARB(shader_prog_obj,"dim3ClosestLightPosition");
	if (var!=-1) {
		if (lspot==NULL) {
			glUniform3fARB(var,0.0f,0.0f,0.0f);
		}
		else {
			glUniform3fARB(var,(float)lspot->pnt.x,(float)lspot->pnt.y,(float)lspot->pnt.z);
		}
	}
	
	var=glGetUniformLocationARB(shader_prog_obj,"dim3ClosestLightNormal");
	if (var!=-1) {
		if (lspot==NULL) {
			glUniform3fARB(var,0.0f,0.0f,0.0f);
		}
		else {
			vector_create(&vct,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,pnt->x,pnt->y,pnt->z);
			glUniform3fARB(var,(float)vct.x,(float)vct.y,(float)vct.z);
		}
	}
	
	var=glGetUniformLocationARB(shader_prog_obj,"dim3ClosestLightColor");
	if (var!=-1) {
		if (lspot==NULL) {
			glUniform4fARB(var,0.0f,0.0f,0.0f,0.0f);
		}
		else {
			glUniform4fARB(var,lspot->col.r,lspot->col.g,lspot->col.b,(float)lspot->intensity);
		}
	}
		
		// textures
		
	var=glGetUniformLocationARB(shader_prog_obj,"dim3Tex");
	if (var!=-1) glUniform1iARB(var,0);

	var=glGetUniformLocationARB(shader_prog_obj,"dim3TexBump");
	if (var!=-1) glUniform1iARB(var,1);
	
	var=glGetUniformLocationARB(shader_prog_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1iARB(var,2);
	
	var=glGetUniformLocationARB(shader_prog_obj,"dim3TexGlow");
	if (var!=-1) glUniform1iARB(var,3);
	
		// texture color
		
	var=glGetUniformLocationARB(shader_prog_obj,"dim3TexColor");
	if (var!=-1) glUniform4fARB(var,texture->col.r,texture->col.g,texture->col.b,1.0f);
	
		// custom variables
		
	for (n=0;n!=max_shader_custom_vars;n++) {
		cvar=&texture->shader.custom_vars[n];

		if (cvar->name[0]==0x0) continue;
		
		var=glGetUniformLocationARB(shader_prog_obj,cvar->name);
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

