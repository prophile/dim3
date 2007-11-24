/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Movies

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

extern server_type			server;
extern setup_type			setup;
extern js_type				js;

int							movie_bitmap_gl_id,
							movie_x_offset,movie_y_offset,
							movie_x_sz,movie_y_sz,
							movie_event_id;
char						movie_name[name_str_len];
bool						movie_start_trigger;

/* =======================================================

      Load Movie
      
======================================================= */

bool movie_initialize(void)
{
	int				wid,high;
	char			path[1024];
	
	file_paths_data(&setup.file_path_setup,path,"Movies",movie_name,"mov");
	if (!bitmap_movie_start(path,&wid,&high)) return(FALSE);
	
		// get movie texture

	movie_bitmap_gl_id=bitmap_movie_get_texture_id();

		// create screen size for movie

	if (wid>high) {
		movie_x_sz=setup.screen.x_sz;
		movie_y_sz=(high*setup.screen.x_sz)/wid;
	}
	else {
		movie_x_sz=(wid*setup.screen.y_sz)/high;
		movie_y_sz=setup.screen.y_sz;
	}

	movie_x_offset=(setup.screen.x_sz-movie_x_sz)>>1;
	movie_y_offset=(setup.screen.y_sz-movie_y_sz)>>1;
	
	return(TRUE);
}

void movie_shutdown(void)
{
	bitmap_movie_end();
}

/* =======================================================

      Draw Movie
      
======================================================= */

void movie_draw(void)
{
		// refresh texture with next movie frame
		
	bitmap_movie_texture_refresh();
	
		// start the frame
	
	gl_frame_start();
	gl_2D_view();
	
	gl_texture_movie_start(movie_bitmap_gl_id);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
			
		// draw movie
		
    glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(movie_x_offset,movie_y_offset);
	glTexCoord2f(1,0);
	glVertex2i((movie_x_offset+movie_x_sz),movie_y_offset);
	glTexCoord2f(1,1);
	glVertex2i((movie_x_offset+movie_x_sz),(movie_y_offset+movie_y_sz));
	glTexCoord2f(0,1);
	glVertex2i(movie_x_offset,(movie_y_offset+movie_y_sz));
    glEnd();
    
	gl_texture_movie_end();
	glEnable(GL_DEPTH_TEST);

	gl_frame_end();
}

/* =======================================================

      Movie Operations
      
======================================================= */

void movie_open(void)
{
	gui_initialize(NULL,NULL);

	if (!movie_initialize()) {
		gui_shutdown();
		server.state=gs_running;
		return;
	}
	
	movie_draw();
	
	server.state=gs_movie;
}

void movie_close(void)
{
	movie_shutdown();
	
	gui_shutdown();
	server.state=gs_running;
}

/* =======================================================

      Movie Triggers
      
======================================================= */

void movie_trigger_clear(void)
{
	movie_start_trigger=FALSE;
}

void movie_trigger_check(void)
{
	if (movie_start_trigger) movie_open();
}	

void movie_trigger_set(char *name,int event_id)
{
	strcpy(movie_name,name);
	movie_event_id=event_id;
	
	movie_start_trigger=TRUE;
}

/* =======================================================

      Run Movie
      
======================================================= */

void movie_run(void)
{
	movie_draw();
	
		// movie over or mouse button pushed?
		
	if ((!bitmap_movie_playing()) || (input_gui_get_mouse_left_button_down())) {

		movie_close();
		if (movie_event_id!=-1) scripts_post_event_console(&js.game_attach,sd_event_interface,sd_event_interface_movie_done,movie_event_id);

	}
}


