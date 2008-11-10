/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Errors

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

#include "scripts.h"
#include "interfaces.h"
#include "sounds.h"
#include "inputs.h"
#include "video.h"

#define error_icon_close_id			0

extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;
extern js_type				js;

extern void intro_open(void);

/* =======================================================

      Error Operations
      
======================================================= */

void error_open(char *err_str,char *err_str_2)
{
	int				x,y;
	char			path[1024],path2[1024];
	
		// setup
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);

		// error
		
	x=hud.scale_x/2;
	
	y=(hud.scale_y/2)-5;
	element_text_add("[Error]",-1,x,y,hud.font.text_size_large,tx_center,FALSE,TRUE);

	y+=gl_text_get_char_height(hud.font.text_size_small)+5;
	element_text_add(err_str,-1,x,y,hud.font.text_size_small,tx_center,FALSE,FALSE);
	
	y+=gl_text_get_char_height(hud.font.text_size_small)+3;
	element_text_add(err_str_2,-1,x,y,hud.font.text_size_small,tx_center,FALSE,FALSE);

		// close button

	y+=5;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","icon_close","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","icon_close_selected","png");
	element_button_bitmap_add(path,path2,error_icon_close_id,x,y,-1,-1,element_pos_center,element_pos_top);

		// running error
		
	server.state=gs_error;
}

void error_close(void)
{
	gui_shutdown();
	intro_open();
}

/* =======================================================

      Run Error Page
      
======================================================= */

void error_run(void)
{
	int				id;

	gui_draw(1.0f,TRUE);
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
	if (id==error_icon_close_id) error_close();
}


