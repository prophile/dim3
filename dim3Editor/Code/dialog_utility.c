/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Dialog Utilities

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

#include "dialog.h"

#define DIALOG_IS_EDITOR			1

#ifdef DIALOG_IS_EDITOR
	extern map_type					map;
#else
	extern model_type				model;
#endif

extern file_path_setup_type			file_path_setup;

/* =======================================================

      Open Dialogs
      
======================================================= */

void dialog_open(WindowRef *wind,char *name)
{
	IBNibRef				nib;
	CFStringRef				cf_str;
	
	InitCursor();
	
#ifdef DIALOG_IS_EDITOR
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Editor",kCFStringEncodingMacRoman);
#else
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Animator",kCFStringEncodingMacRoman);
#endif
	CreateNibReference(cf_str,&nib);
	CFRelease(cf_str);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,name,kCFStringEncodingMacRoman);
	CreateWindowFromNib(nib,cf_str,wind);
	CFRelease(cf_str);
	
	DisposeNibReference(nib);
}

/* =======================================================

      Text and Number Utilities
      
======================================================= */

void dialog_set_text(WindowRef wind,unsigned long sig,int id,char *str)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	if (str!=NULL) {
		SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,strlen(str),str);
	}
	else {
		SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,0,NULL);
	}
}

void dialog_get_text(WindowRef wind,unsigned long sig,int id,char *str,int len)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	memset(str,0x0,len);
	GetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,len,str,NULL);
	str[len-1]=0x0;
}

void dialog_set_int(WindowRef wind,unsigned long sig,int id,int value)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	sprintf(str,"%d",value);
	SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,strlen(str),str);
}

int dialog_get_int(WindowRef wind,unsigned long sig,int id)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	memset(str,0x0,32);
	GetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,32,str,NULL);
	str[31]=0x0;
	
	return(atoi(str));
}

void dialog_set_float(WindowRef wind,unsigned long sig,int id,float value)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	string_convert_float(str,value);
	SetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,strlen(str),str);
}

float dialog_get_float(WindowRef wind,unsigned long sig,int id)
{
	char			str[32];
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	memset(str,0x0,32);
	GetControlData(ctrl,kControlNoPart,kControlEditTextTextTag,32,str,NULL);
	str[31]=0x0;
	
	return(atof(str));
}

/* =======================================================

      Boolean Utilities
      
======================================================= */

void dialog_set_boolean(WindowRef wind,unsigned long sig,int id,bool b)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,(b?1:0));
}

bool dialog_get_boolean(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	return(GetControl32BitValue(ctrl)!=0);
}

/* =======================================================

      Combo Utilities
      
======================================================= */

void dialog_clear_combo(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	MenuRef			menu;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	menu=GetControlPopupMenuHandle(ctrl);
	DeleteMenuItems(menu,1,CountMenuItems(menu));
	
	SetControl32BitMaximum(ctrl,0);
}

void dialog_add_combo_item(WindowRef wind,unsigned long sig,int id,char *str,unsigned long cmd)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	MenuRef			menu;
	CFStringRef		cf_str;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	menu=GetControlPopupMenuHandle(ctrl);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
	AppendMenuItemTextWithCFString(menu,cf_str,0,cmd,NULL);
	CFRelease(cf_str);
	
	SetControl32BitMaximum(ctrl,CountMenuItems(menu));
}

void dialog_set_combo(WindowRef wind,unsigned long sig,int id,int idx)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,(idx+1));
}

int dialog_get_combo(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	return(GetControl32BitValue(ctrl)-1);
}

/* =======================================================

      Radio Button Utilities
      
======================================================= */

void dialog_set_radio_buttons(WindowRef wind,unsigned long sig,int value,int count)
{
	int				n;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	for (n=0;n!=count;n++) {
		ctrl_id.signature=sig;
		ctrl_id.id=n;
		GetControlByID(wind,&ctrl_id,&ctrl);
		SetControl32BitValue(ctrl,(value==n)?1:0);
	}
}

int dialog_get_radio_buttons(WindowRef wind,unsigned long sig,int count)
{
	int				n;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	for (n=0;n!=count;n++) {
		ctrl_id.signature=sig;
		ctrl_id.id=n;
		GetControlByID(wind,&ctrl_id,&ctrl);
		if (GetControl32BitValue(ctrl)!=0) return(n);
	}
	
	return(0);
}

/* =======================================================

      Value Utilities
      
======================================================= */

void dialog_set_value(WindowRef wind,unsigned long sig,int id,int value)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,value);
}

int dialog_get_value(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	return(GetControl32BitValue(ctrl));
}

/* =======================================================

      Tab Utilities
      
======================================================= */

void dialog_switch_tab(WindowRef wind,unsigned long sig,int id,int npane)
{
	int				n,index;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
		// get tab
		
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	index=GetControl32BitValue(ctrl)-1;
	
		// hide all other panes
		
	for (n=0;n!=npane;n++) {
		if (index!=n) {
			ctrl_id.signature='tabt';
			ctrl_id.id=n;
			GetControlByID(wind,&ctrl_id,&ctrl);
			SetControlVisibility(ctrl,FALSE,FALSE);
		}
	}
	
		// show selected pane
		
	ctrl_id.signature='tabt';
	ctrl_id.id=index;
	GetControlByID(wind,&ctrl_id,&ctrl);
	SetControlVisibility(ctrl,TRUE,TRUE);
}

void dialog_set_tab(WindowRef wind,unsigned long sig,int id,int index,int npane)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
		// set tab
		
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControl32BitValue(ctrl,(index+1));
	
		// hide show panes
		
	dialog_switch_tab(wind,sig,id,npane);
}

/* =======================================================

      Color Utilities
      
======================================================= */

void dialog_set_color_icon(ControlRef ctrl,d3col *col)
{
	ControlButtonContentInfo	info;
	Rect						box;
	RGBColor					rgb,black={0x0,0x0,0x0};
 	PicHandle					pic;

		// draw the color
		
	SetRect(&box,0,0,24,12);
	
	pic=OpenPicture(&box);
	rgb.red=(int)(col->r*(float)0xFFFF);
	rgb.green=(int)(col->g*(float)0xFFFF);
	rgb.blue=(int)(col->b*(float)0xFFFF);
	RGBForeColor(&rgb);
	PaintRect(&box);
	RGBForeColor(&black);
	FrameRect(&box);
	ClosePicture();
	
		// set it to button
		
	info.contentType=kControlContentPictHandle;
	info.u.picture=pic;

	SetBevelButtonContentInfo(ctrl,&info);
	
		// set the internal data
		
	SetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),col);
}

static pascal OSStatus dialog_set_color_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	d3col			col;
	RGBColor		color;
	Point			pt;
	WindowRef		wind;
	ControlRef		ctrl;
	
	GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
	wind=GetControlOwner(ctrl);
	
	GetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),NULL,&col);

	color.red=(int)(col.r*(float)0xFFFF);
	color.green=(int)(col.g*(float)0xFFFF);
	color.blue=(int)(col.b*(float)0xFFFF);
	
	pt.h=pt.v=-1;
	
	if (GetColor(pt,"\pChoose the Light Color:",&color,&color)) {
		col.r=((float)color.red/(float)0xFFFF);
		col.g=((float)color.green/(float)0xFFFF);
		col.b=((float)color.blue/(float)0xFFFF);
	
		dialog_set_color_icon(ctrl,&col);
		Draw1Control(ctrl);
	}
	
	return(eventNotHandledErr);
}

void dialog_set_color(WindowRef wind,unsigned long sig,int id,d3col *col)
{
	ControlRef					ctrl;
	ControlID					ctrl_id;
	EventHandlerUPP				ctrl_event_upp;
	EventTypeSpec				ctrl_event_list[]={{kEventClassControl,kEventControlHit}};
	d3col						col2;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
		// if no property yet, then we can setup event
		
	if (GetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),NULL,&col2)!=0) {
		ctrl_event_upp=NewEventHandlerUPP(dialog_set_color_proc);
		InstallControlEventHandler(ctrl,ctrl_event_upp,GetEventTypeCount(ctrl_event_list),ctrl_event_list,wind,NULL);
	}
	
		// create color
		
	dialog_set_color_icon(ctrl,col);
	Draw1Control(ctrl);
}

void dialog_get_color(WindowRef wind,unsigned long sig,int id,d3col *col)
{
	ControlRef					ctrl;
	ControlID					ctrl_id;
 	ByteCount					sz;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	GetControlProperty(ctrl,'dim3','dim3',sizeof(d3col),&sz,col);
}

/* =======================================================

      Keyboard Focus
      
======================================================= */

void dialog_set_focus(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	SetKeyboardFocus(wind,ctrl,kControlFocusNextPart);
}

/* =======================================================

      Enabling/Hiding
      
======================================================= */

void dialog_enable(WindowRef wind,unsigned long sig,int id,bool enable)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	if (enable) {
		EnableControl(ctrl);
	}
	else {
		DisableControl(ctrl);
	}
}

void dialog_hide(WindowRef wind,unsigned long sig,int id,bool show)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	SetControlVisibility(ctrl,show,show);
}

void dialog_redraw(WindowRef wind,unsigned long sig,int id)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);
	
	Draw1Control(ctrl);
}

/* =======================================================

      Special Dialog Combo Utilities
      
======================================================= */

void dialog_special_combo_fill(WindowRef wind,unsigned long sig,int id,int count,char *names,int str_len,char *sel_name)
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
		c+=str_len;
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

void dialog_special_combo_get(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	int				sel_idx;
	ControlRef		ctrl;
	ControlID		ctrl_id;
	MenuRef			menu;
	CFStringRef		cf_str;
	
	ctrl_id.signature=sig;
	ctrl_id.id=id;
	GetControlByID(wind,&ctrl_id,&ctrl);

	sel_idx=GetControl32BitValue(ctrl);
	if (sel_idx<2) {
		sel_name[0]=0x0;
		return;
	}
	
	menu=GetControlPopupMenuHandle(ctrl);
	
	CopyMenuItemTextAsCFString(menu,sel_idx,&cf_str);
	CFStringGetCString(cf_str,sel_name,str_len,kCFStringEncodingMacRoman);
    CFRelease(cf_str);
	
	sel_name[str_len-1]=0x0;
}

/* =======================================================

      Fill Special Dialogs
      
======================================================= */

void dialog_special_combo_fill_sound(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					sound_count,head_tag,tag;
	char				path[1024],sound_names[256][name_str_len];
	
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

	dialog_special_combo_fill(wind,sig,id,sound_count,(char*)sound_names,name_str_len,sel_name);
}

void dialog_special_combo_fill_particle(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					particle_count,data_head_tag,head_tag,tag;
	char				path[1024],particle_names[256][name_str_len];

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

	dialog_special_combo_fill(wind,sig,id,particle_count,(char*)particle_names,name_str_len,sel_name);
}

void dialog_special_combo_fill_ring(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					ring_count,head_tag,tag;
	char				path[1024],ring_names[256][name_str_len];

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

	dialog_special_combo_fill(wind,sig,id,ring_count,(char*)ring_names,name_str_len,sel_name);
}

void dialog_special_combo_fill_shader(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int					shader_count,head_tag,tag;
	char				path[1024],shader_names[256][file_str_len];

	shader_count=0;
	
	file_paths_data(&file_path_setup,path,"Settings","Shaders","xml");
	if (xml_open_file(path)) {
	
		head_tag=xml_findrootchild("Shaders");
		if (head_tag!=-1) {

			tag=xml_findfirstchild("Shader",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",shader_names[shader_count],file_str_len);
				shader_count++;
				tag=xml_findnextchild(tag);
			}
		}
	
		xml_close_file();
	}

	dialog_special_combo_fill(wind,sig,id,shader_count,(char*)shader_names,file_str_len,sel_name);
}

void dialog_special_combo_fill_node(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
#ifdef DIALOG_IS_EDITOR

	int							n,node_count;
	char						node_names[max_node][name_str_len];
	
	node_count=0;
	
	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) {
			strcpy(node_names[node_count],map.nodes[n].name);
			node_count++;
		}
	}
	
	dialog_special_combo_fill(wind,sig,id,node_count,(char*)node_names,name_str_len,sel_name);
	
#endif
}

void dialog_special_combo_fill_script(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int							n,script_count;
	char						script_names[256][file_str_len];
	file_path_directory_type	*fpd;
	
	fpd=file_paths_read_directory_data(&file_path_setup,"Scripts/Objects","js");

	script_count=fpd->nfile;
	
	for (n=0;n!=fpd->nfile;n++) {
		strcpy(script_names[n],fpd->files[n].file_name);
	}
	
	file_paths_close_directory(fpd);
	
	dialog_special_combo_fill(wind,sig,id,script_count,(char*)script_names,file_str_len,sel_name);
}

void dialog_special_combo_fill_map(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int							n,map_count;
	char						map_names[256][file_str_len];
	file_path_directory_type	*fpd;
	
	fpd=file_paths_read_directory_data(&file_path_setup,"Maps","xml");

	map_count=fpd->nfile;
	
	for (n=0;n!=fpd->nfile;n++) {
		strcpy(map_names[n],fpd->files[n].file_name);
	}
	
	file_paths_close_directory(fpd);
	
	dialog_special_combo_fill(wind,sig,id,map_count,(char*)map_names,file_str_len,sel_name);
}

void dialog_special_combo_fill_model(WindowRef wind,unsigned long sig,int id,char *sel_name)
{
	int							n,model_count;
	char						model_names[256][file_str_len];
	file_path_directory_type	*fpd;
	
	fpd=file_paths_read_directory_data_dir(&file_path_setup,"Models","Mesh.xml");

	model_count=fpd->nfile;
	
	for (n=0;n!=fpd->nfile;n++) {
		strcpy(model_names[n],fpd->files[n].file_name);
	}
	
	file_paths_close_directory(fpd);

	dialog_special_combo_fill(wind,sig,id,model_count,(char*)model_names,file_str_len,sel_name);
}

/* =======================================================

      Get Special Dialogs
      
======================================================= */

inline void dialog_special_combo_get_sound(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

inline void dialog_special_combo_get_particle(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

inline void dialog_special_combo_get_ring(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

inline void dialog_special_combo_get_node(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

inline void dialog_special_combo_get_shader(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

inline void dialog_special_combo_get_script(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

inline void dialog_special_combo_get_map(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

inline void dialog_special_combo_get_model(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len)
{
	dialog_special_combo_get(wind,sig,id,sel_name,str_len);
}

/* =======================================================

      Dialog Map Texture Combo Utilities
      
======================================================= */

void dialog_fill_texture_combo(WindowRef wind,unsigned long sig,int id,bool none,int idx)
{
	int					n,ntexture;
	texture_type		*texture;
	
		// clear combo
		
	dialog_clear_combo(wind,sig,id);
	
		// none items
		
	if (none) {
		dialog_add_combo_item(wind,sig,id,"None",0);
		dialog_add_combo_item(wind,sig,id,"-",0);
	}
	
		// textures
		
#ifdef DIALOG_IS_EDITOR
	texture=map.textures;
	ntexture=max_map_texture;
#else
	texture=model.textures;
	ntexture=max_model_texture;
#endif

	for (n=0;n!=ntexture;n++) {
	
		if (texture->frames[0].name[0]==0x0) {
			dialog_add_combo_item(wind,sig,id,"(no texture)",0);
		}
		else {
			dialog_add_combo_item(wind,sig,id,texture->frames[0].name,0);
		}
		
		texture++;
	}
	
	if (idx==-1) {
		dialog_set_combo(wind,sig,id,0);
	}
	else {
		if (!none) {
			dialog_set_combo(wind,sig,id,idx);
		}
		else {
			dialog_set_combo(wind,sig,id,(idx+2));
		}
	}
}

int dialog_get_texture_combo(WindowRef wind,unsigned long sig,int id,bool none)
{
	int			idx;
	
	idx=dialog_get_combo(wind,sig,id);
	
	if (!none) return(idx);
	
	if (idx==0) return(-1);
	return(idx-2);
}

