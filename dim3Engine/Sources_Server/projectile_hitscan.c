/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Hit Scans

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

#include "client.h"
#include "scripts.h"
#include "objects.h"
#include "remotes.h"
#include "projectiles.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      HitScan Projectile																																			
      
======================================================= */

void projectile_hitscan(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang)
{
	d3pnt				spt,hpt;
	proj_type			*proj,*hit_proj;
	ray_trace_contact	contact;
	
		// setup projectile
		
	proj=projectile_create(tick,obj,weap,proj_setup);
    if (proj==NULL) return;

	projectile_set_position(proj,pt,ang);
	projectile_set_origin(proj);
	memmove(&proj->size,&proj_setup->size,sizeof(obj_size));
	
		// call spawn
		
	scripts_post_event_console(&proj->attach,sd_event_spawn,0,0);
		
		// if this object is the player object, then spawn projectile in remotes
		
	if (net_setup.client.joined) {
		if (obj->uid==server.player_obj_uid) {
			network_client_send_hitscan_add(net_setup.client.remote_uid,weap->name,proj_setup->name,pt,ang);
		}
	}
	
		// hitscan
		
	spt.x=proj->pos.x;
	spt.y=proj->pos.y;
	spt.z=proj->pos.z;
	
	contact.obj_on=TRUE;
	contact.proj_on=proj_setup->collision;
	contact.obj_ignore_uid=obj->uid;
	contact.proj_ignore_uid=proj->uid;
	
	ray_trace_map_by_angle(&spt,ang,proj_setup->hitscan.max_dist,&hpt,&contact);

		// reset position and contacts
		
	proj->pos.x=hpt.x;
	proj->pos.y=hpt.y;
	proj->pos.z=hpt.z;
	
	map_find_portal_by_pos(&map,&proj->pos);
	
	proj->contact.wall_seg_idx=proj->contact.floor_seg_idx=proj->contact.ceiling_seg_idx=proj->contact.liquid_seg_idx=-1;

	if (contact.seg_idx!=-1) {

		switch(map.segments[contact.seg_idx].type) {
			case sg_wall:
				proj->contact.wall_seg_idx=contact.seg_idx;
				break;
			case sg_floor:
				proj->contact.floor_seg_idx=contact.seg_idx;
				break;
			case sg_ceiling:
				proj->contact.ceiling_seg_idx=contact.seg_idx;
				break;
		}
	}

	proj->contact.obj_uid=contact.obj_uid;
	proj->contact.proj_uid=contact.proj_uid;
	proj->contact.melee=FALSE;
	
		// projectile collisions force other projectile into hit mode
			
	if (contact.proj_uid!=-1) {
		hit_proj=projectile_find_uid(contact.proj_uid);
		hit_proj->contact.proj_uid=proj->uid;
		if (projectile_hit(tick,hit_proj,TRUE)) {				// force other projectile into hit mode
			projectile_mark_dispose(hit_proj);
		}
	}

		// hit projectile
			
	projectile_hit(tick,proj,TRUE);
	
		// destroy projectile
	
	projectile_mark_dispose(proj);
	projectile_dispose();
}
