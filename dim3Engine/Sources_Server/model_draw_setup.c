/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Draw Setup

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

#include "remotes.h"
#include "weapons.h"
#include "models.h"
#include "effects.h"

extern server_type			server;
extern view_type			view;
extern camera_type			camera;
extern map_type				map;

/* =======================================================

      Model Draw Setup For Objects
      
======================================================= */

void model_draw_setup_object(int tick,obj_type *obj)
{
	int					model_bounce_cnt;
	float				spin_ang;
	model_draw			*draw;
	model_draw_setup	*setup;
		
	draw=&obj->draw;
	setup=&draw->setup;

		// special player mark

	draw->player=obj->player;
	
		// position
	
	draw->pos.rn=obj->pos.rn;
	draw->pos.x=obj->pos.x+draw->offset.x;
	draw->pos.z=obj->pos.z+draw->offset.z;
	draw->pos.y=obj->pos.y+draw->offset.y;

		// regular drawing in 3D space

	draw->no_rot.on=FALSE;
	
		// connection

	draw->connect.obj_uid=obj->uid;
	draw->connect.weap_uid=-1;
	draw->connect.proj_uid=-1;
	draw->connect.net_sound=(obj->uid==server.player_obj_uid);
	draw->connect.motion_vct.x=obj->motion.vct.x;
	draw->connect.motion_vct.y=obj->motion.vct.y;
	draw->connect.motion_vct.z=obj->motion.vct.z;

		// rigid body offsets

	if (obj->rigid_body.on) draw->pos.y+=obj->rigid_body.draw_offset_y;
	
		// bounces

	if (draw->bounce) {
		model_bounce_cnt=(tick>>4)&0x3F;
		if (model_bounce_cnt>32) model_bounce_cnt=64-model_bounce_cnt;
		draw->pos.y-=model_bounce_cnt;
	}

		// center
		
	setup->center.x=draw->center.x;
	setup->center.y=draw->center.y;
	setup->center.z=draw->center.z;
	
		// angles
		
	if (draw->face_forward) {
		setup->ang.x=setup->ang.z=0;
		setup->ang.y=view.draw_forward_ang_y;
	}
	else {
		setup->ang.x=angle_add(obj->ang.x,draw->rot.x);
		setup->ang.y=angle_add(obj->ang.y,draw->rot.y);
		setup->ang.z=angle_add(obj->ang.z,draw->rot.z);
	
			// spins
			
		spin_ang=(float)((tick-draw->spin_tick)>>4);

		if (draw->spin.x!=0) setup->ang.x=angle_add(setup->ang.x,(float)((int)(spin_ang*draw->spin.x)%360));
		if (draw->spin.z!=0) setup->ang.z=angle_add(setup->ang.z,(float)((int)(spin_ang*draw->spin.z)%360));
		if (draw->spin.y!=0) setup->ang.y=angle_add(setup->ang.y,(float)((int)(spin_ang*draw->spin.y)%360));
	}

		// team tint

	remote_get_tint(obj,&draw->tint);
}

/* =======================================================

      Model Draw Setup For Projectiles
      
======================================================= */

void model_draw_setup_projectile(int tick,proj_type *proj)
{
	int					model_bounce_cnt;
	float				spin_ang;
	model_draw			*draw;
	model_draw_setup	*setup;
		
	draw=&proj->draw;
	setup=&draw->setup;

		// player check flag

	draw->player=FALSE;
	
		// position
	
	draw->pos.rn=proj->pos.rn;
	draw->pos.x=proj->pos.x+draw->offset.x;
	draw->pos.z=proj->pos.z+draw->offset.z;
	draw->pos.y=proj->pos.y+draw->offset.y;

		// regular drawing in 3D space

	draw->no_rot.on=FALSE;
	
		// vector
		
	draw->connect.obj_uid=-1;
	draw->connect.weap_uid=-1;
	draw->connect.proj_uid=proj->uid;
	draw->connect.net_sound=FALSE;
	draw->connect.motion_vct.x=proj->motion.vct.x;
	draw->connect.motion_vct.y=proj->motion.vct.y;
	draw->connect.motion_vct.z=proj->motion.vct.z;
	
		// bounces

	if (draw->bounce) {
		model_bounce_cnt=(tick>>4)&0x3F;
		if (model_bounce_cnt>32) model_bounce_cnt=64-model_bounce_cnt;
		draw->pos.y-=model_bounce_cnt;
	}

		// center
		
	setup->center.x=draw->center.x;
	setup->center.y=draw->center.y;
	setup->center.z=draw->center.z;
	
		// angles
		
	if (draw->face_forward) {
		setup->ang.x=setup->ang.z=0;
		setup->ang.y=view.draw_forward_ang_y;
	}
	else {
		setup->ang.x=angle_add(proj->ang.x,draw->rot.x);
		setup->ang.y=angle_add(proj->ang.y,draw->rot.y);
		setup->ang.z=angle_add(proj->ang.z,draw->rot.z);
	
			// spins
			
		spin_ang=(float)(tick>>4);

		if (draw->spin.x!=0) setup->ang.x=angle_add(setup->ang.x,(float)((int)(spin_ang*draw->spin.x)%360));
		if (draw->spin.z!=0) setup->ang.z=angle_add(setup->ang.z,(float)((int)(spin_ang*draw->spin.z)%360));
		if (draw->spin.y!=0) setup->ang.y=angle_add(setup->ang.y,(float)((int)(spin_ang*draw->spin.y)%360));
	}

		// team tint

	draw->tint.r=draw->tint.g=draw->tint.b=1.0f;
}

/* =======================================================

      Model Draw Setup For Weapons
      
======================================================= */

void model_draw_setup_weapon(int tick,obj_type *obj,weapon_type *weap,bool ignore_y_shifts)
{
	int					swap_yadd,weap_mode,
						move_tick,swap_tick,y_shift;
	float				fx,fy,fz;
    model_draw			*draw;
	model_draw_setup	*setup;
	
	draw=&weap->draw;
	setup=&draw->setup;

		// special check for player

	draw->player=obj->player;
	
        // adjust y for raise/lower modes
	
	weap_mode=obj->held_weapon.mode;
	swap_tick=obj->held_weapon.swap_tick;
	
    swap_yadd=0;
	move_tick=tick;

	if (weap_mode==wm_lower) {
		move_tick-=swap_tick;
		swap_yadd=weap->hand.select_shift-(((weap->hand.raise_tick-move_tick)*weap->hand.select_shift)/weap->hand.raise_tick);
	}
	if (weap_mode==wm_raise) {
		move_tick-=swap_tick;
		swap_yadd=((weap->hand.raise_tick-move_tick)*weap->hand.select_shift)/weap->hand.raise_tick;
	}

		// move weapon in front of player
	
	fx=(float)weap->hand.shift.x;
	fy=0.0f;
	fz=(float)weap->hand.shift.z;
	
		// extra y shifting

	y_shift=weap->hand.shift.y+obj->duck.y_move;
	if (!ignore_y_shifts) y_shift+=(swap_yadd+weapon_get_bounce(obj));
		
	fy=((float)y_shift)-fy;
	
		// bump smoothing

	if (obj->bump.on) fy+=(float)obj->bump.smooth_offset;
	
		// position
		
	draw->pos.rn=obj->pos.rn;
	draw->pos.x=(int)fx+obj->pos.x;
	draw->pos.y=(int)fy+obj->pos.y;
	draw->pos.z=(int)fz+obj->pos.z;

		// weapons need rotation fixes
		// as they are rendered without rotation in fpp
		// we need to remember the rotation they would
		// have been at to fix lighting problems
		// these calcs are sort of loose, but will work
		// good for lighting and normals

	draw->no_rot.on=((camera.mode==cv_fpp) && (obj->uid==camera.obj_uid));

	if (draw->no_rot.on) {
		draw->no_rot.center.x=obj->pos.x;
		draw->no_rot.center.y=(obj->pos.y+obj->duck.y_move)+obj->size.eye_offset;
		draw->no_rot.center.z=obj->pos.z;
		draw->no_rot.ang.x=angle_add(weap->hand.ang.x,obj->ang.x);
		draw->no_rot.ang.y=angle_add(angle_add(weap->hand.ang.y,obj->ang.y),180.0f);
		draw->no_rot.ang.z=angle_add(weap->hand.ang.z,obj->ang.z);
	}

		// connection

	draw->connect.obj_uid=obj->uid;
	draw->connect.weap_uid=weap->uid;
	draw->connect.proj_uid=-1;
	draw->connect.net_sound=(obj->uid==server.player_obj_uid);
	draw->connect.motion_vct.x=obj->motion.vct.x;
	draw->connect.motion_vct.y=obj->motion.vct.y;
	draw->connect.motion_vct.z=obj->motion.vct.z;

		// centering

	setup->center.x=draw->center.x;
	setup->center.y=draw->center.y;
	setup->center.z=draw->center.z;

		// angle plus recoil
		// if a 2D drawing, we need to remember the 2D
		// angle so we can use that to forge the drawing vertexes
		
	if (draw->no_rot.on) {
		setup->ang.x=weap->hand.ang.x;
		setup->ang.y=angle_add(weap->hand.ang.y,180.0f);
		setup->ang.z=weap->hand.ang.z;
	}
	else {
		setup->ang.x=angle_add(weap->hand.ang.x,obj->ang.x);
		setup->ang.y=angle_add(angle_add(weap->hand.ang.y,obj->ang.y),180.0f);
		setup->ang.z=angle_add(weap->hand.ang.z,obj->ang.z);
	}

	weapon_recoil_add(obj,weap,&setup->ang);

		// team tint

	draw->tint.r=draw->tint.g=draw->tint.b=1.0f;
}

