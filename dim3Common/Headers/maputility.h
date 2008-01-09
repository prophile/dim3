/****************************** File *********************************

Module: dim3 Map Utility
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
// Constants
//

extern char segment_type_str[][32];
extern char segment_clip_str[][32];
extern char segment_curve_str[][32];
extern char segment_push_str[][32];
extern char light_type_str[][32];

//
// map sizes
//

#define map_enlarge									144				// each units of maze equals this many game units

#define map_x_size									2500			// # of units on X
#define map_z_size									2500			// # of units on Z

//
// map maximums
//

#define max_portal									256				// maximum number of portals in a map
#define max_segment									32768			// maximum number of segments in a map

#define max_map_scenery								512				// maximum number of ambient scenery
#define max_map_light								256				// maximum number of ambient lights
#define max_map_sound								256				// maximum number of ambient sounds
#define max_map_particle							256				// maximum number of ambient particles

#define max_sight_list								64				// maximum number of portals that can be seen from current portal
#define max_sight_link								8				// maximum number of sight links from a single portal

#define max_group									128				// maximum number of segment groups

#define max_map_texture								128				// maximum number of textures in a map

#define max_spot									512				// maximum number of spot in a map
#define max_node									512				// maximum number of nodes in a map
#define max_node_link								16				// maximum number of connected nodes

#define max_movement								64				// maximum number of movements
#define max_movement_move							16				// maximum number of separate moves in a movement

#define nopath										-1				// no path in sight structs

//
// strings
//

#define param_str_len								256				// length of parameter string

//
// physics
//

#define floor_slop									128				// how close to floor to be "standing" on it

#define gravity_start_power							0				// starting gravity
#define gravity_factor								5000			// factor to divide gravity out by when combing with weight

#define gravity_slope_min_y							0.4f			// minimum slope needed to activate gravity
#define gravity_slope_max_y							1.5f			// maximum climbable slope
#define gravity_slope_factor						150.0f			// slope gravity factor
#define gravity_slope_down_cut						0.2f			// amount to count gravity when running with it

#define proj_reflect_slope_max_y					0.5f			// how big a slope before bounces become reflects
#define proj_bounce_min_speed						10.0f			// minimum speed before bounces cancel

#define ambient_push_size							4				// pixel push for ambients

//
// segment constants
//

#define sg_wall										0					// segment types
#define sg_floor									1
#define sg_ceiling									2
#define sg_liquid									3
#define sg_ambient_wall								4
#define sg_ambient_fc								5

#define max_primitive_stack							8					// size of primitive stack (for hierarchy)

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

#define wc_tessel_start								4					// which wall clippings cause tesselation?
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

//
// texture angles
//

#define ta_0										0
#define ta_90										1
#define ta_180										2
#define ta_270										3

//
// lights
//

#define max_light_spot								64					// maximum number of lighting objects in a view

#define lt_normal									0
#define lt_blink									1
#define lt_glow										2
#define lt_pulse									3
#define lt_flicker									4
#define lt_failing									5

//
// media types
//

#define mi_none										0
#define mi_story									1
#define mi_title									2
#define mi_movie									3

//
// sky types
//

#define st_globe									0
#define st_dome_panoramic							1
#define st_dome_hemisphere							2
#define st_cylinder									3
#define st_cube										4

//
// liquids
//

#define ld_horizontal								0
#define ld_vertical									1

//
// light tessel constants
//

#define light_tessel_grid_sz						6
#define light_tessel_max_size						(map_enlarge*64)

#define light_tessel_max_trig						72
#define light_tessel_max_vertex						216

//
// portal vertex list constants
//

#define flag_pvl_none								0x00
#define flag_pvl_moveable							0x01
#define flag_pvl_shiftable							0x02

//
// group structure
//

typedef struct		{
						int							seg_count;
						char						name[name_str_len];
						short						*seg_list;
					} group_type;

//
// mesh structures
//

typedef struct		{
						bool						flat;
						d3pnt						min,max;
					} map_mesh_poly_box_type;

typedef struct		{
						float						y,ang_y,move_x,move_z;
					} map_mesh_poly_slope_type;

typedef struct		{
						int							v[8],txt_frame_offset;
						bool						simple_tessel,shiftable;
					} map_mesh_poly_draw_type;

typedef struct		{
						int							ptsz,v[8],txt_idx;
						float						gx[8],gy[8],x_shift,y_shift,
													alpha;
						map_mesh_poly_box_type		box;
						map_mesh_poly_slope_type	slope;
						map_mesh_poly_draw_type		draw;
					} map_mesh_poly_type;

typedef struct		{
						int							nvertex,npoly;
						d3pnt						*vertexes;
						map_mesh_poly_type			*polys;
					} map_mesh_type;

//
// segment structures
//

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
													x[4],z[4],y[4];
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
						int							trig_count,
													trig_vertex_idx[light_tessel_max_vertex];
					} segment_light_type;
					
typedef struct		{
						int							stencil_pass,stencil_idx,
													lod_dist;
						float						normal[3];
						bool						light_simple;
					} segment_render_type;

typedef struct		{
						short						type,rn,group_idx,
													primitive_uid[max_primitive_stack],
													tag,clip,curve,fill,txt_ang,txt_offset,
													decal_count;
						float						x_txtfact,y_txtfact,x_txtoff,y_txtoff,
													x_shift,y_shift,dark_factor,alpha;
						bool						on,pass_through,moveable,shiftable,climbable,
													lock,simple_tessel,touched;
						d3pnt						middle;
						segment_draw_type			draw;
						segment_light_type			light;
						segment_render_type			render;
						segment_data				data;
					} segment_type;

//
// portal structures
//

typedef struct		{
						short						rn,link[max_sight_link];
                        bool						root;
					} portal_sight_list_type;

typedef struct		{
						int							max_count,count;
						short						*list;
					} portal_segment_list_type;

typedef struct		{
						int							opaque_normal_count,
													opaque_bump_count,
													opaque_light_count,
													opaque_simple_normal_count,
													opaque_specular_count,
													opaque_glow_count,
													transparent_count,
													shader_count;
						short						*opaque_normal_list,
													*opaque_bump_list,
													*opaque_light_list,
													*opaque_simple_normal_list,
													*opaque_specular_list,
													*opaque_glow_list,
													*transparent_list,
													*transparent_sort_list,
													*shader_list;
						float						*transparent_sort_z_list;
					} portal_segment_draw_type;

typedef struct		{
						int							nspot;
						light_spot_type				*spots;
					} portal_light_type;
					
typedef struct		{
						int							entry_id,exit_id,base_team;
						bool						entry_on,exit_on,base_on,map_change_on;
						char						map_name[name_str_len],
													map_spot_name[name_str_len],map_spot_type[name_str_len];
					} portal_message_type;

typedef struct		{
						short						flags;
						float						x,y,z,gx,gy;
					} portal_vertex_list_type;

typedef struct		{
						int							nvlist;
						float						*pvert,*pcoord,*pcolor;
						portal_vertex_list_type		*vertex_list;
					} portal_vertex_type;

typedef struct		{
						int							nmesh;
						map_mesh_type				*meshes;
					} portal_mesh_type;

typedef struct		{
						int							x,z,ex,ez,mx,mz,ty,by,
													opaque_stencil_pass_start,
													opaque_stencil_pass_end,
													decal_count;
						bool						in_path,reset_segment_list;
						char						name[name_str_len];
						portal_message_type			msg;
						portal_sight_list_type		sight[max_sight_list];
						portal_mesh_type			mesh;
						portal_segment_list_type	wall_list_hit,
													fc_list_hit,
													liquid_list_hit,
													segment_list_all,
													segment_list_draw,
													segment_list_texture_shift;
						portal_segment_draw_type	segment_draw;
						portal_light_type			light,light_last;
						portal_vertex_type			vertexes;
					} portal_type;

//
// map scenery, light, sound and particle structures
//

					
typedef struct		{
						int							lighting_mode;
						char						model_name[name_str_len],
													animation_name[name_str_len];
						bool						contact,contact_hit_box,face_forward,
													shadow,shadow_cast_down,override_size;
						d3pos						pos;
						d3ang						ang;
						d3pnt						size;
					} map_scenery_type;

typedef struct		{
						short						type;
						int							intensity;
						bool						on,confine_to_portal;
						d3pos						pos;
						d3col						col;
					} map_light_type;
					
typedef struct		{
						int							buffer_idx;
                        float						pitch;
						char						name[name_str_len];
						bool						on;
						d3pos						pos;
					} map_sound_type;
					
typedef struct		{
                        int							spawn_tick,slop_tick,next_spawn_tick,
													particle_idx;
						char						name[name_str_len];
						bool						on;
						d3pos						pos;
					} map_particle_type;
                    
//
// spot and node structures
//

typedef struct		{
						char						name[name_str_len],
													type[name_str_len],
													script[name_str_len],
													display_model[name_str_len],
													params[param_str_len],
													attach_name[name_str_len],
													attach_type[name_str_len],
													attach_script[file_str_len],
													attach_params[param_str_len];
						bool						attach;
						d3pos						pos;
						d3ang						ang;
					} spot_type;

typedef struct		{
						short						idx,user_value,
													link[max_node_link],
													path_hint[max_node];
						char						name[name_str_len];
						d3pos						pos;
						d3ang						ang;
					} node_type;

//
// movement structures
//
 
typedef struct		{
						int							msec,user_id,sound_buffer_idx;
						char						sound_name[name_str_len];
						float						sound_pitch;
						d3pnt						mov;
					} movement_move_type;

typedef struct		{
						int							nmove,group_idx,reverse_group_idx,
													auto_open_distance,cur_move_idx;
						char						name[name_str_len];
						bool						auto_start,auto_open,auto_open_never_close,
													loop,started,opened,reverse;
						d3pnt						pnt;
						movement_move_type			moves[max_movement_move];
					} movement_type;

//
// map media and music
//

typedef struct		{
						int							type;
						char						name[name_str_len],title_sound_name[name_str_len];
					} map_media_type;
					
typedef struct		{
						int							fade_msec;
						char						name[name_str_len];
					} map_music_type;
					
//
// map info and sky structures
//

typedef struct		{
						char						name[name_str_len],
													title[name_str_len],
													author[name_str_len],
													host_name[name_str_len],
													player_start_name[name_str_len],
													player_start_type[name_str_len],
													load_path[1024];
						bool						in_load;
					} map_info_type;
					
typedef struct		{
						float						gravity,gravity_max_power,gravity_max_speed,
													resistance,txt_scale_x,txt_scale_y;
						bool						speculars_hilite,editor_link_always_start;
					} map_settings_type;
					
typedef struct		{
						int							lod_light_distance,
													lod_bump_distance,
													lod_specular_distance,
													lod_glow_distance,
													lod_model_distance,
													lod_shadow_distance,
													lod_effect_distance;
					} map_optimizations_type;

typedef struct		{
						float						sound_pitch,light_drop_off_factor;
						char						sound_name[name_str_len];
						d3col						light_color;
					} map_ambient_type;					

typedef struct		{
						int							density,radius,height,speed,
													line_width,line_length,slant_add,
													slant_time_msec,slant_change_msec;
						float						alpha;
						bool						on,reset;
						d3col						start_color,end_color;
					} map_rain_type;					
					
typedef struct		{
						int							fill;
						float						x_scroll_fact,y_scroll_fact;
						bool						on;
					} map_background_type;					
					
typedef struct		{
						int							type,radius,extra_height,dome_y,
													fill,bottom_fill,north_fill,south_fill,east_fill,west_fill;
						float						txt_fact,txt_x_shift,txt_y_shift;
						bool						on;
					} map_sky_type;
					
typedef struct		{
						int							count,inner_radius,outer_radius,
													high,drop,texture_idx;
						float						speed,txt_x_fact,txt_y_fact,
													alpha;
						bool						on,use_solid_color;
						d3col						col;
					} map_fog_type;

//
// main map structure
//

typedef struct		{
						int							nportal,nlight,nsound,nparticle,
													nspot,nnode,nscenery,nmovement,
													ngroup,nsegment,
													start_game_tick;
													
						map_info_type				info;
						
						map_ambient_type			ambient;
						map_background_type			background;
						map_sky_type				sky;
						map_fog_type				fog;
						map_rain_type				rain;
						
						map_settings_type			settings;
						map_optimizations_type		optimizations;
						map_media_type				media;
						map_music_type				music;
						
						texture_type				*textures;
						
						portal_type					*portals;
						
						map_scenery_type			*sceneries;
						map_light_type				*lights;
						map_sound_type				*sounds;
						map_particle_type			*particles;
						
						spot_type					*spots;
						node_type					*nodes;
						
						movement_type				*movements;
						
						group_type					*groups;
						segment_type				*segments;
						
					} map_type;

//
// Functions
//

extern void map_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int texture_quality_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression);
extern bool map_new(map_type *map,char *name);
extern bool map_open(map_type *map,char *name,bool load_bitmaps,bool setup_glowmaps,bool load_shaders);
extern bool map_open_primitive(map_type *map,char *path);
extern bool map_reload(map_type *map);
extern bool map_save(map_type *map);
extern void map_close(map_type *map);
extern void map_refresh_textures(map_type *map);

extern void map_prepare(map_type *map,int curve_level);

extern int map_count_texture_frames(map_type *map,int txt);
extern void map_setup_animated_textures(map_type *map,int tick);

extern int map_portal_create(map_type *map,int x,int z,int ex,int ez);
extern int map_find_portal(map_type *map,int x,int y,int z);
extern int map_find_portal_hint(map_type *map,int hint_rn,int x,int y,int z);
extern bool map_find_portal_by_pos(map_type *map,d3pos *pos);
extern int map_portal_count_segments(map_type *map,int rn);
extern void map_portal_calculate_center(map_type *map,int rn,int *x,int *y,int *z);
extern void map_portal_calculate_center_floor(map_type *map,int rn,int *x,int *y,int *z);
extern void map_portal_delete(map_type *map,int rn);
extern int map_portal_duplicate(map_type *map,int rn,int x,int z);
extern bool map_portal_touching_portal(map_type *map,int base_rn,int rn);
extern int map_portal_draw_sort(map_type *map,int rn,int cx,int cy,int cz,int *portal_list);

extern void map_portal_sight_clear(map_type *map,int rn);
extern void map_portal_sight_prune(map_type *map,int rn,int site_idx);
extern void map_portal_sight_delete_adjust_path(map_type *map,int rn);
extern void map_portal_sight_generate_paths(map_type *map,bool ignore_depth);

extern void map_portal_clear_segment_list_changes(map_type *map);
extern void map_portal_set_segment_list_changes(map_type *map,int rn);
extern void map_portal_rebuild_segment_list_changes(map_type *map);

extern bool map_portal_create_vertex_lists(map_type *map,bool high_quality_lighting);
extern void map_portal_dispose_vertex_lists(map_type *map);
extern void map_portal_rebuild_vertex_lists(map_type *map,bool high_quality_lighting);
extern bool map_portal_create_segment_lists(map_type *map);
extern void map_portal_dispose_segment_lists(map_type *map);

extern void map_portal_liquid_vertex_set_tide(map_type *map,int tick,portal_type *portal);
extern void map_portal_segment_update_shift(map_type *map,int tick,int rn);

extern void map_set_light_base(d3col *col);
extern void map_portal_clear_lights(portal_type *portal);
extern void map_portal_add_light(map_type *map,portal_type *portal,light_spot_type *lspot);
extern bool map_portal_light_check_changes(portal_type *portal);
extern void map_portal_light_check_changes_reset(portal_type *portal);
extern void map_portal_light_check_changes_reset_all(map_type *map);
extern bool map_portal_light_in_view(map_type *map,int x,int y,int z,int intensity);
extern void map_portal_get_light(portal_type *portal,double x,double y,double z,d3col *col);
extern light_spot_type* map_portal_find_closest_light(portal_type *portal,double x,double y,double z,int *p_dist);
extern void map_portal_calculate_light_color(portal_type *portal,double x,double y,double z,float *cf);
extern void map_portal_calculate_normal_vector(portal_type *portal,double x,double y,double z,float *nf);
extern void map_portal_calculate_normal_vector_smooth(portal_type *portal,double x,double y,double z,float *nf);
extern bool map_portal_create_light_spots(map_type *map);
extern void map_portal_dispose_light_spots(map_type *map);

extern int map_count_spot(map_type *map,char *name,char *type);
extern int map_find_spot(map_type *map,char *name,char *type);
extern int map_find_random_spot(map_type *map,char *name,char *type);
extern void map_spot_clear_attach(map_type *map);
extern void map_spot_attach_object(spot_type *spot,char *name,char *type,char *script,char *params);

extern int map_find_node(map_type *map,char *name);
extern int map_find_nearest_node(map_type *map,int x,int y,int z,int user_value,float ang,float ang_sweep,int min_dist,int max_dist);
extern int map_find_next_node_in_path(map_type *map,int from_idx,int to_idx);
extern int map_node_to_node_distance(map_type *map,int from_idx,int to_idx);

extern int map_primitive_create_uid(map_type *map);
extern void map_primitive_push_uid(map_type *map,segment_type *seg,int primitive_uid);
extern void map_primitive_pop_uid(map_type *map,segment_type *seg);

extern bool map_portal_mesh_add(map_type *map,int portal_idx,int add_count);
extern bool map_portal_mesh_delete(map_type *map,int portal_idx,int mesh_idx);
extern bool map_portal_mesh_set_vertex_count(map_type *map,int portal_idx,int mesh_idx,int nvertex);
extern bool map_portal_mesh_set_poly_count(map_type *map,int portal_idx,int mesh_idx,int npoly);

extern int map_segment_group_find_name(map_type *map,char *name);
extern void map_segment_move(map_type *map,segment_type *seg,int x,int y,int z);
extern void map_segment_calculate_extent(map_type *map,int seg_idx,int *min_x,int *min_z,int *min_y,int *max_x,int *max_z,int *max_y);
extern void map_segment_calculate_center(map_type *map,int seg_idx,int *x,int *y,int *z);
extern void map_segments_clear_touch(map_type *map);
extern int map_segments_count_touch(map_type *map);
extern void map_segment_reset_texture_uvs(map_type *map,segment_type *seg);

extern bool map_group_create_segment_list(map_type *map);
extern void map_group_dispose_segment_list(map_type *map);
extern void map_group_get_center(map_type *map,int group_idx,int *x,int *y,int *z);

extern int map_movement_find(map_type *map,char *name);
extern int map_movement_add(map_type *map);
extern void map_movement_delete(map_type *map,int movement_idx);
extern int map_movement_move_add(map_type *map,int movement_idx);
extern void map_movement_move_delete(map_type *map,int movement_idx,int move_idx);

extern bool map_add_texture_frame(map_type *map,int txt,char *bitmap_name,char *bumpmap_name,char *specularmap_name,char *glowmap_name);
extern bool map_delete_texture_frame(map_type *map,int txt);
