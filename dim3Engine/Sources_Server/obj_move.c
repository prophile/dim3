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

#define obj_hit_fudge		40
	// supergumba -- this needs to be setable

extern bool collide_object_to_map(obj_type *obj,int hit_fudge,int *xadd,int *yadd,int *zadd);

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

bool object_bump_up(obj_type *obj,int xmove,int zmove)
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
        ydif=obj->pos.y-(hit_obj->pos.y-hit_obj->size.y);
    }
    
        // bump up a wall
	
	poly_ptr=&obj->contact.hit_poly;

	if (poly_ptr->portal_idx!=-1) {
		mesh_poly=&map.portals[poly_ptr->portal_idx].mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];
        ydif=obj->pos.y-mesh_poly->box.min.y;
    }
    
		// get bump move
		
	if ((ydif<=0) || (ydif>obj->bump.high)) return(FALSE);

	ymove=pin_upward_movement_obj(obj,-ydif);
	
		// check if bump will be OK
// supergumba -- need to rewrite this		
//	if (!move_obj_check_bump(obj,xmove,zmove,ymove)) return(FALSE);
	
		// do bump
	
	obj->pos.y+=ymove;
	obj->bump.smooth_offset+=abs(ymove);					// bump moves player up, but offset pushes them down and smoothes out the bump
	
	return(obj->contact.head_poly.portal_idx==-1);
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
	/* supergumba
	if (obj->contact.wall_seg_idx==-1) {
		obj->bounce.wall_seg_idx=-1;
		return;
	}

		// only bounce if bounce factor != 0 and not same wall segment
		
	if (obj->bounce.factor==0.0f) return;
	if (obj->bounce.wall_seg_idx==obj->contact.wall_seg_idx) return;
	
		// bounce
		
	obj->force.vct.x-=(obj->motion.vct.x*obj->bounce.factor);
	obj->force.vct.z-=(obj->motion.vct.z*obj->bounce.factor);
	*/
}

void object_to_object_push(obj_type *obj,float xmove,float zmove)
{
	int				weight_dif;
	float			f;
	obj_type		*pushed_obj;
	
	if (obj->contact.obj_uid==-1) return;
	
		// get object to push

	pushed_obj=object_find_uid(obj->contact.obj_uid);
	if (pushed_obj==NULL) return;

	if (!pushed_obj->contact.pushable) return;

		// compare weights

	weight_dif=obj->size.weight-pushed_obj->size.weight;
	if (weight_dif<0) return;

		// reduce movement for weight

	if (weight_dif>500) weight_dif=500;
	
	f=1.0f+(((float)weight_dif)/1000.0f);
	
		// add in vector
	
	pushed_obj->force.vct.x=xmove*f;
	pushed_obj->force.vct.z=zmove*f;
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
		
	x=obj->pos.x+(int)(*xmove);
	y=obj->pos.y;
	z=obj->pos.z+(int)(*zmove);

	sy=find_poly_for_downward_point(x,y,z,obj->size.y,&poly);
	if (poly.portal_idx==-1) return;

		// ignore flat polygons

	mesh_poly=&map.portals[poly.portal_idx].mesh.meshes[poly.mesh_idx].polys[poly.poly_idx];
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
		obj->pos.y+=fy;
		ymove+=fy;
		if (ymove>=0) return;
	}

		// go upwards

	up_move=pin_upward_movement_obj(obj,ymove);
	obj->pos.y+=up_move;
	
	if (up_move>ymove) {
		obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}

		// check for above map

	if (obj->pos.y<=0) {
		obj->pos.y=0;
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

	y=obj->pos.y;
		
	fy=pin_downward_movement_obj(obj,floor_slop);
	fy+=y;

		// deal with polygon contacts
		
	if (obj->contact.stand_poly.portal_idx!=-1) {
	
		obj->stand_obj_uid=-1;

			// below or on the floor?

		if (y>=fy) {
			obj->air_mode=am_ground;
			obj->pos.y=fy;
			return;
		}
		
			// above the polygon

		ymove=(int)obj->force.vct.y;
		if ((y>(fy-floor_slop)) || ((y+ymove)>fy)) {		// next floor or slop will go past floor
			if (ymove>=0) {
				obj->air_mode=am_ground;
				obj->pos.y=fy;
			}
			return;
		}
	}
	
		// check standing on objects

	uid=collide_find_object_for_standing_object(obj);
	if (uid!=-1) {
		hit_obj=object_find_uid(uid);
		
		obj->contact.obj_uid=uid;
		obj->pos.y=hit_obj->pos.y-hit_obj->size.y;
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
	int			fy,by;
	
		// fix objects gravity and view angles
		// if on ground
		
	if (obj->air_mode==am_ground) {
 		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}
	
		// moving down
		
	fy=pin_downward_movement_obj(obj,ymove);
	obj->pos.y+=fy;
	
	if (obj->air_mode==am_falling) obj->fall.dist+=fy;
	
		// check for below portal
		
	by=map.portals[obj->pos.rn].by;
	if (obj->pos.y>=by) {
		obj->pos.y=by;
		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
	}
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
	int				by;
	
		// moving up
		
	if (ymove<0) {
		obj->pos.y+=pin_upward_movement_obj(obj,ymove);
		if (obj->contact.head_poly.portal_idx!=-1) return;

			// check for above map
			
		if (obj->pos.y<=0) {
			obj->pos.y=0;
			return;
		}

		return;
	}
	
		// moving down
		
	if (ymove<=0) return;
	
	obj->pos.y+=pin_downward_movement_obj(obj,ymove);
	
		// check for below portal
		
	by=map.portals[obj->pos.rn].by;
	if (obj->pos.y>=by) obj->pos.y=by;
}

/* =======================================================

      X/Z Movement
      
======================================================= */

bool object_move_xz(obj_type *obj,int *xadd,int *yadd,int *zadd)
{
		// map mesh collisions

	if (collide_object_to_map(obj,obj_hit_fudge,xadd,yadd,zadd)) return(FALSE);

		// supergumba -- need object collisions here

	return(TRUE);
}

bool object_move_xz_slide_line(obj_type *obj,int *xadd,int *yadd,int *zadd,int lx,int rx,int lz,int rz)
{
	int					n,xadd2,zadd2,mx,mz;
	float				f,ang,rang;
	bool				cwise;
	d3vct				line_vct,obj_vct;
	
		// special check for horizontal/vertical walls

	if (lx==rx) {
		xadd2=zadd2=0;
		if (!object_move_xz(obj,&xadd2,yadd,zadd)) return(FALSE);
		return(object_move_xz(obj,xadd,yadd,zadd));
	}
	
	if (lz==rz) {
		xadd2=zadd2=0;
		if (!object_move_xz(obj,xadd,yadd,&zadd2)) return(FALSE);
		return(object_move_xz(obj,&xadd2,yadd,zadd));
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

		if (!object_move_xz(obj,&xadd2,yadd,&zadd2)) {

			xadd2=mx;
			zadd2=0;
			object_move_xz(obj,&xadd2,yadd,&zadd2);

		}
		else {

			xadd2=mx;
			zadd2=0;

			if (!object_move_xz(obj,&xadd2,yadd,&zadd2)) {

				xadd2=0;
				zadd2=mz;
				object_move_xz(obj,&xadd2,yadd,&zadd2);
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

		// attempt to move cleanly

	xadd2=*xadd;
	yadd2=*yadd;
	zadd2=*zadd;

	if (!object_move_xz(obj,&xadd2,&yadd2,&zadd2)) {
		*xadd=xadd2;
		*yadd=yadd2;
		*zadd=zadd2;
		return(FALSE);
	}

		// if wall hit, then find vector for wall
		// that was hit and attempt to slide across
		// the wall
	
	if (obj->contact.hit_poly.portal_idx!=-1) {

		/* supergumba
		wall=&map.segments[obj->contact.wall_seg_idx].data.wall;

			// don't slide on walls that are bump up candidates

		if (obj->bump.on) {
			bump_y=obj->pos.y-wall->ty;
			if ((bump_y>0) && (bump_y<=obj->bump.high)) return(TRUE);
		}
		
			// slide against the angle of the wall
			
		return(object_move_xz_slide_line(obj,xadd,yadd,zadd,wall->lx,wall->rx,wall->lz,wall->rz));
		*/

		return(FALSE);
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
	int				i_xmove,i_ymove,i_zmove,
					xchg,zchg;
    float			xmove,zmove,ymove;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);
    
		// get exact movement
		
	xchg=obj->pos.x;
	zchg=obj->pos.z;
	
        // moving x/z

	i_xmove=(int)xmove;
	i_ymove=(int)ymove;
	i_zmove=(int)zmove;
		
	if (object_move_xz(obj,&i_xmove,&i_ymove,&i_zmove)) {
		object_to_object_push(obj,xmove,zmove);
	}
	
		// force move other objects
		
	xchg=obj->pos.x-xchg;
	zchg=obj->pos.z-zchg;
	
	if ((xchg!=0) || (zchg!=0)) {
		object_move_with_object(obj,xchg,zchg);
	}
	
		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_move_y_fly(obj,(int)ymove);
}

void object_move_swim(obj_type *obj)
{
	int				i_xmove,i_ymove,i_zmove;
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

	if (object_move_xz(obj,&i_xmove,&i_ymove,&i_zmove)) {
		object_to_object_push(obj,xmove,zmove);
	}
	
		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_move_y_fly(obj,i_ymove);
}

void object_move_normal(obj_type *obj)
{
	int				rn,x,z,xadd,yadd,zadd,
					start_y,fall_damage;
    float			xmove,zmove,ymove;
	bool			old_falling;
	d3pos			old_pos;

		// get object motion
		
	object_motion_setup(obj,&xmove,&ymove,&zmove);
	object_motion_slope_alter_movement(obj,&xmove,&zmove);
	object_motion_lock(obj,&xmove,&ymove,&zmove);
	object_motion_set_script_property(obj,xmove,ymove,zmove);

		// save old settings

	old_falling=(obj->air_mode==am_falling);
	memmove(&old_pos,&obj->pos,sizeof(d3pos));

		// get int version of movement

	xadd=(int)xmove;
	zadd=(int)zmove;

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

	x=obj->pos.x+xadd;
	z=obj->pos.z+zadd;

	start_y=obj->pos.y;

	rn=map_find_portal_hint(&map,obj->pos.rn,x,obj->pos.y,z);
	if (rn==-1) return;

	obj->pos.x=x;
	obj->pos.z=z;

	yadd=(int)ymove;

	if (yadd<0) {
		object_move_y_up(obj,yadd);
	}
	else {
		object_move_y_fall(obj);
		object_move_y_down(obj,yadd);
	}

	obj->pos.x-=xadd;
	obj->pos.z-=zadd;

		// add in the last y change. we use this
		// information in the next move to help
		// the object move over radically bumping
		// ground
		//
		// we only use the y change in x/z movement
		// if we are going up

	yadd=(obj->pos.y-start_y)+obj->motion.last_y_change;
	if (yadd>0) yadd=0;

	obj->motion.last_y_change=obj->pos.y-start_y;

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

	while (TRUE) {
		object_move_xz_slide(obj,&xadd,&yadd,&zadd);

		obj->pos.x+=xadd;
		obj->pos.z+=zadd;

		if (!map_find_portal_by_pos(&map,&obj->pos)) {
			xadd=zadd=0;
			memmove(&obj->pos,&old_pos,sizeof(d3pos));
		}

			// objects pushing other objects

		if ((xadd!=0) || (zadd!=0)) {
			object_move_with_object(obj,xadd,zadd);
		}
			
			// if bumped up, move again

		if (object_bump_up(obj,xadd,zadd)) {
			yadd=0;
			continue;	
		}

		break;
	}

		// objects with automatic bouncing

	object_move_xz_bounce(obj);
	
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

	x=obj->pos.x;
	y=obj->pos.y;
	z=obj->pos.z;

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

	if (obj->contact.hit_poly.portal_idx!=-1) {

			// collide events

		if (!obj->in_collide_event) {
			obj->in_collide_event=TRUE;
			scripts_post_event_console(&obj->attach,sd_event_collide,0,0);
		}

			// ladder check

		obj->on_ladder=map.portals[obj->contact.hit_poly.portal_idx].mesh.meshes[obj->contact.hit_poly.mesh_idx].flag.climbable;
	}
	else {
		obj->in_collide_event=FALSE;

		if ((x!=obj->pos.x) && (y!=obj->pos.y) && (z!=obj->pos.z)) obj->on_ladder=FALSE;
	}
}

void object_move_remote(obj_type *obj)
{
	int			i_xmove,i_ymove,i_zmove;

	i_xmove=(int)obj->motion.vct.x;
	i_ymove=(int)obj->motion.vct.y;
	i_zmove=(int)obj->motion.vct.z;

	object_move_xz(obj,&i_xmove,&i_ymove,&i_zmove);
	object_move_y(obj,i_ymove);
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

/* =======================================================

      Objects Moving with Segments/Objects
      
======================================================= */

inline bool object_move_with_move(obj_type *obj,int xmove,int zmove)
{
	int			ymove;

	if (obj->lock.x) xmove=0;
	if (obj->lock.z) zmove=0;
	
	return(object_move_xz(obj,&xmove,&ymove,&zmove));
}

// supergumba -- this all needs work
bool object_move_with_wall_segment_check_wall(d3box *box,int seg_idx,int xmove,int zmove)
{
	int					lx,rx,lz,rz;
	wall_segment_data	*wall;
	
	wall=&map.segments[seg_idx].data.wall;
		
		// completely out of Ys?
		
	if ((box->min_y>=wall->by) || (box->max_y<=wall->ty)) return(FALSE);
		
		// get wall points plus movement
			
	lx=wall->lx+xmove;
	rx=wall->rx+xmove;
	lz=wall->lz+zmove;
	rz=wall->rz+zmove;
		
		// check x/z (2D) wall
			
	return(box_collide_2D_line(box,lx,rx,lz,rz));
}

void object_move_with_wall_segment(int seg_idx,int xmove,int zmove)
{
	int			i;
	d3box		box;
	obj_type	*obj;

	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {

		if (!obj->remote.on) {

				// has wall moved into object?

			box_create_from_object(&box,obj);

			if (object_move_with_wall_segment_check_wall(&box,seg_idx,xmove,zmove)) {
				if (object_move_with_move(obj,xmove,zmove)) {
					object_crush(obj,TRUE);					// if move fails, then auto-crush
				}
			}
		}

		obj++;
	}
}

void object_move_with_floor_segment(int seg_idx,int xmove,int zmove)
{
	/* supergumba
	int			i;
	obj_type	*obj;
	
	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {

		if (!obj->remote.on) {

				// are we standing on this floor?

			if (obj->contact.floor_seg_idx==seg_idx) object_move_with_move(obj,xmove,zmove);

		}
		obj++;
	}
	*/
}

void object_move_with_object(obj_type *obj,int xmove,int zmove)
{
	int			i;
	obj_type	*obj_check;
	
	obj_check=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		if (!obj_check->remote.on) {
			if (obj_check->stand_obj_uid==obj->uid) {
				if (!obj_check->suspend) {
					object_move_with_move(obj_check,xmove,zmove);
				}
			}
			obj_check++;
		}
	}
}

