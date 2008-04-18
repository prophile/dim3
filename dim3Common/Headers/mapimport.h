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

#define ag_ceiling_type_count						3

#define ag_ceiling_type_closed						0
#define ag_ceiling_type_open						1
#define ag_ceiling_type_cross						2

#define ag_stair_neg_z								0
#define ag_stair_pos_z								1
#define ag_stair_neg_x								2
#define ag_stair_pos_x								3

//
// auto-generate blockings
//

#define max_ag_block_sz								10

#define ag_block_preset_empty						0
#define ag_block_preset_circle						1
#define ag_block_preset_top_u						2
#define ag_block_preset_bottom_u					3
#define ag_block_preset_left_u						4
#define ag_block_preset_right_u						5
#define ag_block_preset_vertical_h					6
#define ag_block_preset_horizontal_h				7

//
// auto-generate constants
//

#define ag_constant_portal_high_percent				0.10f

#define ag_constant_portal_random_percent			0.4f
#define ag_constant_portal_merge_percent			0.08f
#define ag_constant_portal_connect_percent			1.0f
#define ag_constant_portal_split_factor_percent		0.1f

#define ag_constant_portal_high_extra_top			0.75f
#define ag_constant_portal_high_extra_bottom		0.50f

#define ag_constant_corridor_size_percent			0.25f
#define ag_constant_corridor_random_percent			0.4f

#define ag_constant_step_high						(2*map_enlarge)
#define ag_constant_step_min_size					(6*map_enlarge)
#define ag_constant_step_max_size					(12*map_enlarge)

#define ag_constant_ramp_min_high					(4*map_enlarge)
#define ag_constant_ramp_min_size					(10*map_enlarge)
#define ag_constant_ramp_size_percent				4.0f

//
// auto-generate structure
//

typedef struct	{
					int										sz,
															left,right,top,bottom;
				} auto_generate_setting_map_type;

typedef struct	{
					int										sz;
				} auto_generate_setting_portal_type;
	
typedef struct	{
					bool									type_on[ag_corridor_type_count];
				} auto_generate_setting_corridor_type;
				
typedef struct	{
					bool									type_on[ag_ceiling_type_count];
				} auto_generate_setting_ceiling_type;

typedef struct	{
					int										portal_wall,portal_floor,portal_ceiling,
															corridor_wall,corridor_floor,corridor_ceiling,
															door,steps,ramp;
				} auto_generate_setting_texture_type;

typedef struct	{
					int										seed,
															rough_floor_factor,door_percentage,
															light_fudge_factor,light_fill_percentage,
															light_color_percentage,light_flicker_percentage,
															spot_count;
					char									door_sound[name_str_len];
					bool									rough_portal_floors,rough_corridor_floors,doors,
															lights,sight_path,spots;
					unsigned char							block[max_ag_block_sz][max_ag_block_sz];
					auto_generate_setting_map_type			map;
					auto_generate_setting_portal_type		portal;
					auto_generate_setting_corridor_type		corridor;
					auto_generate_setting_ceiling_type		ceiling;
					auto_generate_setting_texture_type		texture;
				} auto_generate_settings_type;

//
// auto generate functions
//

extern void map_auto_generate_clear(map_type *map);
extern void map_auto_generate_add_simple_lights(map_type *map);
extern void map_auto_generate_add_player_spot(map_type *map);

extern void map_auto_generate_block_preset(auto_generate_settings_type *ags,int block);

extern void map_auto_generate(map_type *map,auto_generate_settings_type *ags);
extern bool map_auto_generate_test(map_type *map,bool load_shaders);
