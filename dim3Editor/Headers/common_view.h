/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Common View Functions

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
// setups
//

typedef struct		{
						d3col					mesh_line,mesh_sel,poly_sel,background;
					} setup_col_type;

typedef struct		{
						int						anisotropic_mode,mipmap_mode,
												duplicate_offset;
						bool					texture_compression,
												view_flip,auto_texture;
						char					engine_name[256];
						setup_col_type			col;
					} setup_type;

//
// selection defines
//

#define select_max_item							1024

//
// selection structure
//

typedef struct		{
                        int						type,main_idx,sub_idx;
					} select_item_type;

//
// os routines
//

extern void os_get_window_box(d3rect *box);
extern void os_set_arrow_cursor(void);
extern void os_set_wait_cursor(void);
extern void os_set_hand_cursor(void);
extern void os_set_drag_cursor(void);
extern void os_set_rotate_cursor(void);
extern void os_set_toward_cursor(void);
extern void os_set_forward_cursor(void);
extern void os_set_resize_cursor(void);
extern void os_set_add_cursor(void);
extern void os_set_cut_cursor(void);
extern bool os_button_down(void);
extern bool os_track_mouse_location(d3pnt *pt,d3rect *offset_box);

//
// selection routines
//

extern void select_clear(void);
extern void select_add(int type,int main_idx,int sub_idx);
extern int select_count(void);
extern void select_get(int sel_idx,int *type,int *main_idx,int *sub_idx);
extern void select_switch(int sel_idx,int type,int main_idx,int sub_idx);
extern int select_find(int type,int main_idx,int sub_idx);
extern bool select_check(int type,int main_idx,int sub_idx);
extern bool select_has_type(int type);
extern void select_flip(int type,int main_idx,int sub_idx);
extern void select_delete_move_index(int type,int main_idx,int sub_idx);
extern void select_sort(void);
extern void select_duplicate_clear(void);
extern void select_duplicate_add(int type,int main_idx,int sub_idx);
extern void select_duplicate_copy(void);

//
// selection move routines
//

extern void select_get_extent(d3pnt *min,d3pnt *max);

//
// groups
//

extern int group_count(int group_idx);
extern void group_set(int group_idx);
extern void group_clear(int group_idx,bool delete_shift);

//
// piece routines
//

extern void piece_duplicate(void);
extern void piece_delete_face(void);
extern void piece_delete(void);
extern void piece_tesselate(void);
extern void piece_resize(void);
extern void piece_reposition(void);
extern void piece_flip(bool flip_x,bool flip_y,bool flip_z);
extern void piece_rotate(float rot_x,float rot_y,float rot_z);
extern void piece_free_rotate(void);
extern void piece_move(int move_x,int move_y,int move_z);
extern void piece_mesh_select_all_poly(void);
extern void mesh_snap_to_grid(int mesh_idx);
extern void mesh_poly_snap_to_grid(int mesh_idx,int poly_idx);
extern void mesh_vertexes_snap_to_grid(int mesh_idx);
extern void piece_mesh_snap_to_grid(void);
extern void piece_mesh_snap_closest_vertex(void);
extern void piece_mesh_poly_snap_to_grid(void);
extern void piece_mesh_vertexes_snap_to_grid(void);
extern void piece_rotate_uvs(void);
extern void piece_reset_uvs(bool poly_only);
extern void piece_poly_hole(void);
extern void piece_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch);

//
// piece create routines
//

extern bool piece_create_texture_ok(void);
extern void piece_create_get_spot(d3pnt *pnt);
extern void piece_create_spot(void);
extern void piece_create_scenery(void);
extern void piece_create_light(void);
extern void piece_create_sound(void);
extern void piece_create_particle(void);
extern void piece_create_node(void);
extern void piece_create_liquid(void);

//
// piece mesh routines
//

extern void piece_add_obj_mesh(void);
extern void piece_add_height_map_mesh(void);
extern void piece_add_grid_mesh(void);
extern void piece_add_polygon_mesh(void);
extern void piece_combine_mesh(void);
extern void piece_split_mesh(void);
extern void map_mesh_move_all(int x,int y,int z);
extern void map_mesh_reset_uv_all(void);
extern void map_optimize(void);

//
// node routines
//

extern bool node_link_click(editor_3D_view_setup *view_setup,d3pnt *pt);
extern void node_path_rebuild(void);

//
// obscuring routines
//

extern bool obscure_test(void);
extern void obscure_reset(void);


