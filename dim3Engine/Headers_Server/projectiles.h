/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectiles Functions Header

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern void projectile_start(void);

extern proj_type* projectile_create(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup);

extern proj_type* projectile_find_uid(int uid);

extern void projectile_set_position(proj_type *proj,d3pnt *pt,d3ang *ang);
extern void projectile_set_origin(proj_type *proj);
extern void projectile_set_motion(proj_type *proj,float speed,float ang_y,float ang_x,int *x,int *y,int *z);
extern void projectile_spawn_position(proj_type *proj,d3pnt *pt,d3ang *ang,obj_type *parentobj);

extern void projectile_mark_dispose(proj_type *proj);
extern void projectile_dispose(void);
extern void projectile_dispose_all(void);

extern void proj_setup_initialize_list(void);

extern proj_setup_type* proj_setups_find_uid(int uid);
extern proj_setup_type* find_proj_setups(weapon_type *weap,char *name);

extern bool proj_setup_add(obj_type *obj,weapon_type *weap,char *name);
extern void proj_setup_start(proj_setup_type *proj_setup);
extern void proj_setup_dispose(int idx);

extern void proj_setup_set_radius(proj_setup_type *proj_setup);
extern void proj_setup_attach_mark(proj_setup_type *proj_setup);
extern proj_setup_type* proj_setup_get_attach(void);
extern proj_type* proj_get_attach(void);

extern void projectile_gravity(proj_type *proj);
extern void projectile_speed(proj_type *proj);
extern bool projectile_move(proj_type *proj);
extern void projectile_turn_xz_towards(proj_type *proj,obj_type *to_obj,float turn_add);
extern void projectile_thrust_y_towards(proj_type *proj,obj_type *to_obj,float thrust_add);
extern void projectile_seek(proj_type *proj,obj_type *to_obj,float turn_add,float thrust_add);
extern bool projectile_bounce(proj_type *proj,float min_ymove,float reduce,bool send_event);
extern float projectile_reflect_angle(proj_type *proj);
extern void projectile_reflect(proj_type *proj,bool send_event);
extern void projectile_reset_angle_for_flight(proj_type *proj);
extern void projectile_eject_vector(proj_type *proj,d3vct *vct);
extern void projectile_reflect_vector(proj_type *proj,d3vct *vct);

extern void projectile_stick(proj_type *proj);
extern void projectile_collision(proj_type *proj);
extern void projectile_decals(proj_type *proj,proj_setup_type *proj_setup);
extern bool projectile_hit(int tick,proj_type *proj,bool hit_scan);

extern void projectile_hitscan(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang);
