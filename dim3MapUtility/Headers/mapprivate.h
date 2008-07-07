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
// Lighting Constants
//

#define light_tessel_overlap_pixel					25

//
// Old map version structures (v1)
//

#define max_segment									32768
#define max_primitive_stack							8
#define ambient_push_size							4

#define sg_wall										0
#define sg_floor									1
#define sg_ceiling									2
#define sg_liquid									3
#define sg_ambient_wall								4
#define sg_ambient_fc								5

#define no_curve									0

#define wc_none										0
#define wc_top										1
#define wc_bottom									2
#define wc_slant									3
#define wc_top_curve								4
#define wc_top_arch									5
#define wc_bottom_curve								6
#define wc_bottom_arch								7
#define wc_top_curve_arch							8
#define wc_bottom_curve_arch						9

#define wc_tessel_start								4
#define wc_tessel_end								9

#define cv_none										0
#define cv_forward									1
#define cv_backward									2

#define ap_none										0
#define ap_top										1
#define ap_bottom									2
#define ap_left										3
#define ap_right									4
#define ap_up										5
#define ap_down										6

#define ta_0										0
#define ta_90										1
#define ta_180										2
#define ta_270										3

typedef struct		{
                        int							lx,rx,lz,rz,ty,by,
													ptsz,x[4],z[4],y[4];
					} wall_segment_data;
					
typedef struct		{
						int							ptsz,x[8],z[8],y[8],
													min_x,max_x,min_z,max_z,min_y,max_y;
						float						slope_y,slope_ang_y,slope_move_x,slope_move_z;
						bool						flat;
					} fc_segment_data;

typedef struct		{
						short						harm,drown_harm,wavesize,
													tiderate,tidesize,tidedirection;
						int							y,lft,rgt,top,bot,
													vl_idx_start,vl_x_sz,vl_z_sz,
													drown_tick;
						float						speed_alter,tint_alpha;
						d3col						col;
					} liquid_segment_data;

typedef struct		{
						short						push;
						int							lx,rx,lz,rz,ty,by,
													ptsz,x[4],z[4],y[4];
					} ambient_wall_segment_data;
					
typedef struct		{
						short						push;
						int							ptsz,x[8],z[8],y[8],
													min_x,max_x,min_z,max_z,min_y,max_y;
					} ambient_fc_segment_data;
					
typedef union		{
						wall_segment_data			wall;
						fc_segment_data				fc;
						liquid_segment_data			liquid;
						ambient_wall_segment_data	ambient_wall;
						ambient_fc_segment_data		ambient_fc;
					} segment_data;

typedef struct		{
						int							ptsz;
						int							idx[8];
						float						gx[8],gy[8];
					} segment_draw_type;

typedef struct		{
						short						type,rn,group_idx,
													primitive_uid[max_primitive_stack],
													tag,clip,curve,fill,txt_ang,txt_offset;
						float						x_txtfact,y_txtfact,x_txtoff,y_txtoff,
													x_shift,y_shift,dark_factor,alpha;
						bool						on,pass_through,moveable,shiftable,climbable,
													lock,simple_tessel;
						d3pnt						middle;
						segment_draw_type			draw;
						segment_data				data;
					} segment_type;

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

extern bool map_portal_build_single_vertex_list(map_type *map,int rn);
extern bool map_portal_create_single_vertex_list(map_type *map,int rn);
extern void map_portal_dispose_single_vertex_list(map_type *map,int rn);

