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

bool crosshair_get_location(int tick,obj_type *obj,weapon_type *weap,int *kx,int *ky,int *hit_obj_uid,int *dist)
{
	int						tx,ty,tz;
	d3pnt					fpt,hpt;
	d3ang					ang;
	ray_trace_contact_type	contact;

		// get projectile position and angle

	switch (weap->crosshair.type) {

		case ct_bone_tracking:
		case ct_bone_tracking_resizing:
			if (!weapon_get_projectile_position_angle_weapon_model(tick,obj,weap,&fpt,&ang,NULL)) return(FALSE);
			break;

		case ct_barrel_tracking:
		case ct_barrel_tracking_resizing:
			if (!weapon_get_projectile_position_angle_weapon_barrel(tick,obj,weap,&fpt,&ang,NULL)) return(FALSE);
			break;

		default:
			weapon_get_projectile_position_center(obj,weap,&fpt,&ang);
			break;
	}

		// trace to find any clicking or hit objects

	contact.obj.on=TRUE;
	contact.obj.ignore_uid=obj->uid;
	contact.proj.on=FALSE;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_object;

	ray_trace_map_by_angle(&fpt,&ang,(map_enlarge*400),&hpt,&contact);

	if (hit_obj_uid!=NULL) *hit_obj_uid=contact.obj.uid;
	
		// get the 2D point
		
	if (weap->crosshair.type==ct_center) {
		*kx=setup.screen.x_sz/2;
		*ky=setup.screen.y_sz/2;
		if (dist!=NULL) *dist=0;
	}
	else {
		gl_3D_view();
		gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
		gl_setup_project();
		
		tx=hpt.x;
		ty=hpt.y;
		tz=hpt.z;
		
		gl_project_point(&tx,&ty,&tz);
		
		*kx=tx;
		*ky=setup.screen.y_sz-ty;
		
		if (dist!=NULL) *dist=distance_get(view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z,hpt.x,hpt.y,hpt.z);
	}
	
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
	int					x,y,sz,dist,obj_uid,
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
	if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) return;
	
		// get crosshair location

	if (!crosshair_get_location(tick,obj,weap,&x,&y,&obj_uid,&dist)) return;
	
	crosshair_draw->on=TRUE;
	crosshair_draw->aim_obj_uid=obj_uid;
	
	crosshair_draw->x=x;
	crosshair_draw->y=y;
	
		// get the crosshair size

	switch (weap->crosshair.type) {

		case ct_bone_tracking:
		case ct_barrel_tracking:
			sz=setup.screen.x_sz>>5;
			break;

		case ct_bone_tracking_resizing:
		case ct_barrel_tracking_resizing:
			if (dist>weap->crosshair.distance) {
				sz=weap->crosshair.min_size;
			}
			else {
				sz=weap->crosshair.max_size-((weap->crosshair.max_size*dist)/weap->crosshair.distance);
				if (sz<weap->crosshair.min_size) sz=weap->crosshair.min_size;
			}
			break;

		default:
			sz=weap->crosshair.min_size;
			break;

	}
	
	crosshair_draw->sz=sz;
	
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

      Crosshair Drawing
      
======================================================= */

void crosshair_draw(obj_type *obj,weapon_type *weap)
{
	int					half_sz,lft,rgt,top,bot;
	obj_crosshair_draw	*crosshair_draw;
	
		// is scope on?
		
	if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) return;

		// can't use centered crosshairs when not in FPP

	if ((weap->crosshair.type==ct_center) && (camera.mode!=cv_fpp)) return;
	
		// is crosshair on?
	
	crosshair_draw=&obj->crosshair_draw;
	if (!crosshair_draw->on) return;
	
		// draw the crosshair
		
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
}


