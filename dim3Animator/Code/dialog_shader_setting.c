/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Shader Setting Dialogs

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

#include "window.h"
#include "dialog.h"

#define kTextureSettingVertexShader					FOUR_CHAR_CODE('vshd')
#define kTextureSettingFragmentShader				FOUR_CHAR_CODE('fshd')
#define kTextureSettingVarName						FOUR_CHAR_CODE('cvnm')
#define kTextureSettingVarValue						FOUR_CHAR_CODE('cvvl')
#define kTextureSettingVarType						FOUR_CHAR_CODE('cvty')

#define kTextureSettingButtonSetVertexShader		FOUR_CHAR_CODE('vset')
#define kTextureSettingButtonClearVertexShader		FOUR_CHAR_CODE('vclr')
#define kTextureSettingButtonSetFragmentShader		FOUR_CHAR_CODE('fset')
#define kTextureSettingButtonClearFragmentShader	FOUR_CHAR_CODE('fclr')

extern model_type			model;

WindowRef					dialog_shader_wind;

/* =======================================================

      Shader Setting Event Handlers
      
======================================================= */

static pascal OSStatus shader_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	char			sub_path[1024],shader_name[file_str_len];
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kTextureSettingButtonSetVertexShader:
					sprintf(sub_path,"Models/%s/Shaders",model.name);
					if (dialog_file_open_run("VertexShaderOpen",sub_path,"vert",shader_name)) dialog_set_text(dialog_shader_wind,kTextureSettingVertexShader,0,shader_name);
					return(noErr);
					
				case kTextureSettingButtonClearVertexShader:
					dialog_set_text(dialog_shader_wind,kTextureSettingVertexShader,0,NULL);
					return(noErr);
					
				case kTextureSettingButtonSetFragmentShader:
					sprintf(sub_path,"Models/%s/Shaders",model.name);
					if (dialog_file_open_run("FragmentShaderOpen",sub_path,"frag",shader_name)) dialog_set_text(dialog_shader_wind,kTextureSettingFragmentShader,0,shader_name);
					return(noErr);
					
				case kTextureSettingButtonClearFragmentShader:
					dialog_set_text(dialog_shader_wind,kTextureSettingFragmentShader,0,NULL);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_shader_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Shader Custom Variables
      
======================================================= */

void dialog_shader_custom_variable_to_string(shader_custom_var_type *cvar,char *str)
{
	char			txt[32];
	
	switch (cvar->var_type) {
	
		case shader_var_type_int:
			sprintf(str,"%d",cvar->value.i);
			return;
	
		case shader_var_type_float:
			string_convert_float(str,cvar->value.f);
			return;
			
		case shader_var_type_vec3:
			string_convert_float(str,cvar->value.vec3[0]);
			strcat(str,",");
			string_convert_float(txt,cvar->value.vec3[1]);
			strcat(str,txt);
			strcat(str,",");
			string_convert_float(txt,cvar->value.vec3[2]);
			strcat(str,txt);
			return;
			
		case shader_var_type_vec4:
			string_convert_float(str,cvar->value.vec4[0]);
			strcat(str,",");
			string_convert_float(txt,cvar->value.vec4[1]);
			strcat(str,txt);
			strcat(str,",");
			string_convert_float(txt,cvar->value.vec4[2]);
			strcat(str,txt);
			strcat(str,",");
			string_convert_float(txt,cvar->value.vec4[3]);
			strcat(str,txt);
			return;
	}
	
	str[0]=0x0;
}

void dialog_shader_string_to_custom_variable(shader_custom_var_type *cvar,char *str)
{
	char			*c,*c2;
	
	memset(&cvar->value,0x0,sizeof(shader_custom_var_type));
	
	switch (cvar->var_type) {
	
		case shader_var_type_int:
			cvar->value.i=atoi(str);
			return;
			
		case shader_var_type_float:
			cvar->value.f=strtof(str,NULL);
			return;

		case shader_var_type_vec3:
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
	
		case shader_var_type_vec4:
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

      Run Shader Settings
      
======================================================= */

void dialog_shader_setting_run(int txt)
{
	int						n;
	char					str[256];
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	texture_type			*texture;
	shader_custom_var_type	*cvar;

		// the texture
		
	texture=&model.textures[txt];

		// open the dialog
		
	dialog_open(&dialog_shader_wind,"ShaderSetting");
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(shader_setting_event_proc);
	InstallWindowEventHandler(dialog_shader_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);

		// dialog controls
		
	dialog_set_text(dialog_shader_wind,kTextureSettingVertexShader,0,texture->shader.vertex_name);
	dialog_set_text(dialog_shader_wind,kTextureSettingFragmentShader,0,texture->shader.fragment_name);
	
	cvar=texture->shader.custom_vars;
	
	for (n=0;n!=max_shader_custom_vars;n++) {
		dialog_set_text(dialog_shader_wind,kTextureSettingVarName,n,cvar->name);
		dialog_shader_custom_variable_to_string(cvar,str);
		dialog_set_text(dialog_shader_wind,kTextureSettingVarValue,n,str);
		dialog_set_combo(dialog_shader_wind,kTextureSettingVarType,n,cvar->var_type);
		cvar++;
	}

		// show window
	
	ShowWindow(dialog_shader_wind);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_shader_wind);
	
		// dialog to data
	
	dialog_get_text(dialog_shader_wind,kTextureSettingVertexShader,0,texture->shader.vertex_name,file_str_len);
	dialog_get_text(dialog_shader_wind,kTextureSettingFragmentShader,0,texture->shader.fragment_name,file_str_len);
	
	cvar=texture->shader.custom_vars;
	
	for (n=0;n!=max_shader_custom_vars;n++) {
		dialog_get_text(dialog_shader_wind,kTextureSettingVarName,n,cvar->name,name_str_len);
		cvar->var_type=dialog_get_combo(dialog_shader_wind,kTextureSettingVarType,n);
		dialog_get_text(dialog_shader_wind,kTextureSettingVarValue,n,str,256);
		dialog_shader_string_to_custom_variable(cvar,str);
		cvar++;
	}
	
		// close window
		
	DisposeWindow(dialog_shader_wind);
}

