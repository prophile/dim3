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
extern void walk_view_click(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool rot_ok,bool dblclick);
extern void walk_view_cursor(bool rot_ok);
extern void walk_view_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch);

//
// walk view draw piece
//

extern bool walk_view_draw_pnt_obscure(d3pnt *pnt);
extern void walk_view_draw_select_mesh_get_grow_handles(int mesh_idx,int *px,int *py,int *pz);
extern void walk_view_draw(editor_3D_view_setup *view_setup,bool draw_position);
extern void walk_view_draw_select(d3pnt *cpt);

//
// walk view click
//

extern void walk_view_click_drag_movement(editor_3D_view_setup *view_setup,int view_move_dir,int x,int y,int *xadd,int *yadd,int *zadd);
extern int walk_view_get_grid(void);
extern void walk_view_click_grid(d3pnt *pt);
extern void walk_view_click_snap(int mesh_idx,int vertex_idx,d3pnt *pt,d3pnt *mpt);
extern void walk_view_click_piece(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool dblclick);
extern bool walk_view_mesh_poly_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,map_mesh_type *mesh,int poly_idx,int *hit_z);
extern bool walk_view_click_drag_mesh_handle(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern bool walk_view_click_drag_mesh(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern bool walk_view_click_drag_mesh_poly(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern bool walk_view_click_drag_vertex(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern bool walk_view_click_drag_liquid_vertex(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern bool walk_view_click_drag_item(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern bool walk_view_click_drag_liquid(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern bool walk_view_click_drag_texture(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool entire_mesh);

//
// walk view draw handle
//

extern void walk_view_sprite_select_size(d3pnt *pnt,int *px,int *py,int *pz);

//
// walk view movement
//

extern void walk_view_mouse_xy_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern void walk_view_mouse_z_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir);
extern void walk_view_scroll_wheel_z_movement(editor_3D_view_setup *view_setup,int delta,int view_move_dir);
extern void walk_view_scroll_wheel_rot_z_movement(editor_3D_view_setup *view_setup,int delta);
extern void walk_view_mouse_turn(d3pnt *pt);
extern void walk_view_portal_go_to_top(void);
extern void walk_view_portal_go_to_bottom(void);
extern void walk_view_portal_go_to_selection(void);

//
// walk view controls
//

extern void walk_view_compass_draw(editor_3D_view_setup *view_setup);
extern bool walk_view_compass_click(editor_3D_view_setup *view_setup,d3pnt *pt);
extern void walk_view_swap_draw(editor_3D_view_setup *view_setup);
extern bool walk_view_swap_click(editor_3D_view_setup *view_setup,d3pnt *pt,bool *swap_on);

//
// walk view models
//

extern void walk_view_models_start(void);
extern void walk_view_models_close(void);
extern void walk_view_models_reset(void);
extern bool walk_view_model_draw(d3pnt *pnt,d3ang *ang,char *name,short *texture_frame,int frame_count);
extern bool walk_view_model_click_select_size(char *name,d3pnt *pnt,d3ang *ang,int *px,int *py,int *pz);
extern bool walk_view_model_draw_select(d3pnt *pnt,d3ang *ang,char *name);
