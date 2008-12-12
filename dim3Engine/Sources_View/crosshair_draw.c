/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Crosshair Setup/Drawing

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
#include "physics.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Alternate Crosshairs
      
======================================================= */

void crosshair_show_alt(int tick,obj_type *obj)
{
	obj->crosshair_draw.alt_tick=tick+200;
}

/* =======================================================

      Track Crosshair
      
======================================================= */

bool crosshair_get_location(int tick,obj_type *obj,weapon_type *weap,int *kx,int *kz,int *ky,int *hit_obj_uid)
{
	d3pnt					fpt,hpt;
	d3ang					ang;
	ray_trace_contact_type	contact;

		// get projectile position and angle

	if ((weap->crosshair.type==ct_bone_tracking) || (weap->crosshair.type==ct_bone_tracking_resizing)) {
		if (!weapon_get_projectile_position_angle_weapon_model(tick,obj,weap,&fpt,&ang,NULL)) return(FALSE);
	}
	else {
		if (!weapon_get_projectile_position_angle_weapon_barrel(tick,obj,weap,&fpt,&ang,NULL)) return(FALSE);
	}

		// trace

	contact.obj.on=TRUE;
	contact.obj.ignore_uid=obj->uid;
	contact.proj.on=FALSE;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_object;

	ray_trace_map_by_angle(&fpt,&ang,(map_enlarge*400),&hpt,&contact);
	
		// return hit
	
	*kx=hpt.x;
	*ky=hpt.y;
	*kz=hpt.z;
	
	*hit_obj_uid=contact.obj.uid;
	
	return(TRUE);
}

/* =======================================================

      Crosshair Setup
      
======================================================= */

void crosshair_setup_click(int tick,obj_type *obj)
{
	unsigned long		idx;
	obj_crosshair_draw	*crosshair_draw;
	
		// if crosshair was off (no weapon, no crosshair) then force to middle
		
	crosshair_draw=&obj->crosshair_draw;
	if (!crosshair_draw->on) {
		crosshair_draw->on=TRUE;
		crosshair_draw->x=setup.screen.x_sz>>1;
		crosshair_draw->y=setup.screen.y_sz-(setup.screen.y_sz>>1);				// 2D view has Y flipped
	}
	
		// static setup for click crosshair
		
	crosshair_draw->aim_obj_uid=-1;
	crosshair_draw->sz=setup.screen.x_sz/10;
	crosshair_draw->alpha=1;
	crosshair_draw->color.r=crosshair_draw->color.g=crosshair_draw->color.b=1;

		// clicking down?

	if (crosshair_draw->alt_tick>tick) {
		idx=obj->click.crosshair_down_idx;
	}
	else {
		idx=obj->click.crosshair_up_idx;
	}

	if (idx==-1) {
		crosshair_draw->on=FALSE;		// click crosshair not set, do not draw
		return;
	}

	crosshair_draw->gl_id=view_images_get_gl_id(server.crosshairs[idx].image_idx);
}

void crosshair_setup_weapon(int tick,obj_type *obj,weapon_type *weap)
{
	int					crosshair_type,x,y,z,tx,ty,tz,sz,dist,obj_uid,
						item_count,weap_mode,move_tick,swap_tick;
	float				alpha;
	obj_crosshair_draw	*crosshair_draw;
	
	crosshair_draw=&obj->crosshair_draw;
	crosshair_draw->on=FALSE;
	crosshair_draw->aim_obj_uid=-1;

		// weapon displays a crosshair?
	
	if (obj->hide_all_weapons) return;
	if (!weap->crosshair.on) return;
	if (weap->crosshair.fire_idx==-1) return;
	if ((weap->zoom.on) && (weap->zoom.active)) return;
	
		// get crosshair screen location

	crosshair_type=weap->crosshair.type;
		
	if (crosshair_type==ct_center) {
		tx=setup.screen.x_sz>>1;
		ty=setup.screen.y_sz>>1;
		sz=weap->crosshair.min_size;
	
		crosshair_draw->on=TRUE;
	}
	else {
		
			// get crosshair location
		
		if (!crosshair_get_location(tick,obj,weap,&x,&z,&y,&obj_uid)) return;
		
		crosshair_draw->on=TRUE;
		crosshair_draw->aim_obj_uid=obj_uid;
		
			// get the crosshair place
			
		gl_setup_viewport(0);
		gl_3D_view(&view.camera);
		gl_3D_rotate(&view.camera.ang);
		gl_setup_project();
		
		tx=x-view.camera.pnt.x;
		ty=y-view.camera.pnt.y;
		tz=view.camera.pnt.z-z;
		
		gl_project_point(&tx,&ty,&tz);
		
			// get the crosshair size
			
		if ((crosshair_type==ct_bone_tracking) || (crosshair_type==ct_barrel_tracking)) {
			sz=setup.screen.x_sz>>5;
		}
		else {
			dist=distance_get(view.camera.pnt.x,view.camera.pnt.y,view.camera.pnt.z,x,y,z);

			if (dist>weap->crosshair.distance) {
				sz=weap->crosshair.min_size;
			}
			else {
				sz=weap->crosshair.max_size-((weap->crosshair.max_size*dist)/weap->crosshair.distance);
				if (sz<weap->crosshair.min_size) sz=weap->crosshair.min_size;
			}
		}
	}
	
		// crosshair alpha if weapon changing
	
	weap_mode=obj->held_weapon.mode;
	swap_tick=obj->held_weapon.swap_tick;
	
	alpha=1;
	
	move_tick=tick;
	
    if (weap_mode==wm_lower) {
        move_tick-=swap_tick;
        alpha=(float)(weap->hand.raise_tick-move_tick)/(float)weap->hand.raise_tick;
	}
    if (weap_mode==wm_raise) {
        move_tick-=swap_tick;
        alpha=1-((float)(weap->hand.raise_tick-move_tick)/(float)weap->hand.raise_tick);
	}
	
		// setup
		
	crosshair_draw->x=tx;
	crosshair_draw->y=setup.screen.y_sz-ty;				// 2D view has Y flipped
	crosshair_draw->sz=sz;
	crosshair_draw->alpha=alpha;
	
		// regular weapon crosshair
		
	crosshair_draw->gl_id=view_images_get_gl_id(server.crosshairs[weap->crosshair.fire_idx].image_idx);
	
		// change color if picking up an item or empty

	item_count=obj->item_count;
	if (item_count!=0) {
		memmove(&crosshair_draw->color,&weap->crosshair.pickup_col,sizeof(d3col));
	}
	else {
		if (weap->ammo.count!=0) {
			memmove(&crosshair_draw->color,&weap->crosshair.col,sizeof(d3col));
		}
		else {
			memmove(&crosshair_draw->color,&weap->crosshair.empty_col,sizeof(d3col));
		}
	}
}

void crosshair_setup(int tick,obj_type *obj,weapon_type *weap)
{
		// setup crosshair
		
	crosshair_setup_weapon(tick,obj,weap);
	
		// if there an object to click?
		
	if (obj->click.current_click_obj_uid==-1) return;
	
		// change to click crosshair
		
	crosshair_setup_click(tick,obj);
}

/* =======================================================

      Crosshair Debug Drawing
      
======================================================= */

void crosshair_draw_debug(obj_type *obj,weapon_type *weap)
{
	int				sx,sy,sz,ex,ey,ez,bx,by,bz,obj_uid,pose_idx,bone_idx;
	model_type		*mdl;

		// setup 3D drawing

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

		// get fire and barrel positions

	mdl=model_find_uid(weap->draw.uid);
	if (mdl==NULL) return;

	bone_idx=model_find_bone(mdl,weap->proj.fire_bone_tag);
	if (bone_idx==-1) return;

	pose_idx=model_find_pose(mdl,weap->proj.fire_pose_name);
	if (pose_idx==-1) return;

	model_draw_setup_weapon(server.time.run_tick,obj,weap,TRUE,FALSE);
	model_calc_draw_bone_position(mdl,&weap->draw.setup,pose_idx,bone_idx,&sx,&sy,&sz);

	sx+=weap->draw.pnt.x;
	sy+=weap->draw.pnt.y;
	sz+=weap->draw.pnt.z;
	
	if (weap->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&sx,&sy,&sz);

	sx-=view.camera.pnt.x;
	sy-=view.camera.pnt.y;
	sz-=view.camera.pnt.z;

	bone_idx=model_find_bone(mdl,weap->proj.barrel_bone_tag);
	if (bone_idx==-1) return;

	model_get_draw_bone_position(&weap->draw.setup,bone_idx,&bx,&by,&bz);

	bx+=weap->draw.pnt.x;
	by+=weap->draw.pnt.y;
	bz+=weap->draw.pnt.z;

	if (weap->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&bx,&by,&bz);
	
	bx-=view.camera.pnt.x;
	by-=view.camera.pnt.y;
	bz-=view.camera.pnt.z;

		// body line

	glDisable(GL_DEPTH_TEST);

	glColor4f(0,1,1,1);
	glLineWidth(2);

	glBegin(GL_LINES);
	glVertex3i(sx,sy,-sz);
	glVertex3i(bx,by,-bz);
	glEnd();

	glLineWidth(1);

	glEnable(GL_DEPTH_TEST);

		// cursor line

	ex=ey=ez=0;					// this is here to stop a "might not be used" error in gcc
	
	crosshair_get_location(0,obj,weap,&ex,&ez,&ey,&obj_uid);
	
	ex-=view.camera.pnt.x;
	ey-=view.camera.pnt.y;
	ez-=view.camera.pnt.z;

	glDisable(GL_DEPTH_TEST);

	glColor4f(1,1,0,1);
	glLineWidth(2);

	glBegin(GL_LINES);
	glVertex3i(sx,sy,-sz);
	glVertex3i(ex,ey,-ez);
	glEnd();

	glLineWidth(1);

	glEnable(GL_DEPTH_TEST);
}

/* =======================================================

      Crosshair Drawing
      
======================================================= */

void crosshair_draw(obj_type *obj,weapon_type *weap)
{
	int					half_sz,lft,rgt,top,bot;
	obj_crosshair_draw	*crosshair_draw;
	
		// is scope on?
		
	if ((weap->zoom.on) && (weap->zoom.active)) return;

		// can't use centered crosshairs when not in FPP

	if ((weap->crosshair.type==ct_center) && (camera.mode!=cv_fpp)) return;
	
		// is crosshair on?
	
	crosshair_draw=&obj->crosshair_draw;
	if (!crosshair_draw->on) return;
	
		// draw the crosshair
		
	gl_setup_viewport(console_y_offset());
	gl_2D_view_screen();
	
	half_sz=crosshair_draw->sz>>1;
	
	lft=crosshair_draw->x-half_sz;
	top=crosshair_draw->y-half_sz;
	rgt=lft+crosshair_draw->sz;
	bot=top+crosshair_draw->sz;
	
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);

	gl_texture_simple_set(crosshair_draw->gl_id,TRUE,crosshair_draw->color.r,crosshair_draw->color.g,crosshair_draw->color.b,crosshair_draw->alpha);
	view_draw_next_vertex_object_2D_texture_quad(lft,rgt,top,bot);
	gl_texture_simple_end();

		// debug?

	if (weap->crosshair.type==ct_barrel_debug) crosshair_draw_debug(obj,weap);
}


