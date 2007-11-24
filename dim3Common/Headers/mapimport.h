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
					int						portal_x_sz,portal_z_sz,portal_x_cnt,portal_z_cnt,
											texture_portal_wall,texture_portal_floor,texture_portal_ceiling;
					bool					lights,sight_path,flip_x,flip_z;
					char					path[1024];
				} import_obj_settings_type;
				
//
// height map import settings
//

#define hm_import_quads						0
#define hm_import_triangles					1

typedef struct	{
					int						import_type,
											portal_x_sz,portal_z_sz,portal_x_cnt,portal_z_cnt,
											portal_ty,portal_by,wall_high,
											texture_portal_wall,texture_portal_high_floor,
											texture_portal_mid_floor,texture_portal_low_floor;
					bool					wall,lights,sight_path,sight_path_all;
					char					path[1024];
				} import_height_map_settings_type;

//
// auto-generate flags and modes
//

#define ag_corridor_flag_portal				0
#define ag_corridor_flag_horizontal			1
#define ag_corridor_flag_vertical			2

#define ag_ceiling_lower_none				0
#define ag_ceiling_lower_neg_x				1
#define ag_ceiling_lower_pos_x				2
#define ag_ceiling_lower_neg_z				3
#define ag_ceiling_lower_pos_z				4

//
// auto-generate types
//

#define ag_corridor_type_normal				0
#define ag_corridor_type_slanted_ceiling	1
#define ag_corridor_type_octagon			2

//
// auto-generate blockings
//

#define max_ag_block_sz						10

#define ag_block_preset_empty				0
#define ag_block_preset_circle				1
#define ag_block_preset_top_u				2
#define ag_block_preset_bottom_u			3
#define ag_block_preset_left_u				4
#define ag_block_preset_right_u				5
#define ag_block_preset_vertical_h			6
#define ag_block_preset_horizontal_h		7

//
// auto-generate structure
//

typedef struct	{
					int						seed,
											max_map_x_size,max_map_z_size,
											map_left,map_right,map_top,map_bottom,
											initial_portal_count,
											min_portal_sz,max_portal_sz,
											portal_by,portal_ty,
											min_portal_extra_y,max_portal_extra_y,open_ceiling_extra_y,
											max_portal_merge_distance,
											min_portal_connect_distance,max_portal_connect_distance,
											min_portal_connect_sz,max_portal_connect_sz,
											corridor_type,ceiling_slant_extra_y,split_factor,
											open_hole_percentage,rough_floor_factor,door_percentage,
											light_fudge_factor,light_fill_percentage,
											light_color_percentage,light_flicker_percentage,
											spot_count,
											texture_portal_wall,texture_portal_floor,texture_portal_ceiling,
											texture_corridor_wall,texture_corridor_floor,texture_corridor_ceiling,
											texture_door;
					char					door_sound[name_str_len];
					bool					walls,floors,rough_portal_floors,rough_corridor_floors,
											ceilings,open_hole_ceilings,doors,
											lights,sight_path,spots;
					unsigned char			block[max_ag_block_sz][max_ag_block_sz],
											corridor_flags[max_portal];
					bool					open_hole_flags[max_portal];
				} auto_generate_settings_type;


