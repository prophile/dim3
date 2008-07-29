/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Hits and Collisions

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
#include "weapons.h"
#include "projectiles.h"
#include "effects.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Projectile Sticking
      
======================================================= */

void projectile_stick(proj_type *proj)
{
    int				ignore_obj_uid;
	
	ignore_obj_uid=-1;
   	if (proj->parent_grace>0) ignore_obj_uid=proj->obj_uid;
    
	proj->contact.obj_uid=collide_find_object_for_projectile_hit(proj,ignore_obj_uid);
}	

/* =======================================================

      Projectile Collisions
      
======================================================= */

void projectile_collision(proj_type *proj)
{
	proj_setup_type		*proj_setup;
	
		// projectile collisions on?
		
	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	if (!proj_setup->collision) return;
    
        // was there a melee hit?
        
    if (proj->flag_melee_hit) {
		proj->contact.melee=TRUE;
        return;
    }
    
        // check projectile collisions
		
	proj->contact.proj_uid=collide_find_projectile_for_projectile(proj);
}
        
/* =======================================================

      Projectiles Decals
      
======================================================= */

void projectile_decals(proj_type *proj,proj_setup_type *proj_setup)
{
	int					mark_idx,size;
	float				alpha;
	
		// can decal this projectile hit?

	if (!setup.mark) return;
	if (!proj_setup->mark.on) return;
	if (proj_setup->mark.idx==-1) return;
	if (proj->contact.hit_poly.mesh_idx==-1) return;
	
		// decal hit

	mark_idx=proj_setup->mark.idx;
	size=proj_setup->mark.size;
	alpha=proj_setup->mark.alpha;
	
	decal_add(&proj->pos,&proj->contact.hit_poly,mark_idx,size,alpha);
}

/* =======================================================

      Projectiles Hits
      
======================================================= */

bool projectile_hit(int tick,proj_type *proj,bool hit_scan)
{
	int					uid;
	bool				auto_hit,wall_hit;
    obj_type			*obj,*hurt_obj;
    weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
		// auto-hit?
		
	auto_hit=FALSE;
	if (proj->action.hit_tick!=0) {
		auto_hit=((proj->start_tick+proj->action.hit_tick)<=tick);
	}
	
		// hits?
    
	if (!hit_scan) {			// hit scans always hit
		if ((proj->contact.hit_poly.mesh_idx==-1) && (proj->contact.obj_uid==-1) && (proj->contact.proj_uid==-1) && (!proj->contact.melee) && (!auto_hit)) return(FALSE);
	}
	
		// auto-bounces and reflects
		
	if ((!auto_hit) && (!hit_scan) && (proj->contact.hit_poly.mesh_idx!=-1)) {
		wall_hit=collide_contact_is_wall_hit(&proj->contact.hit_poly);

		if ((proj->action.reflect) && (wall_hit)) {
			projectile_reflect(proj,TRUE);
			return(FALSE);
		}
		if ((proj->action.bounce) && (!wall_hit)) {
			if (!projectile_bounce(proj,proj->action.bounce_min_move,proj->action.bounce_reduce,TRUE)) return(FALSE);
		}
	}
	
		// call projectile hit
		
	scripts_post_event_console(&proj->attach,sd_event_hit,0,0);
	
	if (!proj->dispose) return(FALSE);
   
        // object damage

	obj=object_find_uid(proj->obj_uid); 
	weap=weapon_find_uid(proj->weap_uid);
	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	
	uid=proj->contact.obj_uid;
    if (uid!=-1) {
	
			// damage object
			
        hurt_obj=object_find_uid(uid);
        object_damage(hurt_obj,obj,weap,proj,NULL,proj_setup->damage);
		
			// push object
			
		if (proj_setup->push.on) object_push(hurt_obj,&proj->ang,proj_setup->push.force,TRUE);
    }
	
		// decals
		
	projectile_decals(proj,proj_setup);

	return(TRUE);
}

