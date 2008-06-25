/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Setup Network

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

#include "interfaces.h"
#include "xmls.h"
#include "video.h"
#include "sounds.h"
#include "inputs.h"

#define ctrl_network_name_id				0
#define ctrl_network_team_id				1
#define ctrl_network_timeout_id				2
#define ctrl_network_show_names_id			3
#define ctrl_network_host_id				4
#define ctrl_network_host_ip_id				5

#define setup_network_host_add_button		10
#define setup_network_host_update_button	11
#define setup_network_host_delete_button	12

#define setup_network_ok_button				100
#define setup_network_cancel_button			101

extern void intro_open(void);

extern render_info_type		render_info;
extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;

extern setup_type			setup_backup;

int							setup_network_host_scroll_pos;
char						setup_team_color_list[][32]=net_team_color_list_def,
							setup_timeout_mode_list[][32]=setup_timeout_mode_list_def,
							setup_host_list[max_setup_network_host+1][128];

/* =======================================================

      Setup Network Operations
      
======================================================= */

void setup_network_create_host_list(void)
{
	int							n;
	setup_network_hosts_type	*host;

	host=setup.network.hosts;

	for (n=0;n!=setup.network.nhost;n++) {
		strcpy(setup_host_list[n],host->ip);
		host++;
	}
	
	setup_host_list[setup.network.nhost][0]=0x0;
}

void setup_network_open(void)
{
	int							x,y,wid,high,padding,control_y_add,separate_y_add;
	char						path[1024],path2[1024];
	element_column_type			cols[1];
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);
	
	element_clear();
	
		// padding
		
	padding=(int)(((float)setup.screen.x_scale)*0.025f);

		// name and timeout
		
	y=70;
	control_y_add=element_get_control_high();
	separate_y_add=element_get_separator_high();

	element_text_field_add("Name",setup.network.name,name_str_len,ctrl_network_name_id,100,y,TRUE);
	y+=control_y_add;
	element_combo_add("Team",(char*)setup_team_color_list,setup.network.team_idx,ctrl_network_team_id,100,y,TRUE);
	y+=control_y_add;
	element_combo_add("Timeout Wait",(char*)setup_timeout_mode_list,setup.network.timeout_mode,ctrl_network_timeout_id,100,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Show Names",setup.network.show_names,ctrl_network_show_names_id,100,y,TRUE);
	y+=control_y_add;

	x=105;
	wid=(setup.screen.x_scale-105)-x;
	high=(setup.screen.y_scale-150)-y;
	
		// setup host list
		
	setup_network_create_host_list();

	strcpy(cols[0].name,"Network Hosts");
	cols[0].percent_size=1.0f;
	
	element_table_add(cols,(char*)setup_host_list,ctrl_network_host_id,1,x,y,wid,high,element_table_bitmap_none);

		// host editing

	y+=(high+control_y_add+8);

	element_text_field_add("Address","",name_str_len,ctrl_network_host_ip_id,100,y,TRUE);
	y+=control_y_add;

	x=105;
	y=setup.screen.y_scale-70;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_network_host_add","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_network_host_add_selected","png");
	element_button_add(path,path2,setup_network_host_add_button,x,y,-1,-1,element_pos_left,element_pos_bottom);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_network_host_update","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_network_host_update_selected","png");
	element_button_add(path,path2,setup_network_host_update_button,x,y,-1,-1,element_pos_left,element_pos_bottom);
	element_hide(setup_network_host_update_button,TRUE);

	x=setup.screen.x_scale-105;
	
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_network_host_delete","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_network_host_delete_selected","png");
	element_button_add(path,path2,setup_network_host_delete_button,x,y,-1,-1,element_pos_right,element_pos_bottom);
	element_enable(setup_network_host_delete_button,FALSE);
	
		// buttons
	
	x=setup.screen.x_scale-padding;
	y=setup.screen.y_scale-padding;
	
	wid=(int)(((float)setup.screen.x_scale)*0.1f);
	high=(int)(((float)setup.screen.x_scale)*0.05f);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_ok","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_ok_selected","png");
	element_button_add(path,path2,setup_network_ok_button,x,y,-1,-1,element_pos_right,element_pos_bottom);

	x=element_get_x_position(setup_network_ok_button)-10;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_cancel","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_cancel_selected","png");
	element_button_add(path,path2,setup_network_cancel_button,x,y,-1,-1,element_pos_right,element_pos_bottom);

		// save setup
		
	memmove(&setup_backup,&setup,sizeof(setup_type));
	
		// server in setup
		
	server.state=gs_setup_network;
}

void setup_network_cancel_close(void)
{
		// restore old settings

	memmove(&setup,&setup_backup,sizeof(setup_type));

		// shutdown and continue

	gui_shutdown();
	
	intro_open();
}

void setup_network_save_close(void)
{
		// write setup
		
	setup_xml_write();

		// return to intro
	
	gui_shutdown();
	intro_open();
}

/* =======================================================

      Network Actions
      
======================================================= */

void setup_network_enable_buttons(void)
{
	int							host_idx;
	setup_network_hosts_type	*host;
	
		// get select host

	host_idx=element_get_value(ctrl_network_host_id);
	if (host_idx==-1) {

			// hide the update and disable the delete

		element_hide(setup_network_host_add_button,FALSE);
		element_hide(setup_network_host_update_button,TRUE);
		element_enable(setup_network_host_delete_button,FALSE);

			// clear the text

		element_set_value_string(ctrl_network_host_ip_id,"");
		return;
	}

		// enable all buttons

	element_hide(setup_network_host_add_button,TRUE);
	element_hide(setup_network_host_update_button,FALSE);
	element_enable(setup_network_host_delete_button,TRUE);

		// set controls

	host=&setup.network.hosts[host_idx];

	element_set_value_string(ctrl_network_host_ip_id,host->ip);
}

void setup_network_host_add_update(bool in_add)
{
	int							host_idx;
	setup_network_hosts_type	*host;
	
		// get item to add or update

	if (in_add) {
		if (setup.network.nhost>=max_setup_network_host) return;
		host_idx=setup.network.nhost;
		setup.network.nhost++;
	}
	else {
		host_idx=element_get_value(ctrl_network_host_id);
		if (host_idx==-1) return;
	}

		// add/update item

	host=&setup.network.hosts[host_idx];

	element_get_value_string(ctrl_network_host_ip_id,host->ip);
	
		// rebuild list
		
	setup_network_create_host_list();
	element_set_table_data(ctrl_network_host_id,(char*)setup_host_list);
	element_set_value(ctrl_network_host_id,host_idx);
	setup_network_enable_buttons();
}

void setup_network_host_delete(void)
{
	int			host_idx,msz;

	if (setup.network.nhost==0) return;

		// delete item

	host_idx=element_get_value(ctrl_network_host_id);
	if (host_idx==-1) return;

	msz=(setup.network.nhost-(host_idx+1))*sizeof(setup_network_hosts_type);
	if (msz>0) memmove(&setup.network.hosts[host_idx],&setup.network.hosts[host_idx+1],msz);

	setup.network.nhost--;
	
		// rebuild list
		
	element_set_value(ctrl_network_host_id,-1);
		
	setup_network_create_host_list();
	element_set_table_data(ctrl_network_host_id,(char*)setup_host_list);
	setup_network_enable_buttons();
}

/* =======================================================

      Setup Network Input
      
======================================================= */

void setup_network_handle_click(int id)
{
	switch (id) {
	
			// buttons
			
		case setup_network_ok_button:
			setup_network_save_close();
			return;
			
		case setup_network_cancel_button:
			setup_network_cancel_close();
			return;
			
		case setup_network_host_add_button:
			setup_network_host_scroll_pos=element_get_scroll_position(ctrl_network_host_id);
			setup_network_host_add_update(TRUE);
			element_set_scroll_position(ctrl_network_host_id,setup_network_host_scroll_pos);
			return;

		case setup_network_host_update_button:
			setup_network_host_scroll_pos=element_get_scroll_position(ctrl_network_host_id);
			setup_network_host_add_update(FALSE);
			element_set_scroll_position(ctrl_network_host_id,setup_network_host_scroll_pos);
			return;

		case setup_network_host_delete_button:
			setup_network_host_scroll_pos=element_get_scroll_position(ctrl_network_host_id);
			setup_network_host_delete();
			element_set_scroll_position(ctrl_network_host_id,setup_network_host_scroll_pos);
			return;

			// controls

		case ctrl_network_name_id:
			element_get_value_string(ctrl_network_name_id,setup.network.name);
			break;

		case ctrl_network_team_id:
			setup.network.team_idx=element_get_value(ctrl_network_team_id);
			break;

		case ctrl_network_timeout_id:
			setup.network.timeout_mode=element_get_value(ctrl_network_timeout_id);
			break;

		case ctrl_network_show_names_id:
			setup.network.show_names=element_get_value(ctrl_network_show_names_id);
			break;

		case ctrl_network_host_id:
			setup_network_enable_buttons();
			break;
			
		case ctrl_network_host_ip_id:
			setup_network_host_add_update(FALSE);
			break;
			
	}
}

void setup_network_keyboard(void)
{
	int			id;

	id=gui_keyboard();
	if (id==-1) return;

	hud_click();

	setup_network_handle_click(id);
}

void setup_network_click(void)
{
	int			id;
	
	id=gui_click();
	if (id==-1) return;
	
	hud_click();

	setup_network_handle_click(id);
}

/* =======================================================

      Run Network Setup
      
======================================================= */

void setup_network_run(void)
{
	gui_draw(1.0f,TRUE);
	setup_network_keyboard();
	setup_network_click();
}

