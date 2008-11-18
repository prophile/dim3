/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Creation/Dispose/Finding

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
#include "physics.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Start Projectile List
      
======================================================= */

void projectile_start(void)
{
	server.count.proj=0;
	server.uid.proj=0;
}

/* =======================================================

      Create Projectile
      
======================================================= */

proj_type* projectile_create(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup)
{
	proj_type			*proj;

	if (server.count.proj>=max_projectile) return(NULL);
	
	proj=&server.projs[server.count.proj];
	server.count.proj++;
	
	proj->uid=server.uid.proj;
	server.uid.proj++;
	
	proj->dispose=FALSE;
	
	proj->obj_uid=obj->uid;
	proj->weap_uid=weap->uid;
	proj->proj_setup_uid=proj_setup->uid;

	proj->start_tick=tick;
	
	object_clear_position(&proj->pnt);
	object_clear_angle(&proj->ang);
	object_clear_motion(&proj->motion);
	object_clear_force(&proj->force);
	
	proj->speed=proj_setup->speed;
	proj->decel_speed=proj_setup->decel_speed;
	proj->decel_min_speed=proj_setup->decel_min_speed;
    proj->stick=FALSE;
    proj->flag_melee_hit=FALSE;
	
	memmove(&proj->size,&proj_setup->size,sizeof(obj_size));

	proj->parent_grace=pd_parent_grace;
	proj->decel_grace=proj_setup->decel_grace;
	proj->reset_angle=proj_setup->reset_angle;

	object_clear_contact(&proj->contact);
	
	memmove(&proj->draw,&proj_setup->draw,sizeof(model_draw));
	memmove(&proj->action,&proj_setup->action,sizeof(proj_action_type));
	
	proj->attach.thing_type=thing_type_projectile;
	proj->attach.thing_uid=proj->uid;
	proj->attach.script_uid=proj_setup->attach.script_uid;

	scripts_clear_attach_data(&proj->attach);

    return(proj);
}

/* =======================================================

      Find Projectile by Unique ID
      
======================================================= */

proj_type* projectile_find_uid(int uid)
{
	int				i;
	proj_type		*proj;
	
	proj=server.projs;
	
	for ((i=0);(i!=server.count.proj);i++) {
		if (proj->uid==uid) return(proj);
		proj++;
	}
	
	return(NULL);
}

/* =======================================================

      Set Projectile Positions
      
======================================================= */

void projectile_set_position(proj_type *proj,d3pnt *pt,d3ang *ang)
{
	d3pnt			*pnt;
	
		// position
		
	pnt=&proj->pnt;
	
	pnt->x=pt->x;
	pnt->y=pt->y;
	pnt->z=pt->z;

	memmove(&proj->last_pnt,pnt,sizeof(d3pnt));

		// angles
		
	memmove(&proj->ang,ang,sizeof(d3ang));
	memmove(&proj->motion.ang,ang,sizeof(d3ang));
	
		// initial gravity
	
	proj->force.gravity=gravity_start_power;
}

/* =======================================================

      Projectile Spawn Position
      
======================================================= */

void projectile_set_origin(proj_type *proj)
{
	memmove(&proj->org_pnt,&proj->pnt,sizeof(d3pnt));
}

void projectile_spawn_position(proj_type *proj,d3pnt *pt,d3ang *ang,obj_type *parentobj)
{
	projectile_set_position(proj,pt,ang);
	projectile_set_origin(proj);
	
	proj->contact.obj_uid=-1;
}

/* =======================================================

      Get Projectile Motion
      
======================================================= */

void projectile_set_motion(proj_type *proj,float speed,float ang_y,float ang_x,int *x,int *y,int *z)
{
	float			fx,fy,fz;
	matrix_type		mat;
	
		// get motion
		
	fx=fy=0;
	fz=-speed;
	
	matrix_rotate_x(&mat,ang_x);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	matrix_rotate_y(&mat,ang_y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
		// add in the forces
		
	fx+=proj->force.vct.x;
	fy+=proj->force.vct.y;
	fz+=proj->force.vct.z;
	
		// save for projectile
		
	proj->motion.vct.x=fx;
	proj->motion.vct.z=fz;
	proj->motion.vct.y=fy;
	
		// return as integers
	
	*x=(int)fx;
	*y=(int)fy;
	*z=(int)fz;
}

/* =======================================================

      Dispose Projectiles
      
======================================================= */

void projectile_mark_dispose(proj_type *proj)
{
	proj->dispose=TRUE;
}

void projectile_dispose(void)
{
	int				i;
	proj_type		*proj;
	
		// delete all projectiles marked as disposed
	
	i=0;
	
	while (i<server.count.proj) {
		proj=&server.projs[i];
		if (!proj->dispose) {
			i++;
			continue;
		}
	
			// delete all timers
			
		timers_clear(&proj->attach,timer_mode_repeat);
		timers_clear(&proj->attach,timer_mode_single);
	
			// delete projectile
			
		if (i<(server.count.proj-1)) {
			memmove(&server.projs[i],&server.projs[i+1],(sizeof(proj_type)*((server.count.proj-i)-1)));
		}
		
		server.count.proj--;
		if (server.count.proj==0) break;
	}
}

void projectile_dispose_all(void)
{
	int				i;
	proj_type		*proj;
	
	proj=server.projs;
	
	for (i=0;i<server.count.proj;i++) {
		timers_clear(&proj->attach,timer_mode_repeat);
		timers_clear(&proj->attach,timer_mode_single);
		proj++;
	}
	
	server.count.proj=0;
}
