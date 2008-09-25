/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Texture Palette

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "window.h"
#include "tab.h"
#include "dialog.h"
#include "model.h"

extern int					cur_mesh,cur_pose,gl_view_x_sz,gl_view_y_sz;
extern bool					fileopen;
extern model_type			model;

extern AGLContext			ctx;
extern WindowRef			model_wind;

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(void)
{
	int						i,x,y,row_texture_count;
	Rect					wbox;
	texture_type			*texture;
	
 	if (!fileopen) return;

		// texture palette viewport
		
	GetWindowPortBounds(model_wind,&wbox);
	
	glViewport(wbox.left,0,gl_view_x_sz,texture_palette_height);
	
	glEnable(GL_SCISSOR_TEST);
	glScissor(wbox.left,0,gl_view_x_sz,texture_palette_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)wbox.left,(GLdouble)(wbox.left+gl_view_x_sz),(GLdouble)(wbox.bottom-info_palette_height),(GLdouble)(wbox.bottom-(info_palette_height+texture_palette_height)),-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
		// draw textures

	row_texture_count=max_model_texture/texture_palette_row_count;
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	glActiveTexture(GL_TEXTURE0);

	x=wbox.left;
	y=wbox.bottom-(info_palette_height+texture_palette_height);

	texture=model.textures;

	for (i=0;i!=max_model_texture;i++) {
		
			// the textures
			
		if (texture->bitmaps[0].gl_id!=-1) {
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,texture->bitmaps[0].gl_id);

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2i(x,y);
			glTexCoord2f(1,0);
			glVertex2i((x+texture_palette_texture_size),y);
			glTexCoord2f(1,1);
			glVertex2i((x+texture_palette_texture_size),(y+texture_palette_texture_size));
			glTexCoord2f(0,1);
			glVertex2i(x,(y+texture_palette_texture_size));
			glEnd();
			
			glDisable(GL_TEXTURE_2D);
		}
		else {
			glColor4f(0.8f,0.8f,0.8f,1.0f);
			
			glBegin(GL_QUADS);
			glVertex2i(x,y);
			glVertex2i((x+texture_palette_texture_size),y);
			glVertex2i((x+texture_palette_texture_size),(y+texture_palette_texture_size));
			glVertex2i(x,(y+texture_palette_texture_size));
			glEnd();
		}

			// the frame
			
        glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,(y+1));
		glVertex2i((x+texture_palette_texture_size),(y+1));
		glVertex2i((x+texture_palette_texture_size),((y+1)+texture_palette_texture_size));
		glVertex2i(x,((y+1)+texture_palette_texture_size));
		glEnd();
		
			// next position
			
		if (((i+1)%row_texture_count)==0) {
			y+=texture_palette_texture_size;
			x=wbox.left;
		}
		else {
			x+=texture_palette_texture_size;
		}
	
		texture++;
	}
	
	aglSwapBuffers(ctx);
}

/* =======================================================

      Texture Palette Clicking
      
======================================================= */

void texture_palette_click(Point pt,bool dblclick)
{
	int					x,y,nsel,row_texture_count;
	Rect				wbox;
	
    if (!fileopen) return;
 
		// drawing area
		
	GetWindowPortBounds(model_wind,&wbox);
	
	wbox.top+=tool_button_size+gl_view_y_sz;
	wbox.bottom=wbox.top+texture_palette_height;

		// find clicked texture
	
	row_texture_count=max_model_texture/texture_palette_row_count;
	
	x=(pt.h-wbox.left)/texture_palette_texture_size;
	y=(pt.v-wbox.top)/texture_palette_texture_size;
	nsel=(y*row_texture_count)+x;
	
	if ((nsel<0) || (nsel>(max_model_texture-1))) return;
		
		// handle click
	
	if (dblclick) {
		dialog_texture_setting_run(nsel);
	}
	else {
		model_set_sel_mask_material(&model,cur_mesh,nsel);
		hilite_vertex_rows();
	}
	
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    texture_palette_draw();
}

