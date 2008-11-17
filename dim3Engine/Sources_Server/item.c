/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Item Routines

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
#include "physics.h"
#include "network.h"

extern server_type			server;
extern network_setup_type	net_setup;

extern bool collide_object_to_item(obj_type *obj1,obj_type *obj2);

/* =======================================================

      Check for Object Picking Up Item
      
======================================================= */

void item_pickup_check(obj_type *obj)
{
	int								n;
	bool							network_on;
	obj_type						*item_obj;
	network_request_remote_pickup	pickup;
	
		// can pick up if you are hidden, no contact,
		// set to ignore pickup, or are a pickup item
		// yourself
		
	if (obj->hidden) return;
	if (!obj->contact.object_on) return;
	if (obj->pickup.on) return;
	if (obj->pickup.ignore) return;

		// dead players can't pickup items

	if ((obj->player) && (obj->status.health==0)) return;

		// detect if we need to send synch
		// pickup network messages

	network_on=(net_setup.client.joined) && (obj->uid==server.player_obj_uid);

		// check for collisions with pickup items
	
	item_obj=server.objs;
    
	for (n=0;n!=server.count.obj;n++) {
    
		if ((item_obj->hidden) || (!item_obj->pickup.on) || (!item_obj->contact.object_on) || (item_obj->uid==obj->uid)) {
			item_obj++;
			continue;
		}

            // check bounds
			
		if (collide_object_to_object(obj,0,0,item_obj,TRUE,TRUE)) {
	
				// setup pickup for items
				
			item_obj->pickup.item_uid=-1;
			item_obj->pickup.obj_uid=obj->uid;
			item_obj->pickup.canceled=FALSE;

				// setup pickup for objects

			obj->pickup.item_uid=item_obj->uid;
			obj->pickup.obj_uid=-1;

				// need to setup any network messages
				// before the pickup event as it could
				// change parts of the object

			if (network_on) net_client_setup_pickup(obj,&pickup);

				// send pickup event to item

			scripts_post_event_console(&item_obj->attach,sd_event_pickup,0,0);
			
			if (item_obj->pickup.canceled) {			// pickup was canceled by script
				item_obj++;
				continue;
			}
		
				// send pickup event to object
				
			scripts_post_event_console(&obj->attach,sd_event_pickup,0,0);

				// send network event

			if (network_on) net_client_send_pickup(net_setup.client.remote_uid,&pickup);
			
				// successfully picked up
				
			obj->item_count=50;
		}
		
		item_obj++;
	}
}

/* =======================================================

      Item Actions
      
======================================================= */

void item_add_ammo(obj_type *obj,weapon_type *weap,int add_count)
{
    weap->ammo.count+=add_count;
    if (weap->ammo.count>weap->ammo.max_count) weap->ammo.count=weap->ammo.max_count;
}

void item_add_weapon(obj_type *obj,weapon_type *weap)
{
		// if already own weapon, then just add ammo

	if (!weap->hidden) {
		item_add_ammo(obj,weap,weap->ammo.init_count);
		return;
	}

		// add the weapon

	weap->hidden=FALSE;
	weapon_reset_ammo(weap);
}

void item_swap_weapon(obj_type *obj,weapon_type *weap)
{
	weapon_type		*weap_hide;

		// if already own weapon, then just add ammo

	if (!weap->hidden) {
		item_add_ammo(obj,weap,weap->ammo.init_count);
		return;
	}

		// hide current weapon

	weap_hide=weapon_find_current(obj);
	weap_hide->hidden=TRUE;

		// add this weapon

	weap->hidden=FALSE;
	weapon_reset_ammo(weap);

	weapon_set(obj,weap);
}

void item_add_clip(obj_type *obj,weapon_type *weap,int add_count)
{
	if (!weap->ammo.use_clips) return;
    
    weap->ammo.clip_count+=add_count;
    if (weap->ammo.clip_count>weap->ammo.max_clip_count) weap->ammo.clip_count=weap->ammo.max_clip_count;
}

void item_add_alt_ammo(obj_type *obj,weapon_type *weap,int add_count)
{
    weap->alt_ammo.count+=add_count;
    if (weap->alt_ammo.count>weap->alt_ammo.max_count) weap->alt_ammo.count=weap->alt_ammo.max_count;
}

void item_add_alt_clip(obj_type *obj,weapon_type *weap,int add_count)
{
	if (!weap->alt_ammo.use_clips) return;
    
    weap->alt_ammo.clip_count+=add_count;
    if (weap->alt_ammo.clip_count>weap->alt_ammo.max_clip_count) weap->alt_ammo.clip_count=weap->alt_ammo.max_clip_count;
}

void item_add_health(obj_type *obj,int add_count)
{
	object_heal(obj,add_count);
}

void item_add_custom(obj_type *obj,int custom_id)
{
	scripts_post_event_console(&obj->attach,sd_event_add_custom,0,custom_id);
}
