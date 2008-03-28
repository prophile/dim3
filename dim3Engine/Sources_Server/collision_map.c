/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Collisions

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
#include "models.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

#define obj_hit_fudge		40
	// supergumba -- this needs to be setable

extern void object_motion_slope_alter_movement(obj_type *obj,float *xmove,float *zmove);
extern void object_move_xz_bounce(obj_type *obj);

/* =======================================================

      Determine if Contact Poly is Wall-Like
      
======================================================= */

bool collide_contact_is_wall_hit(poly_pointer_type *hit_poly)
{
	map_mesh_poly_type		*mesh_poly;

	if (hit_poly->portal_idx==-1) return(FALSE);

	mesh_poly=&map.portals[hit_poly->portal_idx].mesh.meshes[hit_poly->mesh_idx].polys[hit_poly->poly_idx];
	return((mesh_poly->box.common_xz) && (!mesh_poly->box.flat));
}

/* =======================================================

      Check for Object-Map Collisions
      
======================================================= */

// supergumba -- NEW COLLISION -- to collide utility

int collide_point_distance(d3pnt *pt_1,d3pnt *pt_2)
{
	double		dx,dz,dy;
	
	dx=(double)(pt_1->x-pt_2->x);
	dy=(double)(pt_1->y-pt_2->y);
	dz=(double)(pt_1->z-pt_2->z);
	
	return((int)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

void collide_object_ray_trace_points(obj_type *obj,int x_add,int z_add,int *px,int *py,int *pz)
{
	int			x,z,x_sz,z_sz,
				move_side,face_side,side;
	float		move_ang,face_ang;

		// determine the polygon face to
		// check hit points for

	move_ang=angle_find(0,0,x_add,z_add);
	face_ang=angle_add(obj->ang.y,obj->draw.rot.y);

	move_side=((int)((move_ang+45.0f)/90.0f))&0x3;
	face_side=((int)((face_ang+45.0f)/90.0f))&0x3;
	side=(move_side-face_side)&0x3;

		// polygon size

	x=obj->pos.x;
	x_sz=obj->size.x;

	z=obj->pos.z;
	z_sz=obj->size.z;

		// get points for that poly face

	switch (side) {

			// front side

		case 0:
			pz[0]=pz[1]=pz[2]=pz[3]=pz[4]=pz[5]=pz[6]=pz[7]=pz[8]=pz[9]=pz[10]=pz[11]=pz[12]=pz[13]=pz[14]=pz[15]=z-(z_sz>>1);
			px[0]=px[3]=px[6]=px[9]=px[12]=x-(x_sz>>1);
			px[1]=px[4]=px[7]=px[10]=px[13]=x;
			px[2]=px[5]=px[8]=px[11]=px[14]=x+(x_sz>>1);
			break;

			// right side

		case 1:
			px[0]=px[1]=px[2]=px[3]=px[4]=px[5]=px[6]=px[7]=px[8]=px[9]=px[10]=px[11]=px[12]=px[13]=px[14]=px[15]=x+(x_sz>>1);
			pz[0]=pz[3]=pz[6]=pz[9]=pz[12]=z-(z_sz>>1);
			pz[1]=pz[4]=pz[7]=pz[10]=pz[13]=z;
			pz[2]=pz[5]=pz[8]=pz[11]=pz[14]=z+(z_sz>>1);
			break;

			// back side

		case 2:
			pz[0]=pz[1]=pz[2]=pz[3]=pz[4]=pz[5]=pz[6]=pz[7]=pz[8]=pz[9]=pz[10]=pz[11]=pz[12]=pz[13]=pz[14]=pz[15]=z+(z_sz>>1);
			px[0]=px[3]=px[6]=px[9]=px[12]=x-(x_sz>>1);
			px[1]=px[4]=px[7]=px[10]=px[13]=x;
			px[2]=px[5]=px[8]=px[11]=px[14]=x+(x_sz>>1);
			break;

			// left side

		case 3:
			px[0]=px[1]=px[2]=px[3]=px[4]=px[5]=px[6]=px[7]=px[8]=px[9]=px[10]=px[11]=px[12]=px[13]=px[14]=px[15]=x-(x_sz>>1);
			pz[0]=pz[3]=pz[6]=pz[9]=pz[12]=z-(z_sz>>1);
			pz[1]=pz[4]=pz[7]=pz[10]=pz[13]=z;
			pz[2]=pz[5]=pz[8]=pz[11]=pz[14]=z+(z_sz>>1);
			break;

	}

		// rotate face

	rotate_2D_polygon(15,px,pz,x,z,face_ang);

		// setup Ys

	py[0]=py[1]=py[2]=(obj->pos.y-obj->size.y)+(map_enlarge>>1);
	py[3]=py[4]=py[5]=(obj->pos.y-obj->size.y)+(obj->size.y>>2);
	py[6]=py[7]=py[8]=obj->pos.y-(obj->size.y>>1);
	py[9]=py[10]=py[11]=obj->pos.y-(obj->size.y>>2);
	py[12]=py[13]=py[14]=obj->pos.y-(map_enlarge>>1);
}

bool collide_object_to_map(obj_type *obj,int hit_fudge,int *xadd,int *yadd,int *zadd)
{
	int						n,x,z,idx,px[15],py[15],pz[15],
							d,dist;
	d3pnt					spt,ept,hpt[15];
	ray_trace_contact_type	contact[15];

		// get collision points

	collide_object_ray_trace_points(obj,*xadd,*zadd,px,py,pz);

		// run rays on all points
		// find the one that moves the leasts
		// as the most suitable hit point

	idx=-1;
	dist=0;

	for (n=0;n!=15;n++) {
			
			// setup ray trace

		spt.x=px[n];
		spt.y=py[n];
		spt.z=pz[n];

		ept.x=spt.x+(*xadd);
		ept.y=spt.y+(*yadd);
		ept.z=spt.z+(*zadd);

		contact[n].obj_on=TRUE;
		contact[n].obj_ignore_uid=obj->uid;

		contact[n].proj_on=FALSE;
		contact[n].proj_ignore_uid=-1;
	
			// run trace

		if (ray_trace_map_by_point(&spt,&ept,&hpt[n],&contact[n])) {
			d=collide_point_distance(&spt,&hpt[n]);
			if ((d<=dist) || (idx==-1)) {
				dist=d;
				idx=n;
			}
		}
	}

		// no hits?

	if (idx==-1) return(FALSE);
	
		// recalculate the add factors to get to
		// the hit point and use the fudge factor
		// to make sure the distance is always
		// at least fudge away from the polygon

	x=hpt[idx].x-px[idx];
	z=hpt[idx].z-pz[idx];

	d=(int)(sqrt((double)(x*x)+(double)(z*z)));

	if ((d==hit_fudge) || (d==0)) {
		x=z=0;
	}
	else {
		if (d<hit_fudge) {
			x=-((x*hit_fudge)/d);
			z=-((z*hit_fudge)/d);
		}
		else {
			x=(x*(d-hit_fudge))/d;
			z=(z*(d-hit_fudge))/d;
		}
	}

	*xadd=x;
	*zadd=z;

		// setup the hits

	if (contact[idx].poly.portal_idx!=-1) {
		memmove(&obj->contact.hit_poly,&contact[idx].poly,sizeof(poly_pointer_type));
	}

	return(TRUE);
}

/* =======================================================

      Check for Project-Map Collisions
      
======================================================= */

bool collide_projectile_to_map(proj_type *proj,int xadd,int yadd,int zadd)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;
	proj_setup_type			*proj_setup;

		// setup ray trace

	spt.x=proj->pos.x;
	spt.y=proj->pos.y;
	spt.z=proj->pos.z;

	ept.x=spt.x+xadd;
	ept.y=spt.y+yadd;
	ept.z=spt.z+zadd;

	contact.obj_on=TRUE;
	if (proj->parent_grace>0) {
		contact.obj_ignore_uid=proj->obj_uid;
	}
	else {
		contact.obj_ignore_uid=-1;
	}

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	contact.proj_on=proj_setup->collision;
	contact.proj_ignore_uid=proj->uid;

		// run trace

	if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		proj->pos.x=ept.x;
		proj->pos.y=ept.y;
		proj->pos.z=ept.z;
		return(FALSE);
	}

		// move to hit point

	proj->pos.x=hpt.x;
	proj->pos.y=hpt.y;
	proj->pos.z=hpt.z;

		// setup hits

	proj->contact.obj_uid=contact.obj_uid;
	proj->contact.proj_uid=contact.proj_uid;
	memmove(&proj->contact.hit_poly,&contact.poly,sizeof(poly_pointer_type));

	return(TRUE);
}

/* =======================================================

      Object Movement Utilities

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

      Object Movement on Y Axis

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

/* =======================================================

      Object Movement on X/Z Axis

======================================================= */

bool object_move_xz_2(obj_type *obj,int *xadd,int *yadd,int *zadd)
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
		if (!object_move_xz_2(obj,&xadd2,yadd,zadd)) return(FALSE);
		return(object_move_xz_2(obj,xadd,yadd,zadd));
	}
	
	if (lz==rz) {
		xadd2=zadd2=0;
		if (!object_move_xz_2(obj,xadd,yadd,&zadd2)) return(FALSE);
		return(object_move_xz_2(obj,&xadd2,yadd,zadd));
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

		if (!object_move_xz_2(obj,&xadd2,yadd,&zadd2)) {

			xadd2=mx;
			zadd2=0;
			object_move_xz_2(obj,&xadd2,yadd,&zadd2);

		}
		else {

			xadd2=mx;
			zadd2=0;

			if (!object_move_xz_2(obj,&xadd2,yadd,&zadd2)) {

				xadd2=0;
				zadd2=mz;
				object_move_xz_2(obj,&xadd2,yadd,&zadd2);
			}
		}
	}
	
	return(FALSE);
}

bool object_move_xz_slide(obj_type *obj,int *xadd,int *yadd,int *zadd)
{
	int					xadd2,yadd2,zadd2,
						bump_y,lx,rx,lz,rz,hit_box_idx;
	wall_segment_data	*wall;
	obj_type			*cnt_obj;
	model_draw			*draw;
	model_type			*model;

		// attempt to move cleanly

	xadd2=*xadd;
	yadd2=*yadd;
	zadd2=*zadd;

	if (!object_move_xz_2(obj,&xadd2,&yadd2,&zadd2)) {
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

      Object Movement Main Line

======================================================= */

// supergumba -- move to object_move

void object_move_normal_2(obj_type *obj)
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

      Object Flyings

======================================================= */

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

      Object Swimming

======================================================= */

void object_move_swim_2(obj_type *obj)
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

	if (object_move_xz_2(obj,&i_xmove,&i_ymove,&i_zmove)) {
		object_to_object_push(obj,i_xmove,i_zmove);
	}
	
		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_move_y_fly(obj,i_ymove);
}
