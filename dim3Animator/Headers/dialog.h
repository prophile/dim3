/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Dialog Functions

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// dialog utility
//

extern void dialog_open(WindowRef *wind,char *name);
extern void dialog_set_text(WindowRef wind,unsigned long sig,int id,char *str);
extern void dialog_get_text(WindowRef wind,unsigned long sig,int id,char *str,int len);
extern void dialog_set_int(WindowRef wind,unsigned long sig,int id,int value);
extern int dialog_get_int(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_float(WindowRef wind,unsigned long sig,int id,float value);
extern float dialog_get_float(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_boolean(WindowRef wind,unsigned long sig,int id,bool b);
extern bool dialog_get_boolean(WindowRef wind,unsigned long sig,int id);
extern void dialog_clear_combo(WindowRef wind,unsigned long sig,int id);
extern void dialog_add_combo_item(WindowRef wind,unsigned long sig,int id,char *str,unsigned long cmd);
extern void dialog_set_combo(WindowRef wind,unsigned long sig,int id,int idx);
extern int dialog_get_combo(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_radio_buttons(WindowRef wind,unsigned long sig,int value,int count);
extern int dialog_get_radio_buttons(WindowRef wind,unsigned long sig,int count);
extern void dialog_set_value(WindowRef wind,unsigned long sig,int id,int value);
extern int dialog_get_value(WindowRef wind,unsigned long sig,int id);
extern void dialog_switch_tab(WindowRef wind,unsigned long sig,int id,int npane);
extern void dialog_set_tab(WindowRef wind,unsigned long sig,int id,int index,int npane);
extern void dialog_draw_color(WindowRef wind,unsigned long sig,int id,RGBColor *col);
extern void dialog_set_focus(WindowRef wind,unsigned long sig,int id);
extern void dialog_enable(WindowRef wind,unsigned long sig,int id,bool enable);
extern void dialog_hide(WindowRef wind,unsigned long sig,int id,bool show);

//
// special dialog combos
//

extern void dialog_special_combo_fill_sound(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_particle(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_ring(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_shader(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_script(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_fill_model(WindowRef wind,unsigned long sig,int id,char *sel_name);
extern void dialog_special_combo_get_sound(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_particle(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_ring(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_shader(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_script(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);
extern void dialog_special_combo_get_model(WindowRef wind,unsigned long sig,int id,char *sel_name,int str_len);

//
// dialogs
//

extern void dialog_about_run(void);

extern bool dialog_file_new_run(char *title,char *file_name);
extern bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *file_name);
extern void dialog_import_finish_run(model_type *mdl,float *scale);

extern bool dialog_model_settings_run(void);
extern bool dialog_special_bones_settings_run(void);

extern void dialog_texture_setting_run(int txt);
extern void dialog_shader_setting_run(int txt);

extern bool dialog_mesh_info_run(model_mesh_type *mesh);
extern int dialog_copy_mesh_run(void);
extern void dialog_set_bone_combo(WindowRef wind,unsigned long sig,int id,int sel_idx);
extern int dialog_get_bone_combo(WindowRef wind,unsigned long sig,int id);
extern bool dialog_vertex_settings_run(model_vertex_type *vertex);
extern void dialog_bone_fix_string(char *str);
extern bool dialog_bone_settings_run(model_bone_type *bone);
extern bool dialog_set_vertex_bone_run(int *major_bone_idx,int *minor_bone_idx,float *factor);
extern bool dialog_bone_nudge_run(int *x,int *z,int *y,bool *nudge_children,bool *nudge_vertex);
extern bool dialog_pose_settings_run(model_pose_type *pose);
extern bool dialog_bone_move_settings_run(model_bone_move_type *bone_move);
extern bool dialog_animation_settings_run(int animate_idx);
extern bool dialog_ring_settings_run(model_ring_type *ring);
extern bool dialog_particle_settings_run(model_particle_type *particle);
extern bool dialog_animation_reset_time_run(int animate_idx);
extern bool dialog_hit_box_settings_run(void);
extern bool dialog_play_blend_animation_run(void);

extern bool dialog_scale_run(model_type *mdl,float *x,float *z,float *y);
extern bool dialog_nudge_rotate_run(int *x,int *z,int *y,char *diag_name,int def_value);


