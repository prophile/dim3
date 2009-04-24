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
#include "sounds.h"

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern void map_restart_ambient(void);

extern int						console_mode,console_count;
extern console_line_type		console_line[max_console_line];

extern view_type				view;
extern server_type				server;
extern hud_type					hud;
extern setup_type				setup;
extern network_setup_type		net_setup;
extern render_info_type			render_info;

bool							console_start_trigger;

/* =======================================================

      Console Operations
      
======================================================= */

void console_open(void)
{
		// pause game and
		// stop any sounds
		
	game_time_pause_start();
	al_stop_all_looping_sources();

	server.state=gs_console;
}

void console_close(void)
{
		// restart game
			
	input_clear();
	map_restart_ambient();
	game_time_pause_end();
	
	server.state=gs_running;
}

/* =======================================================

      Story Triggers
      
======================================================= */

void console_trigger_clear(void)
{
	console_start_trigger=FALSE;
}

void console_trigger_check(void)
{
	if (console_start_trigger) console_open();
}	

void console_trigger_set(void)
{
	if (server.state!=gs_running) return;

	console_start_trigger=TRUE;
}

/* =======================================================

      Console Input
      
======================================================= */
	
void console_input(void)
{
	if (input_action_get_state_single(nc_console)) console_trigger_set();
}

/* =======================================================

      Draw Console
      
======================================================= */

void console_draw_header(void)
{
 	int					y,lx,rx,ty,by,y_add;
	char				fps_txt[64],lat_txt[32];
	d3col				col;

	y_add=gl_text_get_char_height(hud.font.text_size_small);
    
		// background

	lx=0;
	rx=hud.scale_x;
	ty=0;
	by=y_add;

	y=ty+(y_add+1);

	col.r=col.g=col.b=0.75f;
	view_draw_next_vertex_object_2D_color_quad(lx,ty,&col,rx,ty,&col,rx,by,&col,lx,by,&col,1.0f);
	
	gl_text_start(hud.font.text_size_small);
	col.r=col.g=col.b=0.0f;
	
		// version
		
	gl_text_draw(5,y,dim3_version,tx_left,FALSE,&col,1.0f);

		// project name

	gl_text_draw((hud.scale_x>>1),y,net_setup.host.proj_name,tx_center,FALSE,&col,1.0f);
	
		// fps & latency
	
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

	gl_text_draw((hud.scale_x-5),y,fps_txt,tx_right,FALSE,&col,1.0f);

	gl_text_end();
}

void console_draw_lines(void)
{
	int					n,y,y_add;
	d3col				col;
	console_line_type	*cline;

		// setup fonts
	
    y_add=gl_text_get_char_height(hud.font.text_size_small);

		// console lines

	y=y_add+2;
		
	col.r=col.g=col.b=1.0f;
	gl_text_start(hud.font.text_size_small);
	
	cline=console_line;
	
	for (n=0;n!=console_count;n++) {
		y+=y_add;
		gl_text_draw(5,y,cline->txt,tx_left,FALSE,&cline->color,1.0f);
		cline++;
	}

	gl_text_end();
}

void console_draw(void)
{
		// start frame

	gl_frame_start(NULL);
	gl_2D_view_interface();

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

		// draw header and text

	console_draw_header();
	console_draw_lines();

		// end frame

	gl_frame_end();
}
	
/* =======================================================

      Run Console
      
======================================================= */

void console_run(void)
{
	console_draw();

	input_event_pump();
	if (input_action_get_state_single(nc_console)) console_close();
}


