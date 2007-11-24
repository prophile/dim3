/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Chase Camera

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
#include "weapons.h"
#include "cameras.h"
#include "physics.h"

extern map_type			map;
extern camera_type		camera;
extern setup_type		setup;

extern float object_player_look_constrain(obj_type *obj,weapon_type *weap,float ang_x);

/* =======================================================

      Setup Chase Camera Structure
      
======================================================= */

void camera_chase_connect(void)
{
	obj_type		*obj;

	obj=object_find_uid(camera.obj_uid);
	
	memmove(&camera.pos,&obj->pos,sizeof(d3pos));
	
		// current chasing angle
	
	camera.cur_chase_ang.x=camera.chase_ang.x;
    camera.cur_chase_ang.y=angle_add(obj->ang.y,camera.chase_ang.y);
	camera.cur_chase_ang.z=camera.chase_ang.z;
}

/* =======================================================

      Run Chase Camera
      
======================================================= */

float camera_chase_run_slop(float ang,float goal,float slop,float chase_track_speed)
{
	float		dif;
	bool		cwise;
	
	if ((ang==goal) || (slop==0)) return(goal);
	
		// force angle to within min/max slop
		
	dif=angle_dif(ang,goal,&cwise);
	if (dif>slop) {
		ang=angle_add(goal,(cwise?(-slop):(slop)));
	}
	
		// now turn towards goal
		
	return(angle_turn_toward(ang,goal,chase_track_speed));	
}

void camera_chase_run(void)
{
	float			goal_y;
	obj_type		*obj;

	obj=object_find_uid(camera.obj_uid);
	
	goal_y=angle_add(obj->ang.y,camera.chase_ang.y);
	camera.cur_chase_ang.y=camera_chase_run_slop(camera.cur_chase_ang.y,goal_y,camera.chase_slop.y,camera.chase_track_speed);
}

/* =======================================================

      Find Chase Division
      
======================================================= */

int camera_chase_get_division(int x,int z,int y)
{
	int			k;
	
	k=x;
	if (z>k) k=z;
	if (y>k) k=y;
	
	return(k/map_enlarge);
}

/* =======================================================

      Get Chase Camera Position
      
======================================================= */

void camera_chase_get_position(d3pos *pos,d3ang *ang)
{
	int				n,xadd,yadd,zadd,radius,div,
					crn,cx,cz,cy,nx,nz,old_crn,ychng,seg_idx,hit_obj_uid;
	float			fang,fx,fy,fz;
	matrix_type		mat;
	obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(camera.obj_uid);
	
		// get maximum camera offset
		
	fx=0;
	fy=0;
	fz=(float)camera.chase_distance;
	
	fang=angle_add(camera.cur_chase_ang.x,-obj->view_ang.x);
	
	matrix_rotate_zyx(&mat,fang,camera.cur_chase_ang.y,camera.cur_chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	cx=(int)fx+obj->pos.x;
	cy=(int)fy+obj->pos.y;
	cz=(int)fz+obj->pos.z;
    
        // get camera movement
        
	div=camera_chase_get_division(abs(cx-obj->pos.x),abs(cz-obj->pos.z),abs(cy-obj->pos.y));
	if (div==0) div=1;
	
	xadd=(cx-obj->pos.x)/div;
	yadd=(cy-obj->pos.y)/div;
	zadd=(cz-obj->pos.z)/div;
	
		// get camera starting position
	
	radius=camera.chase_size;
	
	fx=0;
	fy=0;
	fz=(float)(radius>>2);			// always move camera back to protect against hitting walls in front of player
	
	fang=angle_add(camera.cur_chase_ang.x,(obj->view_ang.x*2));
	
	matrix_rotate_zyx(&mat,fang,camera.cur_chase_ang.y,camera.cur_chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	cx=(int)fx+obj->pos.x;
	cz=(int)fz+obj->pos.z;
	cy=(int)fy+(obj->pos.y-obj->size.y);
		
		// move camera
		
	crn=map_find_portal(&map,cx,cy,cz);
	if (crn==-1) return;

	for (n=0;n!=div;n++) {
		
			// xz movement
			
		nx=cx+xadd;
		nz=cz+zadd;
		
		if (!map_spot_empty_sphere(nx,cy,nz,radius,obj->uid,&hit_obj_uid)) {		// box has hit some walls
		
			old_crn=crn;
			
			crn=map_find_portal_hint(&map,crn,nx,cy,nz);
			if (crn!=-1) {
				cx=nx;
				cz=nz;
			}
			else {
				crn=old_crn;
			}
		}
		
			// y movement
			
		ychng=pin_upward_movement_point(crn,cx,cy,cz,yadd,&seg_idx);
		if (seg_idx==-1) cy+=ychng;
	}
	
        // new camera position

	camera.pos.rn=crn;
	camera.pos.x=cx;
	camera.pos.y=cy;
	camera.pos.z=cz;
	
		// return values
		
    pos->rn=camera.pos.rn;
    pos->x=camera.pos.x;
    pos->y=camera.pos.y;
    pos->z=camera.pos.z;
	
		// looking angles
	
	ang->x=(camera.cur_chase_ang.x+camera.ang.x)-obj->view_ang.x;

	weap=weapon_find_current(obj);
	if (weap!=NULL) ang->x=object_player_look_constrain(obj,weap,ang->x);

	ang->y=angle_add(camera.cur_chase_ang.y,camera.ang.y);
	ang->z=camera.ang.z;
}

/* =======================================================

      Get Chase Static Camera Position
      
======================================================= */

void camera_chase_static_get_position(d3pos *pos,d3ang *ang)
{
	int				prn,cx,cz,cy;
	float			fx,fy,fz;
	matrix_type		mat;
	obj_type		*obj;

	obj=object_find_uid(camera.obj_uid);
	
		// get static chase position
		
	fx=0;
	fy=0;
	fz=(float)camera.chase_distance;
	
	matrix_rotate_zyx(&mat,camera.cur_chase_ang.x,camera.cur_chase_ang.y,camera.cur_chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	cx=(int)fx+obj->pos.x;
	cy=(int)fy+obj->pos.y-obj->size.y;
	cz=(int)fz+obj->pos.z;
	
		// in a room?
		
	prn=map_find_portal_hint(&map,obj->pos.rn,cx,cy,cz);
	if (prn==-1) {
		prn=obj->pos.rn;
		cx=obj->pos.x;
		cy=obj->pos.y;
		cz=obj->pos.z;
	}
	
        // new camera position

	pos->rn=camera.pos.rn=prn;
	pos->x=camera.pos.x=cx+camera.chase_offset.x;
	pos->y=camera.pos.y=cy+camera.chase_offset.y;
	pos->z=camera.pos.z=cz+camera.chase_offset.z;
	
		// looking angles
	
	ang->x=(camera.cur_chase_ang.x+camera.ang.x);
	ang->y=angle_add(camera.cur_chase_ang.y,camera.ang.y);
	ang->z=camera.ang.z;
}

