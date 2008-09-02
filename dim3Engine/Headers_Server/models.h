/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Functions Header

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

extern void model_initialize(void);
extern model_type* model_find_uid(int uid);
extern model_type* model_find(char *name);
extern void model_load_and_init(model_draw *draw);
extern void models_dispose(int uid);
extern void models_reset_uid(void);

extern void model_draw_setup_object(int tick,obj_type *obj);
extern void model_draw_setup_projectile(int tick,proj_type *proj);
extern void model_draw_setup_weapon(int tick,obj_type *obj,weapon_type *weap,bool ignore_y_shifts);

extern void model_change_fill(model_draw *draw,int wfill,int txt);
extern int model_find_animation_from_draw(model_draw *draw,char *name);
extern bool model_start_animation(model_draw *draw,char *name);
extern void model_stop_animation(model_draw *draw);
extern bool model_cancel_animation(model_draw *draw,char *name);
extern bool model_change_animation(model_draw *draw,char *name);
extern bool model_interrupt_animation(model_draw *draw,char *name);
extern void model_run_animation(model_draw *draw);
extern void model_calc_animation(model_draw *draw);
extern void model_calc_draw_bones(model_draw *draw);
extern int model_get_current_pose(model_draw *draw);
extern bool model_find_bone_offset(model_draw *draw,char *pose_name,char *bone_name,int *x,int *y,int *z);
extern bool model_find_bone_position(model_draw *draw,char *pose_name,char *bone_name,int *x,int *y,int *z);
extern bool model_find_bone_position_for_current_animation(model_draw *draw,int bone_idx,int *x,int *y,int *z);
extern bool model_get_bone_brightness(model_draw *draw,char *pose_name,char *bone_name,float *bright);

extern void model_animation_effect_setup(model_type *mdl);
extern void model_animation_effect_launch(model_draw *draw,int animate_idx,int pose_idx);

extern void model_fade_clear(model_draw *draw);
extern void model_fade_start(model_draw *draw,int fade_msec,float end_alpha);
extern void model_fade_run(int tick,model_draw *draw);
extern void model_mesh_fade_start(model_draw *draw,int mesh_idx,int fade_in_msec,int fade_life_msec,int fade_out_msec);
extern void model_mesh_fade_run(int tick,model_draw *draw);

extern int model_find_mesh_from_draw(model_draw *draw,char *name);
extern bool model_show_mesh(model_draw *draw,char *name);
extern bool model_show_all_mesh(model_draw *draw);
extern bool model_show_only_mesh(model_draw *draw,char *name);
extern bool model_hide_mesh(model_draw *draw,char *name);
extern bool model_hide_all_mesh(model_draw *draw);
