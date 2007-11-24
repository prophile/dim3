/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Picking (Debug)

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

#define map_pick_close_id			1000

extern map_type				map;
extern server_type			server;
extern setup_type			setup;

bool						map_pick_start_trigger;
file_path_directory_type	*map_pick_fpd;

/* =======================================================

      Map Pick Setup
      
======================================================= */

void map_pick_setup_control(void)
{
	int					n,x,y,y_start,nfile,xadd,high;
	
		// clear current controls
		
	element_clear();
	
		// read the map files
		
	nfile=map_pick_fpd->nfile;
	
	high=gl_text_get_char_height(TRUE);
	
	xadd=setup.screen.x_scale/3;
	
	x=xadd>>1;
	y=y_start=(setup.screen.y_scale-((high+1)*15))>>1;
	
	for (n=0;n!=nfile;n++) {
		element_text_add(map_pick_fpd->files[n].file_name,n,x,y,tx_center,TRUE,TRUE,FALSE);
		if (((n+1)%16)==0) {
			x+=xadd;
			y=y_start;
		}
		else {
			y+=(high+1);
		}
	}
	
	element_text_add("Exit",map_pick_close_id,(setup.screen.x_scale-10),(setup.screen.y_scale-(high+1)),tx_right,TRUE,TRUE,FALSE);
}

/* =======================================================

      Map Pick Open/Close
      
======================================================= */

void map_pick_open(void)
{
	gui_initialize("Bitmaps/Backgrounds","menu");

	map_pick_fpd=file_paths_read_directory(&setup.file_path_setup,"Maps","xml",FALSE);
	map_pick_setup_control();
	
	server.state=gs_map_pick;
}

void map_pick_close(void)
{
	file_paths_close_directory(map_pick_fpd);
	
	gui_shutdown();
	server.state=gs_running;
}

/* =======================================================

      Map Pick Select
      
======================================================= */

void map_pick_select(int id)
{
	if (id!=map_pick_close_id) {
		strncpy(map.info.name,map_pick_fpd->files[id].file_name,name_str_len);
		strcpy(map.info.player_start_name,"Start");
		strcpy(map.info.player_start_type,"Player");
		map.info.in_load=FALSE;
		server.map_change=TRUE;
	}

	map_pick_close();
}

/* =======================================================

      Map Pick Triggers
      
======================================================= */

void map_pick_trigger_clear(void)
{
	map_pick_start_trigger=FALSE;
}

void map_pick_trigger_check(void)
{
	if (map_pick_start_trigger) map_pick_open();
}	

void map_pick_trigger_set(void)
{
	map_pick_start_trigger=TRUE;
}

/* =======================================================

      Map Pick Input and Clicking
      
======================================================= */

void map_pick_click(void)
{
	int			id;
	
		// map pick key flips it off
		
	if (input_action_get_state_single(nc_menu)) {
		map_pick_close();
		return;
	}
	
		// clicks
		
	id=gui_click();
	if (id==-1) return;
		
	hud_click();
	
	map_pick_select(id);
}
	
/* =======================================================

      Run Map Pick
      
======================================================= */

void map_pick_run(void)
{
	gui_draw(1.0f,TRUE);
	map_pick_click();
}

