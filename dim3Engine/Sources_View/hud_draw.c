/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Routines

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

#include "objects.h"
#include "remotes.h"
#include "consoles.h"
#include "interfaces.h"
#include "effects.h"
#include "video.h"
#include "sounds.h"

extern server_type			server;
extern view_type			view;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

/* =======================================================

      Run HUD Item Fades
      
======================================================= */

bool hud_item_fade_run(int tick,hud_item_fade_type *fade,float *alpha)
{
	int			fade_tick;
	float		achg;

		// if fade not on then do nothing
	
	if (!fade->on) return(FALSE);
	
		// fade still on?
		
	fade_tick=tick-fade->start_tick;
	
	if (fade_tick>=(fade->fade_in_tick+fade->life_tick+fade->fade_out_tick)) {
		fade->on=FALSE;
		return(TRUE);
	}
	
	if (fade_tick>(fade->fade_in_tick+fade->life_tick)) {
		achg=1.0f-((float)(fade_tick-(fade->fade_in_tick+fade->life_tick))/(float)fade->fade_out_tick);
	}
	else {
		if (fade_tick>fade->fade_in_tick) {
			achg=1.0f;
		}
		else {
			achg=(float)fade_tick/(float)fade->fade_in_tick;
		}
	}
	
	*alpha=(*alpha)*achg;

	return(FALSE);
}

/* =======================================================

      Draw HUD Bitmaps
      
======================================================= */

void hud_bitmaps_draw(int tick)
{
	int							n,r,
								px[4],py[4],sx,sy,rx,ry,
								wid,high,repeat_count;
	float						gx,gy,gx2,gy2,g_size,alpha,cur_alpha;
	GLuint						cur_gl_id;
	hud_bitmap_type				*bitmap;
	bitmap_type					*bitmap_data;
		
	cur_gl_id=-1;
	cur_alpha=1.0f;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	gl_texture_simple_start();
	
		// draw bitmaps
		
	for (n=0;n!=hud.count.bitmap;n++) {
		bitmap=&hud.bitmaps[n];
		if (!bitmap->show) continue;

			// flashing

		if (bitmap->flash) {
			if (((tick>>9)&0x1)==0x0) continue;		// half-second flash
		}
		
			// fading?
			
		alpha=bitmap->alpha;
		if (hud_item_fade_run(tick,&bitmap->fade,&alpha)) {
			bitmap->show=FALSE;			// a fade has turned off bitmap
			continue;
		}

            // get bitmap and position
			
		bitmap_data=view_images_get_bitmap(bitmap->image_idx);
            
		sx=bitmap->x;
		sy=bitmap->y;
		
		wid=bitmap->x_size;
		if (wid==-1) {
			wid=bitmap_data->wid;
		}
		high=bitmap->y_size;
		if (high==-1) {
			high=bitmap_data->high;
		}
		
			// setup images
			
		effect_image_animate_get_uv((tick-bitmap->show_tick),&bitmap->animate,&gx,&gy,&g_size);

            // flips
           
		if (!bitmap->flip_horz) {
			gx2=gx+g_size;
		}
		else {
			gx2=gx;
			gx+=g_size;
		}

		if (!bitmap->flip_vert) {
			gy2=gy+g_size;
		}
		else {
			gy2=gy;
			gy+=g_size;
		}

			// need to change texture?

		if ((cur_gl_id!=bitmap_data->gl_id) || (cur_alpha!=alpha)) {
			cur_gl_id=bitmap_data->gl_id;
			cur_alpha=alpha;
			gl_texture_simple_set(cur_gl_id,TRUE,1,1,1,cur_alpha);
		}
		
			// find the repeat count
		
		if (bitmap->repeat.on) {
			repeat_count=bitmap->repeat.count;
			if (repeat_count<1) continue;
		}
		else {
			repeat_count=1;
		}

			// scale and rotate bitmap
			
		px[0]=px[3]=sx;
		px[1]=px[2]=sx+wid;

		py[0]=py[1]=sy;
		py[2]=py[3]=sy+high;

		if (bitmap->rot!=0.0f) rotate_2D_polygon(4,px,py,(sx+(wid>>1)),(sy+(high>>1)),bitmap->rot);
		
		gl_scale_2D_point(&px[0],&py[0]);
		gl_scale_2D_point(&px[1],&py[1]);
		gl_scale_2D_point(&px[2],&py[2]);
		gl_scale_2D_point(&px[3],&py[3]);

		rx=bitmap->repeat.x_add;
		ry=bitmap->repeat.y_add;

		gl_scale_2D_point(&rx,&ry);

			// draw bitmaps

		glBegin(GL_QUADS);

		for (r=0;r!=repeat_count;r++) {

				// quad
		
			glTexCoord2f(gx,gy);
			glVertex2i(px[0],py[0]);
			glTexCoord2f(gx2,gy);
			glVertex2i(px[1],py[1]);
			glTexCoord2f(gx2,gy2);
			glVertex2i(px[2],py[2]);
			glTexCoord2f(gx,gy2);
			glVertex2i(px[3],py[3]);
			
			// move to next repeat
				
			px[0]+=rx;
			px[1]+=rx;
			px[2]+=rx;
			px[3]+=rx;
			py[0]+=ry;
			py[1]+=ry;
			py[2]+=ry;
			py[3]+=ry;
		}
			
		glEnd();
	}
	
	gl_texture_simple_end();
}

/* =======================================================

      Draw HUD Text
      
======================================================= */

void hud_texts_fps(char *data)
{
	if (view.fps.total==0) {
		strcpy(data,"--.--");
	}
	else {
		if ((setup.lock_fps_refresh) && (view.fps.total>render_info.monitor_refresh_rate)) {
			sprintf(data,"%d.00",render_info.monitor_refresh_rate);
		}
		else {
			sprintf(data,"%.2f",view.fps.total);
		}
	}
}

void hud_texts_draw_return(int x,int y,int ysz,char *data,int just,d3col *col,float alpha)
{
	char			*c,*c2;
	
	c=data;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		gl_text_draw(x,y,c,just,FALSE,col,alpha);
		
		if (c2==NULL) return;
		
		*c2='{';
		
		c=c2+3;
		y+=ysz;
	}
}

void hud_texts_draw(int tick)
{
	int				n,high;
	float			alpha;
	bool			has_small,has_large;
	hud_text_type  *text;

		// check for sizes

	has_small=has_large=FALSE;

	text=hud.texts;

	for (n=0;n!=hud.count.text;n++) {

		if (text->show) {
			if (text->large) {
				has_large=TRUE;
			}
			else {
				has_small=TRUE;
			}
		}

		text++;
	}

	
		// small texts

	if (has_small) {

		high=gl_text_get_char_height(TRUE);
			
		gl_text_start(TRUE);
		
		text=hud.texts;
		
		for (n=0;n!=hud.count.text;n++) {

			if ((text->show) && (!text->large)) {

					// fading?
			
				alpha=text->alpha;
				if (hud_item_fade_run(tick,&text->fade,&alpha)) {
					text->show=FALSE;			// a fade has turned off bitmap
					continue;
				}

					// draw text

				if (text->fps) hud_texts_fps(text->data);

				if (text->has_return) {
					hud_texts_draw_return(text->x,text->y,high,text->data,text->just,&text->color,alpha);
				}
				else {
					gl_text_draw(text->x,text->y,text->data,text->just,FALSE,&text->color,alpha);
				}
			}

			text++;
		}
		
		gl_text_end();
	}
	
		// large texts

	if (has_large) {

		high=gl_text_get_char_height(FALSE);
			
		gl_text_start(FALSE);
		
		text=hud.texts;
		
		for (n=0;n!=hud.count.text;n++) {

			if ((text->show) && (text->large)) {

					// fading?
			
				alpha=text->alpha;
				if (hud_item_fade_run(tick,&text->fade,&alpha)) {
					text->show=FALSE;			// a fade has turned off bitmap
					continue;
				}

					// draw text

				if (text->fps) hud_texts_fps(text->data);

				if (text->has_return) {
					hud_texts_draw_return(text->x,text->y,high,text->data,text->just,&text->color,alpha);
				}
				else {
					gl_text_draw(text->x,text->y,text->data,text->just,FALSE,&text->color,alpha);
				}
			}

			text++;
		}
		
		gl_text_end();
	}
}

/* =======================================================

      Draw HUD Bars
      
======================================================= */

void hud_bars_draw(void)
{
	int					n,lx,rx,ty,by,wid,high;
	d3col				fill_end_color;
	hud_bar_type		*bar;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
		// draw bars
		
	for (n=0;n!=hud.count.bar;n++) {
		bar=&hud.bars[n];
		if (!bar->show) continue;
		
			// find size
			
		wid=bar->x_size;
		high=bar->y_size;
		
		if (!bar->vert) {
			wid=(int)(((float)wid)*bar->value);
		}
		else {
			high=(int)(((float)high)*bar->value);
		}
		
			// find the color
			
		memmove(&fill_end_color,&bar->fill_end_color,sizeof(d3col));
		fill_end_color.r+=((bar->fill_end_color.r-bar->fill_start_color.r)*bar->value);
		fill_end_color.g+=((bar->fill_end_color.g-bar->fill_start_color.g)*bar->value);
		fill_end_color.b+=((bar->fill_end_color.b-bar->fill_start_color.b)*bar->value);
		
			// draw bar
			
		lx=bar->x;
		rx=lx+wid;
		by=bar->y+bar->y_size;
		ty=by-high;
		
		gl_scale_2D_point(&lx,&ty);
		gl_scale_2D_point(&rx,&by);
			
		glBegin(GL_QUADS);
		
		if (!bar->vert) {
			glColor4f(bar->fill_start_color.r,bar->fill_start_color.g,bar->fill_start_color.b,bar->fill_alpha);
			glVertex2i(lx,by);
			glVertex2i(lx,ty);
			glColor4f(fill_end_color.r,fill_end_color.g,fill_end_color.b,bar->fill_alpha);
			glVertex2i(rx,ty);
			glVertex2i(rx,by);
		}
		else {
			glColor4f(fill_end_color.r,fill_end_color.g,fill_end_color.b,bar->fill_alpha);
			glVertex2i(lx,ty);
			glVertex2i(rx,ty);
			glColor4f(bar->fill_start_color.r,bar->fill_start_color.g,bar->fill_start_color.b,bar->fill_alpha);
			glVertex2i(rx,by);
			glVertex2i(lx,by);
		}
		
		glEnd();
		
			// draw outline
			
		if (bar->outline) {
		
			lx=bar->x;
			rx=lx+bar->x_size;
			ty=bar->y;
			by=ty+bar->y_size;
			
			gl_scale_2D_point(&lx,&ty);
			gl_scale_2D_point(&rx,&by);
			
			glColor4f(bar->outline_color.r,bar->outline_color.g,bar->outline_color.b,bar->outline_alpha);
			
			glBegin(GL_LINE_LOOP);
			glVertex2i(lx,by);
			glVertex2i(lx,ty);
			glVertex2i(rx,ty);
			glVertex2i(rx,by);
			glEnd();
		}
		
	}
}

/* =======================================================

      HUD Sounds
      
======================================================= */

void hud_click(void)
{
	int			buffer_idx;

	if (hud.click_sound[0]==0x0) return;
	
	buffer_idx=al_find_buffer(hud.click_sound);
	if (buffer_idx!=-1) al_play_source(buffer_idx,0,0,0,1,FALSE,FALSE,TRUE,FALSE);
}

/* =======================================================

      Draw HUD
      
======================================================= */

void hud_draw(int tick)
{
		// set up view
		
	gl_setup_viewport(console_y_offset());
	gl_2D_view();
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);
	
		// draw the bitmaps, bars, and text
		
	hud_bitmaps_draw(tick);
	hud_bars_draw();
	hud_texts_draw(tick);
	
		// reset any color changes
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

