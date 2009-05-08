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
	int					tx,ty,tz,old_x,old_y,obj_uid;
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
	if ((!weap->zoom.on) || (weap->zoom.mode==zoom_mode_off)) return;
	
		// iron sites effect masking
				
	if (tick<(zoom_draw->start_tick+weap->zoom.tick)) return;
	
		// get zoom screen location from crosshair

	if (!crosshair_get_location(tick,obj,weap,&tx,&tz,&ty,&obj_uid)) return;

	zoom_draw->on=TRUE;
		
		// get the zoom place
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

	gl_project_point(&tx,&ty,&tz);
	
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
	int				x,y,sz,lft,rgt,top,bot,
					cnt,gl_id;
	float			*vertex_ptr,*uv_ptr;
	
		// is weapon not in zoom?
		
	if ((!weap->zoom.on) || (weap->zoom.mode==zoom_mode_off)) return;
	
		// is there a mask?
		
	if (!obj->zoom_draw.on) return;
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
		
	gl_id=view_images_get_gl_id(server.crosshairs[weap->zoom.mask_idx].image_idx);
	
		// setup drawing
		
	gl_2D_view_screen();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

		// construct VBO

	vertex_ptr=view_bind_map_next_vertex_object(((16*4)*(2+2)));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+((16*4)*2);
	
		// border vertexes

	cnt=0;
		
	if (lft>0) {
		*vertex_ptr++=0.0f;
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)(lft+1);
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)(lft+1);
		*vertex_ptr++=(float)setup.screen.y_sz;
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)setup.screen.y_sz;

		uv_ptr+=8;
		cnt+=4;
	}
	
	if (rgt<setup.screen.x_sz) {
		*vertex_ptr++=(float)(rgt-1);
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)setup.screen.x_sz;
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)setup.screen.x_sz;
		*vertex_ptr++=(float)setup.screen.y_sz;
		*vertex_ptr++=(float)(rgt-1);
		*vertex_ptr++=(float)setup.screen.y_sz;

		uv_ptr+=8;
		cnt+=4;
	}
	
	if (top>0) {
		*vertex_ptr++=0.0f;
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)setup.screen.x_sz;
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)setup.screen.x_sz;
		*vertex_ptr++=(float)(top+1);
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)(top+1);

		uv_ptr+=8;
		cnt+=4;
	}
	
	if (bot<setup.screen.y_sz) {
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)(bot-1);
		*vertex_ptr++=(float)setup.screen.x_sz;
		*vertex_ptr++=(float)(bot-1);
		*vertex_ptr++=(float)setup.screen.x_sz;
		*vertex_ptr++=(float)setup.screen.y_sz;
		*vertex_ptr++=0.0f;
		*vertex_ptr++=(float)setup.screen.y_sz;

		uv_ptr+=8;
		cnt+=4;
	}

		// zoom bitmap vertexes

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)top;

	*uv_ptr++=0.0f;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)top;

	*uv_ptr++=1.0f;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)rgt;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=1.0f;
	*uv_ptr++=1.0f;

	*vertex_ptr++=(float)lft;
	*vertex_ptr++=(float)bot;

	*uv_ptr++=0.0f;
	*uv_ptr++=1.0f;

  	view_unmap_current_vertex_object();

		// draw border and zoom
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

		// borders

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_QUADS,0,cnt);

		// zoom

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((16*4)*2)*sizeof(float)));

	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	gl_texture_simple_set(gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);

	glDrawArrays(GL_QUADS,cnt,4);

	gl_texture_simple_end();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

