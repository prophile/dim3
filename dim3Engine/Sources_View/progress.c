/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Progress Routines

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

#include "video.h"

extern setup_type			setup;
extern hud_type				hud;

float						progress_current;
char						progress_str[256];
bitmap_type					progress_bitmap;

/* =======================================================

      Setup Progress
      
======================================================= */

void progress_initialize(char *action)
{
	char			path[1024];
	
		// progress text
		
	strcpy(progress_str,action);
	
		// progress background
	
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/Backgrounds","load","png");
	bitmap_open(&progress_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);
	
		// current progress
		
	progress_current=-1;
}

void progress_shutdown(void)
{
	bitmap_close(&progress_bitmap);
}

/* =======================================================

      Draw Progress
      
======================================================= */

void progress_draw(float percentage)
{
	int					lft,rgt,top,bot,mid,rgt2,x,y,sz;

		// any change?
		
	if (progress_current==percentage) return;
	
	progress_current=percentage;
	
		// position and sizes of progress pieces
		
	sz=gl_text_get_char_height(TRUE);
	
		// start the frame
	
	gl_frame_start(NULL);
	
		// setup draw
		
	gl_2D_view_interface();
	
		// draw background
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	gl_texture_simple_set(progress_bitmap.gl_id,TRUE,1,1,1,1);
		
    glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(0,0);
	glTexCoord2f(1,0);
	glVertex2i(hud.scale_x,0);
	glTexCoord2f(1,1);
	glVertex2i(hud.scale_x,hud.scale_y);
	glTexCoord2f(0,1);
	glVertex2i(0,hud.scale_y);
    glEnd();
    
	gl_texture_simple_end();
	
		// draw the progress background
		
	lft=hud.progress.lx;
	rgt=hud.progress.rx;
	top=hud.progress.ty;
	bot=hud.progress.by;

	mid=(top+bot)>>1;
	
    glBegin(GL_QUADS);
	glColor4f(hud.progress.base_color_start.r,hud.progress.base_color_start.g,hud.progress.base_color_start.b,1.0f);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glColor4f(hud.progress.base_color_end.r,hud.progress.base_color_end.g,hud.progress.base_color_end.b,1.0f);
	glVertex2i(rgt,mid);
	glVertex2i(lft,mid);
    glEnd();
	
    glBegin(GL_QUADS);
	glColor4f(hud.progress.base_color_end.r,hud.progress.base_color_end.g,hud.progress.base_color_end.b,1.0f);
	glVertex2i(lft,mid);
	glVertex2i(rgt,mid);
	glColor4f(hud.progress.base_color_start.r,hud.progress.base_color_start.g,hud.progress.base_color_start.b,1.0f);
	glVertex2i(rgt,bot);
	glVertex2i(lft,bot);
    glEnd();
	
		// draw the progress foreground
	
	rgt2=lft+(int)((float)(rgt-lft)*(percentage/100));
	
    glBegin(GL_QUADS);
	glColor4f(hud.progress.hilite_color_start.r,hud.progress.hilite_color_start.g,hud.progress.hilite_color_start.b,1.0f);
	glVertex2i(lft,top);
	glVertex2i(rgt2,top);
	glColor4f(hud.progress.hilite_color_end.r,hud.progress.hilite_color_end.g,hud.progress.hilite_color_end.b,1.0f);
	glVertex2i(rgt2,mid);
	glVertex2i(lft,mid);
    glEnd();
	
    glBegin(GL_QUADS);
	glColor4f(hud.progress.hilite_color_end.r,hud.progress.hilite_color_end.g,hud.progress.hilite_color_end.b,1.0f);
	glVertex2i(lft,mid);
	glVertex2i(rgt2,mid);
	glColor4f(hud.progress.hilite_color_start.r,hud.progress.hilite_color_start.g,hud.progress.hilite_color_start.b,1.0f);
	glVertex2i(rgt2,bot);
	glVertex2i(lft,bot);
    glEnd();
	
		// progress text

	gl_text_start(TRUE);
	
	x=(hud.progress.lx+hud.progress.rx)>>1;
	y=(hud.progress.ty+hud.progress.by)>>1;
	
	gl_text_draw(x,y,progress_str,tx_center,TRUE,&hud.progress.text_color,1.0f);
	
	gl_text_end();

	gl_frame_end();
}

