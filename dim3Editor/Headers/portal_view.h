/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Portal View Functions

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
// main routines
//

extern bool portal_is_spot_ok(int rn,int x,int z,int ex,int ez);
extern void portal_duplicate_piece_offset(int rn,int x,int z,int *xadd,int *zadd);
extern bool portal_new(void);
extern void portal_delete(void);
extern void portal_duplicate(void);
extern void portal_move_all_items(int rn,int x,int y,int z);

//
// view routines
//

extern void portal_view_setup(bool active,bool full_screen);
extern void portal_view_map_to_pane(int *x,int *z);
extern void portal_view_pane_to_map(int *x,int *z);
extern void portal_view_distance_pane_to_map(int *x,int *z);
extern void portal_view_draw(void);
extern void portal_view_click(Point pt,bool dblclick);
extern void portal_view_cursor(void);
extern void portal_view_key(char ch);
extern void portal_view_center(void);
extern void portal_view_reset(void);

//
// draw routines
//

extern void portal_view_get_portal(int rn,int *x,int *z,int *ex,int *ez);
extern void portal_view_draw_portal_walls(int rn);
extern void portal_view_portal_position_draw(void);

//
// click routines
//

extern int portal_view_select_portal(Point pt);
extern void portal_view_mouse_move(Point pt);
extern bool portal_view_portal_resize(Point pt);
extern void portal_snap(int rn,int *x,int *z);
extern void portal_view_portal_drag(Point pt);
extern bool portal_view_path_duplicate(int rn);
extern bool portal_view_click_sight(Point pt);

//
// transforms
//

extern void portal_get_size(int rn,int *pex,int *pey);
extern void portal_get_y_size(int rn,int *pty,int *pby);
extern void portal_get_dimension(int rn,int *lx,int *rx,int *tz,int *bz);
extern void portal_set_dimension(int rn,int x,int ex,int z,int ez);
extern void portal_set_spot(int rn,int x,int z);
extern void portal_get_pixelbox(int rn,Rect *box);
extern bool portal_point_in_box(int rn,Point pt);
extern void portal_flip_horizontal(void);
extern void portal_flip_vertical(void);
extern void portal_rotate(void);
extern void portal_y_change(int yadd);
extern void portal_all_y_change(int yadd);
extern void portal_empty(int rn);
extern void portal_resize(void);
extern void portal_reset_textures(int seg_type);
extern void portal_reset_texture_uvs(void);
extern void portal_split(int rn,bool vertical);



