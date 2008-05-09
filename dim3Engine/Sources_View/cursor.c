/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Cursor Routines

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

#include "video.h"
#include "inputs.h"

extern setup_type			setup;

bitmap_type					cursor_bitmap;

/* =======================================================

      Cursor Initialize and Shutdown
      
======================================================= */

void cursor_initialize(void)
{
	char		path[1024];
	
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","cursor","png");
	bitmap_open(&cursor_bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE);
}

void cursor_shutdown(void)
{
	bitmap_close(&cursor_bitmap);
}

/* =======================================================

      Crosshair Draw
      
======================================================= */

void cursor_draw(void)
{
	int				x,y,sz,lft,top,rgt,bot;
	
		// get cursor position
		
	input_gui_get_mouse_position(&x,&y);

	sz=(int)(((float)setup.screen.x_scale)*cursor_size_factor);
		
	lft=x;
	rgt=lft+sz;
	top=y;
	bot=top+sz;

		// draw mouse
		
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	gl_texture_simple_set(cursor_bitmap.gl_id,TRUE,1,1,1,1);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();
	
	gl_texture_simple_end();
}


