/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Dialog Special Combo Utilities

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

#include "menu.h"

extern file_path_setup_type		file_path_setup;

int								sound_count,particle_count,ring_count,
								shader_count,script_count,model_count;

char							sound_names[256][name_str_len],
								particle_names[256][name_str_len],
								ring_names[256][name_str_len],
								shader_names[256][name_str_len],
								script_names[256][name_str_len],
								model_names[256][name_str_len];

/* =======================================================

      Special Dialog Combo Utilities
      
======================================================= */

void dialog_special_combo_fill(WindowRef wind,unsigned long sig,int id,int count,char *names,char *sel_name)
{
	int				n,sel_idx;
	char			*c;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	MenuRef			menu;
	CFStringRef		cf_str;
	
		// clear
		
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	menu=GetControlPopupMenuHandle(ctrl);
	DeleteMenuItems(menu,1,CountMenuItems(menu));
	
	SetControl32BitMaximum(ctrl,0);

		// fill
	
	sel_idx=-1;

	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"None",kCFStringEncodingMacRoman);		// none
	AppendMenuItemTextWithCFString(menu,cf_str,0,0,NULL);
	CFRelease(cf_str);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"-",kCFStringEncodingMacRoman);		// divider
	AppendMenuItemTextWithCFString(menu,cf_str,0,0,NULL);
	CFRelease(cf_str);
	
		// items
		
	c=names;
	
	for (n=0;n!=count;n++) {
		if (strcmp(c,sel_name)==0) sel_idx=n;
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,c,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(menu,cf_str,0,0,NULL);
		CFRelease(cf_str);
		c+=name_str_len;
	}
	
	SetControl32BitMaximum(ctrl,CountMenuItems(menu));
	
		// select
		
	if (sel_idx==-1) {
		SetControl32BitValue(ctrl,1);
	}
	else {
		SetControl32BitValue(ctrl,(sel_idx+3));
	}
}

void dialog_special_combo_get(WindowRef wind,unsigned long sig,int id,int count,char *names,char *sel_name,int str_len)
{
	int				sel_idx;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	sel_idx=GetControl32BitValue(ctrl)-1;
	if (sel_idx<0) {
		sel_name[0]=0x0;
		return;
	}
	
	strncpy(sel_name,(names+((sel_idx-2)*name_str_len)),str_len);
	sel_name[str_len-1]=0x0;
}

/* =======================================================

      Fill Special Dialogs
      
======================================================= */

void dialog_special_combo_fill_sound(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					head_tag,tag;
	char				path[1024];
	
	sound_count=0;
		
	file_paths_data(&file_path_setup,path,"Settings","Sounds","xml");
	if (xml_open_file(path)) {
	
		head_tag=xml_findrootchild("Sounds");
		if (head_tag!=-1) {
	
			tag=xml_findfirstchild("Sound",head_tag);
		
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",sound_names[sound_count],name_str_len);
				sound_count++;
				tag=xml_findnextchild(tag);
			}
		}
		
		xml_close_file();
	}

	dialog_special_combo_fill(wind,sig,id,sound_count,(char*)sound_names,sel_name);
}

void dialog_special_combo_fill_particle(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					data_head_tag,head_tag,tag;
	char				path[1024];

	particle_count=0;
	
	file_paths_data(&file_path_setup,path,"Settings","Particles","xml");
	if (xml_open_file(path)) {
	
		data_head_tag=xml_findrootchild("Particle_Data");
		
		if (data_head_tag==-1) {
			head_tag=xml_findrootchild("Particles");
		}
		else {
			head_tag=xml_findfirstchild("Particles",data_head_tag);
		}
	
		if (head_tag!=-1) {

			tag=xml_findfirstchild("Particle",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",particle_names[particle_count],name_str_len);
				particle_count++;
				tag=xml_findnextchild(tag);
			}
		}
		
		if (data_head_tag==-1) {
			head_tag=xml_findrootchild("Particle_Groups");
		}
		else {
			head_tag=xml_findfirstchild("Particle_Groups",data_head_tag);
		}

		if (head_tag!=-1) {

			tag=xml_findfirstchild("Particle_Group",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",particle_names[particle_count],name_str_len);
				particle_count++;
				tag=xml_findnextchild(tag);
			}
		}
	
		xml_close_file();
	}

	dialog_special_combo_fill(wind,sig,id,particle_count,(char*)particle_names,sel_name);
}

void dialog_special_combo_fill_ring(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					head_tag,tag;
	char				path[1024];

	ring_count=0;
	
	file_paths_data(&file_path_setup,path,"Settings","Rings","xml");
	if (xml_open_file(path)) {
	
		head_tag=xml_findrootchild("Rings");
		if (head_tag!=-1) {

			tag=xml_findfirstchild("Ring",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",ring_names[ring_count],name_str_len);
				ring_count++;
				tag=xml_findnextchild(tag);
			}
		}
	
		xml_close_file();
	}

	dialog_special_combo_fill(wind,sig,id,ring_count,(char*)ring_names,sel_name);
}

void dialog_special_combo_fill_shader(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					head_tag,tag;
	char				path[1024];

	shader_count=0;
	
	file_paths_data(&file_path_setup,path,"Settings","Shaders","xml");
	if (xml_open_file(path)) {
	
		head_tag=xml_findrootchild("Shaders");
		if (head_tag!=-1) {

			tag=xml_findfirstchild("Shader",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",shader_names[shader_count],name_str_len);
				shader_count++;
				tag=xml_findnextchild(tag);
			}
		}
	
		xml_close_file();
	}

	dialog_special_combo_fill(wind,sig,id,shader_count,(char*)shader_names,sel_name);
}

void dialog_special_combo_fill_script(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int							n;
	file_path_directory_type	*fpd;
	
	fpd=file_paths_read_directory_data(&file_path_setup,"Scripts/Objects","js");

	script_count=fpd->nfile;
	
	for (n=0;n!=fpd->nfile;n++) {
		strcpy(script_names[n],fpd->files[n].file_name);
	}
	
	file_paths_close_directory(fpd);
	
	dialog_special_combo_fill(wind,sig,id,script_count,(char*)script_names,sel_name);
}

void dialog_special_combo_fill_model(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int							n;
	file_path_directory_type	*fpd;
	
	fpd=file_paths_read_directory_data(&file_path_setup,"Models",NULL);

	model_count=fpd->nfile;
	
	for (n=0;n!=fpd->nfile;n++) {
		strcpy(model_names[n],fpd->files[n].file_name);
	}
	
	file_paths_close_directory(fpd);

	dialog_special_combo_fill(wind,sig,id,model_count,(char*)model_names,sel_name);
}

/* =======================================================

      Get Special Dialogs
      
======================================================= */

void dialog_special_combo_get_sound(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sound_count,(char*)sound_names,sel_name,str_len);
}

void dialog_special_combo_get_particle(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,particle_count,(char*)particle_names,sel_name,str_len);
}

void dialog_special_combo_get_ring(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,ring_count,(char*)ring_names,sel_name,str_len);
}

void dialog_special_combo_get_shader(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,shader_count,(char*)shader_names,sel_name,str_len);
}

void dialog_special_combo_get_script(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,script_count,(char*)script_names,sel_name,str_len);
}

void dialog_special_combo_get_model(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,model_count,(char*)model_names,sel_name,str_len);
}

