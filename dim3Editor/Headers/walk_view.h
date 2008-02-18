/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Functions

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
// walk view
//

extern bool walk_view_initialize(void);
extern void walk_view_shutdown(void);
extern void walk_view_set_fov(float fov);
extern void walk_view_setup(bool active,bool full_screen_forward,bool full_screen_top);
extern int walk_view_find_start_portal(void);
extern void walk_view_click(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool rot_ok,bool dblclick);
extern void walk_view_cursor(bool rot_ok);
extern void walk_view_key(char ch,int view_move_dir);

//
// walk view click piece
//

extern void walk_view_draw_feedback_poly(int idx,int ptsz,int *x,int *y,int *z);
extern void walk_view_draw_feedback_sprite(int type,int idx,d3pos *pos,d3ang *ang,char *display_model);
extern void walk_view_draw_feedback_map(int rn,bool sel_only);
extern void walk_view_click_piece(editor_3D_view_setup *view_setup,d3pnt *pt,bool dblclick);

//
// walk view draw piece
//

extern void walk_view_sight_path_trace(portal_sight_list_type *sight,int u);
extern void walk_view_sight_path_mark(int rn);
extern void walk_view_gl_setup(editor_3D_view_setup *view_setup);
extern void walk_view_draw(editor_3D_view_setup *view_setup,bool draw_position);

//
// walk view draw select
//

extern void walk_view_draw_select_portal(int rn,d3pnt *cpt);

//
// walk view click handle
//

extern void walk_view_move_similiar_handle(int rn,int x,int z,int y,int xadd,int zadd,int yadd);
extern void walk_view_move_single_handle(int index,int wpt,int xadd,int zadd,int yadd);
extern bool walk_view_drag_segment_handle(Point pt,int index,int wpt);
extern bool walk_view_drag_primitive_handle(Point pt,int index,int wpt);

//
// walk view draw handle
//

extern void walk_view_draw_sel_handles(int ptsz,int *x,int *y,int *z);
extern void walk_view_draw_segment_handles(void);

//
// walk view movement
//

extern void walk_view_mouse_reset_portal(void);
extern void walk_view_mouse_xy_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern void walk_view_mouse_z_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern void walk_view_scroll_wheel_z_movement(editor_3D_view_setup *view_setup,int delta,int view_move_dir);
extern void walk_view_mouse_turn(d3pnt *pt);
extern void walk_view_portal_go_to_top(void);
extern void walk_view_portal_go_to_bottom(void);
extern void walk_view_portal_go_to_selection(void);
extern void walk_view_get_piece_movement(bool cmdkey,int x,int y,int *xadd,int *zadd,int *yadd);

//
// walk view compass
//

extern void walk_view_compass_draw(Rect *box);
extern bool walk_view_compass_click(Rect *box,d3pnt *pt);

//
// walk view models
//

extern void walk_view_models_start(void);
extern void walk_view_models_close(void);
extern void walk_view_models_reset(void);
extern bool walk_view_model_draw(d3pnt *cpt,d3pos *pos,d3ang *ang,char *name);
extern bool walk_view_model_click_select_size(d3pnt *cpt,char *name,d3pos *pos,d3ang *ang,int *px,int *pz,int *ty,int *by);
extern bool walk_view_model_draw_select(d3pnt *cpt,d3pos *pos,d3ang *ang,char *name);
