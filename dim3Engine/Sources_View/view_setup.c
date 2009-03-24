/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Setup

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

extern bool					shadow_on;

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

view_render_type			view_global_render;		// supergumba -- temporary

extern bool fog_solid_on(void);
extern bool model_inview(model_draw *draw);
extern int distance_to_view_center(int x,int y,int z);
extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);
extern bool mesh_inview(map_mesh_type *mesh);

/* =======================================================

      Create Mesh Draw List
      
======================================================= */

bool view_area_mark(char *area_flags)
{
	int				n,k;
	bool			has_area;
	map_area_type	*area,*area2;

	bzero(area_flags,max_area);

	has_area=FALSE;

	for (n=0;n!=map.narea;n++) {
		area=&map.areas[n];

		if ((view.camera.pnt.x<area->lft) || (view.camera.pnt.x>area->rgt)) continue;
		if ((view.camera.pnt.z<area->top) || (view.camera.pnt.z>area->bot)) continue;

			// mark this area

		area_flags[n]=0x1;
		has_area=TRUE;

			// join with any areas with the same color

		area2=map.areas;

		for (k=0;k!=map.narea;k++) {

			if (k!=n) {
				if ((area2->col.r==area->col.r) && (area2->col.g==area->col.g) && (area2->col.b==area->col.b)) area_flags[k]=0x1;
			}

			area2++;
		}
	}

	return(has_area);
}

bool view_area_check_mesh(map_mesh_type *mesh,char *area_flags)
{
	int				n;
	map_area_type	*area;

	for (n=0;n!=map.narea;n++) {
		if (area_flags[n]==0x0) continue;

			// is mesh in this area?

		area=&map.areas[n];

		if (((mesh->box.min.x>=area->lft) && (mesh->box.min.x<=area->rgt)) || ((mesh->box.max.x>=area->lft) && (mesh->box.max.x<=area->rgt))) {
			if ((mesh->box.min.z>=area->top) && (mesh->box.min.z<=area->bot)) return(TRUE);
			if ((mesh->box.max.z>=area->top) && (mesh->box.max.z<=area->bot)) return(TRUE);
		}
		
		if (((area->lft>=mesh->box.min.x) && (area->lft<=mesh->box.max.x)) || ((area->rgt>=mesh->box.min.x) && (area->rgt<=mesh->box.max.x))) {
			if ((area->top>=mesh->box.min.z) && (area->top<=mesh->box.max.z)) return(TRUE);
			if ((area->bot>=mesh->box.min.z) && (area->bot<=mesh->box.max.z)) return(TRUE);
		}
	}

	return(FALSE);
}

void view_create_mesh_draw_list(void)
{
	int					n,t,sz,d,start_mesh_idx,idx,
						never_obscure_dist,obscure_dist;
	char				area_flags[max_area];
	bool				has_area;
	map_mesh_type		*start_mesh,*mesh;
	
		// get mesh camera is in

	start_mesh_idx=map_mesh_find_always(&map,&view.camera.pnt);
	start_mesh=&map.mesh.meshes[start_mesh_idx];

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=view.camera.far_z-view.camera.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}

	never_obscure_dist=abs(view.camera.near_z)*3;

		// if we are in a sight view area, then obscure with that

	has_area=view_area_mark(area_flags);

		// check all visibile meshes from the start mesh
	
	view.render->mesh_draw.count=0;
	
	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
			
		if (!mesh->flag.never_obscure) {

				// is this mesh visible?

			if ((n!=start_mesh_idx) && (has_area)) {
				if (!view_area_check_mesh(mesh,area_flags)) continue;
			}

				// auto-eliminate meshes drawn outside the obscure distance
				
			d=map_mesh_calculate_distance(mesh,&view.camera.pnt);
			if (d>obscure_dist) continue;
				
				// within a certain distance, don't check for obscuring
				// by the view to fix some bugs with large polygons very
				// close to the camera

			if (d>never_obscure_dist) {
				if (!mesh_inview(mesh)) continue;
			}
		}
		else {
			d=map_mesh_calculate_distance(mesh,&view.camera.pnt);
		}
		
			// sort meshes into drawing list
			// top of list is closest items

		idx=-1;
	
		for (t=0;t!=view.render->mesh_draw.count;t++) {
			if (view.render->mesh_draw.dist[t]>d) {
				idx=t;
				break;
			}
		}
	
			// insert at end of list
			
		if (idx==-1) {
			view.render->mesh_draw.dist[view.render->mesh_draw.count]=d;
			view.render->mesh_draw.list[view.render->mesh_draw.count]=n;
			view.render->mesh_draw.count++;
			continue;
		}
		
			// insert in list
			
		sz=sizeof(int)*(view.render->mesh_draw.count-idx);
		memmove(&view.render->mesh_draw.dist[idx+1],&view.render->mesh_draw.dist[idx],sz);
		memmove(&view.render->mesh_draw.list[idx+1],&view.render->mesh_draw.list[idx],sz);
		
		view.render->mesh_draw.dist[idx]=d;
		view.render->mesh_draw.list[idx]=n;
		
		view.render->mesh_draw.count++;
	}
}

bool view_mesh_in_draw_list(int mesh_idx)
{
	int			n;

	for (n=0;n!=view.render->mesh_draw.count;n++) {
		if (view.render->mesh_draw.list[n]==mesh_idx) return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Model Setup
      
======================================================= */

void view_clear_draw_in_view(model_draw *draw)
{
	model_type			*mdl;
	
	draw->in_view=FALSE;

	draw->shadow.in_view=FALSE;
	draw->shadow.texture_idx=-1;
		
	mdl=model_find_uid(draw->uid);
	if (mdl!=NULL) model_clear_draw_setup(mdl,&draw->setup);
}

void view_setup_model_in_view(model_draw *draw,bool in_air,bool is_camera,int mesh_idx)
{
	int					x,z,y,obscure_dist;

	if ((draw->uid==-1) || (!draw->on)) return;

		// is model in a mesh that's in the mesh draw list?

	if ((!is_camera) && (mesh_idx!=-1)) {
		if (!view_mesh_in_draw_list(mesh_idx)) return;
	}
	
		// is model within obscure distance

	if (!fog_solid_on()) {
		obscure_dist=view.camera.far_z-view.camera.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}
		
	x=draw->pnt.x;
	y=draw->pnt.y;
	z=draw->pnt.z;
	
	draw->lod_dist=distance_to_view_center(x,y,z);
	if (draw->lod_dist>=obscure_dist) return;
	
		// is model in view?

	if (!is_camera) {
		draw->in_view=model_inview(draw);
	}
		
		// is shadow in view
		// shadows have a shorter obscure distance

	if ((!shadow_on) || (!draw->shadow.on)) return;

	if (draw->lod_dist>=(obscure_dist>>1)) return;
	
	if (shadow_get_volume(draw,in_air)) {
		draw->shadow.in_view=shadow_inview(draw);
	}
}

void view_setup_model_animation(model_draw *draw)
{
	if ((draw->uid==-1) || (!draw->on)) return;
	if ((!draw->in_view) && (!draw->shadow.in_view)) return;
	
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
		
		view_setup_model_in_view(draw,is_air,is_camera,obj->mesh.cur_mesh_idx);
	
			// setup model animations for models in view
		
		view_setup_model_animation(draw);
		
			// setup held weapon model

		if (obj->held_weapon.current_uid!=-1) {

			weap=weapon_find_uid(obj->held_weapon.current_uid);
			if (weap!=NULL) {
				draw=&weap->draw;
				view_clear_draw_in_view(draw);
				model_draw_setup_weapon(tick,obj,weap,FALSE,FALSE);
				view_setup_model_in_view(draw,FALSE,FALSE,obj->mesh.cur_mesh_idx);
			}
		}
	}
}

/* =======================================================

      Setup Projectiles in Scene
      
======================================================= */

void view_setup_projectiles(int tick)
{
	int					i,mesh_idx;
	proj_type			*proj;

	proj=server.projs;
	
	for (i=0;i!=server.count.proj;i++) {
		
			// clear out all view flags
			
		view_clear_draw_in_view(&proj->draw);
		
			// setup model positions
			
		projectile_reset_angle_for_flight(proj);
		model_draw_setup_projectile(tick,proj);
		
			// find model and shadows in view
			
		mesh_idx=map_mesh_find(&map,&proj->draw.pnt);
		view_setup_model_in_view(&proj->draw,TRUE,FALSE,mesh_idx);
		
			// setup model animations for models in view
			
		view_setup_model_animation(&proj->draw);
		
		proj++;
	}
}

/* =======================================================

      Halos in View
      
======================================================= */

void view_add_model_halo(model_draw *draw,int obj_uid)
{
	int					n,x,z,y;
	model_type			*mdl;
	model_draw_halo		*halo;

		// any model?
		
	mdl=NULL;
	if ((draw->uid!=-1) && (draw->on)) mdl=model_find_uid(draw->uid);
	
		// add halo
		
	halo=draw->halos;

	for (n=0;n!=max_model_halo;n++) {

		if (halo->on) {
			x=draw->pnt.x;
			y=draw->pnt.y;
			z=draw->pnt.z;
			
			if (mdl!=NULL) {
				model_get_halo_position(mdl,&draw->setup,n,&x,&y,&z);
				if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&x,&y,&z);
			}
			
			halo_draw_add(x,z,y,obj_uid,halo);
		}

		halo++;
	}
}

void view_add_halos(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;

		// halos from objects and their weapons
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		view_add_model_halo(&obj->draw,obj->uid);
		obj++;
	}
	
		// halos from projectiles

	proj=server.projs;
	
	for (n=0;n!=server.count.proj;n++) {
		view_add_model_halo(&proj->draw,-1);
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
			
			d=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,effect->pnt.x,effect->pnt.y,effect->pnt.z);
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
	gl_3D_rotate(&view.camera.pnt,&view.camera.ang);
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

		// supergumba -- global view render

	view.render=&view_global_render;

		// get player object
		
	obj=object_find_uid(server.player_obj_uid);
	
		// set view camera
	
	camera_obj=object_find_uid(camera.obj_uid);
	camera_get_position(&view.camera.pnt,&view.camera.ang,camera_obj->size.eye_offset);

	view.camera.under_liquid_idx=camera_check_liquid(&view.camera.pnt);

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
	
	if (camera.mode==cv_fpp) {
		view_calculate_scope(obj,camera_obj);
		view_calculate_recoil(obj);
	}
	view_calculate_shakes(tick,obj);
	view_calculate_sways(tick,obj);
	
		// bump smoothing
		
	if (obj->bump.on) view.camera.pnt.y+=obj->bump.smooth_offset;
	
		// setup viewport
	
	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.pnt,&view.camera.ang);
	gl_setup_project();
	
		// compile all lights in map
		
	gl_lights_compile(tick);

		// setup draw meshes

	view_create_mesh_draw_list();

		// do texture animations
		
	map_setup_animated_textures(&map,tick);
	
		// any texture shifting updates
		
	map_mesh_poly_run_shifts(&map,tick);
	
		// setup objects, projectiles, and scenery in path
		
	view_setup_objects(tick);
	view_setup_projectiles(tick);

		// add scene halos
		
	halo_draw_clear();
	view_add_halos();
}

