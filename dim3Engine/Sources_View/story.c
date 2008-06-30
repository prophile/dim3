/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Story

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

#define story_icon_close_id			0
#define story_icon_back_id			1
#define story_icon_forward_id		2

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern js_type				js;

int							story_page,story_page_count,story_event_id;
char						story_name[name_str_len];
bool						story_start_trigger;

/* =======================================================

      Story Elements
      
======================================================= */

void story_setup_control(void)
{
	int				x,y,lx,rx,ty,by,page_sz;
	char			sub_path[1024],page_name[256],path[1024],path2[1024];
	struct stat		sb;
	
		// get the number of story pages
		
	sprintf(sub_path,"Story/%s",story_name);
	
	story_page_count=1;
	
	while (TRUE) {
		sprintf(page_name,"%.2d",(story_page_count+1));
		file_paths_data(&setup.file_path_setup,path,sub_path,page_name,"png");
		if (stat(path,&sb)==-1) break;
		
		story_page_count++;
	}
	
		// setup elements
		
	element_clear();
		
		// sizes
		
	page_sz=hud.scale_y-((15+32)+(15+32));
	
		// controls
		
	lx=(hud.scale_x-page_sz)>>1;
	rx=hud.scale_x-lx;
	ty=(hud.scale_y-page_sz)>>1;
	by=hud.scale_y-ty;

	ty-=5;
	by+=5;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","icon_close","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","icon_close_selected","png");
	element_button_add(path,path2,story_icon_close_id,rx,ty,-1,-1,element_pos_right,element_pos_bottom);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","icon_back","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","icon_back_selected","png");
	element_button_add(path,path2,story_icon_back_id,lx,by,-1,-1,element_pos_left,element_pos_top);
	element_enable(story_icon_back_id,(story_page>0));

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","icon_forward","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","icon_forward_selected","png");
	element_button_add(path,path2,story_icon_forward_id,rx,by,-1,-1,element_pos_right,element_pos_top);
	element_enable(story_icon_forward_id,(story_page<(story_page_count-1)));
	
		// story bitmap
		
	sprintf(page_name,"%.2d",(story_page+1));
	file_paths_data(&setup.file_path_setup,path,sub_path,page_name,"png");
	
	x=(hud.scale_x-page_sz)>>1;
	y=15+32;
		
	element_bitmap_add(path,-1,x,y,page_sz,page_sz,FALSE);
}

void story_get_page_count(void)
{
}
	
/* =======================================================

      Story Operations
      
======================================================= */

void story_open(void)
{
	gui_initialize(NULL,NULL,TRUE);
	story_setup_control();
	
	server.state=gs_story;
}

void story_close(void)
{
	gui_shutdown();

	if (story_event_id!=-1) scripts_post_event_console(&js.game_attach,sd_event_interface,sd_event_interface_story_done,story_event_id);
	
	server.state=gs_running;
}

/* =======================================================

      Story Triggers
      
======================================================= */

void story_trigger_clear(void)
{
	story_start_trigger=FALSE;
}

void story_trigger_check(void)
{
	if (story_start_trigger) story_open();
}	

void story_trigger_set(char *name,int event_id)
{
	story_page=0;
	story_page_count=1;
	strcpy(story_name,name);
	story_event_id=event_id;
	
	story_start_trigger=TRUE;
}

/* =======================================================

      Story Input
      
======================================================= */

void story_click(void)
{
	int				id;
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run click
		
	switch (id) {
	
		case story_icon_close_id:
			story_close();
			break;
			
		case story_icon_back_id:
			story_page--;
			story_setup_control();
			break;
			
		case story_icon_forward_id:
			story_page++;
			story_setup_control();
			break;
	}
}

/* =======================================================

      Run Story
      
======================================================= */

void story_run(void)
{
	gui_draw(1.0f,TRUE);
	story_click();
}


