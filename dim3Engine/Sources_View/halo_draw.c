/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Halo Drawing

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
#include "physics.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"

extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Add Halos
      
======================================================= */

void halo_draw_clear(void)
{
	view.count.halo_draw=0;
}

void halo_draw_add(int x,int z,int y,int obj_uid,model_draw_halo *mdl_halo)
{
	halo_draw_type		*halo_draw;
	
	if (mdl_halo->idx==-1) return;
	if (view.count.halo_draw>=max_light_spot) return;
	
	halo_draw=&view.halo_draws[view.count.halo_draw];
	view.count.halo_draw++;
	
	halo_draw->idx=mdl_halo->idx;

	halo_draw->obj_uid=obj_uid;
	
	halo_draw->pnt.x=x;
	halo_draw->pnt.z=z;
	halo_draw->pnt.y=y;
	
	halo_draw->min_dist=mdl_halo->min_dist;
	halo_draw->max_dist=mdl_halo->max_dist;
	
	halo_draw->min_pixel_sz=mdl_halo->min_size;
	halo_draw->max_pixel_sz=mdl_halo->max_size;
	
	halo_draw->min_alpha=mdl_halo->min_alpha;
	halo_draw->max_alpha=mdl_halo->max_alpha;
	
	halo_draw->no_clip_object=mdl_halo->no_clip_object;
	halo_draw->no_clip_self=mdl_halo->no_clip_self;
}

/* =======================================================

      Setup Halos
      
======================================================= */

void halo_draw_setup(void)
{
	int						i,x,y,z,x2,y2,z2,pixel_sz,dist,d;
	float					alpha;
	bool					hit;
	d3pnt					spt,ept,hpt;
	obj_type				*obj;
	halo_draw_type			*halo_draw;
	ray_trace_contact_type	contact;
	
	view.count.halo_draw_in_view=0;

	if (!setup.halo) return;
	
		// clear all halo spots
	
	halo_draw=view.halo_draws;
	
	for (i=0;i<view.count.halo_draw;i++) {
		halo_draw->in_view=TRUE;
		halo_draw++;
	}

		// remove halos behind z or off-screen
		
	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

	for (i=0;i<view.count.halo_draw;i++) {
		halo_draw=&view.halo_draws[i];
		
			// translate and rotate point
			
		x=halo_draw->pnt.x-view.camera.pos.x;
		y=halo_draw->pnt.y-view.camera.pos.y;
		z=halo_draw->pnt.z-view.camera.pos.z;
		
			// is it behind the z?

		x2=x;
		y2=y;
		z2=z;
		gl_rotate_point(&x2,&y2,&z2);
		if (z2<0) {
			halo_draw->in_view=FALSE;
			continue;
		}
				
			// project halos
			
		gl_project_point(&x,&y,&z);

		halo_draw->proj_pnt.x=x;
		halo_draw->proj_pnt.y=y;
		halo_draw->proj_pnt.z=z;
	}

		// ray trace halos to camera's eye level
		// to check for visibility

	ept.x=view.camera.pos.x;
	ept.y=view.camera.pos.y;
	ept.z=view.camera.pos.z;

	contact.proj_on=FALSE;
	contact.proj_ignore_uid=-1;
	contact.poly_ignore_non_wall=FALSE;

	for (i=0;i<view.count.halo_draw;i++) {
		halo_draw=&view.halo_draws[i];
		if (!halo_draw->in_view) continue;

		spt.x=halo_draw->pnt.x;
		spt.y=halo_draw->pnt.y;
		spt.z=halo_draw->pnt.z;

			// is ray greater than max distance?

		dist=distance_get(spt.x,spt.y,spt.z,ept.x,ept.y,ept.z);
		if (dist>halo_draw->max_dist) {
			halo_draw->in_view=FALSE;
			continue;
		}

			// ray trace for visibily

		contact.obj_on=!halo_draw->no_clip_object;

		if (halo_draw->no_clip_self) {
			contact.obj_ignore_uid=halo_draw->obj_uid;
		}
		else {
			contact.obj_ignore_uid=-1;
		}
		
			// push slighty towards player to clear
			// any ray trace errors from being too
			// close to projecting object

		if (halo_draw->obj_uid!=-1) {
			obj=object_find_uid(halo_draw->obj_uid);
			ray_push_to_end(&spt,&ept,obj->size.radius);
		}

		hit=ray_trace_map_by_point(&spt,&ept,&hpt,&contact);

		if (camera.mode==cv_fpp) {
			if (hit) {
				if (contact.obj_uid!=server.player_obj_uid) {
					halo_draw->in_view=FALSE;
					continue;
				}
			}
		}
		else {
			if (hit) {
				halo_draw->in_view=FALSE;
				continue;
			}
		}

			// get size

		if (dist>=halo_draw->max_dist) {
			halo_draw->pixel_sz=halo_draw->max_pixel_sz;
			halo_draw->alpha=halo_draw->max_alpha;
		}
		else {
			if (dist<=halo_draw->min_dist) {
				halo_draw->pixel_sz=halo_draw->min_pixel_sz;
				halo_draw->alpha=halo_draw->min_alpha;
			}
			else {
				dist-=halo_draw->min_dist;
				
				d=halo_draw->max_dist-halo_draw->min_dist;
				pixel_sz=halo_draw->max_pixel_sz-halo_draw->min_pixel_sz;
				alpha=halo_draw->max_alpha-halo_draw->min_alpha;
				
				halo_draw->pixel_sz=((pixel_sz*dist)/d)+halo_draw->min_pixel_sz;
				halo_draw->alpha=((alpha*(float)dist)/(float)d)+halo_draw->min_alpha;
			}
		}
		
			// obscure halos outside of view port
			
		if (((halo_draw->proj_pnt.x+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.y+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.x-halo_draw->pixel_sz)>=setup.screen.x_sz) || ((halo_draw->proj_pnt.y-halo_draw->pixel_sz)>=setup.screen.y_sz)) {
			halo_draw->in_view=FALSE;
			continue;
		}
		
			// add to view count

		view.count.halo_draw_in_view++;
	}
}

/* =======================================================

      Render Halos
      
======================================================= */

void halo_draw_render(void)
{
	int						i,x,y,psz;
	halo_draw_type			*halo_draw;
	
		// any halos to draw?
		
	if (view.count.halo_draw_in_view==0) return;
		
		// halos are post-render overlay effects
		
	gl_setup_viewport(console_y_offset());
	gl_2D_view_screen();
	
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);
	
	for (i=0;i<view.count.halo_draw;i++) {
		halo_draw=&view.halo_draws[i];
		if (!halo_draw->in_view) continue;
	
			// halo size
			
		x=halo_draw->proj_pnt.x;
		y=setup.screen.y_sz-halo_draw->proj_pnt.y;
		psz=halo_draw->pixel_sz>>1;

			// draw halo
			
		gl_texture_simple_set(view_images_get_gl_id(server.halos[halo_draw->idx].image_idx),TRUE,1,1,1,halo_draw->alpha);

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2i((x-psz),(y-psz));
		glTexCoord2f(1,0);
		glVertex2i((x+psz),(y-psz));
		glTexCoord2f(1,1);
		glVertex2i((x+psz),(y+psz));
		glTexCoord2f(0,1);
		glVertex2i((x-psz),(y+psz));
		glEnd();
	}
		
	gl_texture_simple_end();
}

