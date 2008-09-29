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
	
	memmove(&camera.pnt,&obj->pnt,sizeof(d3pnt));
	
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

void camera_chase_get_position(d3pnt *pnt,d3ang *ang)
{
	int						n,xadd,yadd,zadd,radius,div,
							cx,cz,cy,nx,nz,ychng,hit_obj_uid;
	float					fang,fx,fy,fz;
	poly_pointer_type		head_poly;
	matrix_type				mat;
	obj_type				*obj;
	weapon_type				*weap;

	obj=object_find_uid(camera.obj_uid);
	
		// get maximum camera offset
		
	fx=0;
	fy=0;
	fz=(float)camera.chase_distance;
	
	fang=angle_add(camera.cur_chase_ang.x,-obj->view_ang.x);
	
	matrix_rotate_zyx(&mat,fang,camera.cur_chase_ang.y,camera.cur_chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	cx=(int)fx+obj->pnt.x;
	cy=(int)fy+obj->pnt.y;
	cz=(int)fz+obj->pnt.z;
    
        // get camera movement
        
	div=camera_chase_get_division(abs(cx-obj->pnt.x),abs(cz-obj->pnt.z),abs(cy-obj->pnt.y));
	if (div==0) div=1;
	
	xadd=(cx-obj->pnt.x)/div;
	yadd=(cy-obj->pnt.y)/div;
	zadd=(cz-obj->pnt.z)/div;
	
		// get camera starting position
	
	radius=camera.chase_size;
	
	fx=0;
	fy=0;
	fz=(float)(radius>>2);			// always move camera back to protect against hitting walls in front of player
	
	fang=angle_add(camera.cur_chase_ang.x,(obj->view_ang.x*2));
	
	matrix_rotate_zyx(&mat,fang,camera.cur_chase_ang.y,camera.cur_chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	cx=(int)fx+obj->pnt.x;
	cz=(int)fz+obj->pnt.z;
	cy=(int)fy+(obj->pnt.y-obj->size.y);
		
		// move camera
		
	for (n=0;n!=div;n++) {
		
			// xz movement
			
		nx=cx+xadd;
		nz=cz+zadd;
		
		if (!map_spot_empty_sphere(nx,cy,nz,radius,obj->uid,&hit_obj_uid)) {		// box has hit some walls
			cx=nx;
			cz=nz;
		}
		
			// y movement
			
		ychng=pin_upward_movement_point(cx,cy,cz,yadd,&head_poly);
		if (head_poly.mesh_idx==-1) cy+=ychng;
	}
	
        // new camera position

	camera.pnt.x=cx;
	camera.pnt.y=cy;
	camera.pnt.z=cz;
	
		// return values
		
    memmove(pnt,&camera.pnt,sizeof(d3pnt));
	
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

void camera_chase_static_get_position(d3pnt *pnt,d3ang *ang)
{
	int				cx,cz,cy;
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
	
	cx=(int)fx+obj->pnt.x;
	cy=(int)fy+obj->pnt.y-obj->size.y;
	cz=(int)fz+obj->pnt.z;
	
        // new camera position

	pnt->x=camera.pnt.x=cx+camera.chase_offset.x;
	pnt->y=camera.pnt.y=cy+camera.chase_offset.y;
	pnt->z=camera.pnt.z=cz+camera.chase_offset.z;
	
		// looking angles
	
	ang->x=(camera.cur_chase_ang.x+camera.ang.x);
	ang->y=angle_add(camera.cur_chase_ang.y,camera.ang.y);
	ang->z=camera.ang.z;
}

