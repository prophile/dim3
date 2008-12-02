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

#define setup_network_pane_player			0
#define setup_network_pane_host				1

#define ctrl_network_tab_id					0

#define ctrl_network_name_id				10
#define ctrl_network_team_id				11
#define ctrl_network_show_names_id			12
#define ctrl_network_host_id				13
#define ctrl_network_host_ip_id				14

#define setup_network_host_add_button		20
#define setup_network_host_update_button	21
#define setup_network_host_delete_button	22

#define setup_network_ok_button				100
#define setup_network_cancel_button			101

extern void intro_open(void);

extern render_info_type		render_info;
extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern hud_type				hud;

extern setup_type			setup_backup;

int							setup_network_tab_value,setup_network_host_scroll_pos;
char						setup_team_color_list[net_team_count+1][32]=net_team_color_list_def,
							setup_host_list[max_setup_network_host+1][128];

/* =======================================================

      Setup Network Panes
      
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

void setup_network_player_pane(void)
{
	int					x,y,
						control_y_add,control_y_sz;

	control_y_add=element_get_control_high();
	control_y_sz=control_y_add*3;
	
	x=(int)(((float)hud.scale_x)*0.4f);
	y=(hud.scale_y>>1)-(control_y_sz>>1);

		// name and timeout
		
	element_text_field_add("Name",setup.network.name,name_str_len,ctrl_network_name_id,x,y,TRUE);
	y+=control_y_add;
	element_combo_add("Team",(char*)setup_team_color_list,setup.network.team_idx,ctrl_network_team_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Show Names",setup.network.show_names,ctrl_network_show_names_id,x,y,TRUE);
}

void setup_network_host_pane(void)
{
	int						x,y,wid,high,padding,control_y_add;
	element_column_type		cols[1];

	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.15f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.68f)-y;

	control_y_add=element_get_control_high();

		// setup host list
		
	setup_network_create_host_list();

	strcpy(cols[0].name,"Network Hosts");
	cols[0].percent_size=1.0f;
	
	element_table_add(cols,(char*)setup_host_list,ctrl_network_host_id,1,x,y,wid,high,element_table_bitmap_none);

		// host editing

	x=(int)(((float)hud.scale_x)*0.15f);
	y+=(high+control_y_add+8);

	element_text_field_add("Address","",name_str_len,ctrl_network_host_ip_id,x,y,TRUE);

		// host buttons

	padding=element_get_padding();

	x=hud.scale_x>>1;
	
	wid=(int)(((float)hud.scale_x)*0.2f);
	high=(int)(((float)hud.scale_x)*0.05f);
	
	y+=(padding+(high/2));

	element_button_text_add("Add Host",setup_network_host_add_button,(x-(padding/2)),y,wid,high,element_pos_right,element_pos_center);

	element_button_text_add("Update Host",setup_network_host_update_button,(x-(padding/2)),y,wid,high,element_pos_right,element_pos_center);
	element_hide(setup_network_host_update_button,TRUE);
	
	element_button_text_add("Delete Host",setup_network_host_delete_button,(x+(padding/2)),y,wid,high,element_pos_left,element_pos_center);
	element_enable(setup_network_host_delete_button,FALSE);
}

void setup_network_create_pane(void)
{
	int			x,y,wid,high,yadd,padding,
				tab_list_wid,tab_pane_high,pane;
	char		tab_list[][32]={"Player","Host"};
							
	element_clear();
	
		// tabs
		
	padding=element_get_padding();;
	
	wid=hud.scale_x;
	yadd=(int)(((float)hud.scale_y)*0.015f);
	high=(int)(((float)hud.scale_y)*0.065f);
	tab_list_wid=(int)(((float)hud.scale_x)*0.85f);
	tab_pane_high=(int)(((float)hud.scale_y)*0.82f);
	
	element_tab_add((char*)tab_list,setup_network_tab_value,ctrl_network_tab_id,2,0,(padding+yadd),wid,high,tab_list_wid,tab_pane_high);
	
		// buttons
		
	wid=(int)(((float)hud.scale_x)*0.2f);
	high=(int)(((float)hud.scale_x)*0.05f);
	
	wid=(int)(((float)hud.scale_x)*0.1f);
	x=hud.scale_x-padding;
	y=hud.scale_y-padding;

	element_button_text_add("OK",setup_network_ok_button,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(setup_network_ok_button)-padding;

	element_button_text_add("Cancel",setup_network_cancel_button,x,y,wid,high,element_pos_right,element_pos_bottom);
	
		// specific pane controls
		
	pane=element_get_value(ctrl_network_tab_id);
		
	switch (pane) {
		case setup_network_pane_player:
			setup_network_player_pane();
			break;
		case setup_network_pane_host:
			setup_network_host_pane();
			break;
	}
}

/* =======================================================

      Setup Network Operations
      
======================================================= */

void setup_network_open(void)
{	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);

		// start with first tab
		
	setup_network_tab_value=0;
	setup_network_create_pane();

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

			// tab
			
		case ctrl_network_tab_id:
			setup_network_tab_value=element_get_value(ctrl_network_tab_id);
			setup_network_create_pane();
			return;
	
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

		case ctrl_network_show_names_id:
			setup.network.show_names=element_get_value(ctrl_network_show_names_id);
			break;

		case ctrl_network_host_id:
			setup_network_enable_buttons();
			break;
			
	}
}

void setup_network_keyboard(void)
{
	int			id;

	id=gui_keyboard();
	if (id==-1) return;

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

