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
#include "consoles.h"
#include "sounds.h"
#include "inputs.h"

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;

char						gui_last_key;
bool						gui_show_view;
bitmap_type					gui_background_bitmap,gui_screenshot_bitmap;
chooser_frame_type			gui_frame;

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern void map_restart_ambient(void);

/* =======================================================

      GUI Screenshots
      
======================================================= */

void gui_screenshot_initialize(void)
{
	gui_screenshot_bitmap.gl_id=-1;
}

void gui_screenshot_free(void)
{
	bitmap_close(&gui_screenshot_bitmap);
}

void gui_screenshot_load_blur_pixel(unsigned char *data,unsigned char *data2,int x,int y)
{
	int				cnt,r,g,b;
	unsigned char	*ptr;
	
	cnt=0;
	r=g=b=0;

	if (x>0) {
		ptr=data+((y*(gui_screenshot_wid*3))+((x-1)*3));
		r+=(int)*ptr++;
		g+=(int)*ptr++;
		b+=(int)*ptr++;
		cnt++;
	}
	if (x<(gui_screenshot_wid-1)) {
		ptr=data+((y*(gui_screenshot_wid*3))+((x+1)*3));
		r+=(int)*ptr++;
		g+=(int)*ptr++;
		b+=(int)*ptr++;
		cnt++;
	}
	if (y>0) {
		ptr=data+(((y-1)*(gui_screenshot_wid*3))+(x*3));
		r+=(int)*ptr++;
		g+=(int)*ptr++;
		b+=(int)*ptr++;
		cnt++;
	}
	if (y<(gui_screenshot_high-1)) {
		ptr=data+(((y+1)*(gui_screenshot_wid*3))+(x*3));
		r+=(int)*ptr++;
		g+=(int)*ptr++;
		b+=(int)*ptr++;
		cnt++;
	}

	ptr=data+((y*(gui_screenshot_wid*3))+(x*3));
	r+=(int)*ptr++;
	g+=(int)*ptr++;
	b+=(int)*ptr++;
	cnt++;
	
	ptr=data2+((y*(gui_screenshot_wid*3))+(x*3));
	*ptr++=(unsigned char)(r/cnt);
	*ptr++=(unsigned char)(g/cnt);
	*ptr++=(unsigned char)(b/cnt);
}

void gui_screenshot_load(void)
{
	int					n,x,y,x_skip,y_skip,dsz;
	unsigned char		*pixel_buffer,*data,*data2,*sptr,*dptr,*s2ptr,*d2ptr;
	
	pixel_buffer=(unsigned char*)malloc((setup.screen.x_sz*3)*setup.screen.y_sz);
	if (pixel_buffer==NULL) return;
	
	glReadPixels(0,0,setup.screen.x_sz,setup.screen.y_sz,GL_RGB,GL_UNSIGNED_BYTE,pixel_buffer);
	
		// reduce size
		
	x_skip=setup.screen.x_sz/gui_screenshot_wid;
	y_skip=setup.screen.y_sz/gui_screenshot_high;
	
		// flip and/or reduce the data
		
	dsz=((gui_screenshot_wid*3)*gui_screenshot_high);
	
	data=(unsigned char*)malloc(dsz);
	if (data==NULL) {
		free(pixel_buffer);
		return;
	}

	sptr=pixel_buffer;
	dptr=data+((gui_screenshot_high-1)*(gui_screenshot_wid*3));

	for (y=0;y!=gui_screenshot_high;y++) {
	
		s2ptr=sptr;
		d2ptr=dptr;
		
		for (x=0;x!=gui_screenshot_wid;x++) {
			*d2ptr++=*s2ptr++;
			*d2ptr++=*s2ptr++;
			*d2ptr++=*s2ptr++;
			
			s2ptr+=((x_skip-1)*3);
		}
			
		sptr+=((setup.screen.x_sz*3)*y_skip);
		dptr-=(gui_screenshot_wid*3);
	}

	free(pixel_buffer);
	
		// blur the data
		
	data2=(unsigned char*)malloc(dsz);
	if (data2==NULL) {
		free(data);
		return;
	}
	
	for (n=0;n!=gui_screenshot_blur_count;n++) {
	
		for (y=0;y!=gui_screenshot_high;y++) {
			for (x=0;x!=gui_screenshot_wid;x++) {
				gui_screenshot_load_blur_pixel(data,data2,x,y);
			}
		}
		
		memmove(data,data2,dsz);
	}

		// save screenshot

	bitmap_data(&gui_screenshot_bitmap,data,gui_screenshot_wid,gui_screenshot_high,FALSE,anisotropic_mode_none,mipmap_mode_trilinear,FALSE);
		
	free(data);
	free(data2);
}

/* =======================================================

      GUI Background
      
======================================================= */

void gui_background_load(char *background_path,char *bitmap_name)
{
	char		name[256],path[1024];

		// load the background bitmap
		// we pick the version with _wide on the
		// end if screen is in widescreen resolutions

	if (gl_is_screen_widescreen()) {
		sprintf(name,"%s_wide",bitmap_name);
		file_paths_data(&setup.file_path_setup,path,background_path,name,"png");
		if (bitmap_open(&gui_background_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE)) return;
	}
	
		// and fail-over to the original if not loaded

	file_paths_data(&setup.file_path_setup,path,background_path,bitmap_name,"png");
	bitmap_open(&gui_background_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
}

/* =======================================================

      GUI Initialize and Shutdown
      
======================================================= */

void gui_initialize(char *background_path,char *bitmap_name,bool show_view)
{
	int			x,y;
	
		// pause game
		
	game_time_pause_start();
	
		// stop playing ambients
		
	al_stop_all_looping_sources();
	
		// initialize cursor and elements
		
	cursor_initialize();
	element_initialize();
	
		// load up the proper background bitmap
		
	gui_show_view=show_view;
	
	if (!gui_show_view) {
		gui_background_bitmap.gl_id=-1;
		gui_background_load(background_path,bitmap_name);
	}
	
	gui_frame.on=FALSE;
	
		// start mouse in middle of screen
		
	x=hud.scale_x>>1;
	y=hud.scale_y>>1;
	input_gui_set_mouse(x,y);

		// no last keypress

	gui_last_key=0x0;
}

void gui_shutdown(void)
{
		// close background bitmap
		
	if (!gui_show_view) bitmap_close(&gui_background_bitmap);
	
		// release cursor and elements
		
	element_shutdown();
	cursor_shutdown();
	
		// clear all movement from gui	
			
	input_clear();
	
		// restart any map ambients
		
	if (server.map_open) map_restart_ambient();
	
		// unpause game
		
	game_time_pause_end();
}

/* =======================================================

      GUI Backgrounds
      
======================================================= */

void gui_draw_background(float alpha)
{
	unsigned long		gl_id;

		// no background at all?

	if (!gui_show_view) {
		gl_id=gui_background_bitmap.gl_id;
	}
	else {
		gl_id=gui_screenshot_bitmap.gl_id;
	}

	if (gl_id==-1) return;

		// 2D draw setup

	gl_2D_view_interface();

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

		// draw bitmap

	gl_texture_simple_start();
	gl_texture_simple_set(gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	view_draw_next_vertex_object_2D_texture_screen(hud.scale_x,hud.scale_y,0.0f,0.0f);
	gl_texture_simple_end();
}

/* =======================================================

      GUI Frames
      
======================================================= */

void gui_set_frame(chooser_frame_type *frame)
{
	memmove(&gui_frame,frame,sizeof(chooser_frame_type));
}

void gui_draw_dialog(void)
{
	int			lft,rgt,top,bot,high,head_top,y;
	bool		is_header;
	d3col		col;
	
	if (!gui_frame.on) return;
	
		// header?
		
	is_header=gui_frame.title[0]!=0x0;
	
		// setup draw

	gl_2D_view_interface();

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	lft=gui_frame.x;
	rgt=lft+gui_frame.wid;
	top=gui_frame.y;
	bot=top+gui_frame.high;

		// darken

	glColor4f(0.0f,0.0f,0.0f,0.5f);

    glBegin(GL_QUADS);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glVertex2i(rgt,bot);
	glVertex2i(lft,bot);
	glEnd();
	
		// header
		
	if (is_header) {
		high=gl_text_get_char_height(hud.font.text_size_small);
		head_top=top-(high+(high/2));
		
		y=(head_top+top)>>1;
			
		glBegin(GL_QUADS);
		
		glColor4f(0.75f,0.75f,0.75f,1.0f);
		glVertex2i(lft,head_top);
		glVertex2i(rgt,head_top);
		glColor4f(0.3f,0.3f,0.3f,1.0f);
		glVertex2i(rgt,y);
		glVertex2i(lft,y);
		
		glColor4f(0.3f,0.3f,0.3f,1.0f);
		glVertex2i(lft,y);
		glVertex2i(rgt,y);
		glColor4f(0.75f,0.75f,0.75f,1.0f);
		glVertex2i(rgt,top);
		glVertex2i(lft,top);
		
		glEnd();
		
		col.r=col.g=col.b=0.0f;
		
		gl_text_start(hud.font.text_size_medium);
		gl_text_draw(((lft+rgt)/2),y,gui_frame.title,tx_center,TRUE,&col,1.0f);
		gl_text_end();
	}
	
		// outline

	glLineWidth(4.0f);
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);
	
	if (is_header) view_draw_next_vertex_object_2D_line_quad(lft,rgt,head_top,top);
	
	glLineWidth(2.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);
	
	if (is_header) view_draw_next_vertex_object_2D_line_quad(lft,rgt,head_top,top);
	
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

	gl_2D_view_interface();

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

	gl_2D_view_interface();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	element_draw(FALSE);
	
		// get band size
		
	high=(gl_text_get_char_height(hud.font.text_size_large)+10)>>1;
	
	y=hud.scale_y>>1;
	ty=y-high;
	by=y+high;
	
	lx=0;
	rx=hud.scale_x;
	
		// draw the band
		
	glColor4f(hud.color.header.r,hud.color.header.g,hud.color.header.b,1.0f);
	
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();
	
		// draw the text
		
	x=hud.scale_x>>1;
	y=hud.scale_y>>1;
	
	col.r=col.g=col.b=0.0f;
	
	gl_text_start(hud.font.text_size_large);
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

	ch=input_gui_get_keyboard_key(TRUE);
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
