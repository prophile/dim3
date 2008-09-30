/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Movement

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
#include "models.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;

extern bool collide_object_to_map(obj_type *obj,int *xadd,int *yadd,int *zadd);

/* =======================================================

      Get Motion Movement
      
======================================================= */

void get_motion_movement(float ang,obj_movement *move,float *xmove,float *zmove)
{
    angle_get_movement_float(ang,move->speed,xmove,zmove);
    if (move->reverse) {
        *xmove=-(*xmove);
        *zmove=-(*zmove);
    }
}

void object_motion_setup(obj_type *obj,float *xmove,float *ymove,float *zmove)
{
	float			ang,xadd,zadd;
	
		// if thrust input type, then use thrust
		
	if (obj->input_mode==im_thrust) {
		*xmove=obj->thrust.vct.x+obj->force.vct.x;
		*ymove=obj->thrust.vct.y+obj->force.vct.y;
		*zmove=obj->thrust.vct.z+obj->force.vct.z;
		return;
	}
	
		// forward motion
		
	ang=obj->motion.ang.y;
	get_motion_movement(ang,&obj->forward_move,xmove,zmove);
	
		// side motion
		
	if (obj->side_step) {
		get_motion_movement(angle_add(ang,270),&obj->side_move,&xadd,&zadd);
		*xmove=(*xmove)+xadd;
		*zmove=(*zmove)+zadd;
	}
	
		// vertical motion
		
	*ymove=0;

		// flying or swiming
	
	if ((obj->fly) || (obj->liquid_mode==lm_under)) {
	
			// in auto-walk?
			
		if (obj->auto_walk.mode!=aw_none) {
			if (obj->forward_move.moving) {
				angle_get_movement_float(obj->vert_move.seek_ang,obj->vert_move.max_walk_speed,&xadd,&zadd);
				*ymove=xadd;
			}
		}
		
			// regular seeking
			
		else {
			*ymove=obj->vert_move.speed;
			if (obj->vert_move.reverse) *ymove=-(*ymove);
		}
	}

		// climbing ladders

	else {

		if (obj->on_ladder) {
			*ymove=obj->vert_move.speed;
			if (obj->vert_move.reverse) *ymove=-(*ymove);
		}

	}

		// add in outside forces

	*xmove=(*xmove)+obj->force.vct.x;
	*ymove=(*ymove)+obj->force.vct.y;
	*zmove=(*zmove)+obj->force.vct.z;
}

/* =======================================================

      Object Speed Control
      
======================================================= */

void object_movement(obj_type *obj,obj_movement *move)
{
    float		speed,max_speed,accelerate,decelerate;
    
    speed=move->speed;
	
		// single speed mode?
		
	if (obj->single_speed) {
		max_speed=move->max_walk_speed;
		accelerate=move->accelerate;
		decelerate=move->decelerate;
	}
	
		// variable speeds
		
	else {
    
		if (obj->duck.mode!=dm_stand) {
			max_speed=move->max_crawl_speed;
			accelerate=move->accelerate;
			decelerate=move->decelerate;
		}
		else {
			if ((obj->air_mode!=am_ground) || (obj->fly) || (obj->liquid_mode==lm_under)) {
				max_speed=move->max_air_speed;
				accelerate=move->air_accelerate;
				decelerate=move->air_decelerate;
			}
			else {
				if (obj->forward_move.running) {
					max_speed=move->max_run_speed;
				}
				else {
					max_speed=move->max_walk_speed;
				}
				accelerate=move->accelerate;
				decelerate=move->decelerate;
			}
		}
	}
    
		// Accelerating
	
	if (move->moving) {
		
        if (speed==max_speed) return;
        
        if (speed<max_speed) {							// if max speed has changed to be less, auto-decelerate
            speed+=(accelerate*map.settings.resistance);
            if (speed>max_speed) speed=max_speed;
		
            move->speed=speed;
            return;
        }
	}
	
		// Decelerating

	speed-=(decelerate*map.settings.resistance);
	if (speed<0) speed=0;
    
	move->speed=speed;
}

void object_simple_movement(obj_type *obj,obj_movement *move)
{
    float		speed,max_speed,accelerate,decelerate;
    
    speed=move->speed;
	
	max_speed=move->max_walk_speed;
	accelerate=move->accelerate;
	decelerate=move->decelerate;

		// Accelerating
	
	if (move->moving) {
		
        if (speed==max_speed) return;
        
        if (speed<max_speed) {							// if max speed has changed to be less, auto-decelerate
            speed+=(accelerate*map.settings.resistance);
            if (speed>max_speed) speed=max_speed;
		
            move->speed=speed;
            return;
        }
	}
	
		// Decelerating

	speed-=(decelerate*map.settings.resistance);
	if (speed<0) speed=0;
    
	move->speed=speed;
}

/* =======================================================

      Increase Gravity
      
======================================================= */

void object_gravity(obj_type *obj)
{
    float		weight,gravity,gravity_max_power,
				liq_speed_alter;
    
		// reset gravity if flying, climbing ladder, or on ground

	if ((obj->fly) || (obj->on_ladder) || (obj->air_mode==am_ground)) {
		obj->force.gravity=gravity_start_power;
		return;
	}

		// under liquids changes gravity

	weight=(float)obj->size.weight;
	gravity_max_power=map.settings.gravity_max_power;

	if (obj->liquid_mode==lm_under) {
		liq_speed_alter=object_liquid_alter_speed(obj);

		weight*=liq_speed_alter;
		gravity_max_power*=liq_speed_alter;
	}

		// run gravity
    
    gravity=obj->force.gravity;
	
		// increase the gravity
		
	if (gravity<gravity_max_power) {
		gravity+=(weight*(map.settings.gravity/gravity_factor));
		if (gravity>gravity_max_power) gravity=map.settings.gravity_max_power;
        
        obj->force.gravity=gravity;
	}
}

void object_fix_force(obj_type *obj)
{
	float			decelerate,gravity_max_speed;
	d3vct			*vct;
	
		// get the deceleration
		
	decelerate=map.settings.resistance;
	
		// reduce the x/z by resistance
		
	vct=&obj->force.vct;
	
	if (vct->x!=0) {
		if (vct->x>0) {
			vct->x-=decelerate;
			if (vct->x<0) vct->x=0;
		}
		else {
			vct->x+=decelerate;
			if (vct->x>0) vct->x=0;
		}
	}
	
	if (vct->z!=0) {
		if (vct->z>0) {
			vct->z-=decelerate;
			if (vct->z<0) vct->x=0;
		}
		else {
			vct->z+=decelerate;
			if (vct->z>0) vct->z=0;
		}
	}

		// gravity different under liquid
		// we are fudging this a bit with the extra
		// 0.5 for some bounancy

	gravity_max_speed=map.settings.gravity_max_speed;
	if (obj->liquid_mode==lm_under) gravity_max_speed*=(object_liquid_alter_speed(obj)*0.5f);
	
		// reduce the y by gravity
		
	if (obj->force.gravity>0) {
		vct->y+=obj->force.gravity;
		if (vct->y>gravity_max_speed) vct->y=gravity_max_speed;
	}
}

/* =======================================================

      Object Alterations
      
======================================================= */

void object_alter_speed(obj_type *obj,float alt_speed)
{
    obj->forward_move.speed*=alt_speed;
    obj->side_move.speed*=alt_speed;
}

void object_alter_gravity(obj_type *obj,float alt_gravity)
{
    obj->force.vct.y*=alt_gravity;
    obj->force.gravity=gravity_start_power;
}

/* =======================================================

      Object Bumping, Bounce, and Push
      
======================================================= */

bool object_bump_up(obj_type *obj)
{
	int					uid,ydif,ymove;
    obj_type			*hit_obj;
	poly_pointer_type	*poly_ptr;
	map_mesh_poly_type	*mesh_poly;
	
	if (!obj->bump.on) return(FALSE);
	if (obj->air_mode!=am_ground) return(FALSE);			// can't bump up in air
    
    ydif=0;
    
        // bump up an object?
    
	uid=obj->contact.obj_uid;
    if (uid!=-1) {
        hit_obj=object_find_uid(uid);
        ydif=obj->pnt.y-(hit_obj->pnt.y-hit_obj->size.y);
    }
    
        // bump up a wall
	
	poly_ptr=&obj->contact.hit_poly;

	if (poly_ptr->mesh_idx!=-1) {
		mesh_poly=&map.mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];
        ydif=obj->pnt.y-mesh_poly->box.min.y;
    }
    
		// get bump move
		
	if ((ydif<=0) || (ydif>obj->bump.high)) return(FALSE);

	ymove=pin_upward_movement_obj(obj,-ydif);
	
		// do bump
	
	obj->pnt.y+=ymove;
	obj->bump.smooth_offset+=abs(ymove);					// bump moves player up, but offset pushes them down and smoothes out the bump

	return(obj->contact.head_poly.mesh_idx==-1);
}

void object_fix_bump_smooth(obj_type *obj)
{
	int			k;
	
	if (obj->bump.smooth_offset==0) return;
	
		// slowly remove bump offset
		
	k=(int)(((float)obj->bump.smooth_offset)*obj->bump.smooth_factor);
	if (k<10) k=10;
	
	obj->bump.smooth_offset-=k;
	if (obj->bump.smooth_offset<0) obj->bump.smooth_offset=0;
}

void object_move_xz_bounce(obj_type *obj)
{
	if (obj->contact.hit_poly.mesh_idx==-1) {
		obj->bounce.mesh_idx=-1;
		return;
	}

		// only bounce if bounce factor != 0 and not same mesh
		
	if (obj->bounce.factor==0.0f) return;
	if (obj->bounce.mesh_idx==obj->contact.hit_poly.mesh_idx) return;
	
		// bounce
		
	obj->force.vct.x-=(obj->motion.vct.x*obj->bounce.factor);
	obj->force.vct.z-=(obj->motion.vct.z*obj->bounce.factor);
}

/* =======================================================

      Object Slope Speed Alterations
      
======================================================= */

void object_motion_slope_alter_movement_single(float *mv,float slope_y,float slope_mv)
{
	bool			same_dir;
	
		// are we going in same direction as slope?
		
	same_dir=FALSE;
	
	if ((*mv)!=0.0f) {
		same_dir=(((*mv)<0.0f) && (slope_mv>=0.0f)) || (((*mv)>=0.0f) && (slope_mv<=0.0f));
	}
	
		// if slope is greater than max gravity,
		// then if going against slope, stop all movement, otherwise always go down

	if (slope_y>=gravity_slope_max_y) {
		if (same_dir) {
			*mv=-slope_mv;
		}
		else {
			*mv=0.0f;
		}
		return;
	}

		// if going in same direction, then cut gravity effect

	if (same_dir) slope_mv*=gravity_slope_down_cut;
	
		// subtract slope push from movement

	*mv-=slope_mv;
}

void object_motion_slope_alter_movement(obj_type *obj,float *xmove,float *zmove)
{
	int					x,y,z,sy;
	poly_pointer_type	poly;
	map_mesh_poly_type	*mesh_poly;

		// if not on ground or ignoring slope gravity, then no speed reduction
		
	if (!obj->slope_gravity) return;
	if (obj->air_mode!=am_ground) return;
	if (obj->stand_obj_uid!=-1) return;
	
		// get floor going to
		
	x=obj->pnt.x+(int)(*xmove);
	y=obj->pnt.y;
	z=obj->pnt.z+(int)(*zmove);

	sy=pin_downward_movement_point(x,y,z,obj->size.y,&poly);
	if (poly.mesh_idx==-1) return;

		// ignore flat polygons

	mesh_poly=&map.mesh.meshes[poly.mesh_idx].polys[poly.poly_idx];
	if (mesh_poly->box.flat) return;

		// if less then min slope, no gravity effects

	if (mesh_poly->slope.y<gravity_slope_min_y) return;

		// apply gravity
	
	object_motion_slope_alter_movement_single(xmove,mesh_poly->slope.y,mesh_poly->slope.move_x);
	object_motion_slope_alter_movement_single(zmove,mesh_poly->slope.y,mesh_poly->slope.move_z);
}

/* =======================================================

      Motion Locks and Script Setup
      
======================================================= */

void object_motion_lock(obj_type *obj,float *xmove,float *ymove,float *zmove)
{
	if (obj->lock.x) *xmove=0;
	if (obj->lock.y) *ymove=0;
	if (obj->lock.z) *zmove=0;
}

void object_motion_set_script_property(obj_type *obj,float xmove,float ymove,float zmove)
{
	obj->motion.vct.x=xmove;
	obj->motion.vct.z=zmove;
	obj->motion.vct.y=ymove;
}

/* =======================================================

      Y Movement
      
======================================================= */

void object_move_y_up(obj_type *obj,int ymove)
{
	int				fy,up_move;

	obj->air_mode=am_up;
	
		// have any floors come up from below

	fy=pin_downward_movement_obj(obj,floor_slop);
	if (fy<0) {
		obj->pnt.y+=fy;
		ymove+=fy;
		if (ymove>=0) return;
	}

		// go upwards

	up_move=pin_upward_movement_obj(obj,ymove);
	obj->pnt.y+=up_move;
	
	if (up_move>ymove) {
		obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}

		// check for above map

	if (obj->pnt.y<=0) {
		obj->pnt.y=0;
		obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}
}

void object_move_y_fall(obj_type *obj)
{
	int				y,fy,ymove,uid;
	obj_type		*hit_obj;
	
		// check standing on polygons

	y=obj->pnt.y;
		
	fy=pin_downward_movement_obj(obj,floor_slop);
	fy+=y;

		// deal with polygon contacts
		
	if (obj->contact.stand_poly.mesh_idx!=-1) {
	
		obj->stand_obj_uid=-1;

			// below or on the floor?

		if (y>=fy) {
			obj->air_mode=am_ground;
			obj->pnt.y=fy;
			return;
		}
		
			// above the polygon

		ymove=(int)obj->force.vct.y;
		if ((y>(fy-floor_slop)) || ((y+ymove)>fy)) {		// next floor or slop will go past floor
			if (ymove>=0) {
				obj->air_mode=am_ground;
				obj->pnt.y=fy;
			}
			return;
		}
	}
	
		// check standing on objects

	uid=collide_find_object_for_standing_object(obj);
	if (uid!=-1) {
		hit_obj=object_find_uid(uid);
		
		obj->contact.obj_uid=uid;
		obj->pnt.y=hit_obj->pnt.y-hit_obj->size.y;
		obj->air_mode=am_ground;
		
			// trigger stand on events
			
		if (obj->stand_obj_uid!=uid) {
			object_setup_touch(hit_obj,obj,TRUE);
			scripts_post_event_console(&hit_obj->attach,sd_event_touch,0,0);
		}
		
		obj->stand_obj_uid=uid;
		return;
	}

	obj->stand_obj_uid=-1;
	
		// find air mode
		
	if (obj->force.vct.y<0) {
		obj->air_mode=am_up;
	}
	else {
		obj->air_mode=am_falling;
	}
}

void object_move_y_down(obj_type *obj,int ymove)
{
	int			fy;
	
		// fix objects gravity and view angles
		// if on ground
		
	if (obj->air_mode==am_ground) {
 		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}
	
		// moving down
		
	fy=pin_downward_movement_obj(obj,ymove);
	obj->pnt.y+=fy;
	
	if (obj->air_mode==am_falling) obj->fall.dist+=fy;
}

void object_move_y(obj_type *obj,int ymove)
{
	if (ymove==0) {
		object_move_y_fall(obj);
		return;
	}

	if (ymove<0) {
		object_move_y_up(obj,ymove);
	}
	else {
		object_move_y_down(obj,ymove);
	}
}

void object_move_y_fly(obj_type *obj,int ymove)
{
		// moving up
		
	if (ymove<0) {
		obj->pnt.y+=pin_upward_movement_obj(obj,ymove);
		if (obj->contact.head_poly.mesh_idx!=-1) return;

			// check for above map
			
		if (obj->pnt.y<=0) {
			obj->pnt.y=0;
			return;
		}

		return;
	}
	
		// moving down
		
	if (ymove<=0) return;
	
	obj->pnt.y+=pin_downward_movement_obj(obj,ymove);
}

/* =======================================================

      X/Z Movement
      
======================================================= */

bool object_move_xz_slide_line(obj_type *obj,int *xadd,int *yadd,int *zadd,int lx,int rx,int lz,int rz)
{
	int					n,xadd2,zadd2,mx,mz;
	float				f,ang,rang;
	bool				hit,cwise;
	d3vct				line_vct,obj_vct;
	
		// special check for horizontal/vertical walls

	if (lx==rx) {

		xadd2=0;
		hit=collide_object_to_map(obj,&xadd2,yadd,zadd);
		obj->pnt.z+=(*zadd);
		if (!hit) return(FALSE);

		zadd2=0;
		hit=collide_object_to_map(obj,xadd,yadd,&zadd2);
		obj->pnt.x+=(*xadd);

		return(hit);
	}
	
	if (lz==rz) {

		zadd2=0;
		hit=collide_object_to_map(obj,xadd,yadd,&zadd2);
		obj->pnt.x+=(*xadd);
		if (!hit) return(FALSE);

		xadd2=0;
		hit=collide_object_to_map(obj,&xadd2,yadd,zadd);
		obj->pnt.z+=(*zadd);

		return(hit);
	}
	
		// get angle between the line and the object movement
			
	obj_vct.x=(float)*xadd;
	obj_vct.y=0.0f;
	obj_vct.z=(float)*zadd;
	vector_normalize(&obj_vct);
	
	vector_create(&line_vct,lx,0,lz,rx,0,rz);		// perpendicular vector (swap x/z)
	
		// get the angle between them

	f=vector_dot_product(&obj_vct,&line_vct);
	f=((float)acos(f))*RAD_to_ANG;

		// get angle of wall.  If the angle between the
		// collision lines is less than 90, then head down
		// the line the opposite way

	ang=angle_find(lx,lz,rx,rz);
	if (f<90.0f) ang=angle_add(ang,180.0f);

		// change the motion to reflect the angle of the wall
		// uses facing instead of motion to check so we
		// don't get motion build up from sliding against a wall
		
	rang=obj->ang.y;
	if (obj->forward_move.reverse) rang=angle_add(rang,180.0f);

	if (angle_dif(rang,ang,&cwise)<90.0f) {
		if (!obj->forward_move.reverse) {
			obj->motion.ang.y=ang;
		}
		else {
			obj->motion.ang.y=angle_add(ang,180.0f);
		}
	}

		// reduce movement to slide against the walls

	mz=*zadd/ws_step_factor;
	mx=*xadd/ws_step_factor;

	for (n=0;n!=ws_step_factor;n++) {

		xadd2=0;
		zadd2=mz;

		hit=collide_object_to_map(obj,&xadd2,yadd,&zadd2);
		obj->pnt.z+=zadd2;

		if (hit) {
			xadd2=mx;
			zadd2=0;
			collide_object_to_map(obj,&xadd2,yadd,&zadd2);
			obj->pnt.x+=xadd2;
		}
		else {

			xadd2=mx;
			zadd2=0;

			hit=collide_object_to_map(obj,&xadd2,yadd,&zadd2);
			obj->pnt.x+=xadd2;

			if (hit) {
				xadd2=0;
				zadd2=mz;
				collide_object_to_map(obj,&xadd2,yadd,&zadd2);
				obj->pnt.z+=zadd2;
			}
		}
	}
	
	return(FALSE);
}

bool object_move_xz_slide(obj_type *obj,int *xadd,int *yadd,int *zadd)
{
	int					xadd2,yadd2,zadd2,
						bump_y,lx,rx,lz,rz,hit_box_idx;
	obj_type			*cnt_obj;
	model_draw			*draw;
	model_type			*model;
	poly_pointer_type	*poly_ptr;
	map_mesh_poly_type	*poly;

		// attempt to move cleanly

	xadd2=*xadd;
	yadd2=*yadd;
	zadd2=*zadd;

	if (!collide_object_to_map(obj,&xadd2,&yadd2,&zadd2)) {
		obj->pnt.x+=xadd2;
		obj->pnt.z+=zadd2;
		return(FALSE);
	}

		// if the hit poly was wall-like, then find vector for wall
		// and attempt to slide across it
	
	poly_ptr=&obj->contact.hit_poly;

	if (poly_ptr->mesh_idx!=-1) {

		poly=&map.mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];
		if (poly->box.wall_like) {

				// don't slide on polys that are bump up candidates

			if (obj->bump.on) {
				bump_y=obj->pnt.y-poly->box.min.y;
				if ((bump_y>0) && (bump_y<=obj->bump.high)) return(TRUE);
			}
		
				// slide against the angle of the wall
			
			return(object_move_xz_slide_line(obj,xadd,yadd,zadd,poly->line.lx,poly->line.rx,poly->line.lz,poly->line.rz));
		}

			// skipped out on wall, so return hit

		*xadd=xadd2;
		*yadd=yadd2;
		*zadd=zadd2;

		return(TRUE);
	}

		// if object hit, find the collision lines
		// between the object and slide across that
		// line

	if (obj->contact.obj_uid==-1) return(TRUE);

	cnt_obj=object_find_uid(obj->contact.obj_uid);
	if (cnt_obj==NULL) return(FALSE);

		// no hit box collisions

	if (!cnt_obj->hit_box.on) {
		if (!collide_object_to_object_slide(obj,*xadd,*zadd,cnt_obj,&lx,&rx,&lz,&rz)) return(TRUE);
		return(object_move_xz_slide_line(obj,xadd,yadd,zadd,lx,rx,lz,rz));
	}

		// hit box collisions

	hit_box_idx=obj->contact.hit_box_idx;
	if (hit_box_idx==-1) return(TRUE);
		
	draw=&cnt_obj->draw;
	if ((draw->uid==-1) || (!draw->on)) return(TRUE);
	
	model=model_find_uid(draw->uid);
	if (model==NULL) return(TRUE);
	
		// check hit boxes
		
	if (!collide_object_to_object_hit_box_slide(obj,*xadd,*zadd,cnt_obj,&model->hit_boxes[hit_box_idx],&lx,&rx,&lz,&rz)) return(TRUE);
	return(object_move_xz_slide_line(obj,xadd,yadd,zadd,lx,rx,lz,rz));
}

/* =======================================================

      Flying, Swiming, Normal Movement Variants
      
======================================================= */

void object_move_fly(obj_type *obj)
{
	int				i_xmove,i_ymove,i_zmove,hit_obj_uid;
    float			xmove,zmove,ymove;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);
    
        // moving x/z

	i_xmove=(int)xmove;
	i_ymove=(int)ymove;
	i_zmove=(int)zmove;
	
	if (object_move_xz_slide(obj,&i_xmove,&i_ymove,&i_zmove)) {
	
			// pushing objects
			
		if (!object_push_with_object(obj,i_xmove,i_zmove)) {
			
			i_xmove=(int)xmove;
			i_ymove=(int)ymove;
			i_zmove=(int)zmove;

			hit_obj_uid=obj->contact.obj_uid;
			object_move_xz_slide(obj,&i_xmove,&i_ymove,&i_zmove);
			obj->contact.obj_uid=hit_obj_uid;
		}
	}
	
		// force move other objects
	
	if ((i_xmove!=0) || (i_zmove!=0)) object_move_with_standing_object(obj,i_xmove,i_zmove);
	
		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_move_y_fly(obj,(int)ymove);
}

void object_move_swim(obj_type *obj)
{
	int				i_xmove,i_ymove,i_zmove,hit_obj_uid;
    float			xmove,ymove,zmove,liq_speed_alter;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);

		// speed cuts

	liq_speed_alter=object_liquid_alter_speed(obj);
	
	xmove*=liq_speed_alter;
  	zmove*=liq_speed_alter;
	ymove*=liq_speed_alter;

		// falling in water

	if (ymove>=0) object_move_y_fall(obj);

	if ((obj->air_mode==am_ground) || (ymove<0)) {
		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
	}

        // moving x/z
		
	i_xmove=(int)xmove;
	i_ymove=(int)ymove;
	i_zmove=(int)zmove;
	
	if (object_move_xz_slide(obj,&i_xmove,&i_ymove,&i_zmove)) {
	
			// pushing objects
			
		if (!object_push_with_object(obj,i_xmove,i_zmove)) {
			
			i_xmove=(int)xmove;
			i_ymove=(int)ymove;
			i_zmove=(int)zmove;

			hit_obj_uid=obj->contact.obj_uid;
			object_move_xz_slide(obj,&i_xmove,&i_ymove,&i_zmove);
			obj->contact.obj_uid=hit_obj_uid;
		}
	}

		// move standing objects

	if ((i_xmove!=0) || (i_zmove!=0)) object_move_with_standing_object(obj,i_xmove,i_zmove);
	
		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_move_y_fly(obj,i_ymove);
}

void object_move_normal(obj_type *obj)
{
	int				x,z,i_xmove,i_ymove,i_zmove,bump_cnt,
					start_y,fall_damage,hit_obj_uid,old_x,old_z;
    float			xmove,zmove,ymove;
	bool			old_falling;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_slope_alter_movement(obj,&xmove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);

		// save old settings

	old_falling=(obj->air_mode==am_falling);

		// get int version of movement

	i_xmove=(int)xmove;
	i_zmove=(int)zmove;

		// move the object in y space at the projected
		// x/z position
		//
		// we want to make sure that we move over land
		// features before they get blocked by wall-like
		// features in the map
		//
		// also, remember the y change and use that when
		// running the x/z ray trace collisions to continue
		// to avoid land features that might block foward
		// movement

	old_x=obj->pnt.x;
	x=obj->pnt.x+i_xmove;
	
	old_z=obj->pnt.z;
	z=obj->pnt.z+i_zmove;

	start_y=obj->pnt.y;

	obj->pnt.x=x;
	obj->pnt.z=z;

	i_ymove=(int)ymove;

	if (i_ymove<0) {
		object_move_y_up(obj,i_ymove);
	}
	else {
		object_move_y_fall(obj);
		object_move_y_down(obj,i_ymove);
	}

	obj->pnt.x=old_x;
	obj->pnt.z=old_z;

		// add in the last y change. we use this
		// information in the next move to help
		// the object move over radically bumping
		// ground
		//
		// we only use the y change in x/z movement
		// if we are going up

	i_ymove=(obj->pnt.y-start_y)+obj->motion.last_y_change;
	if (i_ymove>0) i_ymove=0;

	obj->motion.last_y_change=obj->pnt.y-start_y;

		// if on ground, stop all downward motion
		// and forces
	
	if (obj->air_mode==am_ground) {
		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
	}

		// now we move the object in x/z space
		//
		// we also add in the y movement into the
		// collision detection to help eliminate
		// land features that could hold up the
		// object

	if ((i_xmove!=0) || (i_zmove!=0)) {
	
			// attempt to move

		if (object_move_xz_slide(obj,&i_xmove,&i_ymove,&i_zmove)) {

				// run any bump up, reset x/z and
				// then try the movement again
				// do this a couple time in case you
				// are going up a series of steep bumps
				
			bump_cnt=0;

			while (TRUE) {

				if (object_bump_up(obj)) {

					obj->pnt.x=old_x;
					obj->pnt.z=old_z;
					
					i_xmove=(int)xmove;
					i_ymove=0;
					i_zmove=(int)zmove;
					
					obj->contact.hit_poly.mesh_idx=-1;
					if (!collide_object_to_map(obj,&i_xmove,&i_ymove,&i_zmove)) break;

					bump_cnt++;
					if (bump_cnt>2) break;

					continue;
				}
				
				break;
			}
				
				// push objects, then
				// try the movement again
				// save the hit object uid so
				// the hit still registers

			if (!object_push_with_object(obj,i_xmove,i_zmove)) {
				
				i_xmove=(int)xmove;
				i_ymove=(int)ymove;
				i_zmove=(int)zmove;

				hit_obj_uid=obj->contact.obj_uid;
				object_move_xz_slide(obj,&i_xmove,&i_ymove,&i_zmove);
				obj->contact.obj_uid=hit_obj_uid;
			}
		}

		if ((i_xmove!=0) || (i_zmove!=0)) {

				// move objects standing on object

			object_move_with_standing_object(obj,i_xmove,i_zmove);

				// objects with automatic bouncing

			object_move_xz_bounce(obj);
		}
	}
	
		// check for objects that have finished falling
		//
		// here we do any damage and send any landing
		// events
	
    if (obj->air_mode!=am_falling) {
    
        if (old_falling) {
            if (obj->fall.dist>map_enlarge) {
			
					// get damage
					
				if (obj->fall.dist>=obj->fall.damage_minimum_height) {
					fall_damage=(int)(((float)(obj->fall.dist-obj->fall.damage_minimum_height))*obj->fall.damage_factor);
					if (fall_damage!=0) object_damage(obj,NULL,NULL,NULL,NULL,fall_damage);
				}
				
					// send the land events
					
                scripts_post_event_console(&obj->attach,sd_event_land,0,0);
                object_post_move_animation_event(obj,sd_event_animation_object_land);
            }
        }
        
        obj->fall.dist=0;
        obj->fall.change=FALSE;

		return;
	}
	
		// check for objects that have started falling
	
	if ((obj->fall.dist>map_enlarge) && (!obj->fall.change)) {
		scripts_post_event_console(&obj->attach,sd_event_fall,0,0);
		scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_fall,0);
		obj->fall.change=TRUE;
	}
}

/* =======================================================

      Object Movement Mainline
      
======================================================= */

void object_move(obj_type *obj)
{
	int				x,y,z;

		// clear all contacts

	object_clear_contact(&obj->contact);

		// save original position for ladder checks

	x=obj->pnt.x;
	y=obj->pnt.y;
	z=obj->pnt.z;

		// call proper movement

	if (obj->fly) {
		object_move_fly(obj);
	}
	else {
		if (obj->liquid_mode==lm_under) {
			object_move_swim(obj);
		}
		else {
			object_move_normal(obj);
		}
	}

		// check for contacts
		// only send contact event once per hit, and check for
		// ladder contacts.  If no contacts, only turn off
		// ladder if there was actually movement

	if (obj->contact.hit_poly.mesh_idx!=-1) {

			// collide events

		if (!obj->in_collide_event) {
			obj->in_collide_event=TRUE;
			scripts_post_event_console(&obj->attach,sd_event_collide,0,0);
		}

			// ladder check

		obj->on_ladder=map.mesh.meshes[obj->contact.hit_poly.mesh_idx].flag.climbable;
	}
	else {
		obj->in_collide_event=FALSE;

		if ((x!=obj->pnt.x) && (y!=obj->pnt.y) && (z!=obj->pnt.z)) obj->on_ladder=FALSE;
	}
}

void object_move_remote(obj_type *obj)
{
	int			i_xmove,i_ymove,i_zmove;

	i_xmove=(int)obj->motion.vct.x;
	i_ymove=(int)obj->motion.vct.y;
	i_zmove=(int)obj->motion.vct.z;

	collide_object_to_map(obj,&i_xmove,&i_ymove,&i_zmove);
	object_move_y(obj,i_ymove);
}

/* =======================================================

      Run Thrust
      
======================================================= */

void object_thrust(obj_type *obj)
{
	float			xmove,ymove,zmove,ztemp;
	
	if (obj->input_mode!=im_thrust) return;
	if (!obj->forward_move.moving) return;
	
    angle_get_movement_float(obj->ang.y,obj->thrust.speed,&xmove,&zmove);
	angle_get_movement_float(obj->view_ang.x,obj->thrust.speed,&ymove,&ztemp);
	
	obj->thrust.vct.x+=xmove;
	if (obj->thrust.vct.x>obj->thrust.max_speed) obj->thrust.vct.x=obj->thrust.max_speed;
	if (obj->thrust.vct.x<-obj->thrust.max_speed) obj->thrust.vct.x=-obj->thrust.max_speed;
	
	obj->thrust.vct.y+=ymove;
	if (obj->thrust.vct.y>obj->thrust.max_speed) obj->thrust.vct.y=obj->thrust.max_speed;
	if (obj->thrust.vct.y<-obj->thrust.max_speed) obj->thrust.vct.y=-obj->thrust.max_speed;

	obj->thrust.vct.z+=zmove;
	if (obj->thrust.vct.z>obj->thrust.max_speed) obj->thrust.vct.z=obj->thrust.max_speed;
	if (obj->thrust.vct.z<-obj->thrust.max_speed) obj->thrust.vct.z=-obj->thrust.max_speed;
}
	
/* =======================================================

      Start/Stop Object Movement
      
======================================================= */

void object_move_start(obj_type *obj)
{
	obj->forward_move.moving=TRUE;
}

void object_move_stop(obj_type *obj)
{
	obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;
	
	object_auto_walk_stop(obj);
}

