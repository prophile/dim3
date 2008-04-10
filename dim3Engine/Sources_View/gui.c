/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Code

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
#include "sounds.h"
#include "inputs.h"

extern hud_type				hud;
extern setup_type			setup;

int							gui_save_x_scale,gui_save_y_scale;
bool						gui_has_background,gui_show_view,gui_show_dialog;
char						gui_last_key;
bitmap_type					gui_background_bitmap;

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern void map_restart_ambient(void);
extern void view_gui_draw(void);

/* =======================================================

      GUI Initialize and Shutdown
      
======================================================= */

void gui_initialize(char *background_path,char *bitmap_name,bool show_view,bool show_dialog)
{
	int			x,y;
	char		name[256],path[1024];
	
		// pause game
		
	game_time_pause_start();
	
		// stop playing ambients
		
	al_stop_all_looping_sources();
	
		// initialize cursor and elements
		
	cursor_initialize();
	element_initialize();
	
		// load the background bitmap
		// we pick the version with _wide on the
		// end if screen is in widescreen resolutions
		
	gui_has_background=FALSE;
	gui_show_view=show_view;
	gui_show_dialog=show_dialog;
	
	if (bitmap_name!=NULL) {
		gui_has_background=TRUE;

		if (gl_is_screen_widescreen()) {
			sprintf(name,"%s_wide",bitmap_name);
			file_paths_data(&setup.file_path_setup,path,background_path,name,"png");
		}
		else {
			file_paths_data(&setup.file_path_setup,path,background_path,bitmap_name,"png");
		}

		bitmap_open(&gui_background_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);
	}

		// GUI scaling set by interface.xml
		
	gui_save_x_scale=setup.screen.x_scale;
	gui_save_y_scale=setup.screen.y_scale;
	
	setup.screen.x_scale=hud.scale_x;
	setup.screen.y_scale=hud.scale_y;
	
		// start mouse in middle of screen
		
	x=setup.screen.x_sz>>1;
	y=setup.screen.y_sz>>1;
	input_gui_set_mouse(x,y);

		// no last keypress

	gui_last_key=0x0;
}

void gui_shutdown(void)
{
		// close background bitmap
		
	if (gui_has_background) bitmap_close(&gui_background_bitmap);
	
		// release cursor and elements
		
	element_shutdown();
	cursor_shutdown();
	
		// reset GUI scaling
		
	setup.screen.x_scale=gui_save_x_scale;
	setup.screen.y_scale=gui_save_y_scale;
	
		// clear all movement from gui	
			
	input_clear();
	
		// restart any map ambients
		
	map_restart_ambient();
	
		// unpause game
		
	game_time_pause_end();
}

/* =======================================================

      GUI Backgrounds
      
======================================================= */

void gui_draw_background(float alpha)
{
		// show view in background

	if (gui_show_view) view_gui_draw();

		// no background at all?

	if (!gui_has_background) return;

		// 2D draw setup

	gl_2D_view();

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

		// missing bitmap?

	if (gui_background_bitmap.gl_id==-1) {
		glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i(setup.screen.x_sz,0);
		glVertex2i(setup.screen.x_sz,setup.screen.y_sz);
		glVertex2i(0,setup.screen.y_sz);
		glEnd();
		return;
	}

		// draw bitmap

	gl_texture_simple_start();

	gl_texture_simple_set(gui_background_bitmap.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
		
    glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(0,0);
	glTexCoord2f(1,0);
	glVertex2i(setup.screen.x_sz,0);
	glTexCoord2f(1,1);
	glVertex2i(setup.screen.x_sz,setup.screen.y_sz);
	glTexCoord2f(0,1);
	glVertex2i(0,setup.screen.y_sz);
    glEnd();
    
	gl_texture_simple_end();
}

void gui_draw_dialog(void)
{
	int			lft,rgt,top,bot;

	if (!gui_show_dialog) return;

	gl_2D_view();

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	element_get_dialog_size(&lft,&top,&rgt,&bot);
	gl_scale_2D_point(&lft,&top);
	gl_scale_2D_point(&rgt,&bot);

		// darken

	glColor4f(0.0f,0.0f,0.0f,0.5f);

    glBegin(GL_QUADS);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glVertex2i(rgt,bot);
	glVertex2i(lft,bot);
	glEnd();

		// outline

	glLineWidth(4.0f);
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glVertex2i(rgt,bot);
	glVertex2i(lft,bot);
	glEnd();

	glLineWidth(2.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glVertex2i(rgt,bot);
	glVertex2i(lft,bot);
	glEnd();

	glLineWidth(1.0f);
}

/* =======================================================

      GUI Draw
      
======================================================= */

void gui_draw(float background_alpha,bool cursor)
{
	gl_frame_start(NULL);

		// background

	gui_draw_background(background_alpha);
	gui_draw_dialog();

		// elements

	gl_2D_view();

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	element_draw(TRUE);
	
	if (cursor) cursor_draw();
	
		// end frame

	gl_frame_end();
}

void gui_draw_message(char *txt)
{
	int				x,y,lx,rx,ty,by,high;
	d3col			col;
	
	gl_frame_start(NULL);

		// background

	gui_draw_background(1.0f);

		// messages

	gl_2D_view();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	element_draw(FALSE);
	
		// get band size
		
	high=(gl_text_get_char_height(FALSE)+10)>>1;
	
	y=setup.screen.y_scale>>1;
	ty=y-high;
	by=y+high;
	
	lx=0;
	rx=setup.screen.x_scale;
	
	gl_scale_2D_point(&lx,&ty);
	gl_scale_2D_point(&rx,&by);
	
		// draw the blue band
		
	glColor4f(0.4f,0.1f,1.0f,1.0f);
	
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();
	
		// draw the text
		
	x=setup.screen.x_scale>>1;
	y=setup.screen.y_scale>>1;
	
	col.r=col.g=col.b=0.0f;
	
	gl_text_start(FALSE);
	gl_text_draw(x,y,txt,tx_center,TRUE,&col,1.0f);
	gl_text_end();
	
	gl_frame_end();
}

/* =======================================================

      GUI Clicking
      
======================================================= */

int gui_click(void)
{
	int				x,y;

	input_gui_get_mouse_position(&x,&y);
	
		// button down

	if (input_gui_get_mouse_left_button_down()) {
		element_click_down(x,y);
		return(-1);
	}

		// button up

	return(element_click_up(x,y));
}

/* =======================================================

      GUI Keyboard
      
======================================================= */

int gui_keyboard(void)
{
	char		ch;

		// key press?

	ch=input_gui_get_keyboard_key();
	if (ch==0x0) {
		gui_last_key=0x0;
		return(-1);
	}

		// same as last key?

	if (tolower(gui_last_key)==tolower(ch)) return(-1);
	gui_last_key=ch;

		// send to elements

	return(element_key(ch));
}
