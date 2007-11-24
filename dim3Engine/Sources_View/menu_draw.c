/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Menus

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

#include "scripts.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"

#define menu_editor_continue_id			1000
#define menu_editor_toggle_lighting_id	1001
#define menu_editor_reload_map_id		1002
#define menu_editor_quit_id				1004

extern bool					game_loop_quit;

extern server_type			server;
extern map_type				map;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							menu_idx;
bool						menu_start_trigger;

extern int menu_find(char *name);
extern void map_end(void);
extern void game_end(void);
extern void intro_open(void);
extern void debug_light(void);

/* =======================================================

      Menu Setup
      
======================================================= */

void menu_setup_control_interface(void)
{
	int					n,nitem,x,y,high;
	menu_type			*menu;
	menu_item_type		*item;
		
		// get current menu

	menu=&hud.menus[menu_idx];
		
	nitem=menu->nitem;
	item=menu->items;
	
		// setup the controls
		
	high=gl_text_get_char_height(FALSE);
	
	x=setup.screen.x_scale>>1;
	y=((setup.screen.y_scale-((high+5)*nitem))>>1)+(high+5);
	
	for (n=0;n!=nitem;n++) {
		element_text_add(item->data,n,x,y,tx_center,FALSE,TRUE,FALSE);
		if ((net_setup.client.joined) && (item->multiplayer_disable)) element_enable(n,FALSE);
		y+=(high+5);
		item++;
	}
}

void menu_setup_control_editor(void)
{
	int					x,y,high;
		
	high=gl_text_get_char_height(FALSE);
	
	x=setup.screen.x_scale>>1;
	y=((setup.screen.y_scale-((high+5)*4))>>1)+(high+5);

	element_text_add("Continue",menu_editor_continue_id,x,y,tx_center,FALSE,TRUE,FALSE);
	y+=(high+5);

	element_text_add("Toggle Lighting",menu_editor_toggle_lighting_id,x,y,tx_center,FALSE,TRUE,FALSE);
	y+=(high+5);

	element_text_add("Reload Map",menu_editor_reload_map_id,x,y,tx_center,FALSE,TRUE,FALSE);
	y+=(high+5);

	element_text_add("Quit",menu_editor_quit_id,x,y,tx_center,FALSE,TRUE,FALSE);
}

void menu_setup_control(void)
{
		// clear current controls
		
	element_clear();

		// setup correct menu

	if (setup.editor_override.on) {
		menu_setup_control_editor();
	}
	else {
		menu_setup_control_interface();
	}
}

/* =======================================================

      Menu Open/Close
      
======================================================= */

void menu_open(void)
{
	gui_initialize("Bitmaps/Backgrounds","menu");

	menu_idx=0;
	menu_setup_control();
	
	server.state=gs_menu;
}

void menu_close(void)
{
	gui_shutdown();
	
	if (server.state==gs_menu) server.state=gs_running;			// only reset to running if picked menu item didn't reset to something else
}

/* =======================================================

      Menu Select
      
======================================================= */

void menu_select_interface(int idx)
{
	int				sub_idx;
	menu_type		*menu;
	menu_item_type  *item;
	
	menu=&hud.menus[menu_idx];
	item=&menu->items[idx];
		
		// going into sub-menu
			
	if (item->sub_menu[0]!=0x0) {
		sub_idx=menu_find(item->sub_menu);
		if (sub_idx==-1) return;
			
		menu_idx=sub_idx;
		
		menu_setup_control();
		return;
	}
	
		// selecting
		
	menu_close();
			
		// selecting the quit item
		
	if (item->quit) {

		if (setup.editor_override.on) {			// if in editor override, just quit
			game_loop_quit=TRUE;
		}
		else {
			map_end();
			game_end();
			intro_open();
		}

		return;
	}
	
		// selecting event items
		
	scripts_post_event_console(&js.game_attach,sd_event_menu,0,item->item_id);
}

void menu_select_editor(int id)
{
	menu_close();

	switch (id) {

		case menu_editor_toggle_lighting_id:
			debug_light();
			break;

		case menu_editor_reload_map_id:
			server.map_change=TRUE;
			break;

		case menu_editor_quit_id:
			game_loop_quit=TRUE;
			break;
	}
}

void menu_select(int id)
{
	if (setup.editor_override.on) {
		menu_select_editor(id);
	}
	else {
		menu_select_interface(id);
	}
}

/* =======================================================

      Menu Triggers
      
======================================================= */

void menu_trigger_clear(void)
{
	menu_start_trigger=FALSE;
}

void menu_trigger_check(void)
{
	if (menu_start_trigger) menu_open();
}	

void menu_trigger_set(void)
{
	menu_start_trigger=TRUE;
}

/* =======================================================

      Menu Input
      
======================================================= */

void menu_input(void)
{
		// menu start and end key
		
	if (input_action_get_state_single(nc_menu)) {
		if (server.state==gs_running) {
			menu_open();
		}
		else {
			menu_close();
		}
	}
}

void menu_click(void)
{
	int			id;
		
	id=gui_click();
	if (id==-1) return;
		
	hud_click();
	
	menu_select(id);
}
	
/* =======================================================

      Run Menu
      
======================================================= */

void menu_run(void)
{
	gui_draw(1.0f,TRUE);
	menu_click();
	menu_input();
}

