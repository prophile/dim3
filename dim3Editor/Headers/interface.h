/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Interface Functions

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
// sizes
//

#define tool_button_size					28
#define tool_count							21

#define tool_group_menu_id					160

#define piece_button_size					36
#define piece_count							15

#define toolbar_high						28
#define info_high							16

#define piece_wid							36

//
// magnification size
//

#define magnify_size						6000

//
// menu
//

extern void menu_start(void);
extern void menu_fix_enable(void);
extern void menu_set_view_check(int view);
extern void menu_set_fov_check(float fov);
extern void menu_save_changes_dialog(void);
extern OSStatus menu_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata);

extern void undo_clear(void);
extern void undo_set_segment_move(void);
extern void undo_run(void);

//
// texture palette
//

extern void texture_palette_setup(void);
extern int texture_palette_get_selected_fill(void);
extern void texture_palette_put_selected_fill(int fill);
extern void texture_palette_draw(void);
extern void texture_palette_reset(void);
extern void texture_palette_click(Point pt,bool dbl_click);


//
// info status line
//

extern void info_status_line_draw(void);

//
// progress window
//

extern void progress_wind_start(char *txt,int max);
extern void progress_wind_update(int v);
extern void progress_wind_close(void);

//
// main window
//

extern OSStatus main_wind_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata);
extern void main_wind_open(void);
extern void main_wind_close(void);
extern void main_wind_resize_buttons(void);
extern void main_wind_resize(void);
extern void main_wind_set_title(char *file_name);
extern void main_wind_set_view(int view);
extern void main_wind_rotate_view(void);
extern void main_wind_set_view_piece_portal(void);
extern void main_wind_set_view_piece_segment(void);
extern void main_wind_set_viewport(Rect *view_box,float rgb);
extern void main_wind_set_3D_projection(Rect *view_box,d3ang *ang,float fov,float near_z,float far_z,float near_z_offset);
extern void main_wind_draw_dividers(void);
extern void main_wind_draw(void);
extern void main_wind_set_focus(int focus,bool redraw);
extern void main_wind_center_position_in_map(void);
extern bool main_wind_click(Point pt,bool dblclick);
extern void main_wind_cursor(Point pt);
extern bool main_wind_space_down(void);
extern bool main_wind_option_down(void);
extern bool main_wind_control_down(void);
extern bool main_wind_command_down(void);
extern bool main_wind_shift_down(void);
extern void main_wind_key_cursor(void);
extern void main_wind_key_down(char ch);
extern void main_wind_scroll_wheel(int delta);
extern void main_wind_tool_reset(void);
extern void main_wind_tool_fill_group_combo(void);
extern void main_wind_tool_default(void);
extern void main_wind_tool_fix_enable(void);
extern void main_wind_tool_switch_vertex_mode(void);

//
// dos
//

extern bool file_new_map(bool create_initial_portal);
extern bool file_open_map(void);
extern void file_save_map(void);
extern void file_close_map(void);

//
// launching
//

extern void launch_engine(void);
extern void launch_map_script_editor(void);
extern void launch_spot_script_editor(char *script_name);
