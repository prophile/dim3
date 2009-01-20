/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object State

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
#include "models.h"
#include "physics.h"

extern int					game_obj_rule_uid;

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern js_type				js;
extern hud_type				hud;
extern network_setup_type	net_setup;

/* =======================================================

      Object Spawning
      
======================================================= */

void object_spawn(obj_type *obj)
{
    obj->status.health=obj->status.start_health;
    obj->spawning=FALSE;
	
	object_reset_prepare(obj);
    
	scripts_post_event_console(&obj->attach,sd_event_spawn,0,0);
}

/* =======================================================

      Object Scoring
      
======================================================= */

void object_score_update(obj_type *obj)
{
		// only run rules for players or bots

	if ((obj->uid!=server.player_obj_uid) && (!obj->bot)) return;

		// run rule to update score

	game_obj_rule_uid=obj->uid;
	scripts_post_event_console(&js.game_attach,sd_event_rule,sd_event_rule_score,0);
	game_obj_rule_uid=-1;
}

void object_score_death(obj_type *obj)
{
	obj_type		*source_obj;
	
		// suicide
		
	if ((obj->uid==obj->damage_obj_uid) || (obj->damage_obj_uid==-1)) {
		obj->score.suicide++;
		object_score_update(obj);
	}
	else {

			// death

		obj->score.death++;
		object_score_update(obj);
	
			// kill
		
		if (obj->damage_obj_uid!=-1) {
			source_obj=object_find_uid(obj->damage_obj_uid);
		
			source_obj->score.kill++;
			object_score_update(source_obj);
		}
	}
}

void object_score_goal(obj_type *obj)
{
	obj->score.goal++;
	object_score_update(obj);
}

/* =======================================================

      Object Death
      
======================================================= */

void object_death(obj_type *obj)
{
		// has a death been triggered?
		
	if (!obj->death_trigger) return;
	
		// remove all health
		
    obj->status.health=0;
    
		// send death message
		
	scripts_post_event_console(&obj->attach,sd_event_die,0,0);
	
		// change score
		
	object_score_death(obj);
	
		// turn off trigger
		
	obj->death_trigger=FALSE;

		// send alert to all objects with watches on

	object_watch_death_alert(obj);
	
		// send death if joined and is player
		
	if (net_setup.client.joined) {
		if ((obj->uid==server.player_obj_uid) || (obj->bot)) net_client_send_death(obj->remote.uid,obj->damage_obj_uid,FALSE);
	}
}

/* =======================================================

      Object Telefragging Check
      
======================================================= */

void object_telefrag(obj_type *obj,obj_type *source_obj)
{
	scripts_post_event_console(&obj->attach,sd_event_telefrag,0,0);
	
	obj->death_trigger=TRUE;		// trigger death
	obj->damage_obj_uid=source_obj->uid;
	
	object_death(obj);
}

void object_telefrag_check(obj_type *obj)
{
	int			n;
	obj_type	*check_obj,*hit_obj;

		// only players, remotes, and bots can telefrag

	if ((!obj->player) && (!obj->remote.on) && (!obj->bot)) return;

		// colliding with remotes
		
	hit_obj=NULL;
	
	for (n=0;n!=server.count.obj;n++) {
		check_obj=&server.objs[n];
	
		if ((!check_obj->player) && (!check_obj->remote.on) && (!check_obj->bot)) continue;
		if (obj->uid==check_obj->uid) continue;
		
		if (collide_object_to_object(obj,0,0,check_obj,TRUE,FALSE)) {
			hit_obj=check_obj;
			break;
		}
	}
	
	if (hit_obj==NULL) return;
	
		// telefrag
		
	object_telefrag(obj,hit_obj);

		// send network message to telefrag

	if (net_setup.client.joined) {
		if ((obj->uid==server.player_obj_uid) || (obj->bot)) {
			net_client_send_death(obj->remote.uid,hit_obj->uid,TRUE);
		}
	}
}

/* =======================================================

      Setup Object Touch
      
======================================================= */

void object_setup_touch(obj_type *obj,obj_type *source_obj,bool stand)
{
	obj_touch		*touch;
	
	touch=&obj->touch;
	
	touch->obj_uid=source_obj->uid;
	
	touch->pnt.x=(obj->pnt.x+source_obj->pnt.x)>>1;
	touch->pnt.y=(obj->pnt.y+source_obj->pnt.y)>>1;
	touch->pnt.z=(obj->pnt.z+source_obj->pnt.z)>>1;
	
	touch->ang.x=angle_find(source_obj->pnt.y,source_obj->pnt.z,obj->pnt.y,obj->pnt.z);
	touch->ang.y=angle_find(source_obj->pnt.x,source_obj->pnt.z,obj->pnt.x,obj->pnt.z);
	touch->ang.z=angle_find(source_obj->pnt.x,source_obj->pnt.y,obj->pnt.x,obj->pnt.y);
	
	touch->stand=stand;
}

/* =======================================================

      Object Touch
      
======================================================= */

void object_touch(obj_type *obj)
{
	int				uid;
	obj_type		*hit_obj;
	
		// touching objects
	
	uid=obj->contact.obj_uid;
	if (uid==-1) return;
	
		// if standing, don't send touching callbacks
		
	if (obj->stand_obj_uid==uid) return;

		// send callbacks

	hit_obj=object_find_uid(uid);
	if (hit_obj->remote.on) return;
		
	object_setup_touch(hit_obj,obj,FALSE);
	scripts_post_event_console(&hit_obj->attach,sd_event_touch,0,0);
	
	object_setup_touch(obj,hit_obj,FALSE);
	scripts_post_event_console(&obj->attach,sd_event_touch,0,0);
}

/* =======================================================

      Setup Object Hit
      
======================================================= */

void object_setup_hit(obj_type *obj,obj_type *from_obj,weapon_type *from_weap,proj_type *from_proj,d3pnt *melee_hit_pt,int damage)
{
	int				x,z,y;
	obj_hit			*hit;
	model_type		*model;

	hit=&obj->hit;

		// setup hit object, weapon, projectile
		
	hit->obj_uid=-1;
	hit->weap_uid=-1;
	hit->proj_uid=-1;
	
    if (from_obj!=NULL) hit->obj_uid=from_obj->uid;
	if (from_weap!=NULL) hit->weap_uid=from_weap->uid;
	if (from_proj!=NULL) hit->proj_uid=from_proj->uid;
	
		// damage
		
	hit->damage=damage;
	    
		// hit x,z,y from object or projectile
		
	if (from_proj==NULL) {
		if (from_obj!=NULL) {
			if (melee_hit_pt!=NULL) {
				x=melee_hit_pt->x;
				y=melee_hit_pt->y;
				z=melee_hit_pt->z;
			}
			else {
				x=(from_obj->pnt.x+obj->pnt.x)>>1;
				z=(from_obj->pnt.z+obj->pnt.z)>>1;
				y=(from_obj->pnt.y+obj->pnt.y)>>1;
			}
		}
		else {
			x=obj->pnt.x;
			z=obj->pnt.z;
			y=obj->pnt.y;
		}
	}
	else {
		x=from_proj->pnt.x;
		z=from_proj->pnt.z;
		y=from_proj->pnt.y;
	}
	
	hit->pnt.x=x;
	hit->pnt.y=y;
	hit->pnt.z=z;
		
	hit->ang.x=angle_find(obj->pnt.y,obj->pnt.z,y,z);
	hit->ang.y=angle_find(obj->pnt.x,obj->pnt.z,x,z);
	hit->ang.z=angle_find(obj->pnt.x,obj->pnt.y,x,y);
	
		// find the hit boxes
		
	hit->hit_box_name[0]=0x0;

	if ((obj->hit_box.on) && (obj->hit_box.proj_hit_box_idx!=-1)) {
		model=model_find_uid(obj->draw.uid);
		if (model!=NULL) strcpy(hit->hit_box_name,model->hit_boxes[obj->hit_box.proj_hit_box_idx].name);
	}
}

/* =======================================================

      Setup Click Hit
      
======================================================= */

void object_click(obj_type *obj,obj_type *from_obj)
{
	obj_click		*click;

		// setup click structure
		
	click=&obj->click;
	click->current_click_obj_uid=from_obj->uid;
	
		// post the event
		
	scripts_post_event_console(&obj->attach,sd_event_click,0,0);
}

/* =======================================================

      Damage Object
      
======================================================= */

void object_damage(obj_type *obj,obj_type *source_obj,weapon_type *source_weap,proj_type *source_proj,d3pnt *melee_hit_pt,int damage)
{
	if (obj->remote.on) return;
	if (!obj->damage.on) return;
	
		// need to make sure cascading scripts
		// don't re-enter damage calls
		
	if (obj->damage.in_damage) return;
	
        // health
		
	if (!obj->damage.invincible) {
		if (obj->status.health<=0) return;
	
			// setup damage object
			
		if (source_obj==NULL) {
			obj->damage_obj_uid=-1;
		}
		else {
			obj->damage_obj_uid=source_obj->uid;
		}
		
			// remove the health
			
		obj->status.health-=damage;
		if (obj->status.health<=0) {
			obj->status.health=0;
			obj->death_trigger=TRUE;
			return;
		}
	}
	
		// setup callback
		
	object_setup_hit(obj,source_obj,source_weap,source_proj,melee_hit_pt,damage);

		// run callback

	obj->damage.in_damage=TRUE;
	
	scripts_post_event_console(&obj->attach,sd_event_damage,0,0);
	
	obj->damage.in_damage=FALSE;
}

/* =======================================================

      Health Recover and Heal Object
      
======================================================= */

void object_heal(obj_type *obj,int heal)
{
	obj_status		*status;
    
    status=&obj->status;
	
    status->health+=heal;
    if (status->health>status->max_health) status->health=status->max_health;
	
		// alert object health has been added
		
	scripts_post_event_console(&obj->attach,sd_event_health,0,0);
}

void object_health_recover(obj_type *obj)
{
	int			tick;

	if (obj->status.health==0) return;
	if ((obj->status.health_recover_tick==0) || (obj->status.health_recover_amount==0)) return;

	obj->status.health_recover_count+=10;

	tick=obj->status.health_recover_tick*100;
	if (obj->status.health_recover_count<tick) return;

	obj->status.health_recover_count-=tick;

	object_heal(obj,obj->status.health_recover_amount);
}

/* =======================================================

      Object Crushing
      
======================================================= */

void object_crush(obj_type *obj,bool auto_crush)
{
		// no damage, crush, or health
		
	if ((!obj->damage.on) || (!obj->damage.crushable)) return;
	if (obj->status.health<=0) return;
	
		// moving walls automatically crush you
		// and call this routine.  The only crushing
		// that will be needed to be check
		// regularly is crushing from above
		
	if (!auto_crush) if (!map_stand_crush_object(obj)) return;
	
		// send events and post max damage
		
	scripts_post_event_console(&obj->attach,sd_event_crush,0,0);
	object_damage(obj,NULL,NULL,NULL,NULL,obj->status.max_health);
}

/* =======================================================

      Hide or Freeze State Changes
      
======================================================= */

void object_hide(obj_type *obj,bool hide)
{
	if (hide==obj->hidden) return;
	
	obj->hidden=hide;
	
	object_turn_stop(obj);
	object_player_look_stop(obj);
	object_move_stop(obj);
}

void object_input_freeze(obj_type *obj,bool freeze)
{
	if (freeze==obj->input_freeze) return;
	
	obj->input_freeze=freeze;
	
	object_turn_stop(obj);
	object_player_look_stop(obj);
	object_move_stop(obj);
}

/* =======================================================

      Push and Shove Objects
      
======================================================= */

void object_push(obj_type *obj,d3ang *ang,int force,bool external_force)
{
	int				weight;
	float			speed,xmove,zmove,ymove,temp_z;
	
		// can't push hidden, suspended, or no-contact objects
	
	if ((obj->hidden) || (obj->suspend)) return;
	if (external_force) {
		if (!obj->contact.force_on) return;
	}
	else {
		if (!obj->contact.object_on) return;
	}
	
		// push power
	
	weight=obj->size.weight;
	if (weight<100) weight=100;
	if (weight>1000) weight=1000;
	
	speed=(float)((force*500)/weight)*(2-map.settings.resistance);
	
		// push vector
		
	angle_get_movement_float(ang->y,speed,&xmove,&zmove);
	angle_get_movement_float(ang->x,speed,&ymove,&temp_z);
	
	obj->force.vct.x+=xmove;
	obj->force.vct.z+=zmove;
	obj->force.vct.y+=ymove;
	
		// reset gravity
		
	obj->force.gravity=gravity_start_power;
}

void object_shove(obj_type *obj,d3ang *ang,float speed)
{
	float			xmove,zmove,ymove,temp_z;
	
		// can't push hidden, suspended, or no-contact objects
	
	if ((obj->hidden) || (obj->suspend) || (!obj->contact.force_on)) return;
	
		// shove vector
		
	angle_get_movement_float(ang->y,speed,&xmove,&zmove);
	angle_get_movement_float(ang->x,speed,&ymove,&temp_z);
	
	if ((obj->force.vct.x*xmove)<0) obj->force.vct.x=0;
	obj->force.vct.x+=xmove;
	
	if ((obj->force.vct.z*zmove)<0) obj->force.vct.z=0;
	obj->force.vct.z+=zmove;
	
	if ((obj->force.vct.y*ymove)<0) obj->force.vct.y=0;
	obj->force.vct.y+=ymove;
	
		// reset gravity
		
	obj->force.gravity=gravity_start_power;
}

void object_shove_direct(obj_type *obj,d3vct *vct)
{
		// can't push hidden, suspended, or no-contact objects
	
	if ((obj->hidden) || (obj->suspend) || (!obj->contact.force_on)) return;
	
		// shove vector
		
	if ((obj->force.vct.x*vct->x)<0) obj->force.vct.x=0;
	obj->force.vct.x+=vct->x;
	
	if ((obj->force.vct.z*vct->z)<0) obj->force.vct.z=0;
	obj->force.vct.z+=vct->z;
	
	if ((obj->force.vct.y*vct->y)<0) obj->force.vct.y=0;
	obj->force.vct.y+=vct->y;
	
		// reset gravity
		
	obj->force.gravity=gravity_start_power;
}

/* =======================================================

      Object Target
      
======================================================= */

bool object_is_targetted(obj_type *obj,d3col *col)
{
	int				n;
	obj_type		*player_obj;
	weapon_type		*weap;

		// look for any targetting on player's weapons

	player_obj=object_find_uid(server.player_obj_uid);

	weap=server.weapons;
		
	for (n=0;n!=server.count.weapon;n++) {
		if (weap->obj_uid==player_obj->uid) {
			if ((weap->target.on) && (weap->target.obj_uid==obj->uid)) {
				memmove(col,&weap->target.col,sizeof(d3col));
				return(TRUE);
			}
		}
		weap++;
	}

	return(FALSE);
}

/* =======================================================

      Object Set Radar Icon
      
======================================================= */

bool object_set_radar_icon(obj_type *obj)
{
	int				n,nicon;
	
	obj->radar.icon_idx=-1;
	
		// find radar icon
		
	nicon=hud.radar.nicon;
	
	for (n=0;n!=nicon;n++) {
		if (strcasecmp(hud.radar.icons[n].name,obj->radar.icon)==0) {
			obj->radar.icon_idx=n;
			return(TRUE);
		}
	}

		// unknown radar icon

	JS_ReportError(js.cx,"No radar icon exists with this name: %s",obj->radar.icon);
	return(FALSE);
}

/* =======================================================

      Full Screen Effects
      
======================================================= */

void object_fs_tint_start(int tick,obj_type *obj,float r,float g,float b,float alpha,int fade_in_msec,int life_msec,int fade_out_msec)
{
	obj_fs_tint		*tint;

	if (fade_in_msec<=0) fade_in_msec=1;
	if (life_msec<=0) life_msec=1;
	if (fade_out_msec<=0) fade_out_msec=1;

	tint=&obj->fs_effect.tint;

    tint->on=TRUE;
    tint->start_tick=tick;
    tint->fade_in_tick=fade_in_msec;
    tint->life_tick=life_msec;
	tint->fade_out_tick=fade_out_msec;
	tint->alpha=alpha;
    tint->col.r=r;
    tint->col.g=g;
    tint->col.b=b;
}

void object_fade_start(int tick,obj_type *obj,int x,int y,float start_sz,float end_sz,int life_msec,bool auto_clear)
{
	obj_fs_fade		*fade;

	if (life_msec<=0) life_msec=1;

	fade=&obj->fs_effect.fade;

    fade->on=TRUE;
    fade->start_tick=tick;
    fade->life_tick=life_msec;
    fade->center_x=x;
    fade->center_y=y;
    fade->start_size=start_sz;
    fade->end_size=end_sz;
	fade->auto_clear=auto_clear;
}

void object_fade_clear(obj_type *obj)
{
	obj->fs_effect.fade.on=FALSE;
}

void object_fs_effect_run(int tick,obj_type *obj)
{
	obj_fs_tint		*tint;
	obj_fs_fade		*fade;

		// run tint
		
	tint=&obj->fs_effect.tint;

	if (tint->on) {
		if ((tick-tint->start_tick)>=(tint->fade_in_tick+tint->life_tick+tint->fade_out_tick)) tint->on=FALSE;
	}
	
		// run fade
	
	fade=&obj->fs_effect.fade;

	if ((fade->on) && (fade->auto_clear)) {
		if ((tick-fade->start_tick)>=fade->life_tick) fade->on=FALSE;
	}
}

