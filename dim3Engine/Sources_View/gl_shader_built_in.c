/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Built-In Shaders

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

#include "lights.h"
#include "cameras.h"
#include "consoles.h"
#include "video.h"

GLhandleARB				bis_vertex_obj,bis_fragment_obj,bis_program_obj;

/* =======================================================

      Shader Programs
      
======================================================= */

char bis_vert_test[]={"\
\
uniform vec3 dim3LightPositions[64];\n\
uniform vec3 dim3ClosestLightNormal;\n\
uniform vec3 dim3CameraPosition;\n\
\
varying float distanceToLight[3];\n\
varying vec3 directionOfLight;\n\
varying vec3 directionOfView;\n\
\
void main(void)\n\
{\n\
	gl_Position = ftransform();\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;\n\
\
//	directionOfLight = normalize(gl_Vertex.xyz - gl_LightSource[0].position.xyz);\n\
	directionOfView = normalize(gl_Vertex.xyz - dim3CameraPosition);\n\
\
	vec4 cameraPos=gl_ModelViewMatrix*gl_Vertex;\n\
	vec3 vct=vec3(gl_LightSource[0].position-cameraPos);\n\
	directionOfLight=normalize(vct);\n\
	distanceToLight[0]=length(vct);\n\
\
//	directionOfLight = normalize(gl_LightSource[0].position.xyz);\n\
	directionOfLight = normalize(gl_Vertex.xyz - gl_LightSource[0].position.xyz);\n\
}\n\
\
"};

char bis_frag_test[]={"\
\
uniform sampler2D     dim3Tex;\n\
uniform sampler2D     dim3TexBump;\n\
uniform sampler2D		dim3TexSpecular;\n\
\
varying float         distanceToLight[3];\n\
varying vec3         directionOfLight;\n\
varying vec3         directionOfView;\n\
\
void main(void)\n\
{\n\
	vec3 light_normal=normalize(directionOfLight);\n\
	vec3 view_normal=normalize(directionOfView);\n\
\
	vec4 Tex = texture2D(dim3Tex,gl_TexCoord[0].st);\n\
\
	vec3 BumpMap = texture2D(dim3TexBump, gl_TexCoord[0].st).xyz;\n\
	BumpMap = (BumpMap - 0.5) * 2.0;\n\
	BumpMap.y = -BumpMap.y;\n\
	float Bump = max(dot(light_normal, BumpMap), 0.0);\n\
\
	vec3 r = light_normal - (2.0 * dot(light_normal, BumpMap) * BumpMap);\n\
	float Spec = pow(max(dot(r, view_normal), 0.0), 10.0) * (texture2D(dim3TexSpecular,vec2(gl_TexCoord[0].st)).x+0.3);\n\
\
	float att = 1.0 / (gl_LightSource[0].constantAttenuation +	\n\
					gl_LightSource[0].linearAttenuation * distanceToLight[0] +	\n\
					gl_LightSource[0].quadraticAttenuation * distanceToLight[0] * distanceToLight[0]);\n\
	vec4 Diffuse=clamp((gl_LightSource[0].ambient*att),0.0,1.0);\n\
\
	Diffuse = Tex * Diffuse;\n\
\
	gl_FragColor.rgb = (Diffuse.rgb + gl_LightModel.ambient.rgb) * Bump + Spec;\n\
//	gl_FragColor.rgb = (Diffuse.rgb + gl_LightModel.ambient.rgb) * Bump;\n\
//	gl_FragColor.rgb = BumpMap.rgb;\n\
	gl_FragColor.a = Tex.a;\n\
}\n\
\
"};


bool shader_report_error2(char *err_str,char *vertex_name,char *fragment_name,GLhandleARB hand,char *err_type,int check_type)
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

void shader_built_in_clear_objects(void)
{
	bis_vertex_obj=0;
	bis_fragment_obj=0;
	bis_program_obj=0;
}

void shader_built_in_free_objects(void)
{
	if (!gl_check_shader_ok()) return;

	if (bis_vertex_obj!=0) {
		glDetachObjectARB(bis_program_obj,bis_vertex_obj);
		glDeleteObjectARB(bis_vertex_obj);
	}
	
	if (bis_fragment_obj!=0) {
		glDetachObjectARB(bis_program_obj,bis_fragment_obj);
		glDeleteObjectARB(bis_fragment_obj);
	}
	
	glDeleteObjectARB(bis_program_obj);
	
	shader_built_in_clear_objects();
}

bool shader_built_in_initialize(void)
{
	char				err_str[1024];
	const GLcharARB		*vertex_ptr,*fragment_ptr;
	
	if (!gl_check_shader_ok()) return(TRUE);
	
		// clear shader objects
		
	shader_built_in_clear_objects();
	
		// create the program
		
	bis_program_obj=glCreateProgramObjectARB();
		
		// create the vertex shader
		
	bis_vertex_obj=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		
	vertex_ptr=bis_vert_test;
	glShaderSourceARB(bis_vertex_obj,1,&vertex_ptr,NULL);
	
	glCompileShaderARB(bis_vertex_obj);
		
	if (shader_report_error2(err_str,"bis_vertex",NULL,bis_vertex_obj,"Vertex",GL_OBJECT_COMPILE_STATUS_ARB)) {
		shader_built_in_free_objects();
		fprintf(stdout,err_str);		// supergumba
		return(FALSE);
	}
	
	glAttachObjectARB(bis_program_obj,bis_vertex_obj);
	
		// create the fragment shader
		
	bis_fragment_obj=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	fragment_ptr=bis_frag_test;
	glShaderSourceARB(bis_fragment_obj,1,&fragment_ptr,NULL);
	
	glCompileShaderARB(bis_fragment_obj);
	
	if (shader_report_error2(err_str,NULL,"bis_fragment",bis_fragment_obj,"Fragment",GL_OBJECT_COMPILE_STATUS_ARB)) {
		shader_built_in_free_objects();
		fprintf(stdout,err_str);		// supergumba
		return(FALSE);
	}
	
	glAttachObjectARB(bis_program_obj,bis_fragment_obj);
	
		// link shaders into program
		
	glLinkProgramARB(bis_program_obj);
	if (shader_report_error2(err_str,"bis_vertex","bis_fragment",bis_program_obj,"Program",GL_OBJECT_LINK_STATUS_ARB)) {
		shader_built_in_free_objects();
		fprintf(stdout,err_str);		// supergumba
		return(FALSE);
	}
	
	return(TRUE);
}