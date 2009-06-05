/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Setups

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

#include "scripts.h"
#include "objects.h"
#include "projectiles.h"
#include "models.h"
#include "effects.h"

extern server_type			server;
extern js_type				js;

extern int mark_find(char *name);

/* =======================================================

      Initialize Projectile Setup List
      
======================================================= */

void proj_setup_initialize_list(void)
{
	server.proj_setups=NULL;
	server.count.proj_setup=0;
	server.uid.proj_setup=0;
}

/* =======================================================

      Find Projectile Setup
      
======================================================= */

proj_setup_type* proj_setups_find_uid(int uid)
{
	int				i;
	proj_setup_type	*proj_setup;

	proj_setup=server.proj_setups;
	
	for (i=0;i!=server.count.proj_setup;i++) {
		if (proj_setup->uid==uid)  return(proj_setup);
		proj_setup++;
	}
	
	return(NULL);
}

proj_setup_type* find_proj_setups(weapon_type *weap,char *name)
{
	int				i,weap_uid;
	proj_setup_type	*proj_setup;

	weap_uid=weap->uid;
	proj_setup=server.proj_setups;
	
	for (i=0;i!=server.count.proj_setup;i++) {
		if (proj_setup->weap_uid==weap_uid) {
			if (strcasecmp(proj_setup->name,name)==0) return(proj_setup);
		}
		proj_setup++;
	}
	
	return(NULL);
}

/* =======================================================

      Add Projectile Setup to List
      
======================================================= */

bool proj_setup_add(obj_type *obj,weapon_type *weap,char *name)
{
	proj_setup_type		*proj_setup,*ptr;
	
		// only allow a maximum number of projectile setups

	if (server.count.proj_setup>=max_proj_setup) return(FALSE);

		// create memory for new projectile setup

	ptr=(proj_setup_type*)malloc(sizeof(proj_setup_type)*(server.count.proj_setup+1));
	if (ptr==NULL) return(FALSE);

	if (server.proj_setups!=NULL) {
		memmove(ptr,server.proj_setups,(sizeof(proj_setup_type)*server.count.proj_setup));
		free(server.proj_setups);
	}

	server.proj_setups=ptr;

	proj_setup=&server.proj_setups[server.count.proj_setup];
	server.count.proj_setup++;

		// initialize projectile setup
	
	proj_setup->uid=server.uid.proj_setup;
	server.uid.proj_setup++;
	
	proj_setup->weap_uid=weap->uid;
	proj_setup->obj_uid=obj->uid;
	
	strcpy(proj_setup->name,name);
	
	proj_setup->mark.on=FALSE;
	proj_setup->mark.size=map_enlarge;
	proj_setup->mark.alpha=1;
	proj_setup->mark.idx=-1;
	proj_setup->mark.name[0]=0x0;

	proj_setup->speed=map_enlarge;
	proj_setup->decel_speed=0;
	proj_setup->decel_grace=0;
	proj_setup->decel_min_speed=0;
	proj_setup->inherit_motion_factor=0.0f;
	
	proj_setup->collision=FALSE;
	proj_setup->reset_angle=FALSE;
	proj_setup->damage=0;
	
	proj_setup->action.hit_tick=0;
	proj_setup->action.bounce=FALSE;
	proj_setup->action.bounce_min_move=10.0f;
	proj_setup->action.bounce_reduce=1.0f;
	proj_setup->action.reflect=FALSE;

	object_clear_size(&proj_setup->size);
	
	proj_setup->hitscan.on=FALSE;
	proj_setup->hitscan.max_dist=map_enlarge*100;
	
	proj_setup->push.on=FALSE;
	proj_setup->push.force=0;
	
	proj_setup->melee.strike_bone_tag=model_null_tag;
	proj_setup->melee.strike_pose_name[0]=0x0;
	proj_setup->melee.radius=0;
	proj_setup->melee.distance=0;
	proj_setup->melee.damage=0;
	proj_setup->melee.force=0;
	proj_setup->melee.fall_off=TRUE;
	
	object_clear_draw(&proj_setup->draw);
	
	proj_setup->attach.script_uid=-1;
	
	return(TRUE);
}

/* =======================================================

      Projectile Radius
      
======================================================= */

void proj_setup_set_radius(proj_setup_type *proj_setup)
{
	int			radius;
	
	radius=proj_setup->size.x;
	if (proj_setup->size.z>radius) radius=proj_setup->size.z;
	
	proj_setup->size.radius=radius>>1;
}

/* =======================================================

      Attaching Mark
      
======================================================= */

void proj_setup_attach_mark(proj_setup_type *proj_setup)
{
	proj_setup->mark.idx=mark_find(proj_setup->mark.name);
}

/* =======================================================

      Get Script Projectile Setup
      
======================================================= */

proj_setup_type* proj_setup_get_attach(void)
{
	proj_type			*proj;
	
	if (js.attach.thing_type==thing_type_projectile_setup) {
		return(proj_setups_find_uid(js.attach.thing_uid));
	}
	
	if (js.attach.thing_type==thing_type_projectile) {
		proj=projectile_find_uid(js.attach.thing_uid);
		return(proj_setups_find_uid(proj->proj_setup_uid));
	}
	
	return(NULL);
}

proj_type* proj_get_attach(void)
{
	if (js.attach.thing_type==thing_type_projectile) {
		return(projectile_find_uid(js.attach.thing_uid));
	}
	
	return(NULL);
}

/* =======================================================

      Start/Dispose Projectile Setups
      
======================================================= */

void proj_setup_start(proj_setup_type *proj_setup)
{
	char			err_str[256];

	proj_setup->attach.thing_type=thing_type_projectile_setup;
	proj_setup->attach.thing_uid=proj_setup->uid;

	scripts_clear_attach_data(&proj_setup->attach);

	scripts_add_console(&proj_setup->attach,"Projectiles",proj_setup->name,NULL,err_str);
	model_load_and_init(&proj_setup->draw);
}

void proj_setup_dispose(int idx)
{
	proj_setup_type	*proj_setup,*ptr;

	proj_setup=&server.proj_setups[idx];

		// clear setup

	scripts_dispose(proj_setup->attach.script_uid);
	models_dispose(&proj_setup->draw);

		// is the list completely empty?

	if (server.count.proj_setup==1) {
		free(server.proj_setups);
		server.proj_setups=NULL;
		server.count.proj_setup=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(proj_setup_type*)malloc(sizeof(proj_setup_type)*(server.count.proj_setup-1));

	if (ptr==NULL) {
		if (idx<(server.count.proj_setup-1)) {
			memmove(&server.proj_setups[idx],&server.proj_setups[idx+1],(sizeof(proj_setup_type)*((server.count.proj_setup-idx)-1)));
		}
	}
	else {

		if (idx>0) {
			memmove(ptr,server.proj_setups,(sizeof(proj_setup_type)*idx));
		}
		if (idx<(server.count.proj_setup-1)) {
			memmove(&ptr[idx],&server.proj_setups[idx+1],(sizeof(proj_setup_type)*((server.count.proj_setup-idx)-1)));
		}

		free(server.proj_setups);
		server.proj_setups=ptr;
	}
	
	server.count.proj_setup--;
}

