/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Melee

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
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "physics.h"
#include "interfaces.h"
#include "consoles.h"
#include "video.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Add Melee
      
======================================================= */

void melee_add(obj_type *obj,weapon_type *weap,d3pnt *pt,d3ang *ang,melee_type *melee,int ignore_uid)
{
	int				n,x,y,z,xadd,zadd,damage,dist;
	bool			hit;
	char			weap_name[name_str_len];
	d3pnt			pnt;
	obj_type		*hurt_obj;
	proj_type		*proj;

		// fail under liquid?

	if (weap!=NULL) {
		if ((weap->fail_in_liquid) && (obj->liquid_mode==lm_under)) return;
	}
	
		// original position for network melees
		
	pnt.x=pt->x;
	pnt.y=pt->y;
	pnt.z=pt->z;
    
		// move melee ahead for distance
		
	angle_get_movement(ang->y,melee->distance,&xadd,&zadd);

	x=pt->x+xadd;
	y=pt->y;
	z=pt->z+zadd;
	
		// check objects
		
	for (n=0;n!=server.count.obj;n++) {
	
		hurt_obj=&server.objs[n];

        if ((hurt_obj->hidden) || (!hurt_obj->contact.projectile_on) || (hurt_obj->uid==ignore_uid)) continue;
		
			// melee hit?
			
		if (!collide_sphere_to_object(x,y,z,melee->radius,hurt_obj)) continue;
		
			// which hit box?
		
		hit=TRUE;
		
		if (hurt_obj->hit_box.on) {
			hit=collide_set_object_hit_box_for_sphere_hit(x,y,z,melee->radius,hurt_obj);
		}

			// get damage

		damage=melee->damage;

		if (melee->fall_off) {
			dist=distance_2D_get(x,z,hurt_obj->pnt.x,hurt_obj->pnt.z);
			dist-=obj->size.radius;

			if (dist>0) damage=damage-((damage*dist)/melee->radius);
			if (damage<1) damage=1;
		}
		
			// hurt
            
		if (hit) {
			object_damage(hurt_obj,obj,weap,NULL,pt,damage);
			if (weap==NULL) {
				scripts_post_event_console(&obj->attach,sd_event_melee,sd_event_melee_hit,0);
			}
			else {
				scripts_post_event_console(&weap->attach,sd_event_melee,sd_event_melee_hit,0);
			}
			scripts_post_event_console(&hurt_obj->attach,sd_event_melee,sd_event_melee_hit,0);
		}
	}
  
		// check projectiles
		
	proj=server.projs;
	
	for (n=0;n!=server.count.proj;n++) {
		if (collide_sphere_to_projectile(x,y,z,melee->radius,proj)) proj->flag_melee_hit=TRUE;		// flag the melee hit
		proj++;
	}
	
		// do any pushes
		
	if (melee->force!=0) {
		collide_push_objects(x,y,z,melee->radius,melee->force);
	}
	
		// if this object is the player object, then spawn melee in remotes
		
	if (net_setup.client.joined) {
		if ((obj->uid==server.player_obj_uid) || (obj->bot)) {
			weap_name[0]=0x0;
			if (weap!=NULL) strcpy(weap_name,weap->name);
			net_client_send_melee_add(obj->remote.uid,weap_name,melee->radius,melee->distance,melee->damage,melee->force,&pnt,ang);
		}
	}
}

/* =======================================================

      Get Melee Strike Position
      
======================================================= */

bool melee_strike_position_weapon_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str)
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

	pose_idx=model_find_pose(mdl,weap->melee.strike_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no strike pose");
		return(FALSE);
	}
	
		// get 'fire' bone offset

	bone_idx=model_find_bone(mdl,weap->melee.strike_bone_tag);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no strike bone");
		return(FALSE);
	}

	model_draw_setup_weapon(tick,obj,weap,TRUE,FALSE);

	setup=&weap->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);
	
		// move fire point to weapon

	fire_pnt->x+=weap->draw.pnt.x;
	fire_pnt->y+=weap->draw.pnt.y+obj->duck.y_move;
	fire_pnt->z+=weap->draw.pnt.z;

	if (weap->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);

	return(TRUE);
}

bool melee_strike_position_object_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str)
{
	int						pose_idx,bone_idx;
	melee_type				*melee;
	model_type				*mdl;
	model_draw_setup		*setup;

		// get correct melee settings

	if (weap==NULL) {
		melee=&obj->melee;
	}
	else {
		melee=&weap->melee;
	}

		// get model

	mdl=model_find_uid(obj->draw.uid);
	if (mdl==NULL) {
		if (err_str!=NULL) strcpy(err_str,"Object has no model");
		return(FALSE);
	}

		// get current pose

	pose_idx=model_find_pose(mdl,melee->object_strike_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no strike pose");
		return(FALSE);
	}
	
		// get 'fire' bone offset

	bone_idx=model_find_bone(mdl,melee->object_strike_bone_tag);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no strike bone");
		return(FALSE);
	}
	
	model_draw_setup_object(tick,obj);

	setup=&obj->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);
	
		// move fire point to obj

	fire_pnt->x+=obj->draw.pnt.x;
	fire_pnt->y+=obj->draw.pnt.y+obj->duck.y_move;
	fire_pnt->z+=obj->draw.pnt.z;

	if (obj->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);

	return(TRUE);
}

bool melee_strike_position_projectile_model(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,d3pnt *fire_pnt,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw_setup		*setup;

		// get model

	mdl=model_find_uid(proj->draw.uid);
	if (mdl==NULL) {
		if (err_str!=NULL) strcpy(err_str,"Projectile has no model");
		return(FALSE);
	}

		// get current pose

	pose_idx=model_find_pose(mdl,proj_setup->melee.strike_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Projectile has missing or no strike pose");
		return(FALSE);
	}
	
		// get 'fire' bone offset

	bone_idx=model_find_bone(mdl,proj_setup->melee.strike_bone_tag);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Projectile has missing or no strike bone");
		return(FALSE);
	}

	model_draw_setup_projectile(tick,proj);

	setup=&proj->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);
	
		// move fire point to projectile

	fire_pnt->x+=proj->draw.pnt.x;
	fire_pnt->y+=proj->draw.pnt.y;
	fire_pnt->z+=proj->draw.pnt.z;

	if (proj->draw.no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&fire_pnt->x,&fire_pnt->y,&fire_pnt->z);

	return(TRUE);
}

/* =======================================================

      Script Melee Spawning
      
======================================================= */

bool melee_script_spawn_weapon_model(int tick,obj_type *obj,weapon_type *weap,char *err_str)
{
	d3pnt				pt;

		// get strike position

	if (!melee_strike_position_weapon_model(tick,obj,weap,&pt,err_str)) return(FALSE);

		// add melee
		
	melee_add(obj,weap,&pt,&obj->ang,&weap->melee,obj->uid);

	return(TRUE);
}

bool melee_script_spawn_object_model(int tick,obj_type *obj,weapon_type *weap,char *err_str)
{
	d3pnt				pt;

		// get strike position

	if (!melee_strike_position_object_model(tick,obj,weap,&pt,err_str)) return(FALSE);

		// add melee
	
	if (weap==NULL) {
		melee_add(obj,NULL,&pt,&obj->ang,&obj->melee,obj->uid);
	}
	else {
		melee_add(obj,weap,&pt,&obj->ang,&weap->melee,obj->uid);
	}

	return(TRUE);
}

bool melee_script_spawn_projectile_model(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,char *err_str)
{
	d3pnt				pt;
	
		// get strike position

	if (!melee_strike_position_projectile_model(tick,obj,weap,proj_setup,proj,&pt,err_str)) return(FALSE);

		// add melee
		
	melee_add(obj,weap,&pt,&proj->ang,&proj_setup->melee,-1);

	return(TRUE);
}

