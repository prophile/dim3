/****************************** File *********************************

Module: dim3 Map Import
Author: Brian Barnes
 Usage: External Header

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
// obj import settings
//

typedef struct	{
					int								portal_x_sz,portal_z_sz,portal_x_cnt,portal_z_cnt,
													texture_portal_wall,texture_portal_floor,texture_portal_ceiling;
					bool							lights,sight_path,flip_x,flip_z;
					char							path[1024];
				} import_obj_settings_type;
				
//
// height map import settings
//

#define hm_import_quads								0
#define hm_import_triangles							1

typedef struct	{
					int								import_type,
													portal_x_sz,portal_z_sz,portal_x_cnt,portal_z_cnt,
													portal_ty,portal_by,wall_high,
													texture_portal_wall,texture_portal_high_floor,
													texture_portal_mid_floor,texture_portal_low_floor;
					bool							wall,lights,sight_path,sight_path_all;
					char							path[1024];
				} import_height_map_settings_type;

//
// auto-generate flags and modes
//

#define ag_corridor_flag_portal						0
#define ag_corridor_flag_horizontal					1
#define ag_corridor_flag_vertical					2

#define ag_ceiling_lower_none						0
#define ag_ceiling_lower_neg_x						1
#define ag_ceiling_lower_pos_x						2
#define ag_ceiling_lower_neg_z						3
#define ag_ceiling_lower_pos_z						4

//
// auto-generate types
//

#define ag_corridor_type_count						3

#define ag_corridor_type_normal						0
#define ag_corridor_type_slanted_ceiling			1
#define ag_corridor_type_octagon					2

#define ag_stair_neg_z								0
#define ag_stair_pos_z								1
#define ag_stair_neg_x								2
#define ag_stair_pos_x								3

//
// auto-generate floor types
//

#define ag_floor_type_count							4

#define ag_floor_type_flat							0
#define ag_floor_type_rough							1
#define ag_floor_type_pit							2
#define ag_floor_type_platform						3

//
// auto-generate ceiling types
//

#define ag_ceiling_type_count						7

#define ag_ceiling_type_closed						0
#define ag_ceiling_type_open						1
#define ag_ceiling_type_cross						2
#define ag_ceiling_type_a_frame_horz_bar			3
#define ag_ceiling_type_a_frame_vert_bar			4
#define ag_ceiling_type_horz_bar					5
#define ag_ceiling_type_vert_bar					6

#define ag_ceiling_outer_ring						0
#define ag_ceiling_top_left							1
#define ag_ceiling_top_middle						2
#define ag_ceiling_top_right						3
#define ag_ceiling_middle_left						4
#define ag_ceiling_middle_middle					5
#define ag_ceiling_middle_right						6
#define ag_ceiling_bottom_left						7
#define ag_ceiling_bottom_middle					8
#define ag_ceiling_bottom_right						9
#define ag_ceiling_slant							10

#define ag_ceiling_wall_outer_ring					0
#define ag_ceiling_wall_horizontal					1
#define ag_ceiling_wall_vertical					2
#define ag_ceiling_wall_slant						3

#define ag_ceiling_data_bytes						{ \
													 {1,1,1,1,1,1,1,1,1,1,0}, \
													 {1,0,0,0,0,0,0,0,0,0,0}, \
													 {1,0,1,0,1,1,1,0,1,0,0}, \
													 {1,0,0,0,1,1,1,0,0,0,1}, \
													 {1,0,1,0,0,1,0,0,1,0,1}, \
													 {1,0,0,0,1,1,1,0,0,0,0}, \
													 {1,0,1,0,0,1,0,0,1,0,0} \
													}

#define ag_ceiling_wall_data_bytes					{ \
													 {0,0,0,0}, \
													 {1,0,0,0}, \
													 {1,1,1,0}, \
													 {0,1,0,1}, \
													 {0,0,1,1}, \
													 {1,1,0,0}, \
													 {1,0,1,0} \
													}

//
// auto-generate doors
//

#define ag_door_type_count							2

#define ag_door_type_slide_up						0
#define ag_door_type_split							1

//
// auto-generate blockings
//

#define max_ag_block_sz								10

#define ag_block_none								0
#define ag_block_circle								1
#define ag_block_u									2
#define ag_block_preset_h							3
#define ag_block_preset_l							4

#define ag_block_data_none_bytes					{ \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0} \
													}

#define ag_block_data_circle_bytes					{ \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0} \
													}

#define ag_block_data_u_bytes						{ \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0} \
													}

#define ag_block_data_h_bytes						{ \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0}, \
													 {0,0,0,1,1,1,1,0,0,0} \
													}
													
#define ag_block_data_l_bytes						{ \
													 {0,0,0,0,0,1,1,1,1,1}, \
													 {0,0,0,0,0,1,1,1,1,1}, \
													 {0,0,0,0,0,1,1,1,1,1}, \
													 {0,0,0,0,0,1,1,1,1,1}, \
													 {0,0,0,0,0,1,1,1,1,1}, \
													 {0,0,0,0,0,1,1,1,1,1}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0}, \
													 {0,0,0,0,0,0,0,0,0,0} \
													}

//
// auto-generate constants
//

#define ag_constant_portal_high_percent				0.10f
#define ag_constant_portal_story_high_add_percent	0.15f

#define ag_constant_portal_random_percent			0.4f
#define ag_constant_portal_merge_percent			0.08f
#define ag_constant_portal_connect_percent			1.0f
#define ag_constant_portal_split_factor_percent		0.1f

#define ag_constant_portal_high_extra_top			0.75f
#define ag_constant_portal_high_extra_bottom		0.50f
#define ag_constant_portal_high_slop_y				0.10f
#define ag_constant_portal_rough_floor_percent		0.025f
#define ag_constant_portal_ceiling_slant_percent	0.40f

#define ag_constant_portal_story_high				(4*map_enlarge)
#define ag_constant_story_steps_split_factor		1

#define ag_constant_corridor_size_percent			0.25f
#define ag_constant_corridor_random_percent			0.4f

#define ag_constant_door_width						(2*map_enlarge)
#define ag_constant_door_open_millisec				1000

#define ag_constant_step_high						(2*map_enlarge)
#define ag_constant_step_corridor_size				(8*map_enlarge)
#define ag_constant_step_story_size					(3*map_enlarge)

#define ag_constant_ramp_min_high					(4*map_enlarge)
#define ag_constant_ramp_size_percent				4.0f

//
// auto-generate structure
//

typedef struct	{
					int										map_sz,portal_sz,
															left,right,top,bottom;
				} auto_generate_setting_map_type;

typedef struct	{
					int										portal_wall,portal_floor,portal_ceiling,
															corridor_wall,corridor_floor,corridor_ceiling,
															second_story,door,steps,ramp;
				} auto_generate_setting_texture_type;

typedef struct	{
					int										seed,block,door_percentage,
															light_fudge_factor,light_fill_percentage,
															light_color_percentage,light_flicker_percentage,
															spot_count;
					char									door_sound[name_str_len];
					bool									doors,lights,spots;
					unsigned char							floor_type_on[ag_floor_type_count],
															ceiling_type_on[ag_ceiling_type_count],
															corridor_type_on[ag_corridor_type_count],
															door_type_on[ag_door_type_count];
					auto_generate_setting_map_type			map;
					auto_generate_setting_texture_type		texture;
				} auto_generate_settings_type;

//
// auto generate functions
//

extern void map_auto_generate_clear(map_type *map);
extern void map_auto_generate_add_simple_lights(map_type *map);
extern void map_auto_generate_add_player_spot(map_type *map);

extern void map_auto_generate(map_type *map,auto_generate_settings_type *ags);
extern bool map_auto_generate_test(map_type *map,bool load_shaders);
