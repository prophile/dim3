/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Interface Functions Header

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

//
// gui
//

extern void gui_initialize(char *background_path,char *bitmap_name,bool show_view);
extern void gui_shutdown(void);
extern void gui_set_frame(chooser_frame_type *frame);
extern void gui_draw(float background_alpha,bool cursor);
extern void gui_draw_message(char *txt);
extern int gui_click(void);
extern int gui_keyboard(void);

//
// cursors
//

extern void cursor_initialize(void);
extern void cursor_shutdown(void);
extern void cursor_draw(void);

//
// elements
//

extern void element_initialize(void);
extern void element_shutdown(void);

extern void element_clear(void);

extern int element_get_control_high(void);
extern int element_get_separator_high(void);
extern int element_get_padding(void);

extern void element_button_add(char *path,char *path2,int id,int x,int y,int wid,int high,int x_pos,int y_pos);
extern void element_bitmap_add(char *path,int id,int x,int y,int wid,int high,bool framed);
extern void element_text_add(char *str,int id,int x,int y,int just,bool small_text,bool selectable,bool alert);
extern void element_text_field_add(char *str,char *value_str,int max_value_str_sz,int id,int x,int y,bool selectable);
extern void element_checkbox_add(char *str,int value,int id,int x,int y,bool selectable);
extern void element_combo_add(char *str,char *combo_data,int value,int id,int x,int y,bool selectable);
extern void element_slider_add(char *str,float value,float value_min,float value_max,int id,int x,int y,bool selectable);
extern void element_table_add(element_column_type* cols,char *row_data,int id,int ncolumn,int x,int y,int wid,int high,int bitmap_mode);
extern void element_tab_add(char *path_list,char *path2_list,int value,int id,int ntab,int x,int y,int wid,int high,int list_wid,int ext_high);

extern int element_get_selected(void);

extern int element_get_value(int id);
extern float element_get_slider_value(int id);
extern void element_get_value_string(int id,char *str);
extern void element_set_value(int id,int value);
extern void element_set_slider_value(int id,float value);
extern void element_set_value_string(int id,char *str);
extern void element_enable(int id,bool enabled);
extern void element_hide(int id,bool hide);
extern void element_table_busy(int id,bool busy);
extern void element_text_change(int id,char *str);
extern void element_set_table_data(int id,char *row_data);
extern void element_set_bitmap(int id,char *path);
extern int element_get_scroll_position(int id);
extern void element_set_scroll_position(int id,int pos);
extern int element_get_x_position(int id);

extern void element_draw(bool cursor_hilite);

extern void element_click_down(int x,int y);
extern int element_click_up(int x,int y);
extern int element_key(char ch);

//
// progress
//

extern void progress_initialize(char *action);
extern void progress_shutdown(void);
extern void progress_draw(float percentage);

//
// hud
//

extern void hud_initialize(void);

extern hud_bitmap_type* hud_bitmaps_find(char *name);
extern hud_text_type* hud_texts_find(char *name);
extern hud_bar_type* hud_bars_find(char *name);
extern void hud_text_set(hud_text_type *text,char *data);
extern void hud_bitmaps_hide_all(void);
extern void hud_texts_hide_all(void);
extern void hud_bars_hide_all(void);

extern void hud_click(void);
extern void hud_draw(int tick);

extern void network_draw(int tick);

//
// radar
//

extern void radar_draw(void);

//
// crosshairs
//

extern void crosshair_initialize(void);

extern int crosshair_find(char *name);
extern void crosshair_show_alt(int tick,obj_type *obj);

extern bool crosshair_get_location(int tick,obj_type *obj,weapon_type *weap,int *kx,int *kz,int *ky,int *hit_obj_uid);
extern void crosshair_setup(int tick,obj_type *obj,weapon_type *weap);
extern void crosshair_draw(obj_type *obj,weapon_type *weap);

extern void zoom_setup(int tick,obj_type *obj,weapon_type *weap);
extern void zoom_draw(obj_type *obj,weapon_type *weap);

//
// interfaces
//

extern void setup_game_trigger_clear(void);
extern void setup_game_trigger_check(void);
extern void setup_game_trigger_set(void);
extern void setup_game_run(void);

extern void setup_network_run(void);

extern void file_trigger_clear(void);
extern void file_trigger_check(void);
extern void file_trigger_set(void);
extern void file_run(void);

extern void join_run(void);
extern void host_run(void);

extern void intro_open(void);
extern void intro_run(void);

extern void chooser_trigger_clear(void);
extern void chooser_trigger_check(void);
extern void chooser_trigger_set(char *name,char *sub_txt);
extern void chooser_run(void);

extern void menu_trigger_clear(void);
extern void menu_trigger_check(void);
extern void menu_trigger_set(void);
extern void menu_run(void);

extern void title_trigger_clear(void);
extern void title_trigger_check(void);
extern void title_trigger_set(char *dir,char *name,char *sound_name,int event_id);
extern void title_run(void);

extern void story_trigger_clear(void);
extern void story_trigger_check(void);
extern void story_trigger_set(char *name,int event_id);
extern void story_run(void);

extern void movie_trigger_clear(void);
extern void movie_trigger_check(void);
extern void movie_trigger_set(char *name,int event_id);
extern void movie_run(void);

extern void interface_quit_trigger_clear(void);
extern void interface_quit_trigger_set(void);
extern bool interface_quit_trigger_check(void);

extern void error_open(char *err_str,char *err_str_2);
extern void error_run(void);

extern void map_pick_trigger_clear(void);
extern void map_pick_trigger_check(void);
extern void map_pick_trigger_set(void);
extern void map_pick_run(void);
