/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Setup

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
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

int							draw_portal_cnt,draw_portal_list[max_portal];

extern void view_portal_create_sight_path(d3pos *pos);
extern void segment_render_setup(int tick,int portal_cnt,int *portal_list);
extern bool model_inview(model_draw *draw);
extern int distance_to_view_center(int x,int y,int z);

/* =======================================================

      Portal Updates
      
======================================================= */

void view_portal_updates(int tick)
{
	int				i;
	portal_type		*portal;
	
	portal=map.portals;

	for (i=0;i!=map.nportal;i++) {
		if (portal->in_path) map_portal_segment_update_shift(&map,tick,i);
		portal++;
	}
}

/* =======================================================

      Model Setup
      
======================================================= */

void view_clear_draw_in_view(model_draw *draw)
{
	int					n;
	model_type			*mdl;
	
	draw->in_view=FALSE;

	draw->all_lights_in_view=FALSE;

	for (n=0;n!=max_model_light;n++) {
		draw->lights[n].in_view=FALSE;
	}

	draw->shadow.in_view=FALSE;
	draw->shadow.texture_idx=-1;
		
	mdl=model_find_uid(draw->uid);
	if (mdl!=NULL) model_clear_draw_setup(mdl,&draw->setup);
}

void view_setup_model_in_view(model_draw *draw,bool in_air,bool is_camera)
{
	int					x,z,y;
	model_type			*mdl;

	if ((draw->uid==-1) || (!draw->on)) return;
	
		// is model in view?
		
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

	x=draw->pos.x;
	y=draw->pos.y;
	z=draw->pos.z;
	
	draw->lod_dist=distance_to_view_center(x,y,z);
	
	if (!is_camera) {
		if (draw->lod_dist<map.optimizations.lod_model_distance) {
			draw->in_view=model_inview(draw);
		}
	}
		
		// is shadow in view
		
	if ((setup.shadow_mode==shadow_mode_none) || (!draw->shadow.on)) return;
	if (draw->lod_dist>map.optimizations.lod_shadow_distance) return;
	
	if (shadow_get_volume(draw,in_air)) {
		draw->shadow.in_view=shadow_inview(draw);
	}
}

void view_setup_model_light_in_view(model_draw *draw)
{
	int					n,x,z,y;
	model_type			*mdl;
	model_draw_light	*light;

		// get the model

	mdl=NULL;
	if ((draw->uid!=-1) && (draw->on)) mdl=model_find_uid(draw->uid);
	
		// run through the lights

	for (n=0;n!=max_model_light;n++) {
		light=&draw->lights[n];

		if (!light->on) continue;
	
			// get light position
			
		x=draw->pos.x;
		y=draw->pos.y;
		z=draw->pos.z;
		
		if (mdl!=NULL) {
			model_get_light_position(mdl,&draw->setup,n,&x,&y,&z);
			if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&x,&y,&z);
		}
			
			// light in view?
			
		light->in_view=map_portal_light_in_view(&map,x,y,z,light->intensity);

		draw->all_lights_in_view=draw->all_lights_in_view||light->in_view;
	}
}

void view_setup_model_animation(model_draw *draw)
{
	if ((draw->uid==-1) || (!draw->on)) return;
	if ((!draw->in_view) && (!draw->shadow.in_view) && (!draw->all_lights_in_view)) return;
	
	model_calc_animation(draw);
	model_calc_draw_bones(draw);
}

/* =======================================================

      Setup Objects in Scene
      
======================================================= */

void view_setup_objects(int tick)
{
	int					i;
	bool				is_air,is_camera;
	obj_type			*obj;
	weapon_type			*weap;
	model_draw			*draw;
	
	for (i=0;i!=server.count.obj;i++) {
		obj=&server.objs[i];
		if (obj->hidden) continue;
		
		draw=&obj->draw;
		
		is_air=(obj->air_mode!=am_ground);
		is_camera=((camera.mode==cv_fpp) && (obj->uid==camera.obj_uid));
		
			// clear out all view flags
			
		view_clear_draw_in_view(draw);
		
			// setup model positions
			
		object_rigid_body_reset_angle(obj);
		object_fly_reset_angle(obj);
		model_draw_setup_object(tick,obj);
	
			// find model, shadows, and light in view
		
		view_setup_model_in_view(draw,is_air,is_camera);
		view_setup_model_light_in_view(draw);
	
			// setup model animations for models in view
		
		view_setup_model_animation(draw);
		
			// setup held weapon model

		if (obj->held_weapon.current_uid!=-1) {
		
			weap=weapon_find_uid(obj->held_weapon.current_uid);
			if (weap!=NULL) {
			
				draw=&weap->draw;
				draw->on=TRUE;
				
				view_clear_draw_in_view(draw);
				model_draw_setup_weapon(tick,obj,weap,FALSE);
				view_setup_model_in_view(draw,FALSE,FALSE);
				view_setup_model_light_in_view(draw);
			}
		}
	}
}

/* =======================================================

      Setup Projectiles in Scene
      
======================================================= */

void view_setup_projectiles(int tick)
{
	int					i;
	proj_type			*proj;

	proj=server.projs;
	
	for (i=0;i!=server.count.proj;i++) {
		
			// clear out all view flags
			
		view_clear_draw_in_view(&proj->draw);
		
			// setup model positions
			
		projectile_reset_angle_for_flight(proj);
		model_draw_setup_projectile(tick,proj);
		
			// find model and shadows in view
			
		view_setup_model_in_view(&proj->draw,TRUE,FALSE);
		view_setup_model_light_in_view(&proj->draw);
		
			// setup model animations for models in view
			
		view_setup_model_animation(&proj->draw);
		
		proj++;
	}
}

/* =======================================================

      Lights in View
      
======================================================= */

void view_add_model_light(model_draw *draw,int obj_uid)
{
	int					n,x,z,y;
	d3pos				pos;
	model_type			*mdl;
	model_draw_light	*light;
	model_draw_halo		*halo;

		// any model?
		
	mdl=NULL;
	if ((draw->uid!=-1) && (draw->on)) mdl=model_find_uid(draw->uid);
	
		// add lights
		
	if (draw->all_lights_in_view) {

		light=draw->lights;
		
		for (n=0;n!=max_model_light;n++) {

			if (light->on) {
				memmove(&pos,&draw->pos,sizeof(d3pos));
				
				if (mdl!=NULL) {
					model_get_light_position(mdl,&draw->setup,n,&pos.x,&pos.y,&pos.z);
					if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&pos.x,&pos.y,&pos.z);
				}
				
				light_add(&pos,light->type,light->intensity,light->confine_to_portal,&light->col);
			}

			light++;
		}
	}
	
		// add halo
		
	halo=draw->halos;

	for (n=0;n!=max_model_halo;n++) {

		if (halo->on) {
			x=draw->pos.x;
			y=draw->pos.y;
			z=draw->pos.z;
			
			if (mdl!=NULL) {
				model_get_halo_position(mdl,&draw->setup,n,&x,&y,&z);
				if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&x,&y,&z);
			}
			
			halo_draw_add(x,z,y,obj_uid,halo);
		}

		halo++;
	}
}

void view_add_lights(void)
{
	int					i;
	map_light_type		*maplight;
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;

		// map lights
		
	maplight=map.lights;
		
	for (i=0;i!=map.nlight;i++) {
		if (maplight->on) {
			if (map_portal_light_in_view(&map,maplight->pos.x,maplight->pos.y,maplight->pos.z,maplight->intensity)) {
				light_add(&maplight->pos,maplight->type,maplight->intensity,maplight->confine_to_portal,&maplight->col);
			}
		}
		maplight++;
	}	

		// lights from objects and their weapons
	
	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		view_add_model_light(&obj->draw,obj->uid);
		if (obj->held_weapon.current_uid!=-1) {
			weap=weapon_find_uid(obj->held_weapon.current_uid);
			if (weap!=NULL) view_add_model_light(&weap->draw,-1);
		}
		obj++;
	}
	
		// lights from projectiles

	proj=server.projs;
	
	for (i=0;i!=server.count.proj;i++) {
		view_add_model_light(&proj->draw,-1);
		proj++;
	}
}

/* =======================================================

      Scope FOV changes
      
======================================================= */

void view_calculate_scope(obj_type *obj,obj_type *camera_obj)
{
	float			f_step,f_max_step;
	weapon_type		*weap;

		// is object the camera object?

	if (camera_obj->uid!=obj->uid) return;
	
		// scoped weapon on?
	
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
	
	if ((!weap->zoom.on) || (!weap->zoom.active)) return;
	
		// calculate fov
		
	if (weap->zoom.step_count<=1) {
		view.camera.fov=weap->zoom.fov_max;
		return;
	}
		
	f_step=(float)weap->zoom.current_step;
	f_max_step=(float)(weap->zoom.step_count-1);
	
	view.camera.fov=weap->zoom.fov_max-(((weap->zoom.fov_max-weap->zoom.fov_min)/f_max_step)*f_step);
}

void view_calculate_recoil(obj_type *obj)
{
	weapon_type		*weap;

	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	weapon_recoil_add(obj,weap,&view.camera.ang);
}

/* =======================================================

      Shakes and Sways
      
======================================================= */

void view_calculate_shakes(int tick,obj_type *obj)
{
	int					n,k,d,shake_sz,shake_cnt;
	float				shake_freq;
	effect_type			*effect;
	
	shake_cnt=0;
	shake_freq=0.0f;
	shake_sz=0;
	
		// find any shake effects
		
	effect=server.effects;
	
	for (n=0;n!=server.count.effect;n++) {
	
		if (effect->effecttype==ef_shake) {
			
			d=distance_get(obj->pos.x,obj->pos.y,obj->pos.z,effect->pos.x,effect->pos.y,effect->pos.z);
			if (d<effect->data.shake.distance) {
				shake_sz+=effect->size;
				shake_freq+=(0.3f-((0.3f*(float)d)/(float)effect->data.shake.distance))+0.2f;
				shake_cnt++;
			}
		}
		
		effect++;
	}
	
		// any shake data?
		
	if (shake_cnt==0) return;

	shake_sz=shake_sz/shake_cnt;
	shake_freq=shake_freq/(float)shake_cnt;
	
		// do shake
		
	k=(int)((float)(tick-effect->start_tick)*shake_freq)%shake_sz;
	if (k>(shake_sz>>1)) k=shake_sz-k;
	
	view.camera.ang.x=angle_add(view.camera.ang.x,((float)k/25));
}

void view_calculate_sways(int tick,obj_type *obj)
{
	float				f;

	return;

	f=(float)(((tick>>4)%12)-6);
	view.camera.ang.z+=f;

	// supergumba -- work on sways

}

/* =======================================================

      View Script 3D to 2D transform
      
======================================================= */

void view_script_transform_3D_to_2D(int x,int y,int z,int *x2,int *y2)
{
	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
	
	gl_project_point(&x,&y,&z);
	
	*x2=x;
	*y2=y;
}

/* =======================================================

      View Setup Mainline
      
======================================================= */

void view_draw_setup(int tick)
{
	obj_type		*obj,*camera_obj;

		// get player object
		
	obj=object_find_uid(server.player_obj_uid);
	
		// set view camera
	
	camera_obj=object_find_uid(camera.obj_uid);
	camera_get_position(&view.camera.pos,&view.camera.ang,camera_obj->size.eye_offset);
	if (view.camera.pos.rn==-1) {
		view.camera.pos.rn=camera_obj->pos.rn;
	}
	view.camera.under_liquid_idx=camera_check_liquid(&view.camera.pos);

	view.camera.projection_type=camera.plane.type;
	view.camera.lft=camera.plane.lft;
	view.camera.rgt=camera.plane.rgt;
	view.camera.top=camera.plane.top;
	view.camera.bot=camera.plane.bot;
	view.camera.near_z=camera.plane.near_z;
	view.camera.far_z=camera.plane.far_z;
	view.camera.near_z_offset=camera.plane.near_z_offset;
	view.camera.fov=camera.plane.fov;
	view.camera.aspect_ratio=camera.plane.aspect_ratio;
	
		// camera adjustments
	
	view_calculate_scope(obj,camera_obj);
	view_calculate_recoil(obj);
	view_calculate_shakes(tick,obj);
	view_calculate_sways(tick,obj);
	
		// bump smoothing
		
	if (obj->bump.on) view.camera.pos.y+=obj->bump.smooth_offset;		
	
		// y for forward facing models
		
	view.draw_forward_ang_y=view.camera.ang.y;
	
		// setup viewport
	
	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

		// do texture animations
		
	map_setup_animated_textures(&map,tick);

		// get sight path

	view_portal_create_sight_path(&view.camera.pos);
	
		// any portal updates
		
	view_portal_updates(tick);
	
		// setup objects, projectiles, and scenery in path
		
	view_setup_objects(tick);
	view_setup_projectiles(tick);

		// clear halos
		
	halo_draw_clear();
	
		// add lights and halos
		
	light_clear();
	view_add_lights();
	effect_add_lights(tick);
	
		// setup portals and portal segments for drawing
		
	draw_portal_cnt=map_portal_draw_sort(&map,obj->pos.rn,obj->pos.x,obj->pos.y,obj->pos.z,draw_portal_list);

	segment_render_setup(tick,draw_portal_cnt,draw_portal_list);
}

