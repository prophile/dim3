/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Remote Player Status Icons and Names

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

#include "models.h"
#include "remotes.h"
#include "physics.h"
#include "consoles.h"
#include "video.h"

extern map_type				map;
extern view_type			view;
extern camera_type			camera;
extern hud_type				hud;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							remote_slow_image_idx,remote_talk_image_idx;

/* =======================================================

      Draw Remote Status Icons
      
======================================================= */

void remote_draw_icon(obj_type *obj,unsigned long gl_id)
{
	int				x,y,z,x_sz,y_sz,z_sz;
	
		// get the position and rotation
		
	x=obj->pnt.x-view.camera.pnt.x;
	y=((obj->pnt.y-obj->size.y)-map_enlarge)-view.camera.pnt.y;
	z=obj->pnt.z-view.camera.pnt.z;
	
	x_sz=map_enlarge<<1;
	y_sz=map_enlarge<<2;
	z_sz=0;
	rotate_2D_point_center(&x_sz,&z_sz,view.camera.ang.y);

		// draw the bitmap
		
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	gl_texture_simple_set(gl_id,TRUE,1,1,1,1);

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3i((x-x_sz),(y-y_sz),-(z-z_sz));
	glTexCoord2f(1,0);
	glVertex3i((x+x_sz),(y-y_sz),-(z+z_sz));
	glTexCoord2f(1,1);
	glVertex3i((x+x_sz),y,-(z+z_sz));
	glTexCoord2f(0,1);
	glVertex3i((x-x_sz),y,-(z-z_sz));
	glEnd();
		
	gl_texture_simple_end();
}

void remote_draw_status(obj_type *obj)
{
		// slow status
		
	if (remote_timed_out(obj)) {
		remote_draw_icon(obj,view_images_get_gl_id(remote_slow_image_idx));
		return;
	}
	
		// talking status
		
	if (obj->remote.talking) {
		remote_draw_icon(obj,view_images_get_gl_id(remote_talk_image_idx));
		return;
	}
}

/* =======================================================

      Remote Names
      
======================================================= */

void remote_draw_names_setup(void)
{
	int						n,x,y,z,dist;
	bool					hit,has_tag;
	d3pnt					spt,ept,hpt;
	obj_type				*obj;
	model_type				*mdl;
	ray_trace_contact_type	contact;
	
	if (!net_setup.client.joined) return;
	if (!setup.network.show_names) return;
	
		// clear all name draws
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		obj->draw.remote_name.on=FALSE;
		obj++;
	}

		// remove halos behind z or off-screen
		// ignore console as it won't matter for projection
		
	gl_setup_viewport(0);
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
	
	for (n=0;n!=server.count.obj;n++) {
		obj=&server.objs[n];
		if (!obj->remote.on) continue;
		if (!obj->draw.in_view) continue;
		
			// get name point
		
		has_tag=FALSE;

		mdl=NULL;
		if ((obj->draw.uid!=-1) && (obj->draw.on)) mdl=model_find_uid(obj->draw.uid);

		if (mdl!=NULL) {
			x=obj->pnt.x;
			y=obj->pnt.y;
			z=obj->pnt.z;
			has_tag=model_get_name_position(mdl,&obj->draw.setup,&x,&y,&z);
		}

		if (!has_tag) {
			x=obj->pnt.x;
			y=(obj->pnt.y-obj->size.y)-map_enlarge;
			z=obj->pnt.z;
		}
		
			// translate and rotate point
			
		dist=distance_get(x,y,z,view.camera.pnt.x,view.camera.pnt.y,view.camera.pnt.z);
		if (dist>=remote_name_max_distance) continue;

		obj->draw.remote_name.pnt.x=x;
		obj->draw.remote_name.pnt.y=y;
		obj->draw.remote_name.pnt.z=z;
			
		x-=view.camera.pnt.x;
		y-=view.camera.pnt.y;
		z=view.camera.pnt.z-z;
		
			// is it behind the z?

		if (!gl_project_in_view_z(x,y,z)) continue;
				
			// project names

		gl_project_point(&x,&y,&z);

		obj->draw.remote_name.proj_pnt.x=x;
		obj->draw.remote_name.proj_pnt.y=y;
		obj->draw.remote_name.proj_pnt.z=z;
		
			// calculate the fade
			
		if (dist<remote_name_min_distance) {
			obj->draw.remote_name.fade=1.0f;
		}
		else {
			obj->draw.remote_name.fade=1.0f-((float)(dist-remote_name_min_distance)/(float)(remote_name_max_distance-remote_name_min_distance));
		}
		
		obj->draw.remote_name.size=hud.font.text_size_medium-(int)((float)(hud.font.text_size_medium*dist)/(float)(remote_name_max_distance-remote_name_min_distance));
		if (obj->draw.remote_name.size<10) obj->draw.remote_name.size=10;
		
		obj->draw.remote_name.on=TRUE;
	}

		// ray trace remote name to camera's eye level
		// to check for visibility

	ept.x=view.camera.pnt.x;
	ept.y=view.camera.pnt.y;
	ept.z=view.camera.pnt.z;

	contact.obj.on=TRUE;
	contact.proj.on=FALSE;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_object;

	for (n=0;n!=server.count.obj;n++) {
		obj=&server.objs[n];
		if (!obj->draw.remote_name.on) continue;

		spt.x=obj->draw.remote_name.pnt.x;
		spt.y=obj->draw.remote_name.pnt.y;
		spt.z=obj->draw.remote_name.pnt.z;

		contact.obj.ignore_uid=obj->uid;
		
		hit=ray_trace_map_by_point(&spt,&ept,&hpt,&contact);
		
		if (camera.mode==cv_fpp) {
			if (hit) {
				if (contact.obj.uid!=server.player_obj_uid) {
					obj->draw.remote_name.on=FALSE;
					continue;
				}
			}
			else {
				obj->draw.remote_name.on=FALSE;
				continue;
			}
		}
		else {
			if (hit) {
				obj->draw.remote_name.on=FALSE;
				continue;
			}
		}
	}
}

void remote_draw_names_render(void)
{
	int					n,x,y,text_size;
	d3col				col;
	obj_type			*obj;
	
	if (!net_setup.client.joined) return;
	if (!setup.network.show_names) return;

	gl_setup_viewport(console_y_offset());
	gl_2D_view_interface();
	
	glDisable(GL_DEPTH_TEST);
	
		// draw all names

	text_size=-1;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
	
		if (obj->draw.remote_name.on) {

				// get 2D position in screen resolution

			x=obj->draw.remote_name.proj_pnt.x;
			y=setup.screen.y_sz-obj->draw.remote_name.proj_pnt.y;

				// covert to interface resolution

			x=(x*hud.scale_x)/setup.screen.x_sz;
			y=(y*hud.scale_y)/setup.screen.y_sz;

				// draw text
				
			if (text_size!=obj->draw.remote_name.size) {
				text_size=obj->draw.remote_name.size;
				gl_text_start(text_size);
			}

			remote_get_ui_color(obj,&col);
			gl_text_draw(x,y,obj->name,tx_center,FALSE,&col,obj->draw.remote_name.fade);
		}
		
		obj++;
	}

	gl_text_end();		// can call end without a start
}
