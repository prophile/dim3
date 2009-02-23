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
\n\
uniform vec3 dim3CameraPosition;\n\
\n\
varying vec3 directionOfView;\n\
varying vec3 directionOfLight[3];\n\
varying float doubleLightDist[3];\n\
varying vec3 combineLightNormal;\n\
\n\
void main(void)\n\
{\n\
	vec3 vct;\n\
	vec3 vertexEyePos=vec3(gl_ModelViewMatrix*gl_Vertex);\n\
\n\
	directionOfView = normalize(gl_Vertex.xyz - dim3CameraPosition);\n\
\n\
	vct=gl_LightSource[0].position.xyz-vertexEyePos;\n\
	doubleLightDist[0]=length(vct);\n\
	doubleLightDist[0]=doubleLightDist[0]*doubleLightDist[0];\n\
	directionOfLight[0]=normalize(vct*gl_NormalMatrix);\n\
//	directionOfLight[0]=vec3(vec4(directionOfLight[0],0.0)*gl_ModelViewMatrix);\n\
\n\
	vct=gl_LightSource[1].position.xyz-vertexEyePos;\n\
	doubleLightDist[1]=length(vct);\n\
	doubleLightDist[1]=doubleLightDist[1]*doubleLightDist[1];\n\
    directionOfLight[1]=normalize(vct);\n\
\n\
	vct=gl_LightSource[2].position.xyz-vertexEyePos;\n\
	doubleLightDist[2]=length(vct);\n\
	doubleLightDist[2]=doubleLightDist[2]*doubleLightDist[2];\n\
    directionOfLight[2]=normalize(vct);\n\
\n\
	combineLightNormal=normalize(directionOfLight[0]+directionOfLight[1]+directionOfLight[2]);\n\
\n\
	gl_Position = ftransform();\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;\n\
}\n\
"};

char bis_frag_test[]={"\
\n\
uniform sampler2D dim3Tex;\n\
uniform sampler2D dim3TexBump;\n\
uniform sampler2D dim3TexSpecular;\n\
uniform float dim3DarkFactor;\n\
uniform float dim3BumpFactor;\n\
uniform float dim3SpecularFactor;\n\
\n\
varying vec3 directionOfView;\n\
varying vec3 directionOfLight[3];\n\
varying float doubleLightDist[3];\n\
varying vec3 combineLightNormal;\n\
\n\
void main(void)\n\
{\n\
	float att,bump,spec,specMap;\n\
	vec4 tex,diffuse;\n\
	vec3 bumpMap,specBump;\n\
	vec3 lightNormal=normalize(directionOfLight[0]);\n\
//	vec3 lightNormal=normalize(combineLightNormal);\n\
	vec3 viewNormal=normalize(directionOfView);\n\
\n\
	bumpMap = texture2D(dim3TexBump, gl_TexCoord[0].st).rgb;\n\
	bumpMap = (bumpMap - 0.5) * 2.0;\n\
	bump = max(dot(bumpMap,lightNormal), 0.0) * dim3BumpFactor;\n\
\n\
	att = 1.0 / (gl_LightSource[0].quadraticAttenuation * doubleLightDist[0]);\n\
	diffuse=gl_LightSource[0].ambient*att;\n\
	att = 1.0 / (gl_LightSource[1].quadraticAttenuation * doubleLightDist[1]);\n\
	diffuse+=gl_LightSource[1].ambient*att;\n\
	att = 1.0 / (gl_LightSource[2].quadraticAttenuation * doubleLightDist[2]);\n\
	diffuse+=gl_LightSource[2].ambient*att;\n\
\n\
	diffuse=clamp(diffuse,0.0,1.0);\n\
\n\
//	specMap=texture2D(dim3TexSpecular,vec2(gl_TexCoord[0].st)).r;\n\
//	specBump = lightNormal - (2.0 * dot(lightNormal, bumpMap) * bumpMap);\n\
//	spec = (pow(max(dot(specBump, viewNormal), 0.0), 10.0) * specMap) * dim3SpecularFactor;\n\
\n\
	tex = texture2D(dim3Tex,gl_TexCoord[0].st);\n\
	diffuse=tex*diffuse*gl_FrontMaterial.ambient;\n\
\n\
//	gl_FragColor.rgb=(diffuse.rgb+gl_LightModel.ambient.rgb)*bump+spec;\n\
	gl_FragColor.rgb=(diffuse.rgb+gl_LightModel.ambient.rgb);\n\
	gl_FragColor.a=tex.a;\n\
}\n\
\n\
"};


/* supergumba -- old bump


*/
/* supergumba -- old direction calls
//	directionOfLight = normalize(gl_Vertex.xyz - gl_LightSource[0].position.xyz);\n\
	directionOfView = normalize(gl_Vertex.xyz - dim3CameraPosition);\n\
\n\
	vec4 cameraPos=gl_ModelViewMatrix*gl_Vertex;\n\
	vec3 vct=vec3(gl_LightSource[0].position-cameraPos);\n\
//	directionOfLight=normalize(vct);\n\
	distanceToLight[0]=length(vct);\n\
\n\
//	directionOfLight = normalize(gl_LightSource[0].position.xyz);\n\
\n\
    directionOfLight[0]=normalize(gl_LightSource[0].position.xyz);\n\
//	distanceToLight[0]=length(gl_Vertex.xyz - (dim3CameraPosition.xyz+gl_LightSource[0].position.xyz));\n\
//	distanceToLight[0]=length(vec3(gl_LightSource[0].position-(gl_ModelViewMatrix*gl_Vertex)));\n\
*/

/* supergumba -- old attenuation
	float att = 1.0 / (gl_LightSource[0].constantAttenuation +	\n\
					gl_LightSource[0].linearAttenuation * distanceToLight[0] +	\n\
					gl_LightSource[0].quadraticAttenuation * distanceToLight[0] * distanceToLight[0]);\n\
	vec4 Diffuse=clamp(((gl_LightSource[0].ambient*att)*1.5),0.0,1.0);\n\
*/


// supergumba -- need support for
// 4. high lighted polygons

bool shader_report_error2(char *err_str,char *vertex_name,char *fragment_name,GLhandleARB hand,char *err_type,char *code,int check_type)
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
		
	if (shader_report_error2(err_str,"bis_vertex",NULL,bis_vertex_obj,"Vertex",bis_vert_test,GL_OBJECT_COMPILE_STATUS_ARB)) {
		shader_built_in_free_objects();
		return(FALSE);
	}
	
	glAttachObjectARB(bis_program_obj,bis_vertex_obj);
	
		// create the fragment shader
		
	bis_fragment_obj=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	fragment_ptr=bis_frag_test;
	glShaderSourceARB(bis_fragment_obj,1,&fragment_ptr,NULL);
	
	glCompileShaderARB(bis_fragment_obj);
	
	if (shader_report_error2(err_str,NULL,"bis_fragment",bis_fragment_obj,"Fragment",bis_frag_test,GL_OBJECT_COMPILE_STATUS_ARB)) {
		shader_built_in_free_objects();
		return(FALSE);
	}
	
	glAttachObjectARB(bis_program_obj,bis_fragment_obj);
	
		// link shaders into program
		
	glLinkProgramARB(bis_program_obj);
	if (shader_report_error2(err_str,"bis_vertex","bis_fragment",bis_program_obj,"Program",NULL,GL_OBJECT_LINK_STATUS_ARB)) {
		shader_built_in_free_objects();
		fprintf(stdout,err_str);		// supergumba
		return(FALSE);
	}
	
	return(TRUE);
}