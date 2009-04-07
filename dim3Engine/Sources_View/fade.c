/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Fading

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

#include "consoles.h"
#include "video.h"
#include "interfaces.h"

int							fade_screen_tick;

extern hud_type				hud;
extern setup_type			setup;

/* =======================================================

      Map Screen Fade
      
======================================================= */

void fade_screen_start(int tick)
{
	if (hud.fade.map_msec<=0) {
		fade_screen_tick=-1;
		return;
	}

	fade_screen_tick=tick;
}

void fade_screen_cancel(void)
{
	fade_screen_tick=-1;
}

void fade_screen_draw(int tick)
{
	float			alpha;

	if (fade_screen_tick==-1) return;

	tick-=fade_screen_tick;
	if (tick>hud.fade.map_msec) {
		fade_screen_tick=-1;
		return;
	}

		// calculate fade

	alpha=1.0f-(float)tick/(float)hud.fade.map_msec;
	glColor4f(0.0f,0.0f,0.0f,alpha);
	
		// draw
		
	gl_2D_view_screen();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
		
    glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(setup.screen.x_sz,0);
	glVertex2i(setup.screen.x_sz,setup.screen.y_sz);
	glVertex2i(0,setup.screen.y_sz);
    glEnd();
}

/* =======================================================

      Objects Fade
      
======================================================= */

void fade_object_draw(int tick,obj_type *obj)
{
	int					i,x,y,sz,alpha_sz,r;
	float				fsz;
	obj_fs_fade			*fade;
	GLUquadricObj		*quadric;
	
	fade=&obj->fs_effect.fade;
	
		// fades on?
		
	if (!fade->on) return;
	
		// get fade size
		
	tick-=fade->start_tick;
	if (tick>fade->life_tick) {
		fsz=fade->end_size;
	}
	else {
		fsz=fade->start_size+((fade->end_size-fade->start_size)*((float)tick/(float)fade->life_tick));
	}
	
	sz=(int)(((float)setup.screen.x_sz)*fsz);
	
		// center point

	x=fade->center_x;
	y=fade->center_y;

		// draw fade

	gl_2D_view_screen();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glTranslatef((float)x,(float)y,0.0f);
	
		// faded disk
		
	alpha_sz=80;
	if (sz<80) alpha_sz=sz;
	
	for (i=0;i<alpha_sz;i+=2) {
		glColor4f(0.0f,0.0f,0.0f,(((float)i)*0.025f));
	
		r=(sz+i);
		quadric=gluNewQuadric();
		gluDisk(quadric,r,(r+2),24,24);
		gluDeleteQuadric(quadric);
	}
	
		// rest of black disk
		
	sz+=alpha_sz;
	if (sz<setup.screen.x_sz) {
		glColor4f(0.0f,0.0f,0.0f,1.0f);

		quadric=gluNewQuadric();
		gluDisk(quadric,sz,setup.screen.x_sz,24,24);
		gluDeleteQuadric(quadric);
	}
}

