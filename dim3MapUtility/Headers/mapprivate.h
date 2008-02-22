/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Private Header

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// Setting Structures
//

typedef struct		{
						int						anisotropic_mode,texture_quality_mode,mipmap_mode;
						bool					card_generated_mipmaps,compression;
						file_path_setup_type	file_path_setup;
                    } maputility_settings_type;

//
// Functions
//

extern int map_count_texture_frames(map_type *map,int txt);
extern bool map_textures_read(map_type *map);
extern bool map_textures_setup_glowmaps(map_type *map);
extern void map_textures_close(map_type *map);
extern bool map_shaders_read(map_type *map);
extern void map_shaders_close(map_type *map);
extern bool read_map_xml(map_type *map);
extern bool write_map_xml(map_type *map);
extern void map_textures_new(map_type *map);

extern void map_segments_clear_touch(map_type *map);

extern void map_prepare_setup_curve_constant(int curve_level);
extern void map_prepare_create_wall_curve(map_type *map,segment_type *seg);
extern void map_prepare_create_fc_curve(map_type *map,segment_type *seg);
extern void map_prepare_create_wall_clip(map_type *map,segment_type *seg);
extern void map_prepare_create_wall_segment_polygon(segment_type *seg);
extern void map_prepare_create_wall_segment_uv(segment_type *seg);
extern void map_prepare_create_fc_segment_uv(segment_type *seg);
extern void map_prepare_create_push_ambient_wall_segment_polygon(segment_type *seg);
extern void map_prepare_create_ambient_wall_segment_uv(segment_type *seg);
extern void map_prepare_push_ambient_fc_segment_polygon(segment_type *seg);
extern void map_prepare_create_ambient_fc_segment_uv(segment_type *seg);
extern void map_prepare_calculate_segment_vertex_middle(segment_type *seg);
extern void map_prepare_create_segment_middles(map_type *map);
extern void map_prepare_set_ambient_fc_segment_square(segment_type *seg);
extern bool map_portal_build_single_vertex_list(map_type *map,int rn,bool high_quality_lighting);
extern bool map_portal_create_single_vertex_list(map_type *map,int rn,bool high_quality_lighting);
extern void map_portal_dispose_single_vertex_list(map_type *map,int rn);
extern void map_portal_build_single_segment_list(map_type *map,int rn);

