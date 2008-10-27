/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Projectiles

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

#include "network.h"
#include "scripts.h"
#include "physics.h"
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "interfaces.h"
#include "consoles.h"
#include "video.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern float object_player_look_constrain(obj_type *obj,weapon_type *weap,float ang_x);

/* =======================================================

      Setup Weapon Fire Script Objects
      
======================================================= */

void weapon_setup_fire(weapon_type *weap,int method)
{
	obj_type		*obj;
	
		// setup fire method
		
	weap->fire.method=method;

		// not canceled

	weap->fire.cancel=FALSE;
	
		// setup weapon fire in object
		
	obj=object_find_uid(weap->obj_uid);

	strcpy(obj->weapon_fire.name,weap->name);
	obj->weapon_fire.method=method;
}

/* =======================================================

      Add Projectile
      
======================================================= */

bool weapon_add_projectile(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang)
{
	d3pnt					spt,ept,hpt;
	proj_type				*proj;
	ray_trace_contact_type	contact;
	
		// create new projectile
		
	proj=projectile_create(tick,obj,weap,proj_setup);
	if (proj==NULL) return(FALSE);

	projectile_spawn_position(proj,pt,ang,obj);
	
		// call spawn
		
	scripts_post_event_console(&proj->attach,sd_event_spawn,0,0);
	
		// if this object is the player object, then spawn projectile in remotes
		
	if (net_setup.client.joined) {
		if (obj->uid==server.player_obj_uid) {
			net_client_send_projectile_add(net_setup.client.remote_uid,weap->name,proj_setup->name,pt,ang);
		}
	}

		// detect weapons being fired into walls
		// if so, destroy projectile

	spt.x=obj->pnt.x;
	spt.y=obj->pnt.y;
	spt.z=obj->pnt.z;

	ept.x=proj->pnt.x;
	ept.y=proj->pnt.y;
	ept.z=proj->pnt.z;
	
	contact.obj_on=TRUE;
	contact.proj_on=FALSE;
	contact.obj_ignore_uid=obj->uid;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_projectile;
	
	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {

		proj->pnt.x=hpt.x;
		proj->pnt.y=hpt.y;
		proj->pnt.z=hpt.z;

		proj->contact.hit_poly.mesh_idx=contact.poly.mesh_idx;
		proj->contact.hit_poly.poly_idx=contact.poly.poly_idx;
		proj->contact.obj_uid=contact.obj_uid;
		proj->contact.proj_uid=-1;

		projectile_hit(tick,proj,FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Get Projectile Fire Position and Angle
      
======================================================= */

bool weapon_get_projectile_position_angle_weapon_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw_setup		*setup;

		// get model

	mdl=model_find_uid(weap->draw.uid);
	if (mdl==NULL) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has no model");
		return(FALSE);
	}

		// get current pose

	pose_idx=model_find_pose(mdl,weap->proj.fire_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire pose");
		return(FALSE);
	}
	
		// get 'fire' bone offset and calc

	bone_idx=model_find_bone(mdl,weap->proj.fire_bone_tag);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire bone");
		return(FALSE);
	}

	model_draw_setup_weapon(tick,obj,weap,TRUE);

	setup=&weap->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);

	fire_pnt->x+=weap->draw.pnt.x;
	fire_pnt->y+=weap->draw.pnt.y;
	fire_pnt->z+=weap->draw.pnt.z;

	if (weap->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);
	
		// fire angle

	fire_ang->x=angle_add(setup->ang.x,obj->draw.setup.ang.x);
	fire_ang->y=angle_add(setup->ang.y,angle_add(obj->draw.setup.ang.y,180.0f));
	fire_ang->z=angle_add(setup->ang.z,obj->draw.setup.ang.z);

	return(TRUE);
}

bool weapon_get_projectile_position_angle_weapon_barrel(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str)
{
	int						pose_idx,bone_idx,dist;
	d3pnt					barrel_pnt;
	model_type				*mdl;
	model_draw_setup		*setup;

		// get model

	mdl=model_find_uid(weap->draw.uid);
	if (mdl==NULL) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has no model");
		return(FALSE);
	}

		// get current pose

	pose_idx=model_find_pose(mdl,weap->proj.fire_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire pose");
		return(FALSE);
	}
	
		// get 'fire' bone offset and calc

	bone_idx=model_find_bone(mdl,weap->proj.fire_bone_tag);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire bone");
		return(FALSE);
	}

	model_draw_setup_weapon(tick,obj,weap,TRUE);

	setup=&weap->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);

	fire_pnt->x+=weap->draw.pnt.x;
	fire_pnt->y+=weap->draw.pnt.y;
	fire_pnt->z+=weap->draw.pnt.z;

	if (weap->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);

		// get 'barrel' bone offset (draw bones already calced above)

	bone_idx=model_find_bone(mdl,weap->proj.barrel_bone_tag);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no barrel bone");
		return(FALSE);
	}

	model_get_draw_bone_position(setup,bone_idx,&barrel_pnt.x,&barrel_pnt.y,&barrel_pnt.z);
	
	barrel_pnt.x+=weap->draw.pnt.x;
	barrel_pnt.y+=weap->draw.pnt.y;
	barrel_pnt.z+=weap->draw.pnt.z;

	if (weap->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&barrel_pnt.x,&barrel_pnt.y,&barrel_pnt.z);

		// angles between them

	dist=distance_2D_get(0,0,(fire_pnt->x-barrel_pnt.x),(fire_pnt->z-barrel_pnt.z));
	fire_ang->x=-(180.0f-angle_find(0,0,(fire_pnt->y-barrel_pnt.y),dist));

	fire_ang->y=angle_find(barrel_pnt.x,barrel_pnt.z,fire_pnt->x,fire_pnt->z);

	fire_ang->z=0;

	return(TRUE);
}

bool weapon_get_projectile_position_angle_object_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw_setup		*setup;
	
		// get model

	mdl=model_find_uid(obj->draw.uid);
	if (mdl==NULL) {
		if (err_str!=NULL) strcpy(err_str,"Object has no model");
		return(FALSE);
	}

		// get current pose

	pose_idx=model_find_pose(mdl,weap->proj.object_fire_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no fire pose");
		return(FALSE);
	}
	
		// get bone offset and calc

	bone_idx=model_find_bone(mdl,weap->proj.object_fire_bone_tag);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no fire bone");
		return(FALSE);
	}

	model_draw_setup_object(tick,obj);

	setup=&obj->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);

		// move fire point in front of obj

	fire_pnt->x+=obj->draw.pnt.x;
	fire_pnt->y+=obj->draw.pnt.y;
	fire_pnt->z+=obj->draw.pnt.z;

	if (weap->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);
	
		// angle from object
		
	fire_ang->x=setup->ang.x;
	fire_ang->y=setup->ang.y;
	fire_ang->z=setup->ang.z;

	return(TRUE);
}

void weapon_get_projectile_position_angle_add_offset(d3ang *ang,d3ang *off_ang)
{
	if (off_ang==NULL) return;
	
	ang->x+=off_ang->x;
	ang->y+=off_ang->y;
	ang->z+=off_ang->z;
}

/* =======================================================

      Obj.Weapon.Fire Method
      
======================================================= */

void weapon_script_fire(int tick,obj_type *obj,weapon_type *weap,int method)
{
		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid_mode==lm_under)) return;

		// send event

	weapon_setup_fire(weap,method);
	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_single,0);

	if (!weap->fire.cancel) weap->fire.last_fire_tick=tick;
}

/* =======================================================

      Weapon.Projectile.Spawn Methods
      
======================================================= */

bool weapon_script_projectile_spawn(int tick,obj_type *obj,weapon_type *weap,char *proj_name,d3pnt *pt,d3ang *ang,int count,float slop,char *err_str)
{
	int					n;
	float				r_slop;
	d3ang				proj_ang;
	proj_setup_type		*proj_setup;

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid_mode==lm_under)) return(TRUE);

		// get projectile setup

	proj_setup=find_proj_setups(weap,proj_name);
	if (proj_setup==NULL) {
		sprintf(err_str,"Named projectile does not exist in weapon: %s",proj_name);
		return(FALSE);
	}

		// create projectiles

	for (n=0;n<count;n++) {

		memmove(&proj_ang,ang,sizeof(d3ang));

			// add in slop

		if (slop!=0.0f) {
			r_slop=slop*2.0f;
			proj_ang.x+=(slop-random_float(r_slop));
			proj_ang.y+=(slop-random_float(r_slop));
			proj_ang.z+=(slop-random_float(r_slop));
		}

			// hit-scan fires
			
		if (proj_setup->hitscan.on) {
			projectile_hitscan(tick,obj,weap,proj_setup,pt,&proj_ang);
		}
		
			// projectile fires
			
		else {

			if (!weapon_add_projectile(tick,obj,weap,proj_setup,pt,&proj_ang)) {
				strcpy(err_str,"No more spots to spawn projectiles");
				return(FALSE);
			}

		}
	}

	return(TRUE);
}

/* =======================================================

      Weapon.Projectile.Spawn Methods (Object Models)
      
======================================================= */

bool weapon_script_projectile_spawn_object_model(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

		// get fire position and angle

	if (!weapon_get_projectile_position_angle_object_model(tick,obj,weap,&pt,&ang,err_str)) return(FALSE);

	weapon_get_projectile_position_angle_add_offset(&ang,off_ang);

		// spawn projectile
		
	return(weapon_script_projectile_spawn(tick,obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

/* =======================================================

      Weapon.Projectile.Spawn Methods (Center)
      
======================================================= */

bool weapon_script_projectile_spawn_center(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

	pt.x=obj->pnt.x;
	pt.z=obj->pnt.z;
	pt.y=(obj->pnt.y+obj->duck.y_move)+obj->size.eye_offset;

	ang.x=-obj->view_ang.x;
	ang.y=obj->ang.y;
	ang.z=0.0f;

	weapon_recoil_add(obj,weap,&ang);
		
	return(weapon_script_projectile_spawn(tick,obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

/* =======================================================

      Weapon.Projectile.Spawn Methods (Weapon Models)
      
======================================================= */

bool weapon_script_projectile_spawn_weapon_model(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

		// if not in fpp, auto-switch to center spawn

	if (camera.mode!=cv_fpp) {
		return(weapon_script_projectile_spawn_center(tick,obj,weap,proj_name,count,slop,off_ang,err_str));
	}

		// get fire position and angle

	if (!weapon_get_projectile_position_angle_weapon_model(tick,obj,weap,&pt,&ang,err_str)) return(FALSE);

	weapon_get_projectile_position_angle_add_offset(&ang,off_ang);

		// spawn projectile
		
	return(weapon_script_projectile_spawn(tick,obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

bool weapon_script_projectile_spawn_weapon_barrel(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

		// if not in fpp, auto-switch to center spawn

	if (camera.mode!=cv_fpp) {
		return(weapon_script_projectile_spawn_center(tick,obj,weap,proj_name,count,slop,off_ang,err_str));
	}
	
		// get fire position and angle

	if (!weapon_get_projectile_position_angle_weapon_barrel(tick,obj,weap,&pt,&ang,err_str)) return(FALSE);
	
	weapon_get_projectile_position_angle_add_offset(&ang,off_ang);

		// spawn projectile

	return(weapon_script_projectile_spawn(tick,obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

/* =======================================================

      Player Fire Weapons
      
======================================================= */

void weapon_player_fire_down(int tick,obj_type *obj,weapon_type *weap,int method)
{
	int			clicked_obj_uid;
	obj_type	*clicked_obj;
	
		// if no weapon, send message directly to object
		
	if (weap==NULL) {
		scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_down,0);
		return;
	}
	
		// disable repeats unless weapon fires
		
	weap->proj.repeat_ok=FALSE;
	
		// if crosshair in click mode, the click
		
	clicked_obj_uid=obj->click.current_click_obj_uid;
	if (clicked_obj_uid!=-1) {
		clicked_obj=object_find_uid(clicked_obj_uid);
		object_click(clicked_obj,obj);
		crosshair_show_alt(tick,obj);
		return;
	}

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid_mode==lm_under)) return;
	
		// handle weapon fire down
		
	weap->proj.next_repeat_tick=tick+weap->proj.repeat_tick;
	
	weapon_setup_fire(weap,method);
	
        // send fire message to weapon first
		
	weap->fire.cancel=FALSE;
	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_down,0);
	if (weap->fire.cancel) return;
	
		// set last fire
		
	weap->fire.last_fire_tick=tick;
	weap->proj.repeat_ok=TRUE;
   
        // if weapon OKs message, send message to player
	
	scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_down,0);
}

void weapon_player_fire_repeat(int tick,obj_type *obj,weapon_type *weap)
{
		// if no weapon then no repeat
		
	if (weap==NULL) return;
	
		// are repeats allowed?
		
	if (!weap->proj.repeat_ok) return;
	
		// no fire messages if crosshair is in click mode
		
	if (obj->click.current_click_obj_uid!=-1) return;

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid_mode==lm_under)) return;
	
		// time to repeat?
		
	if (tick<weap->proj.next_repeat_tick) return;
	weap->proj.next_repeat_tick-=weap->proj.repeat_tick;
	
		// setup firing
		
	weapon_setup_fire(weap,weap->fire.method);
	
        // send fire message to weapon first
		
	weap->fire.cancel=FALSE;
	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_repeat,0);
	if (weap->fire.cancel) return;
	
		// set last fire
		
	weap->fire.last_fire_tick=tick;
    
        // if weapon OKs message, send message to player
        
	scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_repeat,0);
}

void weapon_player_fire_up(obj_type *obj,weapon_type *weap,int method)
{
		// if no weapon, send message directly to object
		
	if (weap==NULL) {
		scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_up,0);
		return;
	}
	
		// no fire messages if crosshair is in click mode
		
	if (obj->click.current_click_obj_uid!=-1) return;

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid_mode==lm_under)) return;
	
		// handle weapon fire up

	weapon_setup_fire(weap,method);
	
        // send fire message to weapon first
		
	weap->fire.cancel=FALSE;
	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_up,0);
	if (weap->fire.cancel) return;
    
        // if weapon OKs message, send message to player
        
	scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_up,0);
}

/* =======================================================

      Kickback Weapons
      
======================================================= */

void weapon_kickback(int tick,obj_type *obj,weapon_type *weap)
{
	d3ang			ang;

	ang.x=0;
	ang.y=angle_add(obj->ang.y,180.0f);
	ang.z=0;

	object_shove(obj,&ang,(float)weap->kickback.size);
}

/* =======================================================

      Clear and Run Recoils

======================================================= */

void weapon_recoil_clear(weapon_type *weap)
{
	weap->recoil.ang.x=0.0f;
	weap->recoil.ang.y=0.0f;
	weap->recoil.ang.z=0.0f;
}

void weapon_recoil_copy(weapon_type *to_weap,weapon_type *from_weap)
{
	to_weap->recoil.ang.x=from_weap->recoil.ang.x;
	to_weap->recoil.ang.y=from_weap->recoil.ang.y;
	to_weap->recoil.ang.z=from_weap->recoil.ang.z;
}

float weapon_recoil_run_single_angle(float ang,float pct)
{
	ang*=(1.0f-pct);

	if ((ang<0.0f) && (ang>=-pct)) return(0.0f);
	if ((ang>0.0f) && (ang<=pct)) return(0.0f);

	return(ang);
}

void weapon_recoil_run(obj_type *obj,weapon_type *weap)
{
	float			look_add,turn_add;

		// regular weapon recoils

	weap->recoil.ang.x=weapon_recoil_run_single_angle(weap->recoil.ang.x,weap->recoil.reset_ang.x);
	weap->recoil.ang.y=weapon_recoil_run_single_angle(weap->recoil.ang.y,weap->recoil.reset_ang.y);
	weap->recoil.ang.z=weapon_recoil_run_single_angle(weap->recoil.ang.z,weap->recoil.reset_ang.z);

		// looking always reduces x recoil

	look_add=obj->look.ang_add;
    if (look_add!=0) {
		weap->recoil.ang.x=weapon_recoil_run_single_angle(weap->recoil.ang.x,weap->recoil.look_reset);
	}

		// turning always reduces x recoil

	turn_add=obj->turn.ang_add.y;
    if (turn_add!=0) {
		weap->recoil.ang.y=weapon_recoil_run_single_angle(weap->recoil.ang.y,weap->recoil.turn_reset);
	}
}

void weapon_recoil_add(obj_type *obj,weapon_type *weap,d3ang *ang)
{
	ang->x=object_player_look_constrain(obj,weap,(ang->x+weap->recoil.ang.x));
	ang->y=angle_add(ang->y,weap->recoil.ang.y);
	ang->z+=weap->recoil.ang.z;
}

/* =======================================================

      Launch Recoils

======================================================= */

float weapon_recoil_single_angle(float min,float max)
{
	float			dif,sign;
	
	if (min==max) return(min);
	
	sign=1.0f;
	dif=max-min;
	
	if (dif<0.0f) {
		sign=-1.0f;
		dif=fabsf(dif);
	}

	return(min+(random_float(dif)*sign));
}

void weapon_recoil(obj_type *obj,weapon_type *weap)
{
	d3ang			ang;

	ang.x=weapon_recoil_single_angle(weap->recoil.min_ang.x,weap->recoil.max_ang.x);
	ang.y=weapon_recoil_single_angle(weap->recoil.min_ang.y,weap->recoil.max_ang.y);
	ang.z=weapon_recoil_single_angle(weap->recoil.min_ang.z,weap->recoil.max_ang.z);

	weap->recoil.ang.x=object_player_look_constrain(obj,weap,(weap->recoil.ang.x-ang.x));
	weap->recoil.ang.y+=ang.y;
	weap->recoil.ang.z+=ang.z;
}
