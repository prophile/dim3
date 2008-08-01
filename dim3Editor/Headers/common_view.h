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
						int							anisotropic_mode,texture_quality_mode,mipmap_mode,
													duplicate_offset;
						bool						mipmap_card_generated,texture_compression,
													view_flip,auto_texture;
						char						engine_name[256];
					} setup_type;

//
// selection defines
//

#define select_max_item								1024

//
// selection structure
//

typedef struct		{
                        int							type,main_idx,sub_idx;
					} select_item_type;

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

extern void group_add(void);
extern void group_clear(void);
extern void group_delete(void);
extern int group_count(int group_idx);
extern void group_set(int group_idx);

//
// piece routines
//

extern void piece_duplicate(void);
extern void piece_delete_face(void);
extern void piece_delete(void);
extern void piece_tesselate(void);
extern void piece_resize(void);
extern void piece_flip(bool flip_x,bool flip_y,bool flip_z);
extern void piece_rotate(float rot_x,float rot_y,float rot_z);
extern void piece_free_rotate(void);
extern void mesh_snap_to_grid(int mesh_idx);
extern void mesh_poly_snap_to_grid(int mesh_idx,int poly_idx);
extern void mesh_vertexes_snap_to_grid(int mesh_idx);
extern void piece_mesh_snap_to_grid(void);
extern void piece_mesh_snap_closest_vertex(void);
extern void piece_mesh_poly_snap_to_grid(void);
extern void piece_mesh_vertexes_snap_to_grid(void);
extern void piece_rotate_uvs(void);
extern void piece_reset_uvs(bool poly_only);
extern void piece_poly_hole(int hole_type);
extern void piece_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch);

//
// piece create routines
//

extern bool piece_create_texture_ok(void);
extern int piece_create_get_spot(int *x,int *y,int *z,int x_wid,int z_wid,int high);
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

extern void piece_add_library_mesh(void);
extern void piece_replace_library_mesh(void);
extern void piece_combine_mesh(int portal_idx);

//
// clipboard routines
//

extern void clipboard_clear(void);

//
// node routines
//

extern bool node_link_click(editor_3D_view_setup *view_setup,d3pnt *pt);
extern void node_path_rebuild(void);
