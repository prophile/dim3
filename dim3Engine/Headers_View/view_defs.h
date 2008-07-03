/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Definitions Header

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
// view images (huds, particles, crosshairs, etc)
//

#define max_view_image				1024

//
// input rate
//

#define input_tick_rate				33

//
// stencil constants
//

#define stencil_none				0
#define stencil_sky					1
#define stencil_poly_start			2
#define stencil_poly_end			255

//
// remote names
//

#define remote_name_max_distance	(map_enlarge*500)	// how far away you can see map names
#define remote_name_min_distance	(map_enlarge*100)	// closest you can see map names in full

//
// sorting types
//

#define view_sort_object			0
#define view_sort_projectile		1

//
// image structures
//

typedef struct		{
						char					path[1024];
						bitmap_type				bitmap;
					} view_image_type;

//
// object sorting structures
//

typedef struct		{
						short					type,idx;
						double					dist;
					} view_sort_item_type;

typedef struct		{
						int						count;
						view_sort_item_type		items[max_object+max_projectile];
					} view_sort_type;

//
// effect sorting structures
//

typedef struct		{
						short					idx;
						int						dist;
					} view_sort_effect_type;

//
// join UI structures
//

typedef struct		{
						int							ping_msec,player_count,player_max_count;
						bool						local;
						char						ip[256],name[name_str_len],
													game_name[name_str_len],map_name[name_str_len];
					} join_server_info;

//
// halo structures
//
 
typedef struct		{
						int						idx,obj_uid,
												min_dist,max_dist,
												min_pixel_sz,max_pixel_sz,pixel_sz;
						float					min_alpha,max_alpha,alpha;
						bool					in_view,no_clip_object,no_clip_self;
						d3pnt					pnt,proj_pnt;
					} halo_draw_type;

//
// rain structures
//

typedef struct		{
						int						x,z,y,by,
												x_off,z_off;
					} rain_draw_type;

//
// timing structures
//

typedef struct		{
						int						input_tick,draw_tick,draw_time,
												run_tick,ambient_tick;
					} view_time_type;

//
// view draw type
//

typedef struct		{
						int						projection_type,
												lft,rgt,top,bot,
												near_z,far_z,near_z_offset;
						float					fov,aspect_ratio;
						d3pos					pos;
						d3ang					ang;
						liquid_pointer_type		under_liquid;
					} view_camera_type;

//
// fps type
//

typedef struct		{
						int						tick,count,time;
						float					total;
					} view_fps_type;

//
// count structure
//

typedef struct		{
						int						image,
												halo_draw,halo_draw_in_view;
					} view_count_type;

//
// main display structure
//
 
typedef struct		{
						view_count_type			count;
						view_camera_type		camera;
						view_time_type			time;
						view_fps_type			fps;
						view_sort_type			sort;
						view_image_type			*images;
						halo_draw_type			*halo_draws;
						rain_draw_type			*rain_draws;
					} view_type;


