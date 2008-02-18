/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Site Path View Functions

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
// view routines
//

extern void site_path_view_setup(bool active,bool full_screen);
extern void site_path_view_map_to_pane(int *x,int *z);
extern void site_path_view_pane_to_map(int *x,int *z);
extern void site_path_view_distance_pane_to_map(int *x,int *z);
extern void site_path_view_draw(void);
extern void site_path_view_click(d3pnt *pt,bool dblclick);
extern void site_path_view_cursor(void);
extern void site_path_view_key(char ch);

//
// draw routines
//

extern void site_path_view_get_box(int rn,Rect *box);
extern void site_path_view_draw_portals(void);
extern void site_path_view_draw_single_path(int r1,int r2);
extern void site_path_view_draw_path_list(int rn,int idx);
extern void site_path_view_draw_portal_sight_path(int rn);
extern void site_path_view_get_portal(int rn,int *x,int *z,int *ex,int *ez);
extern void site_path_view_draw_position(void);

//
// site path utilities
//

extern void site_path_view_get_site_path_box(int rn,Rect *box);
extern bool site_path_portal_in_path(int rn,int in_rn);

//
// click routines
//

extern int site_path_view_select_portal(d3pnt *pt);
extern int site_path_view_find_open_sight(void);
extern void site_path_view_mouse_move(d3pnt *pt);
extern bool site_path_view_cut_sight(d3pnt *pt);
extern bool site_path_view_click_sight(d3pnt *pt);
