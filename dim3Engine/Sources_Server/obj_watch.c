/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Watching

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

extern map_type				map;
extern server_type			server;
extern js_type				js;

/* =======================================================

      Object Watching
      
======================================================= */

void object_watch(obj_type *obj)
{
	int			n,dist,x,z,y,kx,kz,ky;
	double		dx,dz,dy;
	bool		is_near;
	obj_type	*watch_obj;
	
		// watching on?
		
	if (!obj->watch.on) return;
	
		// object check properties
		
	x=obj->pos.x;
	z=obj->pos.z;
	y=obj->pos.y;
	dist=obj->watch.dist;
	if (dist==0) return;		// a distance of 0 ignores distance checks (just watches for deaths)

		// check object
	
	for (n=0;n!=server.count.obj;n++) {
	
		watch_obj=&server.objs[n];
		if ((watch_obj->hidden) || (watch_obj==obj)) continue;

            // check dist
			
		is_near=FALSE;
			
		kx=abs(x-watch_obj->pos.x);
		if (kx<=dist) {
			kz=abs(z-watch_obj->pos.z);
			if (kz<=dist) {
				ky=abs(y-watch_obj->pos.y);
				if (ky<=dist) {
					dx=(double)(kx*kx);
					dz=(double)(kz*kz);
					dy=(double)(ky*ky);
					is_near=(((int)sqrt(dx+dz+dy))<=dist);
				}
			}
		}
		
			// has there been a change
		
		if (is_near) {
			if (obj->watch.obj_flags[n]==0x0) {
				obj->watch.obj_flags[n]=0x1;
				obj->watch.obj_uid=watch_obj->uid;
				scripts_post_event_console(&obj->attach,sd_event_watch,sd_event_watch_object_near,0);
			}
		}
		else {
			if (obj->watch.obj_flags[n]==0x1) {
				obj->watch.obj_flags[n]=0x0;
				obj->watch.obj_uid=watch_obj->uid;
				scripts_post_event_console(&obj->attach,sd_event_watch,sd_event_watch_object_far,0);
			}
		}
	}
}

void object_watch_death_alert(obj_type *dead_obj)
{
	int				n;
	obj_type		*obj;
	
		// notify watching objects of death

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->watch.on) {
			obj->watch.obj_uid=dead_obj->uid;
			scripts_post_event_console(&obj->attach,sd_event_watch,sd_event_watch_object_death,0);
		}
		obj++;
	}
}

void object_watch_portal_alert(obj_type *portal_obj,bool entry)
{
	int				n,rn;
	obj_type		*obj;
	
	rn=portal_obj->pos.rn;
	
		// notify watching objects of other object entering their portals

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->watch.on) {
			if (obj->pos.rn==rn) {
				obj->watch.obj_uid=portal_obj->uid;
				if (entry) {
					scripts_post_event_console(&obj->attach,sd_event_watch,sd_event_watch_object_enter_portal,0);
				}
				else {
					scripts_post_event_console(&obj->attach,sd_event_watch,sd_event_watch_object_exit_portal,0);
				}
			}
		}
		obj++;
	}
}

void object_watch_base_alert(portal_type *portal,obj_type *enter_obj,bool entry)
{
	int				n;
	obj_type		*obj;
	
		// notify watching objects of base entries/exits

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->watch.on) {
			obj->watch.obj_uid=enter_obj->uid;
			obj->watch.base_team=portal->msg.base_team;
			if (entry) {
				scripts_post_event_console(&obj->attach,sd_event_watch,sd_event_watch_object_enter_base,0);
			}
			else {
				scripts_post_event_console(&obj->attach,sd_event_watch,sd_event_watch_object_exit_base,0);
			}
		}
		obj++;
	}
}
