/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Defines for Editor

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
// UI sizes
//

#define tool_button_size						28
#define tool_count								22

#define tool_object_menu_id						160
#define tool_node_menu_id						161
#define tool_group_menu_id						162

#define piece_button_size						36
#define piece_count								13

#define toolbar_high							28
#define info_high								16

#define piece_wid								36

//
// magnify factors
//

#define magnify_size							6000

#define magnify_factor_min						10
#define magnify_factor_max						910

#define magnify_factor_default					800

//
// views
//

#define vw_3_panel								0
#define vw_4_panel								1
#define vw_top_only								2
#define vw_forward_only							3

//
// perspective
//

#define ps_perspective							0
#define ps_ortho								1

//
// movement directions
//

#define vm_dir_forward							0
#define vm_dir_side								1
#define vm_dir_top								2

//
// keyboard panel focus
//

#define kf_panel_forward						0
#define kf_panel_side							1
#define kf_panel_top							2
#define kf_panel_walk							3

//
// vertex modes
//

#define vertex_mode_none						0
#define vertex_mode_lock						1
#define vertex_mode_snap						2

//
// drag modes
//

#define drag_mode_mesh							0
#define drag_mode_polygon						1
#define drag_mode_vertex						2

//
// grid modes
//

#define grid_mode_none							0
#define grid_mode_small							1
#define grid_mode_medium						2
#define grid_mode_large							3

//
// selection types
//

#define mesh_piece								100
#define liquid_piece							101
#define node_piece								102
#define spot_piece								103
#define scenery_piece							104
#define light_piece								105
#define sound_piece								106
#define particle_piece							107
#define area_piece								108

//
// view mouse scales
//

#define mouse_forward_view_scale				30.0f
#define mouse_side_view_scale					30.0f
#define mouse_top_view_scale					0.4f

#define mouse_top_view_drag_scale				0.01f

//
// walk view types
//

#define walk_view_proj_type_forward				0
#define walk_view_proj_type_side				1
#define walk_view_proj_type_top					2
#define walk_view_proj_type_walk				3

//
// fovs
//

#define walk_view_forward_fov					45
#define walk_view_side_fov						45
#define top_view_fov							30

//
// walk view opengl settings
//

#define walk_view_near_z						(6*map_enlarge)
#define walk_view_far_z							(2000*map_enlarge)
#define walk_view_near_offset					(3*map_enlarge)

#define walk_view_handle_size					8.0f

//
// draw passing struct
//

typedef struct		{
						int						proj_type,clip_y,portal_y;
						d3rect					box;
						d3pnt					cpt;
						d3ang					ang;
						float					fov;
						bool					mesh_only,draw_area,draw_light_circle,
												swap_on,clip_on;
					} editor_3D_view_setup;

