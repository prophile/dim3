/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Finding

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
#include "physics.h"

extern server_type			server;

/* =======================================================

      Find Objects by Unique ID
      
======================================================= */

obj_type* object_find_uid(int uid)
{
	int				n;
	obj_type		*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		if (obj->uid==uid) return(obj);
		obj++;
	}
	
	return(NULL);
}

int object_find_index_uid(int uid)
{
	int				n;
	obj_type		*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		if (obj->uid==uid) return(n);
		obj++;
	}
	
	return(-1);
}

obj_type* object_find_remote_uid(int uid)
{
	int				n;
	obj_type		*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		if (obj->remote.on) {
			if (obj->remote.uid==uid) return(obj);
		}
		obj++;
	}
	
	return(NULL);
}

int object_find_index_remote_uid(int uid)
{
	int				n;
	obj_type		*obj;
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		if (obj->remote.on) {
			if (obj->remote.uid==uid) return(n);
		}
		obj++;
	}
	
	return(-1);
}

/* =======================================================

      Find Objects by Spawning Index
      
======================================================= */

obj_type* object_find_spawn_idx(int spawn_idx)
{
	int				i;
	obj_type		*obj;
	
	obj=server.objs;
	
	for ((i=0);(i!=server.count.obj);i++) {
		if (obj->spawn_idx==spawn_idx) return(obj);
		obj++;
	}
	
	return(NULL);
}

/* =======================================================

      Find Objects by Name and Tag
      
======================================================= */

obj_type* object_find_name(char *name)
{
	int			i;
	obj_type	*obj;
	
	obj=server.objs;
	
	for ((i=0);(i!=server.count.obj);i++) {
		if (strcasecmp(obj->name,name)==0) return(obj);
		obj++;
	}
	
	return(NULL);
}

/* =======================================================

      Find Objects by Position
      
======================================================= */

obj_type* object_find_nearest(int x,int z,int y,char *name,char *type,int team_idx,float ang,float ang_sweep,int min_dist,int max_dist,bool player,bool remote)
{
	int				n,i,d,dist;
	float			fang;
	obj_type		*obj;

	i=-1;
    dist=max_dist;
	
	for ((n=0);(n!=server.count.obj);n++) {
		obj=&server.objs[n];
		
		if (obj->hidden) continue;
		if (!obj->find_on) continue;
		
			// player and remotes
			
		if (player) {
			if (!obj->player) continue;
		}
		if (remote) {
			if (!obj->remote.on) continue;
		}
		
			// check name
			
		if (name!=NULL) {
			if (strstr(obj->name,name)==NULL) continue;
		}
		
			// check type
			
		if (type!=NULL) {
			if (strcasecmp(obj->type,type)!=0) continue;
		}
		
			// check team
			
		if (team_idx!=-1) {
			if (obj->team_idx!=team_idx) continue;
		}
		
			// check distance
			
		d=distance_get(x,y,z,obj->pnt.x,obj->pnt.y,obj->pnt.z);
		if (d<min_dist) continue;
		if (d>dist) continue;
		
			// check angle
			
		if (ang!=-1) {
			fang=angle_find(x,z,obj->pnt.x,obj->pnt.z);
			if (angle_dif(fang,ang,NULL)>ang_sweep) continue;
		}
		
		i=n;
		dist=d;
	}
	
	if (i==-1) return(NULL);
	
	return(&server.objs[i]);
}

/* =======================================================

      Count Teams
      
======================================================= */

int object_count_team(int team_idx,int ignore_obj_uid)
{
	int				n,count;
	obj_type		*obj;

	count=0;
	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if ((obj->uid!=ignore_obj_uid) && (obj->team_idx==team_idx)) count++;
		obj++;
	}

	return(count);
}

/* =======================================================

      Find Click Object
      
======================================================= */

int object_find_uid_click_object(obj_type *obj)
{
	int				n,uid,d,dist;
	float			fang;
	obj_type		*click_obj;
	
	uid=-1;
	dist=0;

	for (n=0;n!=server.count.obj;n++) {
		click_obj=&server.objs[n];
		
		if (click_obj->hidden) continue;
		if (!click_obj->click.on) continue;
		
			// check distance
			
		d=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,click_obj->pnt.x,click_obj->pnt.y,click_obj->pnt.z);
		if (d>click_obj->click.distance) continue;
		if ((uid!=-1) && (d>dist)) continue;
		
			// check angle
			
		fang=angle_find(obj->pnt.x,obj->pnt.z,click_obj->pnt.x,click_obj->pnt.z);
		if (angle_dif(fang,obj->ang.y,NULL)>ci_object_click_angle) continue;
		
		uid=click_obj->uid;
		dist=d;
	}
	
	return(uid);
}

/* =======================================================

      Find Objects by Standing Under
      
======================================================= */

int object_find_uid_by_stood_on_object_uid(int stand_obj_uid)
{
	int				i;
	obj_type		*obj;
	
	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		if (obj->stand_obj_uid==stand_obj_uid) return(obj->uid);
		obj++;
	}
	
	return(-1);
}

/* =======================================================

      Sight Path Tests
      
======================================================= */

bool object_sight_test_object(obj_type *obj,int test_obj_uid)
{
	int						x,y,side_div,look_div;
	float					side_start,look_start,side_add,look_add;
	d3pnt					spt,hpt;
	d3ang					ang;
	ray_trace_contact_type	contact;

		// setup contact

	contact.obj_on=TRUE;
	contact.proj_on=FALSE;

	contact.obj_ignore_uid=obj->uid;
	contact.proj_ignore_uid=-1;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_object;

		// set divisions

	side_div=obj->sight.side_division;
	if (side_div<=0) side_div=1;

	look_div=obj->sight.look_division;
	if (look_div<=0) look_div=1;

		// get ray angles

	if (side_div==1) {
		side_start=obj->ang.y;
		side_add=0.0f;
	}
	else {
		side_start=obj->ang.y-(obj->sight.side_angle/2.0f);
		side_add=(obj->sight.side_angle/(float)side_div);
	}

	if (look_div==1) {
		look_start=obj->ang.x;
		look_add=0.0f;
	}
	else {
		look_start=obj->ang.x-(obj->sight.look_angle/2.0f);
		look_add=(obj->sight.look_angle/(float)look_div);
	}

		// run the ray traces

	ang.x=look_start;
	ang.z=0.0f;

	for (y=0;y!=look_div;y++) {

		ang.y=side_start;

		for (x=0;x!=side_div;x++) {

				// get starting position

			spt.x=obj->pnt.x;
			spt.y=obj->pnt.y-obj->size.eye_offset;
			spt.z=obj->pnt.z;

			ray_push(&spt,&ang,obj->size.radius);

				// ray trace

			if (ray_trace_map_by_angle(&spt,&ang,obj->sight.distance,&hpt,&contact)) {
				if (contact.obj_uid==test_obj_uid) return(TRUE);
			}

			ang.y=angle_add(ang.y,side_add);		// these angles are constrained 0...360
		}

		ang.x+=look_add;			// these angles are constrained -x...x
	}


	return(FALSE);
}

