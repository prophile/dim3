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

GLhandleARB					gl_shader_current_prog_obj;

extern int game_time_get(void);
extern float game_time_fequency_second_get(void);

/* =======================================================

      Shader Programs
      
======================================================= */

void gl_shader_program_start(int texture_cnt,texture_type *texture)
{
	int				n;

	gl_shader_current_prog_obj=0;

		// some variables only need to be set once

	for (n=0;n!=texture_cnt;n++) {
		texture->shader.global_vars_set=FALSE;
		texture++;
	}
}

void gl_shader_program_end(void)
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

/* =======================================================

      Shader Variables
      
======================================================= */

void gl_shader_set_variables(GLhandleARB shader_prog_obj,d3pnt *pnt,texture_type *texture,int nlight)
{
	int						n;
	GLint					var;
	shader_custom_var_type	*cvar;

	if (shader_prog_obj==0) return;

		// per texture variables

	if (!texture->shader.global_vars_set) {

		texture->shader.global_vars_set=TRUE;

			// time/frequency variables

		var=glGetUniformLocationARB(shader_prog_obj,"dim3TimeMillisec");
		if (var!=-1) glUniform1iARB(var,game_time_get());
			
		var=glGetUniformLocationARB(shader_prog_obj,"dim3FrequencySecond");
		if (var!=-1) glUniform1fARB(var,game_time_fequency_second_get());

			// camera position
			// camera is always at 0,0,0
			
		var=glGetUniformLocationARB(shader_prog_obj,"dim3CameraPosition");
		if (var!=-1) glUniform3fARB(var,(float)view.camera.pnt.x,(float)view.camera.pnt.y,(float)view.camera.pnt.z);

			// ambient light

		var=glGetUniformLocationARB(shader_prog_obj,"dim3AmbientLightColor");
		glUniform3fARB(var,map.ambient.light_color.r,map.ambient.light_color.g,map.ambient.light_color.b);
			
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

		// per-polygon variables
		
	var=glGetUniformLocationARB(shader_prog_obj,"dim3RenderPosition");
	if (var!=-1) glUniform3fARB(var,(float)pnt->x,(float)pnt->y,(float)pnt->z);

	var=glGetUniformLocationARB(shader_prog_obj,"dim3LightCount");
	if (var!=-1) glUniform1iARB(var,nlight);
}

