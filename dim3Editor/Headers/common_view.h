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
                        short						type,
													portal_idx,main_idx,sub_idx;
					} select_item_type;

//
// selection routines
//

extern void select_clear(void);
extern void select_add(int type,int portal_idx,int main_idx,int sub_idx);
extern int select_count(void);
extern void select_get(int sel_idx,int *type,int *portal_idx,int *main_idx,int *sub_idx);
extern void select_switch(int sel_idx,int type,int portal_idx,int main_idx,int sub_idx);
extern int select_find(int type,int portal_idx,int main_idx,int sub_idx);
extern bool select_check(int type,int portal_idx,int main_idx,int sub_idx);
extern bool select_check_segment(int index);
extern int select_check_primitive_find_index(int primitive_uid);
extern bool select_has_type(int type);
extern void select_flip(int type,int portal_idx,int main_idx,int sub_idx);
extern void select_sort(void);
extern void select_duplicate_clear(void);
extern void select_duplicate_add(int type,int portal_idx,int main_idx,int sub_idx);
extern void select_duplicate_copy(void);

//
// selection move routines
//

extern void select_get_extent(d3pnt *min,d3pnt *max);

//
// multi-vertex move routines
//

extern void vertexes_move_similiar_to_segment(segment_type *seg,int xadd,int yadd,int zadd);
extern void vertexes_move_to_2D_similiar_to_segment(segment_type *seg,int whand,int x,int z);
extern void vertexes_move_to_2D(segment_type *seg,int whand,int x,int z);

//
// groups
//

extern void group_add(void);
extern void group_delete(void);
extern void group_clear(void);
extern int group_count(int group_idx);
extern void group_set(int group_idx);

//
// segment routines
//

extern void poly_find_bound_rect(int ptsz,int *x,int *z,int *lft,int *rgt,int *top,int *bot);
extern void poly_find_bound_high(int ptsz,int *y,int *top,int *bot);
extern bool segment_is_floor_ceiling_square(int rn,fc_segment_data *fc,Rect *box);
extern bool segment_create_texture_ok(void);
extern void segment_clear(segment_type *seg,int type,int primitive_uid);
extern void segment_add_wall(int lx,int lz,int rx,int rz,int ty,int by,int clip,int curve,int primitive_uid,bool redraw);
extern void segment_add_floor_quad(int x1,int z1,int y1,int x2,int z2,int y2,int x3,int z3,int y3,int x4,int z4,int y4,int primitive_uid,bool redraw);
extern void segment_add_floor_trig(int x1,int z1,int y1,int x2,int z2,int y2,int x3,int z3,int y3,int primitive_uid,bool redraw);
extern void segment_add_floor_box(int lx,int lz,int rx,int rz,int y,int primitive_uid,bool redraw);
extern void segment_add_ceiling_quad(int x1,int z1,int y1,int x2,int z2,int y2,int x3,int z3,int y3,int x4,int z4,int y4,int primitive_uid,bool redraw);
extern void segment_add_ceiling_box(int lx,int lz,int rx,int rz,int y,int primitive_uid,bool redraw);
extern void segment_add_liquid(bool redraw);
extern void segment_add_ambient_wall(int lx,int rx,int lz,int rz,int ty,int by,bool redraw);
extern void segment_add_ambient_fc(int lx,int lz,int rx,int rz,int y,bool redraw);
extern void segment_move(segment_type *seg,int xchg,int zchg,int ychg);
extern int segment_duplicate(int idx,int primitive_uid,int xadd,int zadd);
extern void segment_delete(int idx);
extern void segment_add_point(void);
extern void segment_sub_point(void);
extern void segment_tesselate(void);
extern void segment_flip_horizontal(segment_type *seg,int min,int max);
extern void segment_flip_vertical(segment_type *seg,int min,int max);
extern void segment_rotate(segment_type *seg,int cx,int cz);
extern void segment_rotate_free(segment_type *seg,float ang,int cx,int cz);

//
// piece routines
//

extern void piece_move_to_portal(int rn);
extern void piece_duplicate(void);
extern void piece_delete(void);
extern void piece_tesselate(void);
extern void piece_flip(bool flip_x,bool flip_y,bool flip_z);
extern void piece_rotate(float rot_x,float rot_y,float rot_z);
extern void piece_free_rotate(void);
extern void piece_key(int rn,char ch,bool walk,bool on_side);

//
// piece create routines
//

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

extern int node_link_find_node_by_point(Point pt);
extern int node_link_is_node_link_selected(void);
extern int node_link_get_free_link(int k);
extern bool node_link_has_link(int k,int lnk);
extern void node_link_add(Point pt);
extern void node_link_cut(Point pt);
extern int node_path_node_in_path(int from_node,int to_node,unsigned char *node_hits,int curdist,int windist);
extern int node_path_find_next_node_to_node(int from_node,int to_node);
extern void node_path_rebuild(void);
