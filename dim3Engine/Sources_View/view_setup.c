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

extern bool					shadow_on;

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

extern bool fog_solid_on(void);
extern bool model_inview(model_draw *draw);
extern double distance_to_view_center(int x,int y,int z);
extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);
extern bool mesh_inview(map_mesh_type *mesh);
extern bool effect_inview(effect_type *effect,int count);

/* =======================================================

      View Areas
      
======================================================= */

void view_create_area_mask(void)
{
	int				n,k;
	map_area_type	*area,*area2;

	bzero(view.render->area_mask,max_area);

	view.render->has_area=FALSE;

	for (n=0;n!=map.narea;n++) {
		area=&map.areas[n];

		if ((view.render->camera.pnt.x<area->lft) || (view.render->camera.pnt.x>area->rgt)) continue;
		if ((view.render->camera.pnt.z<area->top) || (view.render->camera.pnt.z>area->bot)) continue;

			// mark this area

		view.render->area_mask[n]=0x1;
		view.render->has_area=TRUE;

			// join with any areas with the same color

		area2=map.areas;

		for (k=0;k!=map.narea;k++) {

			if (k!=n) {
				if ((area2->col.r==area->col.r) && (area2->col.g==area->col.g) && (area2->col.b==area->col.b)) view.render->area_mask[k]=0x1;
			}

			area2++;
		}
	}
}

bool view_area_check_mesh(map_mesh_type *mesh)
{
	int				n;
	map_area_type	*area;

	for (n=0;n!=map.narea;n++) {
		if (view.render->area_mask[n]==0x0) continue;

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

bool view_area_check_liquid(map_liquid_type *liq)
{
	int				n;
	map_area_type	*area;

	for (n=0;n!=map.narea;n++) {
		if (view.render->area_mask[n]==0x0) continue;

			// is mesh in this area?

		area=&map.areas[n];

		if (((liq->lft>=area->lft) && (liq->lft<=area->rgt)) || ((liq->rgt>=area->lft) && (liq->rgt<=area->rgt))) {
			if ((liq->top>=area->top) && (liq->top<=area->bot)) return(TRUE);
			if ((liq->bot>=area->top) && (liq->bot<=area->bot)) return(TRUE);
		}
		
		if (((area->lft>=liq->lft) && (area->lft<=liq->rgt)) || ((area->rgt>=liq->lft) && (area->rgt<=liq->rgt))) {
			if ((area->top>=liq->top) && (area->top<=liq->bot)) return(TRUE);
			if ((area->bot>=liq->top) && (area->bot<=liq->bot)) return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Draw Lists
      
======================================================= */

void view_start_draw_list(void)
{
	view.render->draw_list.count=0;
}

void view_add_draw_list(int item_type,int item_idx,double item_dist)
{
	int				t,idx,sz;

		// room for any more items?

	if (view.render->draw_list.count==max_view_render_item) return;

		// find place in sorted list

	idx=-1;

	for (t=0;t!=view.render->draw_list.count;t++) {
		if (view.render->draw_list.items[t].dist>item_dist) {
			idx=t;
			break;
		}
	}

		// insert at end of list
		
	if (idx==-1) {
		view.render->draw_list.items[view.render->draw_list.count].type=item_type;
		view.render->draw_list.items[view.render->draw_list.count].dist=item_dist;
		view.render->draw_list.items[view.render->draw_list.count].idx=(short)item_idx;
	}
	
		// insert in list
	
	else {
		sz=sizeof(view_render_draw_list_item_type)*(view.render->draw_list.count-idx);
		memmove(&view.render->draw_list.items[idx+1],&view.render->draw_list.items[idx],sz);
		
		view.render->draw_list.items[idx].type=item_type;
		view.render->draw_list.items[idx].dist=item_dist;
		view.render->draw_list.items[idx].idx=(short)item_idx;
	}

		// add up list

	view.render->draw_list.count++;
}

bool view_mesh_in_draw_list(int mesh_idx)
{
	int			n;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_mesh) {
			if (view.render->draw_list.items[n].idx==mesh_idx) return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Add Map Items to Draw Lists
      
======================================================= */

void view_add_mesh_draw_list(void)
{
	int					n,start_mesh_idx;
	double				d,obscure_dist;
	map_mesh_type		*start_mesh,*mesh;
	
		// get mesh camera is in

	start_mesh_idx=map_mesh_find_always(&map,&view.render->camera.pnt);
	start_mesh=&map.mesh.meshes[start_mesh_idx];

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.plane.far_z-camera.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

		// check all visibile meshes from the start mesh
	
	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
			
		if (!mesh->flag.never_obscure) {

				// is this mesh visible?

			if ((n!=start_mesh_idx) && (view.render->has_area)) {
				if (!view_area_check_mesh(mesh)) continue;
			}

				// auto-eliminate meshes drawn outside the obscure distance
				
			d=map_mesh_calculate_distance(mesh,&view.render->camera.pnt);
			if (d>obscure_dist) continue;
				
				// within a certain distance, don't check for obscuring
				// by the view to fix some bugs with large polygons very
				// close to the camera

			if (d>view_never_obscure_dist) {
				if (!mesh_inview(mesh)) continue;
			}
		}
		else {
			d=map_mesh_calculate_distance(mesh,&view.render->camera.pnt);
		}
		
			// sort meshes into drawing list

		view_add_draw_list(view_render_type_mesh,n,d);
	}
}

void view_add_liquid_draw_list(void)
{
	int					n;
	double				d,obscure_dist;
	map_liquid_type		*liq;

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.plane.far_z-camera.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

		// find all drawable liquids

	for (n=0;n!=map.liquid.nliquid;n++) {

		liq=&map.liquid.liquids[n];
			
			// is this liquid visible?

		if (view.render->has_area) {
			if (!view_area_check_liquid(liq)) continue;
		}

			// auto-eliminate liquids drawn outside the obscure distance
				
		d=map_liquid_calculate_distance(liq,&view.render->camera.pnt);
		if (d>obscure_dist) continue;
				
			// within a certain distance, don't check for obscuring
			// by the view to fix some bugs with large polygons very
			// close to the camera

		if (d>view_never_obscure_dist) {
			if (!boundbox_inview(liq->lft,liq->top,liq->rgt,liq->bot,liq->y,liq->y)) continue;
		}
		
			// sort liquids into drawing list

		view_add_draw_list(view_render_type_liquid,n,d);
	}
}

/* =======================================================

      Model Setup and Draw List
      
======================================================= */

bool view_setup_model_in_view(model_draw *draw,int mesh_idx)
{
	double					obscure_dist;

	if ((draw->uid==-1) || (!draw->on)) return(FALSE);

		// is model in a mesh that's in the mesh draw list?

	if (mesh_idx!=-1) {
		if (!view_mesh_in_draw_list(mesh_idx)) return(FALSE);
	}
	
		// is model within obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.plane.far_z-camera.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}
		
	draw->draw_dist=distance_to_view_center(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	if (draw->draw_dist>=obscure_dist) return(FALSE);
	
		// is model in view?

	return(model_inview(draw));
}

void view_setup_objects(int tick)
{
	int					n;
	bool				is_camera;
	obj_type			*obj;
	weapon_type			*weap;
	
	for (n=0;n!=server.count.obj;n++) {
		obj=&server.objs[n];
		if (obj->hidden) continue;
		
		is_camera=((camera.mode==cv_fpp) && (obj->uid==camera.obj_uid));
		
			// setup model positions
			
		object_rigid_body_reset_angle(obj);
		object_fly_reset_angle(obj);
		model_draw_setup_object(tick,obj);
	
			// detect if model is in view
			
		obj->model_in_view=obj->shadow_in_view=FALSE;
		
		if (is_camera) {
			obj->shadow_in_view=TRUE;
		}
		else {
			if (!view_setup_model_in_view(&obj->draw,obj->mesh.cur_mesh_idx)) continue;
			
			obj->model_in_view=obj->shadow_in_view=TRUE;
			
			// supergumba -- deal with shadows in view here
		}
		
			// add to draw list

		view_add_draw_list(view_render_type_object,n,obj->draw.draw_dist);
	
			// setup model animations for models in view
		
		model_calc_animation(&obj->draw);
		model_calc_draw_bones(&obj->draw);
		
			// setup held weapon model

		if (obj->held_weapon.current_uid!=-1) {

			weap=weapon_find_uid(obj->held_weapon.current_uid);
			if (weap!=NULL) {
				model_draw_setup_weapon(tick,obj,weap,FALSE,FALSE);
				view_setup_model_in_view(&weap->draw,obj->mesh.cur_mesh_idx);
			}
		}
	}
}

void view_setup_projectiles(int tick)
{
	int					n,mesh_idx;
	proj_type			*proj;

	for (n=0;n!=server.count.proj;n++) {
		proj=&server.projs[n];
		
			// setup model positions
			
		projectile_reset_angle_for_flight(proj);
		model_draw_setup_projectile(tick,proj);
		
			// find model and shadows in view
			
		mesh_idx=map_mesh_find(&map,&proj->draw.pnt);
		if (!view_setup_model_in_view(&proj->draw,mesh_idx)) continue;

			// add to draw list

		view_add_draw_list(view_render_type_projectile,n,proj->draw.draw_dist);
		
			// setup model animations for models in view
			
		model_calc_animation(&proj->draw);
		model_calc_draw_bones(&proj->draw);
	}
}

/* =======================================================

      Effects in View
      
======================================================= */

void view_add_effect_draw_list(int tick)
{
	int					n;
	double				d,obscure_dist;
	effect_type			*effect;

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.plane.far_z-camera.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

		// find all effects

	for (n=0;n!=server.count.effect;n++) {

		effect=&server.effects[n];

			// effect inside a mesh that's hidden?

		if (effect->mesh_idx!=-1) {
			if (!view_mesh_in_draw_list(effect->mesh_idx)) continue;
		}

			// auto-eliminate effects drawn outside the obscure distance
				
		d=distance_to_view_center(effect->pnt.x,effect->pnt.y,effect->pnt.z);
		if (d>obscure_dist) continue;
				
			// within a certain distance, don't check for obscuring
			// by the view to fix some bugs with large polygons very
			// close to the camera

		if (d>view_never_obscure_dist) {
			if (!effect_inview(effect,(tick-effect->start_tick))) continue;
		}
		
			// sort liquids into drawing list

		view_add_draw_list(view_render_type_effect,n,d);
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
				if (draw->no_rot.on) gl_project_fix_rotation(&x,&y,&z);
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
		view.render->camera.fov=weap->zoom.fov_max;
		return;
	}
		
	f_step=(float)weap->zoom.current_step;
	f_max_step=(float)(weap->zoom.step_count-1);
	
	view.render->camera.fov=weap->zoom.fov_max-(((weap->zoom.fov_max-weap->zoom.fov_min)/f_max_step)*f_step);
}

void view_calculate_recoil(obj_type *obj)
{
	weapon_type		*weap;

	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	weapon_recoil_add(obj,weap,&view.render->camera.ang);
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
	
	view.render->camera.ang.x=angle_add(view.render->camera.ang.x,((float)k/25));
}

void view_calculate_sways(int tick,obj_type *obj)
{
	float				f;

	return;

	f=(float)(((tick>>4)%12)-6);
	view.render->camera.ang.z+=f;

	// supergumba -- work on sways

}

void view_calculate_bump(obj_type *obj)
{
	if (obj->bump.on) view.render->camera.pnt.y+=obj->bump.smooth_offset;
}

/* =======================================================

      View Script 3D to 2D transform
      
======================================================= */

void view_script_transform_3D_to_2D(int x,int y,int z,int *x2,int *y2)
{
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
	gl_project_point(&x,&y,&z);
	
	*x2=x;
	*y2=y;
}

