/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Movement

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
#include "projectiles.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;

extern bool collide_projectile_to_map(proj_type *proj,int xadd,int yadd,int zadd);	// supergumba

/* =======================================================

      Increase Gravity
      
======================================================= */

void projectile_gravity(proj_type *proj)
{
	float			gravity;
    
		// increase the gravity
	
	gravity=proj->force.gravity;
	if (gravity<map.settings.gravity_max_power) {
		gravity=gravity+((float)proj->size.weight*(map.settings.gravity/gravity_factor));
		if (gravity>map.settings.gravity_max_power) gravity=map.settings.gravity_max_power;
		proj->force.gravity=gravity;
	}
	
		// positive gravity drags down the Y move
	
	if (gravity>0) proj->force.vct.y+=gravity;
}

/* =======================================================

      Projectile Speed
      
======================================================= */

void projectile_speed(proj_type *proj)
{
	if (proj->decel_grace>0) {
		proj->decel_grace--;
		return;
	}

	proj->speed=proj->speed-proj->decel_speed;
	if (proj->speed<proj->decel_min_speed) proj->speed=proj->decel_min_speed;
}

/* =======================================================

      Do Movement
      
======================================================= */

bool projectile_move(proj_type *proj)
{
	int				xmove,zmove,ymove;

		// project movement
		
	projectile_set_motion(proj,proj->speed,proj->motion.ang.y,proj->motion.ang.x,&xmove,&ymove,&zmove);
	if ((xmove==0) && (zmove==0) && (ymove==0)) return(FALSE);
		
		// save old position

	memmove(&proj->last_pnt,&proj->pnt,sizeof(d3pnt));
	
        // move

	collide_projectile_to_map(proj,xmove,ymove,zmove);
	
	return(FALSE);
}

/* =======================================================

      Turn and Seek
      
======================================================= */

void projectile_turn_xz_towards(proj_type *proj,obj_type *to_obj,float turn_add)
{
	float			ang;
	
	ang=angle_find(proj->pnt.x,proj->pnt.z,to_obj->pnt.x,to_obj->pnt.z);
	proj->ang.y=proj->motion.ang.y=angle_turn_toward(proj->ang.y,ang,turn_add);
}

void projectile_thrust_y_towards(proj_type *proj,obj_type *to_obj,float thrust_add)
{
	int			py,oy;
	float		ymove;
	
		// find new thrust

	py=proj->pnt.y-(proj->size.y>>1);
	oy=to_obj->pnt.y-(to_obj->size.y>>1);
	
	ymove=0;

	if (abs(py-oy)>thrust_add) {
		if (py<oy) {
			ymove=(float)(oy-py);
			if (ymove>thrust_add) ymove=thrust_add;
		}
		else {
			ymove=(float)(py-oy);
			if (ymove<-thrust_add) ymove=-thrust_add;
		}
	}

		// smooth out changes

	proj->force.vct.y=(proj->force.vct.y+thrust_add)/2.0f;
}

void projectile_seek(proj_type *proj,obj_type *to_obj,float turn_add,float thrust_add)
{
	projectile_turn_xz_towards(proj,to_obj,turn_add);
	projectile_thrust_y_towards(proj,to_obj,thrust_add);
}

/* =======================================================

      Projectile Bounce
      
======================================================= */

bool projectile_bounce(proj_type *proj,float min_ymove,float reduce,bool send_event)
{
	int					xmove,ymove,zmove;
	float				fy,slope_y;
	poly_pointer_type	*poly;

		// get polygon slope

	poly=&proj->contact.hit_poly;
	slope_y=0.0f;

	if (poly->mesh_idx!=-1) slope_y=map.mesh.meshes[poly->mesh_idx].polys[poly->poly_idx].slope.y;

		// reset to last good position

	memmove(&proj->pnt,&proj->last_pnt,sizeof(d3pnt));

		// if floor is over a certain slope, just reflect instead

	if (slope_y>=proj_reflect_slope_max_y) {
		proj->ang.y=proj->motion.ang.y=angle_add(proj->motion.ang.y,180);
		return(FALSE);
	}

		// if any slope, slow down projectile
		// if slowed down to nothing, then bounce is over

	if (slope_y!=0.0f) {
		proj->speed*=(1.0f-(slope_y/proj_reflect_slope_max_y));
	}

	if (proj->speed<proj_bounce_min_speed) return(TRUE);

		// bounce projectile
	
	projectile_set_motion(proj,proj->speed,proj->motion.ang.y,proj->motion.ang.x,&xmove,&ymove,&zmove);
		
	fy=((float)ymove)*reduce;
	if (fy<min_ymove) fy=min_ymove;
		
	proj->force.vct.y=-fy;
	proj->motion.ang.x=-proj->motion.ang.x;

	if (send_event) scripts_post_event_console(&proj->attach,sd_event_projectile,sd_event_projectile_bounce,0);
	
	return(ymove==min_ymove);
}

/* =======================================================

      Projectile Reflect
      
======================================================= */

float projectile_reflect_angle(proj_type *proj)
{
	int					x,z,y;
	float				f,ang;
	bool				wall_hit;
	d3vct				proj_vct,normal_vct;
	poly_pointer_type	*poly;
	map_mesh_poly_type	*mesh_poly;

		// get information about collided mesh

	poly=&proj->contact.hit_poly;

	mesh_poly=NULL;
	wall_hit=FALSE;
	
	if (poly->mesh_idx!=-1) {
		mesh_poly=&map.mesh.meshes[poly->mesh_idx].polys[poly->poly_idx];
		wall_hit=mesh_poly->box.wall_like;
	}
	
		// if not wall-like, then just reverse
		
	if ((mesh_poly==NULL) || (!wall_hit)) {
		return(proj->motion.ang.y=angle_add(proj->motion.ang.y,180.0f));
	}

		// get the projectile and polygon vector

	projectile_set_motion(proj,1000,proj->motion.ang.y,0,&x,&y,&z);
	
	proj_vct.x=(float)x;			// projectile vector
	proj_vct.y=0.0f;
	proj_vct.z=(float)z;
	vector_normalize(&proj_vct);
	
	vector_create(&normal_vct,mesh_poly->box.min.x,0,mesh_poly->box.min.z,mesh_poly->box.max.x,0,mesh_poly->box.max.z);		// perpendicular vector (swap x/z)
	
		// get the angle between them

	f=vector_dot_product(&proj_vct,&normal_vct);
	f=((float)acos(f))*RAD_to_ANG;

	if (f>90.0f) f=180.0f-f;							// always use the most shallow angle

		// calculate the reflection angle
		
	ang=angle_add(proj->motion.ang.y,-(180.0f-(f*2)));
	if (ang==proj->ang.y) ang=angle_add(ang,180.0f);	// special check for straight hits

	return(ang);
}

void projectile_reflect_vector(proj_type *proj,d3vct *vct)
{
	int				x,y,z;
	float			ang_y;

	ang_y=projectile_reflect_angle(proj);

	projectile_set_motion(proj,1000,ang_y,proj->motion.ang.x,&x,&y,&z);
	
	vct->x=(float)x;
	vct->y=(float)y;
	vct->z=(float)z;
	vector_normalize(vct);

	fprintf(stdout,"%.2f,%.2f,%.2f\n",vct->x,vct->y,vct->z);
}

void projectile_reflect(proj_type *proj,bool send_event)
{
		// setup the reflect

	memmove(&proj->pnt,&proj->last_pnt,sizeof(d3pnt));
	proj->ang.y=proj->motion.ang.y=projectile_reflect_angle(proj);
	
		// send event

	if (send_event) scripts_post_event_console(&proj->attach,sd_event_projectile,sd_event_projectile_reflect,0);
}

/* =======================================================

      Reset Projectile Angles
      
======================================================= */

void projectile_reset_angle_for_flight(proj_type *proj)
{
	int				dist;
	float			f,fx,fz,fy;
	
	if (!proj->reset_angle) return;

		// get projectile motion

	fx=proj->motion.vct.x;
	fz=proj->motion.vct.z;
	fy=proj->motion.vct.y;
	
		// rest angle if moving down
		
	f=(fx*fx)+(fz*fz);
	dist=(int)sqrtf(f);
	proj->ang.x=angle_add(180,angle_find(0,0,(int)fy,dist));
}

