/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Functions Header

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

extern void weapon_initialize_list(void);

extern weapon_type* weapon_find_uid(int uid);
extern int weapon_index_find_uid(int uid);
extern weapon_type* weapon_find_name(obj_type *obj,char *name);
extern weapon_type* weapon_find_current(obj_type *obj);
extern weapon_type* weapon_find_offset(obj_type *obj,int offset);

extern bool weapon_create(obj_type *obj,char *name);
extern void weapon_start(weapon_type *weap);
extern void weapon_dispose(int idx);

extern void weapon_attach_fire_crosshair(weapon_type *weap);
extern void weapon_attach_zoom_mask(weapon_type *weap);

extern void weapon_reset_ammo(weapon_type *weap);
extern void weapon_reset_ammo_object(obj_type *obj);
extern void weapon_set(obj_type *obj,weapon_type *weap);

extern void weapon_lower(int tick,obj_type *obj);
extern void weapon_raise(int tick,obj_type *obj);
extern void weapon_held(obj_type *obj);
extern void weapon_goto(int tick,obj_type *obj,weapon_type *weap);
extern void weapon_switch(int tick,obj_type *obj,int dir);
extern void weapon_pick(int tick,obj_type *obj,int offset);

extern void weapon_target_next_object(obj_type *obj,weapon_type *weap);
extern void weapon_target_previous_object(obj_type *obj,weapon_type *weap);
extern bool weapon_target_start(obj_type *obj,weapon_type *weap,char *target_type);
extern bool weapon_target_end(obj_type *obj,weapon_type *weap);

extern void weapon_zoom_on(obj_type *obj,weapon_type *weap);
extern void weapon_zoom_off(obj_type *obj,weapon_type *weap);

extern void weapon_run_hand(obj_type *obj,int tick);
extern void weapon_hand_bounce(obj_type *obj);
extern int weapon_get_bounce(obj_type *obj,weapon_type *weap);

extern void weapon_setup_fire(weapon_type *weap,int method);
extern bool weapon_add_projectile(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang);
extern bool weapon_get_projectile_position_angle_weapon_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str);
extern bool weapon_get_projectile_position_angle_weapon_barrel(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str);
extern bool weapon_get_projectile_position_angle_object_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str);
extern void weapon_get_projectile_position_center(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang);
extern void weapon_get_projectile_position_angle_add_offset(d3ang *ang,d3ang *off_ang);
extern bool weapon_script_fire(int tick,obj_type *obj,weapon_type *weap,int method);
extern bool weapon_script_projectile_spawn(int tick,obj_type *obj,weapon_type *weap,char *proj_name,d3pnt *pt,d3ang *ang,int count,float slop,char *err_str);
extern bool weapon_script_projectile_spawn_weapon_model(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern bool weapon_script_projectile_spawn_weapon_barrel(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern bool weapon_script_projectile_spawn_object_model(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern bool weapon_script_projectile_spawn_center(int tick,obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str);
extern void weapon_player_fire_down(int tick,obj_type *obj,weapon_type *weap,int method);
extern void weapon_player_fire_repeat(int tick,obj_type *obj,weapon_type *weap);
extern void weapon_player_fire_up(obj_type *obj,weapon_type *weap,int method);

extern void weapon_kickback(int tick,obj_type *obj,weapon_type *weap);
extern void weapon_recoil_clear(weapon_type *weap);
extern void weapon_recoil_copy(weapon_type *to_weap,weapon_type *from_weap);
extern void weapon_recoil_run(obj_type *obj,weapon_type *weap);
extern void weapon_recoil_add(obj_type *obj,weapon_type *weap,d3ang *ang);
extern void weapon_recoil(obj_type *obj,weapon_type *weap);

extern void melee_add(obj_type *obj,weapon_type *weap,d3pnt *pt,d3ang *ang,melee_type *melee,int ignore_uid);
extern bool melee_strike_position_weapon_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str);
extern bool melee_strike_position_object_model(int tick,obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,char *err_str);
extern bool melee_strike_position_projectile_model(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,d3pnt *fire_pnt,char *err_str);
extern bool melee_script_spawn_weapon_model(int tick,obj_type *obj,weapon_type *weap,char *err_str);
extern bool melee_script_spawn_object_model(int tick,obj_type *obj,weapon_type *weap,char *err_str);
extern bool melee_script_spawn_projectile_model(int tick,obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,proj_type *proj,char *err_str);


