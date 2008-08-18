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

#define map_enlarge											144				// each units of maze equals this many game units

#define map_max_size										500000			// maximum size of map on x/z units

#define map_simple_tessel_size								500				// below this size makes simple lighting tessel

//
// map maximums
//

#define max_mesh											8192			// maximum number of meshes in a map
#define max_mesh_visibility_bytes							1024			// maximum number of bits for mesh list

#define max_map_texture										128				// maximum number of textures in a map

#define max_group											128				// maximum number of mesh groups

#define max_map_scenery										1024			// maximum number of ambient scenery
#define max_map_light										256				// maximum number of ambient lights
#define max_map_sound										256				// maximum number of ambient sounds
#define max_map_particle									256				// maximum number of ambient particles

#define max_spot											512				// maximum number of spot in a map
#define max_node											512				// maximum number of nodes in a map
#define max_node_link										16				// maximum number of connected nodes

#define max_movement										64				// maximum number of movements
#define max_movement_move									16				// maximum number of separate moves in a movement

#define max_sort_poly										256				// maximum number of transparent polys in a single scene

//
// strings
//

#define param_str_len										256				// length of parameter string

//
// physics
//

#define floor_slop											128				// how close to floor to be "standing" on it

#define gravity_start_power									0				// starting gravity
#define gravity_factor										5000			// factor to divide gravity out by when combing with weight

#define gravity_slope_min_y									0.4f			// minimum slope needed to activate gravity
#define gravity_slope_max_y									1.5f			// maximum climbable slope
#define gravity_slope_factor								150.0f			// slope gravity factor
#define gravity_slope_down_cut								0.2f			// amount to count gravity when running with it

#define proj_reflect_slope_max_y							0.5f			// how big a slope before bounces become reflects
#define proj_bounce_min_speed								10.0f			// minimum speed before bounces cancel

//
// lights
//

#define max_light_spot										64					// maximum number of lighting objects in a view

#define lt_normal											0
#define lt_blink											1
#define lt_glow												2
#define lt_pulse											3
#define lt_flicker											4
#define lt_failing											5

//
// media types
//

#define mi_none												0
#define mi_story											1
#define mi_title											2
#define mi_movie											3

//
// sky types
//

#define st_globe											0
#define st_dome_panoramic									1
#define st_dome_hemisphere									2
#define st_cylinder											3
#define st_cube												4

//
// liquids
//

#define ld_horizontal										0
#define ld_vertical											1

//
// light tessel constants
//

#define light_tessel_grid_sz								6
#define light_tessel_max_size								(map_enlarge*64)

#define light_tessel_max_trig								72
#define light_tessel_max_vertex								216

//
// group types
//

#define group_type_mesh										0
#define group_type_liquid									1

//
// skills
//

#define skill_easy											0
#define skill_medium										1
#define skill_hard											2

//
// group structure
//

typedef struct		{
						int									type,idx;
					} group_unit_type;

typedef struct		{
						int									thing_type,thing_uid,
															script_uid;
					} group_script_attach_type;

typedef struct		{
						int									count,movement_idx,user_id;
						bool								on,freeze,has_mov,has_rot;
						d3fpnt								cur_mov;
						d3vct								mov_add,rot_add;
						group_script_attach_type			attach;
					} group_move_type;

typedef struct		{
						int									unit_count;
						char								name[name_str_len];
						group_move_type						move;
						group_unit_type						*unit_list;
					} group_type;

//
// mesh structures
//

typedef struct		{
						bool								flat,wall_like;
						d3pnt								min,max,mid;
					} map_mesh_poly_box_type;

typedef struct		{
						int									lx,rx,lz,rz;
					} map_mesh_poly_line_type;

typedef struct		{
						float								y,ang_y,move_x,move_z;
					} map_mesh_poly_slope_type;

typedef struct		{
						int									x,y,z;
						float								gx,gy;
					} map_mesh_poly_tessel_vertex_type;

typedef struct		{
						int									nvertex,ntrig;
						int									*trig_vertex_idx,*trig_vertex_draw_idx;
						map_mesh_poly_tessel_vertex_type	*vertexes;
					} map_mesh_poly_light_type;

typedef struct		{
						int									portal_v[8],txt_frame_offset,
															stencil_pass,stencil_idx,decal_stencil_idx;
						float								x_shift_offset,y_shift_offset;
						bool								simple_tessel,shift_on;
					} map_mesh_poly_draw_type;

typedef struct		{
						int									ptsz,v[8],txt_idx;
						float								gx[8],gy[8],
															x_shift,y_shift,
															dark_factor,alpha;
						map_mesh_poly_box_type				box;
						map_mesh_poly_line_type				line;
						map_mesh_poly_slope_type			slope;
						map_mesh_poly_light_type			light;
						map_mesh_poly_draw_type				draw;
					} map_mesh_poly_type;
					
typedef struct		{
						d3pnt								min,max,mid;
					} map_mesh_box_type;

typedef struct		{
						bool								on,pass_through,moveable,
															hilite,climbable,shiftable,
															lock_uv,touched;
					} map_mesh_flag_type;

typedef struct		{
						int									entry_id,exit_id,base_team;
						bool								entry_on,exit_on,base_on,map_change_on;
						char								map_name[name_str_len],
															map_spot_name[name_str_len],map_spot_type[name_str_len];
					} map_mesh_message_type;

typedef struct		{
						int									nvertex,npoly,group_idx;
						unsigned char						mesh_visibility_flag[max_mesh_visibility_bytes];
						d3pnt								rot_off;
						d3pnt								*vertexes;
						map_mesh_poly_type					*polys;
						map_mesh_box_type					box;
						map_mesh_flag_type					flag;
						map_mesh_message_type				msg;
					} map_mesh_type;

//
// liquid structures
//

typedef struct		{
						int									in_harm,drown_harm,drown_tick;
					} map_liquid_harm_type;

typedef struct		{
						int									rate,high,direction,split;
					} map_liquid_tide_type;

typedef struct		{
						int									v_cnt,x_sz,z_sz;
					} map_liquid_draw_type;

typedef struct		{
						int									y,depth,lft,rgt,top,bot,txt_idx,group_idx;
						float								alpha,speed_alter,tint_alpha,
															x_txtfact,y_txtfact,x_txtoff,y_txtoff,
															x_shift,y_shift;
						d3col								col;
						map_liquid_harm_type				harm;
						map_liquid_tide_type				tide;
						map_liquid_draw_type				draw;
					} map_liquid_type;

//
// mesh and liquid collections
//

typedef struct		{
						int									nmesh;
						map_mesh_type						*meshes;
					} map_mesh_collection_type;

typedef struct		{
						int									nliquid;
						map_liquid_type						*liquids;
					} map_liquid_collection_type;

//
// map vertex draw lists
//

typedef struct		{
						int									sz,offset;
					} map_vertex_array_type;

typedef struct		{
						int									max_vertex_count,draw_vertex_count;
						int									*index_ptr;
						map_vertex_array_type				vert,uv,color,normal;
					} map_vertex_type;

//
// map sort lists
//

typedef struct		{
						int									mesh_idx,poly_idx;
						float								dist;
					} map_poly_sort_item_type;

typedef struct		{
						int									count;
						map_poly_sort_item_type				*list;
					} map_poly_sort_type;

//
// scenery, light, sound and particle structures
//
					
typedef struct		{
						int									lighting_mode;
						char								model_name[name_str_len],
															animation_name[name_str_len];
						bool								contact_object_on,contact_projectile_on,
															contact_hit_box,face_forward,
															shadow,shadow_cast_down,override_size;
						d3pnt								pnt,size;
						d3ang								ang;
					} map_scenery_type;

typedef struct		{
						int									type,intensity;
						bool								on;
						d3pnt								pnt;
						d3col								col;
					} map_light_type;
					
typedef struct		{
						int									buffer_idx;
                        float								pitch;
						char								name[name_str_len];
						bool								on;
						d3pnt								pnt;
					} map_sound_type;
					
typedef struct		{
                        int									spawn_tick,slop_tick,next_spawn_tick,
															particle_idx;
						char								name[name_str_len];
						bool								on;
						d3pnt								pnt;
					} map_particle_type;
                    
//
// script spots
//

typedef struct		{
						int									skill;
						char								name[name_str_len],
															type[name_str_len],
															script[name_str_len],
															display_model[name_str_len],
															params[param_str_len],
															attach_name[name_str_len],
															attach_type[name_str_len],
															attach_script[file_str_len],
															attach_params[param_str_len];
						bool								attach;
						d3pnt								pnt;
						d3ang								ang;
					} spot_type;

//
// nodes
//

typedef struct		{
						bool								goal,red_flag,blue_flag,
															weapon,ammo,armor,health,
															normal_cover,duck_cover,sniper;
					} node_flag_type;

typedef struct		{
						int									idx,user_value,
															link[max_node_link],
															path_hint[max_node];
						char								name[name_str_len];
						d3pnt								pnt;
						d3ang								ang;
						node_flag_type						flag;
					} node_type;

//
// movement structures
//
 
typedef struct		{
						int									msec,user_id,sound_buffer_idx;
						char								sound_name[name_str_len];
						float								sound_pitch;
						d3pnt								mov;
						d3ang								rot;
					} movement_move_type;

typedef struct		{
						int									nmove,group_idx,reverse_group_idx,
															auto_open_distance,cur_move_idx;
						char								name[name_str_len];
						bool								auto_start,auto_open,auto_open_never_close,
															loop,started,opened,reverse;
						d3pnt								pnt;
						movement_move_type					moves[max_movement_move];
					} movement_type;

//
// map media and music
//

typedef struct		{
						int									type;
						char								name[name_str_len],title_sound_name[name_str_len];
					} map_media_type;
					
typedef struct		{
						int									fade_msec;
						char								name[name_str_len];
					} map_music_type;
					
//
// map info and sky structures
//

typedef struct		{
						char								name[name_str_len],
															title[name_str_len],
															author[name_str_len],
															host_name[name_str_len],
															player_start_name[name_str_len],
															player_start_type[name_str_len],
															load_path[1024];
						bool								in_load;
					} map_info_type;
					
typedef struct		{
						float								gravity,gravity_max_power,gravity_max_speed,
															resistance,txt_scale_x,txt_scale_y;
						bool								editor_link_always_start;
						char								network_game_list[256];
					} map_settings_type;
					
typedef struct		{
						float								sound_pitch,light_drop_off_factor;
						char								sound_name[name_str_len];
						d3col								light_color;
					} map_ambient_type;					

typedef struct		{
						int									density,radius,height,speed,
															line_width,line_length,slant_add,
															slant_time_msec,slant_change_msec;
						float								alpha;
						bool								on,reset;
						d3col								start_color,end_color;
					} map_rain_type;					
					
typedef struct		{
						int									fill;
						float								x_scroll_fact,y_scroll_fact;
						bool								on;
					} map_background_type;					
					
typedef struct		{
						int									type,radius,extra_height,dome_y,
															fill,bottom_fill,north_fill,south_fill,east_fill,west_fill;
						float								txt_fact,txt_x_shift,txt_y_shift;
						bool								on;
					} map_sky_type;
					
typedef struct		{
						int									count,inner_radius,outer_radius,
															high,drop,texture_idx;
						float								speed,txt_x_fact,txt_y_fact,
															alpha;
						bool								on,use_solid_color;
						d3col								col;
					} map_fog_type;

//
// main map structure
//

typedef struct		{

						int									nlight,nsound,nparticle,
															nspot,nnode,nscenery,nmovement,
															ngroup,start_game_tick;
													
						map_info_type						info;
						
						map_ambient_type					ambient;
						map_background_type					background;
						map_sky_type						sky;
						map_fog_type						fog;
						map_rain_type						rain;
						
						map_settings_type					settings;
						map_media_type						media;
						map_music_type						music;
						
						texture_type						*textures;
						
						map_scenery_type					*sceneries;
						map_light_type						*lights;
						map_sound_type						*sounds;
						map_particle_type					*particles;
						spot_type							*spots;
						node_type							*nodes;
						
						movement_type						*movements;
						group_type							*groups;
	
						map_mesh_collection_type			mesh;
						map_liquid_collection_type			liquid;

						map_vertex_type						mesh_vertexes,liquid_vertexes;
						map_poly_sort_type					sort;
						
					} map_type;

//
// functions
//

extern void map_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int texture_quality_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression);
extern bool map_new(map_type *map,char *name);
extern bool map_open(map_type *map,char *name,bool load_bitmaps,bool setup_glowmaps,bool load_shaders);
extern bool map_reload(map_type *map);
extern bool map_save(map_type *map);
extern void map_close(map_type *map);
extern void map_refresh_textures(map_type *map);

extern void map_prepare(map_type *map);
extern void map_center(map_type *map);

extern int map_count_texture_frames(map_type *map,int txt);
extern void map_setup_animated_textures(map_type *map,int tick);

extern bool map_check_game_type(char *game_type,char *map_name);

extern bool map_create_vertex_lists(map_type *map);
extern void map_dispose_vertex_lists(map_type *map);

extern int map_count_spot(map_type *map,char *name,char *type);
extern int map_find_spot(map_type *map,char *name,char *type);
extern int map_find_random_spot(map_type *map,char *name,char *type);
extern void map_spot_clear_attach(map_type *map);
extern void map_spot_attach_object(spot_type *spot,char *name,char *type,char *script,char *params);

extern int map_find_node(map_type *map,char *name);
extern int map_find_nearest_node(map_type *map,int x,int y,int z,int user_value,float ang,float ang_sweep,int min_dist,int max_dist);
extern int map_find_next_node_in_path(map_type *map,int from_idx,int to_idx);
extern int map_node_to_node_distance(map_type *map,int from_idx,int to_idx);

extern int map_mesh_add(map_type *map);
extern bool map_mesh_delete(map_type *map,int mesh_idx);
extern bool map_mesh_set_vertex_count(map_type *map,int mesh_idx,int nvertex);
extern bool map_mesh_set_poly_count(map_type *map,int mesh_idx,int npoly);
extern int map_mesh_duplicate(map_type *map,int mesh_idx);
extern int map_mesh_count_total_poly(map_type *map);
extern int map_mesh_add_poly(map_type *map,int mesh_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt_idx);
extern bool map_mesh_delete_poly(map_type *map,int mesh_idx,int poly_idx);
extern bool map_mesh_delete_unused_vertexes(map_type *map,int mesh_idx);
extern void map_mesh_calculate_extent(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_mesh_calculate_center(map_type *map,int mesh_idx,d3pnt *mpt);
extern void map_mesh_calculate_uv_center(map_type *map,int mesh_idx,float *gx,float *gy);

extern int map_find_mesh(map_type *map,d3pnt *pnt);
extern int map_calculate_mesh_distance(map_mesh_type *mesh,d3pnt *pnt);

extern int map_mesh_combine(map_type *map,int mesh_1_idx,int mesh_2_idx);
extern void map_mesh_move(map_type *map,int mesh_idx,int x,int y,int z);
extern void map_mesh_resize(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_mesh_flip(map_type *map,int mesh_idx,bool flip_x,bool flip_y,bool flip_z);
extern void map_mesh_rotate(map_type *map,int mesh_idx,float rot_x,float rot_y,float rot_z);
extern bool map_mesh_tesselate(map_type *map,int mesh_idx);
extern bool map_mesh_poly_punch_hole(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_poly_run_shifts(map_type *map,int tick);
extern void map_mesh_get_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,float *x_txtoff,float *y_txtoff,float *x_txtfact,float *y_txtfact);
extern void map_mesh_set_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,float x_txtoff,float y_txtoff,float x_txtfact,float y_txtfact);
extern void map_mesh_rotate_poly_uv(map_type *map,int mesh_idx,int poly_idx,int rot_ang);
extern void map_mesh_reset_poly_uv(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_reset_uv(map_type *map,int mesh_idx);

extern int map_liquid_add(map_type *map);
extern bool map_liquid_delete(map_type *map,int liquid_idx);
extern int map_liquid_duplicate(map_type *map,int liquid_idx);
extern void map_liquid_calculate_center(map_type *map,int liquid_idx,d3pnt *pt);

extern void map_liquid_move(map_type *map,int liquid_idx,int x,int y,int z);

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
