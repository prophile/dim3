/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Top View Functions

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

extern void top_view_map_to_pane(int *x,int *z);
extern void top_view_pane_to_map(int *x,int *z);
extern int top_view_pane_to_map_factor(int k);
extern void top_view_distance_pane_to_map(int *x,int *z);
extern void top_view_drag_pane_to_map(int x,int z,int *rx,int *rz);

extern void top_view_setup(bool active,bool full_screen);
extern bool top_view_hide_y(fc_segment_data *fc);

extern void top_view_draw(void);
extern void top_view_click(Point pt,bool dblclick);
extern void top_view_cursor(void);
extern void top_view_key(char ch);

extern void top_view_center(void);
extern void top_view_reset(void);

//
// portal clicking
//

extern int top_view_portal_find(Point pt);
extern bool top_view_portal_select(Point pt);
extern void top_view_portal_hand_scroll(Point pt);
extern bool top_view_click_position(Point pt);

//
// portal drawing routines
//

extern void top_view_portal_selection_draw(void);
extern void top_view_portal_draw(void);
extern void top_view_portal_position_draw(void);

//
// piece region routines
//

extern void top_view_make_rect(int rn,int lft,int rgt,int top,int bot,Rect *box);
extern void top_view_make_rect_by_pos(d3pos *pos,int sz,Rect *box);
extern void top_view_make_poly(int rn,int ptsz,int *x,int *z,PolyHandle *tply);
extern bool top_view_line_click(int rn,int lx,int lz,int rx,int rz,int tx,int tz);
extern bool top_view_polygon_click(int rn,int ptsz,int *x,int *z,int tx,int tz);
extern bool top_view_pos_rect_click(d3pos *pos,int sz,int tx,int tz);
extern bool top_view_rect_click(int rn,int lft,int rgt,int top,int bot,int tx,int tz);

//
// piece clicking routines
//

extern void top_view_snap_handle(int rn,int index,int *x,int *z);
extern bool top_view_single_segment_handle_drag(int index,int whand);
extern bool top_view_primitive_handle_drag(int index,int whand);
extern bool top_view_segment_handle_drag(Point pt);

extern void top_view_piece_drag(Point pt);
extern bool top_view_piece_click(Point pt,int *type,int *index);
extern void top_view_piece_double_click(int type,int index);

//
// piece drawing routines
//

extern void top_view_piece_draw_liquid(int rn,liquid_segment_data *liq);
extern void top_view_piece_draw_wall(int rn,wall_segment_data *wall,int curve,int clip);
extern void top_view_piece_draw_ambient_wall(int rn,ambient_wall_segment_data *ambient_wall,Rect *clipbox);
extern void top_view_piece_draw_ambient_fc(int rn,ambient_fc_segment_data *ambient_fc);
extern void top_view_piece_draw_icon(d3pos *pos,CIconHandle icn,RGBColor *col,Rect *clipbox);
extern void top_view_piece_draw_get_rect_by_pos(d3pos *pos,int sz,Rect *box);

extern void top_view_piece_draw(int rn);

//
// piece selection routines
//

extern void top_view_piece_selection_draw_line(int rn,int lx,int rx,int lz,int rz);
extern void top_view_piece_selection_draw_rect(int rn,int lft,int rgt,int top,int bot);
extern void top_view_piece_selection_draw_rect_pos(d3pos *pos,int sz);
extern void top_view_piece_selection_draw_poly(int rn,int ptsz,int *x,int *z);

extern void top_view_piece_selection_draw_segment(int idx);
extern void top_view_piece_selection_draw_primitive(int primitive_uid);
extern void top_view_piece_selection_draw(void);
