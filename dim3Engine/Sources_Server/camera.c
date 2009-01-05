/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Camera Main Line

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
#include "cameras.h"

extern map_type				map;

camera_type					camera;
camera_type					state_camera;

/* =======================================================

      Setup Camera Structure
      
======================================================= */

void camera_initialize(void)
{
	camera.mode=cv_fpp;
	
	camera.chase_size=map_enlarge*8;
    camera.chase_distance=map_enlarge*50;
	camera.chase_track_speed=1;
    
	camera.ang.x=0;
	camera.ang.y=0;
	camera.ang.z=0;
    
	camera.chase_ang.x=0;
	camera.chase_ang.y=0;
	camera.chase_ang.z=0;
	
	camera.chase_offset.x=0;
	camera.chase_offset.y=0;
	camera.chase_offset.z=0;
	
	camera.chase_slop.x=0;
	camera.chase_slop.y=0;
	camera.chase_slop.z=0;
    
    camera.static_follow=TRUE;

	camera.auto_walk.on=FALSE;
	camera.auto_walk.turn_speed=1.0f;

	camera.plane.type=cp_fov;
	camera.plane.fov=60;
	camera.plane.aspect_ratio=1;
	camera.plane.lft=-(6*map_enlarge);
	camera.plane.rgt=6*map_enlarge;
	camera.plane.top=-(6*map_enlarge);
	camera.plane.bot=3*map_enlarge;
	camera.plane.near_z=6*map_enlarge;
	camera.plane.far_z=2000*map_enlarge;
	camera.plane.near_z_offset=-(3*map_enlarge);
	
	memmove(&state_camera,&camera,sizeof(camera_type));
}

void camera_connect(obj_type *obj)
{
	camera.obj_uid=obj->uid;
    
    camera_chase_connect();
    camera_static_connect();
}

/* =======================================================

      Find Camera Position
      
======================================================= */

void camera_get_position(d3pnt *pnt,d3ang *ang,int eye_offset)
{
	switch (camera.mode) {
	
		case cv_fpp:
            camera_fpp_get_position(pnt,ang,eye_offset);
			break;
		case cv_chase:
            camera_chase_get_position(pnt,ang);
			break;
		case cv_static:
            camera_static_get_position(pnt,ang);
			break;
		case cv_chase_static:
            camera_chase_static_get_position(pnt,ang);
			break;
	}
    
        // always set static camera
        
	if (camera.mode!=cv_static) camera_static_update(pnt->x,pnt->z,pnt->y);
}

void camera_get_angle_from(d3pnt *pt,d3ang *ang)
{
	int				dist,eye_offset;
	d3pnt			pnt;
	d3ang			temp_ang;
	obj_type		*camera_obj;

		// get camera position

	eye_offset=0;

	camera_obj=object_find_uid(camera.obj_uid);
	if (camera_obj!=NULL) eye_offset=camera_obj->size.eye_offset;
		
	camera_get_position(&pnt,&temp_ang,eye_offset);

		// find angle

	dist=distance_2D_get(0,0,(pnt.x-pt->x),(pnt.z-pt->z));
	ang->x=-angle_find(0,0,(pnt.y-pt->y),dist);

	ang->y=angle_find(pt->x,pt->z,pnt.x,pnt.z);

	ang->z=0.0f;
}

/* =======================================================

      Check Camera for Liquids
      
======================================================= */

int camera_check_liquid(d3pnt *pnt)
{
	int					n,nliquid;
	map_liquid_type		*liq;
	
	nliquid=map.liquid.nliquid;
	liq=map.liquid.liquids;
	
	for (n=0;n!=nliquid;n++) {
		if ((pnt->x>=liq->lft) && (pnt->x<=liq->rgt) && (pnt->z>=liq->top) && (pnt->z<=liq->bot) && (pnt->y>=liq->y)) return(n);
		liq++;
	}

	return(-1);
}

/* =======================================================

      Camera States
      
======================================================= */

void camera_save(void)
{
	memmove(&state_camera,&camera,sizeof(camera_type));
}

void camera_restore(void)
{
	memmove(&camera,&state_camera,sizeof(camera_type));
}
	
/* =======================================================

      Run Cameras
      
======================================================= */

void camera_run(void)
{
	switch (camera.mode) {
	
		case cv_chase:
			camera_chase_run();
			return;

 		case cv_static:
			camera_static_run();
			return;
  } 
}

