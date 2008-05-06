/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Console Routines

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

#include "consoles.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"

extern int						console_mode,console_count;
extern console_line_type		console_line[max_console_line];

extern view_type				view;
extern server_type				server;
extern setup_type				setup;
extern network_setup_type		net_setup;
extern render_info_type			render_info;

/* =======================================================

      Console View Offsets
      
======================================================= */

int console_y_offset(void)
{
	int			y;
	
	switch (console_mode) {
		
		case console_mode_close:
			return(0);
			
		case console_mode_fps:
			y=gl_text_get_char_height(TRUE);
			return(y);
			
		case console_mode_open:
			y=gl_text_get_char_height(TRUE);
			return((max_console_line*y)+2);
		
	}
	
	return(0);
}

/* =======================================================

      Console Input
      
======================================================= */
	
void console_input(void)
{
	if (input_action_get_state_single(nc_console)) console_next_mode();
}

/* =======================================================

      Draw Console
      
======================================================= */

void console_draw_fps(void)
{
	int					y,lx,rx,ty,by,y_add;
	char				fps_txt[64],lat_txt[32];
	d3col				col;
	
		// setup view
				
	gl_setup_viewport(0);
	gl_2D_view_interface();
	
		// setup fonts
	
    y_add=gl_text_get_char_height(TRUE);
    
		// background

	lx=0;
	rx=setup.screen.x_scale;
	ty=setup.screen.y_scale-y_add;
	by=setup.screen.y_scale;

	y=ty+(y_add+1);
		
	glColor3f(0.75f,0.75f,0.75f);
	
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();
	
		// fps & latency
		
	gl_text_start(TRUE);
	
	if (view.fps.total==0) {
		strcpy(fps_txt,"--.--");
	}
	else {
		if ((setup.lock_fps_refresh) && (view.fps.total>render_info.monitor_refresh_rate)) {
			sprintf(fps_txt,"%d.00",render_info.monitor_refresh_rate);
		}
		else {
			sprintf(fps_txt,"%.2f",view.fps.total);
		}
	}

	if (net_setup.client.joined) {
		if (net_setup.client.latency>=100) {
			strcat(fps_txt," [??]");
		}
		else {
			sprintf(lat_txt,"[%d]",net_setup.client.latency);
			strcat(fps_txt,lat_txt);
		}
	}

	col.r=col.g=col.b=0.0f;
	gl_text_draw((setup.screen.x_scale-5),y,fps_txt,tx_right,FALSE,&col,1.0f);

	gl_text_end();
}
	
void console_draw_open(void)
{
	int					i,y,lx,rx,ty,ty2,by,y_add;
	console_line_type	*cline;
	
		// setup view
				
	gl_setup_viewport(0);
	gl_2D_view_interface();
	
		// setup fonts
	
    y_add=gl_text_get_char_height(TRUE);
    
		// console background
		
	lx=0;
	rx=setup.screen.x_scale;
	ty=setup.screen.y_scale-((max_console_line*y_add)+2);
	ty2=ty+2;
	by=setup.screen.y_scale;
	
	y=ty2+1;
	
	glColor3f(0.75f,0.75f,0.75f);
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,ty2);
	glVertex2i(lx,ty2);
	glEnd();
	
	glColor3f(0.05f,0.05f,0.05f);
	glBegin(GL_QUADS);
	glVertex2i(lx,ty2);
	glVertex2i(rx,ty2);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();
	
		// console lines
		
	gl_text_start(TRUE);
	
	cline=console_line;
	
	for (i=0;i!=console_count;i++) {
		y+=y_add;
		gl_text_draw(5,y,cline->txt,tx_left,FALSE,&cline->color,1.0f);
		cline++;
	}

	gl_text_end();
}

void console_draw(void)
{
	switch (console_mode) {
	
		case console_mode_close:
			return;
			
		case console_mode_fps:
			console_draw_fps();
			return;
			
		case console_mode_open:
			console_draw_open();
			return;
			
	}
}
	
