/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Zoom Setup/Drawing

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
#include "weapons.h"
#include "models.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Zoom Setup
      
======================================================= */

void zoom_setup(int tick,obj_type *obj,weapon_type *weap)
{
	int					crosshair_type,x,y,z,tx,ty,tz,old_x,old_y,obj_uid;
	obj_zoom_draw		*zoom_draw;
	
	zoom_draw=&obj->zoom_draw;
	
		// get last position is zoom was on
		
	old_x=old_y=-1;
	
	if (zoom_draw->on) {
		old_x=zoom_draw->x;
		old_y=zoom_draw->y;
	}

		// zoom on?
		
	zoom_draw->on=FALSE;
	
	if (obj->hide_all_weapons) return;
	if ((!weap->zoom.on) || (!weap->zoom.active)) return;
	
		// get zoom screen location from crosshair

	crosshair_type=weap->crosshair.type;
		
	if (crosshair_type==ct_center) {
		tx=setup.screen.x_sz>>1;
		ty=setup.screen.y_sz>>1;
		zoom_draw->on=TRUE;
	}
	else {
		
			// get crosshair location
		
		if (!crosshair_get_location(tick,obj,weap,&x,&z,&y,&obj_uid)) return;

		zoom_draw->on=TRUE;
		
			// get the zoom place
			
		gl_setup_viewport(0);
		gl_3D_view(&view.camera);
		gl_3D_rotate(&view.camera.ang);
		gl_setup_project();
		
		tx=x-view.camera.pos.x;
		ty=y-view.camera.pos.y;
		tz=z-view.camera.pos.z;

		gl_project_point(&tx,&ty,&tz);
	}
	
		// smooth out
		
	ty=setup.screen.y_sz-ty;				// 2D view has Y flipped
	
	if (old_x!=-1) {
		tx=(tx+old_x)>>1;
		ty=(ty+old_y)>>1;
	}
	
		// setup
		
	zoom_draw->x=tx;
	zoom_draw->y=ty;
}

/* =======================================================

      Zoom Drawing
      
======================================================= */

void zoom_draw(obj_type *obj,weapon_type *weap)
{
	int					x,y,sz,lft,rgt,top,bot,gl_id;
	
		// is weapon not in zoom?
		
	if ((!weap->zoom.on) || (!weap->zoom.active)) return;
	
		// is there a mask?
		
	if (weap->zoom.mask_idx==-1) return;
		
		// position
		
	x=obj->zoom_draw.x;
	y=obj->zoom_draw.y;
	
	sz=setup.screen.y_sz>>1;

	lft=x-sz;
	rgt=x+sz;
	top=y-(sz+1);
	bot=y+(sz+1);
	
		// scope bitmap
		
	gl_id=server.crosshairs[weap->zoom.mask_idx].bitmap.gl_id;
	
		// setup drawing
		
	gl_setup_viewport(console_y_offset());
	gl_2D_view();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
		// draw borders
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	if (lft>0) {
		glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i((lft+1),0);
		glVertex2i((lft+1),setup.screen.y_sz);
		glVertex2i(0,setup.screen.y_sz);
		glEnd();
	}
	
	if (rgt<setup.screen.x_sz) {
		glBegin(GL_QUADS);
		glVertex2i((rgt-1),0);
		glVertex2i(setup.screen.x_sz,0);
		glVertex2i(setup.screen.x_sz,setup.screen.y_sz);
		glVertex2i((rgt-1),setup.screen.y_sz);
		glEnd();
	}
	
	if (top>0) {
		glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i(setup.screen.x_sz,0);
		glVertex2i(setup.screen.x_sz,(top+1));
		glVertex2i(0,(top+1));
		glEnd();
	}
	
	if (bot<setup.screen.y_sz) {
		glBegin(GL_QUADS);
		glVertex2i(0,(bot-1));
		glVertex2i(setup.screen.x_sz,(bot-1));
		glVertex2i(setup.screen.x_sz,setup.screen.y_sz);
		glVertex2i(0,setup.screen.y_sz);
		glEnd();
	}

		// draw scope
		
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	gl_texture_simple_set(gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
	
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

