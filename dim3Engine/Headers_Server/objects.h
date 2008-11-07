/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Functions Header

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

extern void object_initialize_list(void);

extern obj_type* object_create(int bind);
extern int object_start(spot_type *spot,bool player,int bind,char *err_str);
extern void object_dispose_single(int idx);
extern void object_dispose_2(int bind);

extern int object_script_spawn(char *name,char *type,char *script,char *params,d3pnt *pnt,d3ang *ang);
extern bool object_script_remove(int uid);

extern void object_set_radius(obj_type *obj);
extern void object_set_spawn_mesh(obj_type *obj);
extern void object_set_position(obj_type *obj,int x,int y,int z,float ang_y,float ymove);
extern void object_stop(obj_type *obj);

extern bool object_start_script(obj_type *obj,char *name,char *params,char *err_str);

extern void object_clear_size(obj_size *size);
extern void object_clear_position(d3pnt *pnt);
extern void object_clear_angle(d3ang *ang);
extern void object_clear_motion(obj_motion *motion);
extern void object_clear_movement(obj_movement *move);
extern void object_clear_force(obj_force *force);
extern void object_clear_contact(obj_contact *contact);
extern void object_clear_touch(obj_touch *touch);
extern void object_clear_hit(obj_hit *hit);
extern void object_clear_pickup(obj_pickup *pickup);
extern void object_clear_watch(obj_watch *watch);
extern void object_clear_remote(obj_remote *remote);
extern void object_clear_draw(model_draw *draw);

extern void object_reset_prepare(obj_type *obj);
extern void object_reset(obj_type *obj);

extern obj_type* object_find_uid(int uid);
extern int object_find_index_uid(int uid);
extern obj_type* object_find_remote_uid(int uid);
extern int object_find_index_remote_uid(int uid);
extern obj_type* object_find_spawn_idx(int spawn_idx);
extern obj_type* object_find_name(char *name);
extern obj_type* object_find_nearest(int x,int z,int y,char *name,char *type,int team_idx,float ang,float ang_sweep,int min_dist,int max_dist,bool player,bool remote);
extern int object_count_team(int team_idx,int ignore_obj_uid);
extern int object_find_uid_click_object(obj_type *obj);
extern int object_find_uid_by_stood_on_object_uid(int stand_obj_uid);
extern bool object_sight_test_object(obj_type *obj,int test_obj_uid);

extern void object_attach_click_crosshair_up(obj_type *obj);
extern void object_attach_click_crosshair_down(obj_type *obj);

extern void object_spawn(obj_type *obj);
extern void object_score_update(obj_type *obj);
extern void object_score_death(obj_type *obj);
extern void object_score_goal(obj_type *obj);
extern void object_death(obj_type *obj);
extern void object_telefrag(obj_type *obj,obj_type *source_obj);
extern void object_telefrag_check(obj_type *obj);
extern void object_setup_touch(obj_type *obj,obj_type *source_obj,bool stand);
extern void object_touch(obj_type *obj);
extern void object_setup_hit(obj_type *obj,obj_type *from_obj,weapon_type *from_weap,proj_type *from_proj,d3pnt *melee_hit_pt,int damage);
extern void object_click(obj_type *obj,obj_type *from_obj);
extern void object_damage(obj_type *obj,obj_type *source_obj,weapon_type *source_weap,proj_type *source_proj,d3pnt *melee_hit_pt,int damage);
extern void object_heal(obj_type *obj,int heal);
extern void object_health_recover(obj_type *obj);
extern void object_crush(obj_type *obj,bool auto_crush);
extern void object_hide(obj_type *obj,bool hide);
extern void object_input_freeze(obj_type *obj,bool freeze);
extern void object_push(obj_type *obj,d3ang *ang,int force,bool external_force);
extern void object_shove(obj_type *obj,d3ang *ang,float speed);
extern void object_shove_direct(obj_type *obj,d3vct *vct);
extern bool object_set_radar_icon(obj_type *obj);

extern void object_fs_tint_start(int tick,obj_type *obj,float r,float g,float b,float alpha,int fade_in_msec,int life_msec,int fade_out_msec);
extern void object_fs_effect_run(int tick,obj_type *obj);

extern void get_motion_movement(float ang,obj_movement *move,float *xmove,float *zmove);
extern void object_motion_setup(obj_type *obj,float *xmove,float *ymove,float *zmove);
extern void object_movement(obj_type *obj,obj_movement *move);
extern void object_simple_movement(obj_type *obj,obj_movement *move);
extern void object_gravity(obj_type *obj);
extern void object_fix_force(obj_type *obj);
extern void object_alter_speed(obj_type *obj,float alt_speed);
extern void object_alter_gravity(obj_type *obj,float alt_gravity);
extern bool object_bump_up(obj_type *obj);
extern void object_fix_bump_smooth(obj_type *obj);
extern void object_move_xz_bounce(obj_type *obj);
extern void object_motion_slope_alter_movement_single(float *mv,float slope_y,float slope_mv);
extern void object_motion_slope_alter_movement(obj_type *obj,float *xmove,float *zmove);
extern void object_motion_lock(obj_type *obj,float *xmove,float *ymove,float *zmove);
extern void object_motion_set_script_property(obj_type *obj,float xmove,float ymove,float zmove);
extern void object_move_y_up(obj_type *obj,int ymove);
extern void object_move_y_fall(obj_type *obj);
extern void object_move_y_down(obj_type *obj,int ymove);
extern void object_move_y(obj_type *obj,int ymove);
extern void object_move_y_fly(obj_type *obj,int ymove);
extern bool object_move_xz_slide_line(obj_type *obj,int *xadd,int *yadd,int *zadd,int lx,int rx,int lz,int rz);
extern bool object_move_xz_slide(obj_type *obj,int *xadd,int *yadd,int *zadd);
extern void object_move_fly(obj_type *obj);
extern void object_move_swim(obj_type *obj);
extern void object_move_normal(obj_type *obj);
extern void object_move(obj_type *obj);
extern void object_move_remote(obj_type *obj);
extern void object_thrust(obj_type *obj);
extern void object_move_start(obj_type *obj);
extern void object_move_stop(obj_type *obj);

extern void object_move_with_mesh(int mesh_idx,int xmove,int zmove);
extern void object_rotate_with_mesh(int mesh_idx,float y);
extern bool object_push_object_allowed(obj_type *obj,obj_type *push_obj);
extern bool object_push_with_object(obj_type *obj,int xmove,int zmove);
extern void object_move_with_standing_object(obj_type *obj,int xmove,int zmove);
extern void object_rotate_with_standing_object(obj_type *obj,float y);

extern float object_liquid_alter_speed(obj_type *obj);
extern void object_liquid(int tick,obj_type *obj);

extern void object_rigid_body_reset_angle(obj_type *obj);
extern void object_fly_reset_angle(obj_type *obj);

extern float object_get_turn_speed(obj_type *obj);
extern float object_get_turn_motion(obj_type *obj);
extern void object_turn(obj_type *obj);
extern void object_turn_stop(obj_type *obj);
extern void object_fix_motion(obj_type *obj);
extern void object_player_turn(obj_type *obj);
extern void object_player_turn_direct(obj_type *obj,float ang);
extern void object_player_look(obj_type *obj);
extern void object_player_look_stop(obj_type *obj);
extern void object_player_look_fix_zoom(obj_type *obj,weapon_type *weap);

extern void object_post_move_animation_event(obj_type *obj,int sub_event);
extern bool object_post_move_animation_event_change(obj_type *obj,int sub_event);
extern void object_post_turn_animation_event(obj_type *obj,int sub_event);
extern bool object_post_turn_animation_event_change(obj_type *obj,int sub_event);
extern void object_event_animations(obj_type *obj);
extern void object_start_jump(obj_type *obj);
extern void object_liquid_jump(obj_type *obj);
extern void object_start_stand(obj_type *obj);
extern void object_start_duck(obj_type *obj);
extern void object_ducking(obj_type *obj);
extern void object_set_ambient(obj_type *obj,char *name,float pitch);
extern void object_change_ambient_pitch(obj_type *obj,float pitch);
extern void object_clear_ambient(obj_type *obj);
extern bool object_enter_vehicle(obj_type *obj,char *err_str);
extern bool object_exit_vehicle(obj_type *vehicle_obj,bool ignore_errors,char *err_str);

extern bool object_auto_walk_node_setup(obj_type *obj,char *start_node,char *end_node,int event_id);
extern bool object_auto_walk_object_setup(obj_type *obj,int uid,bool turn_only);
extern bool object_auto_walk_player_setup(obj_type *obj,bool turn_only);
extern bool object_auto_walk_position_setup(obj_type *obj,d3pnt *pnt,int slop);
extern bool object_auto_walk_node_resume(obj_type *obj);
extern void object_auto_walk_stop(obj_type *obj);
extern void object_auto_walk(obj_type *obj);

extern void object_watch(obj_type *obj);
extern void object_watch_death_alert(obj_type *dead_obj);
extern void object_watch_base_alert(map_mesh_type *mesh,obj_type *enter_obj,bool entry);
extern void object_watch_sound_alert(int x,int y,int z,int sound_obj_uid,char *sound_name);

extern int object_held_add(obj_type *obj,char *name,char *type,char *script,char *params,char *err_str);
extern bool object_held_drop(obj_type *obj,int uid,float y_ang,int y_change,char *err_str);

extern void object_grow_clear(obj_type *obj);
extern void object_grow_direct(obj_type *obj,float end_resize);
extern void object_grow_start(obj_type *obj,int grow_msec,float end_resize,d3pnt *end_offset);
extern void object_grow_run(obj_type *obj);

extern void item_pickup_check(obj_type *obj);
extern void item_add_ammo(obj_type *obj,weapon_type *weap,int add_count);
extern void item_add_weapon(obj_type *obj,weapon_type *weap);
extern void item_swap_weapon(obj_type *obj,weapon_type *weap);
extern void item_add_clip(obj_type *obj,weapon_type *weap,int add_count);
extern void item_add_alt_ammo(obj_type *obj,weapon_type *weap,int add_count);
extern void item_add_alt_clip(obj_type *obj,weapon_type *weap,int add_count);
extern void item_add_health(obj_type *obj,int add_count);
extern void item_add_custom(obj_type *obj,int custom_id);

extern bool player_attach_object(void);
extern void player_detach_object(void);
extern void player_get_ui_color(d3col *col);

extern void player_clear_input(void);
extern void player_command_input(obj_type *obj);
extern void player_turn_input(obj_type *obj,float mouse_x);
extern void player_look_input(obj_type *obj,float mouse_y);
extern void player_movement_input(obj_type *obj);
extern void player_jump_duck_input(obj_type *obj);
extern void player_weapon_zoom_input(obj_type *obj,weapon_type *weap);
extern void player_weapon_change_input(int tick,obj_type *obj,weapon_type *weap);
extern void player_weapon_fire_input(int tick,obj_type *obj,weapon_type *weap);
extern void player_weapon_input(int tick,obj_type *obj);
extern void player_enter_vehicle(obj_type *obj);
extern void player_exit_vehicle(obj_type *obj);
extern void player_enter_exit_input(obj_type *obj);
extern bool player_message_input(int tick,obj_type *obj);
extern void player_get_input(int tick);
