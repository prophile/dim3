/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Load/Save Games

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

#include "interfaces.h"
#include "video.h"
#include "inputs.h"

#define max_file_count					256

#define file_button_save_id				0
#define file_button_load_id				1
#define file_button_delete_id			2
#define file_button_cancel_id			3
#define file_directory_id				4

extern bool game_file_save(char *err_str);
extern bool game_file_quick_save(char *err_str);
extern bool game_file_load(char *file_name,char *err_str);
extern void console_add_error(char *txt);
extern void intro_open(void);
extern void map_end(void);
extern void game_end(void);
extern void game_time_pause_end(void);

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;

char						*file_table_data,*file_name_data;
bool						file_start_trigger;
			
/* =======================================================

      Build File List Utilities
      
======================================================= */

void file_dir_name_to_name(char *dir_name,char *file_name)
{
	char		*c;
	
	strcpy(file_name,&dir_name[18]);
	
	c=strchr(file_name,'.');
	if (c!=NULL) *c=0x0;
}

void file_dir_name_to_time(char *dir_name,char *time_str)
{
	int			hr;
	char		hour_str[8];
	bool		am;
	
		// turn hour into am-pm
		
	am=TRUE;
	memmove(hour_str,&dir_name[8],2);
	hour_str[2]=0x0;
	
	hr=atoi(hour_str);
	if (hr>=12) {
		am=FALSE;
		if (hr>12) hr-=12;
	}
	
	if (hr==0) hr=12;
	
	sprintf(hour_str,"%.2d",hr);
	
		// time string
	
	memmove(&time_str[0],&dir_name[4],2);
	time_str[2]='/';
	memmove(&time_str[3],&dir_name[6],2);
	time_str[5]='/';
	memmove(&time_str[6],&dir_name[0],4);
	time_str[10]=' ';
	memmove(&time_str[11],hour_str,2);
	time_str[13]=':';
	memmove(&time_str[14],&dir_name[10],2);
	time_str[16]=0x0;
	
	if (am) {
		strcat(time_str," am");
	}
	else {
		strcat(time_str," pm");
	}
}

void file_dir_name_to_elapsed(char *dir_name,char *elapse_str)
{
	memmove(&elapse_str[0],&dir_name[13],2);
	elapse_str[2]=':';
	memmove(&elapse_str[3],&dir_name[15],2);
	elapse_str[5]=0x0;
}

/* =======================================================

      Build File List
      
======================================================= */

void file_build_list(void)
{
	int							n,sz;
	char						*c,name_str[64],time_str[64],elapse_str[64];
	file_path_directory_type	*fpd;
	
		// data for maximum number of files
		
	sz=max_file_count*128;
	
	file_table_data=valloc(sz);
	bzero(file_table_data,sz);
	
	file_name_data=valloc(sz);
	bzero(file_name_data,sz);
	
		// grab the files
		
	fpd=file_paths_read_directory(&setup.file_path_setup,"Saved Games","sav",TRUE);
	if (fpd==NULL) return;
	
	for (n=0;n!=fpd->nfile;n++) {
	
			// save name
			
		c=file_name_data+(128*n);
		strcpy(c,fpd->files[n].file_name);
		
			// table data
			
		file_dir_name_to_name(fpd->files[n].file_name,name_str);
		file_dir_name_to_time(fpd->files[n].file_name,time_str);
		file_dir_name_to_elapsed(fpd->files[n].file_name,elapse_str);
		
		c=file_table_data+(128*n);
		sprintf(c,"Saved Games;%s;%s\t%s\t%s",fpd->files[n].file_name,name_str,time_str,elapse_str);
	}

	file_paths_close_directory(fpd);
}

void file_close_list(void)
{
	free(file_table_data);
	free(file_name_data);
}

/* =======================================================

      Save File Selected
      
======================================================= */

void file_save_selected(void)
{
	int				idx;
	
	idx=element_get_value(file_directory_id);
	
		// nothing selected
		
	if (idx==-1) {

		if (server.map_open) {
			element_hide(file_button_save_id,FALSE);
			element_hide(file_button_load_id,TRUE);
		}
		else {
			element_hide(file_button_save_id,TRUE);
			element_hide(file_button_load_id,FALSE);
			element_enable(file_button_load_id,FALSE);
		}

		element_enable(file_button_delete_id,FALSE);
		return;
	}
	
		// save game selected

	element_hide(file_button_save_id,TRUE);
	element_hide(file_button_load_id,FALSE);
	element_enable(file_button_load_id,TRUE);
	element_enable(file_button_delete_id,TRUE);
}

/* =======================================================

      Save File Delete
      
======================================================= */

void file_save_delete(void)
{
	int				idx;
	char			*c,file_name[128],path[1024];
	
	idx=element_get_value(file_directory_id);
	if (idx==-1) return;
	
		// delete save game and png
		
	strcpy(file_name,(char*)(file_name_data+(128*idx)));
	
	c=strrchr(file_name,'.');		// get rid of .sav
	if (c!=NULL) *c=0x0;
	
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",file_name,"png");
	remove(path);
	
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",file_name,"sav");
	remove(path);
	
		// reset table
		
	file_close_list();
	file_build_list();
	
	element_set_table_data(file_directory_id,file_table_data);
	
	element_set_value(file_directory_id,-1);
	file_save_selected();
}

/* =======================================================

      File Operations
      
======================================================= */

void file_open(void)
{
	int					x,y,wid,high,padding;
	element_column_type	cols[4];
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);
	
		// title
		
	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.09f);

	element_text_add("Load Saved Game",-1,x,y,hud.font.text_size_large,tx_left,FALSE,FALSE);
	
		// make the file list
		
	file_build_list();
	
		// files
		
	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.12f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.88f)-y;

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=0.50f;
	strcpy(cols[1].name,"Save Time");
	cols[1].percent_size=0.32f;
	strcpy(cols[2].name,"Elapsed Time");
	cols[2].percent_size=0.18f;

	element_table_add(cols,file_table_data,file_directory_id,3,x,y,wid,high,element_table_bitmap_document);
	
		// buttons
		
	padding=element_get_padding();
	
	wid=(int)(((float)hud.scale_x)*0.1f);
	high=(int)(((float)hud.scale_x)*0.05f);
	
	x=hud.scale_x-padding;
	y=hud.scale_y-padding;
	
	element_button_text_add("Save",file_button_save_id,x,y,wid,high,element_pos_right,element_pos_bottom);
	element_hide(file_button_save_id,!server.map_open);
	
	element_button_text_add("Load",file_button_load_id,x,y,wid,high,element_pos_right,element_pos_bottom);
	element_enable(file_button_load_id,FALSE);
	element_hide(file_button_load_id,server.map_open);

	x=element_get_x_position(file_button_load_id)-padding;
	
	element_button_text_add("Delete",file_button_delete_id,x,y,wid,high,element_pos_right,element_pos_bottom);
	element_enable(file_button_delete_id,FALSE);
	
	x=element_get_x_position(file_button_delete_id)-padding;

	element_button_text_add("Cancel",file_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);

		// in file chooser
		
	server.state=gs_file;
}

void file_close(void)
{
	gui_shutdown();
	file_close_list();
}

void file_return_to_game(void)
{
	server.state=gs_running;
}

/* =======================================================

      File Triggers
      
======================================================= */

void file_trigger_clear(void)
{
	file_start_trigger=FALSE;
}

void file_trigger_check(void)
{
	if (file_start_trigger) file_open();
}	

void file_trigger_set(void)
{
	file_start_trigger=TRUE;
}

/* =======================================================

      File Input
      
======================================================= */

void file_input(void)
{
	char		err_str[256];
	
	if (input_action_get_state_single(nc_save_load)) {
		file_open();
		return;
	}
	if (input_action_get_state_single(nc_quick_save)) {
		if (!game_file_save(err_str)) console_add_error(err_str);
		return;
	}
}

void file_click(void)
{
	int			id,k;
	char		err_str[256],file_name[256];
	
		// is element being clicked
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run selection
		
	switch (id) {
	
		case file_button_save_id:
			if (!game_file_save(err_str)) console_add_error(err_str);
			file_close();
			file_return_to_game();
			break;
			
		case file_button_load_id:
			k=element_get_value(file_directory_id);
			if (k==-1) break;
			
			strcpy(file_name,(char*)(file_name_data+(128*k)));

			file_close();

			if (game_file_load(file_name,err_str)) {
				game_time_pause_end();
				file_return_to_game();
				break;
			}

			if (server.map_open) map_end();
			if (server.game_open) game_end();

			error_open(err_str,"Game Load Canceled");
			break;
			
		case file_button_delete_id:
			file_save_delete();
			break;
			
		case file_button_cancel_id:
			file_close();
			if (!server.map_open) {
				intro_open();
				return;
			}
			file_return_to_game();
			break;
			
		case file_directory_id:
			file_save_selected();
			break;
			
	}
}
	
/* =======================================================

      Run File
      
======================================================= */

void file_run(void)
{
	gui_draw(1.0f,TRUE);
	file_click();
}

