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
// constants
//

extern char light_type_str[][32];

//
// map sizes
//

#define map_enlarge									144				// each units of maze equals this many game units

#define map_max_size								500000			// maximum size of map on x/z units

#define map_simple_tessel_size						500				// below this size makes simple lighting tessel

//
// map maximums
//

#define max_portal									256				// maximum number of portals in a map

#define max_map_scenery								512				// maximum number of ambient scenery
#define max_map_light								256				// maximum number of ambient lights
#define max_map_sound								256				// maximum number of ambient sounds
#define max_map_particle							256				// maximum number of ambient particles

#define max_sight_list								64				// maximum number of portals that can be seen from current portal
#define max_sight_link								8				// maximum number of sight links from a single portal

#define max_group									128				// maximum number of mesh groups

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
// mesh poly draw flag
//

#define map_mesh_poly_draw_stencil_normal			0
#define map_mesh_poly_draw_stencil_bump				1
#define map_mesh_poly_draw_simple_normal			2
#define map_mesh_poly_draw_simple_bump				3
#define map_mesh_poly_draw_hilite_normal			4
#define map_mesh_poly_draw_hilite_bump				5
#define map_mesh_poly_draw_transparent				6
#define map_mesh_poly_draw_opaque_shader			7
#define map_mesh_poly_draw_transparent_shader		8

//
// group types
//

#define group_type_mesh								0
#define group_type_liquid							1

//
// skills
//

#define skill_easy									0
#define skill_medium								1
#define skill_hard									2

//
// group structure
//

typedef struct		{
						short						type,portal_idx,idx;
					} group_unit_type;

typedef struct		{
						int							thing_type,thing_uid,
													script_uid;
					} group_script_attach_type;

typedef struct		{
						int							count,movement_idx,user_id;
						bool						on,freeze,has_mov,has_rot;
						d3fpnt						cur_mov;
						d3vct						mov_add,rot_add;
						group_script_attach_type	attach;
					} group_move_type;

typedef struct		{
						int							unit_count;
						char						name[name_str_len];
						group_move_type				move;
						group_unit_type				*unit_list;
					} group_type;

//
// mesh structures
//

typedef struct		{
						bool						flat,wall_like;
						d3pnt						min,max,mid;
					} map_mesh_poly_box_type;

typedef struct		{
						int							lx,rx,lz,rz;
					} map_mesh_poly_line_type;

typedef struct		{
						float						y,ang_y,move_x,move_z;
					} map_mesh_poly_slope_type;

typedef struct		{
						int							trig_count,
													trig_vertex_idx[light_tessel_max_vertex];
					} map_mesh_poly_light_type;

typedef struct		{
						int							portal_v[8],draw_type,
													cur_frame,txt_frame_offset,
													stencil_pass,stencil_idx;
						float						normal[3];
						bool						simple_tessel,shift_on,
													is_simple_lighting,is_glow,is_specular;
					} map_mesh_poly_draw_type;

typedef struct		{
						int							ptsz,v[8],txt_idx;
						float						gx[8],gy[8],x_shift,y_shift,
													dark_factor,alpha;
						map_mesh_poly_box_type		box;
						map_mesh_poly_line_type		line;
						map_mesh_poly_slope_type	slope;
						map_mesh_poly_light_type	light;
						map_mesh_poly_draw_type		draw;
					} map_mesh_poly_type;
					
typedef struct		{
						d3pnt						min,max,mid;
					} map_mesh_box_type;

typedef struct		{
						bool						on,pass_through,moveable,
													hilite,climbable,shiftable,
													lock_uv,touched;
					} map_mesh_flag_type;

typedef struct		{
						int							stencil_pass_start,stencil_pass_end;
						bool						has_stencil_normal,has_stencil_bump,
													has_simple_normal,has_simple_bump,
													has_hilite_normal,has_hilite_bump,
													has_transparent,has_specular,has_glow,
													has_opaque_shader,has_transparent_shader;
					} map_mesh_draw_type;

typedef struct		{
						int							nvertex,npoly,group_idx;
						d3pnt						rot_off;
						d3pnt						*vertexes;
						map_mesh_poly_type			*polys;
						map_mesh_box_type			box;
						map_mesh_flag_type			flag;
						map_mesh_draw_type			draw;
					} map_mesh_type;

//
// liquid structures
//

typedef struct		{
						int							in_harm,drown_harm,drown_tick;
					} map_liquid_harm_type;

typedef struct		{
						int							rate,high,direction,split;
					} map_liquid_tide_type;

typedef struct		{
						int							v_cnt,x_sz,z_sz;
						int							*idx_list;
						float						*vl_list,*uv_list,*cl_list;
					} map_liquid_draw_type;

typedef struct		{
						int							y,lft,rgt,top,bot,txt_idx,group_idx;
						float						alpha,speed_alter,tint_alpha,
													x_txtfact,y_txtfact,x_txtoff,y_txtoff,
													x_shift,y_shift;
						d3col						col;
						map_liquid_harm_type		harm;
						map_liquid_tide_type		tide;
						map_liquid_draw_type		draw;
					} map_liquid_type;
					
//
// mesh poygon sort structure
//

typedef struct		{
						int							mesh_idx,poly_idx;
						float						dist;
					} map_mesh_poly_sort_type;

//
// portal structures
//

typedef struct		{
						short						rn,link[max_sight_link];
                        bool						root;
					} portal_sight_list_type;

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
						unsigned char				*phit;
						portal_vertex_list_type		*vertex_list;
					} portal_vertex_type;

typedef struct		{
						int							sort_cnt;
						bool						has_stencil_normal,has_stencil_bump,has_stencil_lighting,
													has_simple_normal,has_simple_bump,has_simple_lighting,
													has_hilite_normal,has_hilite_bump,
													has_transparent,has_glow,has_specular,
													has_opaque_shader,has_transparent_shader;
						map_mesh_poly_sort_type		*sort_list;
					} portal_mesh_draw_type;

typedef struct		{
						int							nmesh;
						map_mesh_type				*meshes;
						portal_mesh_draw_type		draw;
					} portal_mesh_type;

typedef struct		{
						int							nliquid;
						map_liquid_type				*liquids;
					} portal_liquid_type;

typedef struct		{
						int							x,z,ex,ez,mx,mz,ty,by,
													opaque_stencil_pass_start,
													opaque_stencil_pass_end,
													decal_count;
						bool						in_path;
						char						name[name_str_len];
						portal_message_type			msg;
						portal_sight_list_type		sight[max_sight_list];
						portal_mesh_type			mesh;
						portal_liquid_type			liquid;
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
						int							skill;
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
						d3ang						rot;
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
						bool						editor_link_always_start;
						char						network_game_list[256];
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
													ngroup,start_game_tick;
													
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
						
					} map_type;

//
// Functions
//

extern void map_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int texture_quality_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression);
extern bool map_new(map_type *map,char *name);
extern bool map_open(map_type *map,char *name,bool load_bitmaps,bool setup_glowmaps,bool load_shaders);
extern bool map_reload(map_type *map);
extern bool map_save(map_type *map);
extern void map_close(map_type *map);
extern void map_refresh_textures(map_type *map);

extern void map_prepare(map_type *map);

extern int map_count_texture_frames(map_type *map,int txt);
extern void map_setup_animated_textures(map_type *map,int tick);

extern bool map_check_game_type(char *game_type,char *map_name);

extern int map_portal_create(map_type *map,int x,int z,int ex,int ez);
extern int map_find_portal(map_type *map,int x,int y,int z);
extern int map_find_portal_hint(map_type *map,int hint_rn,int x,int y,int z);
extern bool map_find_portal_by_pos(map_type *map,d3pos *pos);
extern void map_portal_calculate_y_extent(map_type *map,int rn,int *p_ty,int *p_by);
extern void map_portal_calculate_center(map_type *map,int rn,int *x,int *y,int *z);
extern void map_portal_delete(map_type *map,int rn);
extern int map_portal_duplicate(map_type *map,int rn,int x,int z);
extern bool map_portal_touching_portal(map_type *map,int base_rn,int rn);
extern int map_portal_draw_sort(map_type *map,int rn,int cx,int cy,int cz,int *portal_list);

extern void map_portal_sight_clear(map_type *map,int rn);
extern void map_portal_sight_prune(map_type *map,int rn,int site_idx);
extern void map_portal_sight_delete_adjust_path(map_type *map,int rn);
extern void map_portal_sight_generate_paths(map_type *map,bool ignore_depth);

extern bool map_portal_create_vertex_lists(map_type *map,bool high_quality_lighting);
extern void map_portal_dispose_vertex_lists(map_type *map);
extern void map_portal_rebuild_vertex_lists(map_type *map,bool high_quality_lighting);

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

extern int map_portal_mesh_add(map_type *map,int portal_idx);
extern bool map_portal_mesh_delete(map_type *map,int portal_idx,int mesh_idx);
extern bool map_portal_mesh_set_vertex_count(map_type *map,int portal_idx,int mesh_idx,int nvertex);
extern bool map_portal_mesh_set_poly_count(map_type *map,int portal_idx,int mesh_idx,int npoly);
extern int map_portal_mesh_duplicate(map_type *map,int portal_idx,int mesh_idx);
extern int map_portal_mesh_count_total_poly(map_type *map,int portal_idx);
extern int map_portal_mesh_add_poly(map_type *map,int portal_idx,int mesh_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt_idx);
extern bool map_portal_mesh_delete_poly(map_type *map,int portal_idx,int mesh_idx,int poly_idx);
extern bool map_portal_mesh_delete_unused_vertexes(map_type *map,int portal_idx,int mesh_idx);
extern bool map_portal_mesh_create_transparent_sort_lists(map_type *map);
extern void map_portal_mesh_dispose_transparent_sort_lists(map_type *map);
extern void map_portal_mesh_calculate_extent(map_type *map,int portal_idx,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_portal_mesh_calculate_center(map_type *map,int portal_idx,int mesh_idx,d3pnt *mpt);
extern void map_portal_mesh_calculate_uv_center(map_type *map,int portal_idx,int mesh_idx,float *gx,float *gy);

extern int map_portal_mesh_combine(map_type *map,int portal_idx,int mesh_1_idx,int mesh_2_idx);
extern int map_portal_mesh_switch_portal(map_type *map,int portal_idx,int mesh_idx,int new_portal_idx);
extern void map_portal_mesh_move(map_type *map,int portal_idx,int mesh_idx,bool do_portal_vertex_list,int x,int y,int z);
extern void map_portal_mesh_resize(map_type *map,int portal_idx,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_portal_mesh_flip(map_type *map,int portal_idx,int mesh_idx,bool flip_x,bool flip_y,bool flip_z);
extern void map_portal_mesh_rotate(map_type *map,int portal_idx,int mesh_idx,bool do_portal_vertex_list,float rot_x,float rot_y,float rot_z);
extern bool map_portal_mesh_tesselate(map_type *map,int portal_idx,int mesh_idx);
extern bool map_portal_mesh_poly_punch_hole(map_type *map,int portal_idx,int mesh_idx,int poly_idx,int hole_type);
extern void map_portal_mesh_shift_portal_vertex_list(map_type *map,int portal_idx,int tick);
extern void map_portal_mesh_get_poly_uv_as_box(map_type *map,int portal_idx,int mesh_idx,int poly_idx,float *x_txtoff,float *y_txtoff,float *x_txtfact,float *y_txtfact);
extern void map_portal_mesh_set_poly_uv_as_box(map_type *map,int portal_idx,int mesh_idx,int poly_idx,float x_txtoff,float y_txtoff,float x_txtfact,float y_txtfact);
extern void map_portal_mesh_reset_poly_uv(map_type *map,int portal_idx,int mesh_idx,int poly_idx);
extern void map_portal_mesh_reset_uv(map_type *map,int portal_idx,int mesh_idx);

extern int map_portal_liquid_add(map_type *map,int portal_idx);
extern bool map_portal_liquid_delete(map_type *map,int portal_idx,int liquid_idx);
extern int map_portal_liquid_duplicate(map_type *map,int portal_idx,int liquid_idx);
extern void map_portal_liquid_calculate_center(map_type *map,int portal_idx,int liquid_idx,d3pnt *pt);

extern int map_portal_liquid_switch_portal(map_type *map,int portal_idx,int liquid_idx,int new_portal_idx);
extern void map_portal_liquid_move(map_type *map,int portal_idx,int liquid_idx,int x,int y,int z);

extern bool map_group_create_unit_list(map_type *map);
extern void map_group_dispose_unit_list(map_type *map);
extern void map_group_get_center(map_type *map,int group_idx,int *x,int *y,int *z);

extern int map_movement_find(map_type *map,char *name);
extern int map_movement_add(map_type *map);
extern void map_movement_delete(map_type *map,int movement_idx);
extern int map_movement_move_add(map_type *map,int movement_idx);
extern void map_movement_move_delete(map_type *map,int movement_idx,int move_idx);

extern bool map_add_texture_frame(map_type *map,int txt,char *bitmap_name,char *bumpmap_name,char *specularmap_name,char *glowmap_name);
extern bool map_delete_texture_frame(map_type *map,int txt);
