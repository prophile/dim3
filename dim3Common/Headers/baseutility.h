/****************************** File *********************************

Module: dim3 Math Utility
Author: Brian Barnes
 Usage: External Header

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

/* =======================================================

      Version
      
======================================================= */

#define dim3_version			"2.5b9"

/* =======================================================

      Base Definitions
      
======================================================= */

//
// lengths
//

#define name_str_len					32
#define file_str_len					64

//
// dim3 base types
//

typedef struct		{
						int				x,y,z;
					} d3pnt;

typedef struct		{
						int				lx,rx,ty,by;
					} d3rect;
					
typedef struct		{
						float			x,y,z;
					} d3fpnt;
					
typedef struct		{
						float			x,y,z;
					} d3vct;
					
typedef struct		{
						float			x,y,z;
					} d3ang;
					
typedef struct		{
						int				x,z,y,
										min_x,max_x,min_z,max_z,min_y,max_y;
					} d3box;
					
typedef struct		{
						float			r,g,b;
					} d3col;

/* =======================================================

     Shared Script Structures
      
======================================================= */

#define d3_jsval_type_int							0
#define d3_jsval_type_float							1
#define d3_jsval_type_boolean						2
#define d3_jsval_type_string						3

#define max_d3_jsval_str_len						128


#define max_msg_data								8

typedef union		{
						int							d3_int;
						float						d3_float;
						bool						d3_boolean;
						char						d3_string[max_d3_jsval_str_len];
					} d3_jsval_data_type;
 

typedef struct		{
						int							type;
						d3_jsval_data_type			data;
					} attach_msg_type;

typedef struct		{
						int							thing_type,thing_uid,
													script_uid;
						attach_msg_type				set_msg_data[max_msg_data],get_msg_data[max_msg_data];
					} attach_type;
 
/* =======================================================

      Map/Model Utility Shared Structures
      
======================================================= */

typedef struct		{
						double						d_x,d_y,d_z,
													intensity,inv_intensity,
													d_intensity,d_inv_intensity,
													d_col_r,d_col_g,d_col_b,
													calc_light_mult;
						d3pnt						pnt;
						d3col						col;
					} light_spot_type;

/* =======================================================

      Math
      
======================================================= */

//
// Defines
//

#define TRIG_PI					3.14159265358979f
#define ANG_to_RAD				(float)(TRIG_PI/180.0f)
#define RAD_to_ANG				(float)(180.0f/TRIG_PI)

//
// Matrix Structure
//

typedef struct		{
                        float			data[4][4];
                    } matrix_type;

//
// Functions
//

extern void time_start(void);
extern int time_get(void);

extern void random_reset(void);
extern int random_int(int max);
extern float random_float(float max);
extern bool random_boolean(void);
extern int random_int_spread(int spread);

extern void matrix_identity(matrix_type *mat);
extern void matrix_multiply(matrix_type *mat_1,matrix_type *mat_2);
extern void matrix_vertex_multiply(matrix_type *mat,float *x,float *y,float *z);
extern void matrix_vertex_multiply_ignore_transform(matrix_type *mat,float *x,float *y,float *z);
extern void matrix_angle_multiply(matrix_type *mat,float *x,float *y,float *z);
extern void matrix_rotate_x(matrix_type *mat,float ang);
extern void matrix_rotate_y(matrix_type *mat,float ang);
extern void matrix_rotate_z(matrix_type *mat,float ang);
extern void matrix_rotate_xyz(matrix_type *mat,float x_ang,float y_ang,float z_ang);
extern void matrix_rotate_zyx(matrix_type *mat,float x_ang,float y_ang,float z_ang);
extern void matrix_rotate_xzy(matrix_type *mat,float x_ang,float y_ang,float z_ang);
extern bool matrix_has_rotation(matrix_type *mat);
extern void matrix_translate(matrix_type *mat,float x,float y,float z);
extern void matrix_average(matrix_type *mat,int nmatrix,matrix_type *mats);

extern void vector_normalize(d3vct *v);
extern void vector_create(d3vct *v,int x1,int y1,int z1,int x2,int y2,int z2);
extern void vector_cross_product(d3vct *v,d3vct *v1,d3vct *v2);
extern float vector_dot_product(d3vct *v1,d3vct *v2);

extern void fast_trig_setup(void);
extern float fast_atan2f(float y,float x);
extern float fast_sinf(float x);
extern float fast_cosf(float x);

extern float angle_add(float ang1,float ang2);
extern float angle_dif(float ang1,float ang2,bool *cwise);
extern void angle_get_movement(float ang,int mv,int *xadd,int *zadd);
extern void angle_get_movement_float(float ang,float fmv,float *xadd,float *zadd);
extern void angle_add_movement(float ang,int mv,int *x,int *z);
extern float angle_find(int x,int z,int tox,int toz);
extern float angle_turn_toward(float ang,float rang,float mv);
extern float angle_to_normal(int x,int z,int tox,int toz);

extern void rotate_point(int *x,int *y,int *z,int cx,int cy,int cz,float ang_x,float ang_y,float ang_z);
extern void rotate_polygon(int ptsz,int *x,int *y,int *z,int cx,int cy,int cz,float ang_x,float ang_y,float ang_z);
extern void rotate_point_center(int *x,int *y,int *z,float ang_x,float ang_y,float ang_z);
extern void rotate_polygon_center(int ptsz,int *x,int *y,int *z,float ang_x,float ang_y,float ang_z);
extern void rotate_2D_point(int *x,int *z,int cx,int cz,float ang_y);
extern void rotate_2D_point_center(int *x,int *z,float ang_y);
extern void rotate_2D_line(int *lx,int *lz,int *rx,int *rz,int cx,int cz,float ang_y);
extern void rotate_2D_polygon(int ptsz,int *x,int *z,int cx,int cz,float ang_y);

extern void line_get_movement(int x1,int y1,int z1,int x2,int y2,int z2,int mv,int *xadd,int *yadd,int *zadd);
extern bool line_2D_get_intersect(int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3,int *x,int *y);
extern bool line_2D_test_intersect(int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3);
extern void line_2D_find_angle_hit(int x,int z,float ang,int lx,int lz,int rx,int rz,int *kx,int *kz);
extern void line_2D_find_inside_infinite(int x,int z,int wid,int lx,int lz,int rx,int rz,int *klx,int *klz,int *krx,int *krz);
extern bool line_2D_all_points_in_line(int ptsz,int *px,int *pz,float slop);

extern void polygon_get_box(int ptsz,int *px,int *py,int *pz,int *p_min_x,int *p_max_x,int *p_min_y,int *p_max_y,int *p_min_z,int *p_max_z);
extern void polygon_find_center(int ptsz,int *px,int *py,int *pz,int *mx,int *my,int *mz);
extern void polgyon_enlarge(int ptsz,int *px,int *py,int *pz,int *ex,int *ey,int *ez,int factor);
extern void polygon_2D_get_box(int ptsz,int *px,int *py,int *p_lft,int *p_rgt,int *p_top,int *p_bot);
extern bool polygon_2D_point_inside(int ptsz,int *px,int *py,int x,int y);
extern bool polygon_2D_polygon_points_inside(int ptsz,int *px,int *py,int ptsz_2,int *px_2,int *py_2);
extern bool polygon_2D_polygon_lines_intersect(int ptsz,int *px,int *py,int ptsz_2,int *px_2,int *py_2);
extern void polygon_2D_collision_setup(int ptsz,int *px,int *py);
extern bool polygon_2D_collision_line(int lx,int lz,int rx,int rz);
extern bool polygon_2D_collision_bound_box(int lft,int rgt,int top,int bot);
extern bool polygon_2D_collision_bound_polygon(int ptsz,int *x,int *y,int lft,int rgt,int top,int bot);
extern bool polygon_2D_collision_polygon(int ptsz,int *x,int *y);
extern float polygon_get_slope_y(int ptsz,int *px,int *py,int *pz,float *ang_y);
extern int polygon_find_y(int ptsz,int *px,int *py,int *pz,int kx,int kz);
extern int polygon_infinite_find_y(int ptsz,int *px,int *py,int *pz,int kx,int kz);

extern int area_2D_trig(int *px,int *py);
extern int area_2D_polygon(int ptsz,int *px,int *py);

extern int distance_get(int x,int y,int z,int tox,int toy,int toz);
extern bool distance_check(int x,int y,int z,int tox,int toy,int toz,int max_distance);
extern int distance_2D_get(int x,int z,int tox,int toz);

extern void clip_line_to_2D_horizontal_line(int clip_z,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern void clip_line_to_2D_vertical_line(int clip_x,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern void clip_extend_line_to_2D_horizontal_line(int clip_z,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern void clip_extend_line_to_2D_vertical_line(int clip_x,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern int clip_polygon_to_2D_rect(int ptsz,int *x,int *y,int *z,int lx,int rx,int tz,int bz);
extern int clip_extend_polygon_to_2D_rect(int ptsz,int *x,int *y,int *z,int lx,int rx,int tz,int bz);

extern float acceleration_calculate(float factor,float acceleration);

extern void string_convert_float(char *str,float f);

/* =======================================================

      Files and Paths
      
======================================================= */

//
// defines
//

#define file_paths_max_directory_file		1024

#define file_paths_dir_type_count			4

#define file_paths_dir_type_app				0
#define file_paths_dir_type_data			1
#define file_paths_dir_type_data2			2
#define file_paths_dir_type_documents		3

#define file_paths_dir_names				{"App","Data","Data2","Documents"}

//
// file path structure
//

typedef struct	{
					char			app_name[256],
									org_app_name[256],
									path_base[1024],
									path_app[1024],
									path_data[1024],
									path_data_2[1024];
				} file_path_setup_type;
				
//
// file directory structures
//

typedef struct	{
					int								dir_type;
					char							file_name[file_str_len];
				} file_path_directory_file_type;

typedef struct	{
					int								nfile;
					file_path_directory_file_type	files[file_paths_max_directory_file];
				} file_path_directory_type;

//
// file path routines
//

extern bool file_paths_setup(file_path_setup_type *file_path_setup,bool app_name_override);

extern void file_paths_data(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern void file_paths_data_default(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern void file_paths_app(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern void file_paths_base(file_path_setup_type *file_path_setup,char *path,char *file_name,char *ext_name);

extern void file_paths_documents(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern bool file_paths_documents_exist(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);

extern file_path_directory_type* file_paths_read_directory(file_path_setup_type *file_path_setup,char *sub_path,char *ext_name,bool document_dir_only);
extern void file_paths_close_directory(file_path_directory_type *fpd);
extern void file_paths_descript_directory_file(char *descript,int dir_type);

/* =======================================================

      XML
      
======================================================= */

extern bool xml_new_file(void);
extern bool xml_open_file(char *path);
extern bool xml_save_file(char *path);
extern void xml_close_file(void);
extern char* xml_get_stream(int *sz);

extern int xml_findfirstchild(char *name,int parent);
extern int xml_findrootchild(char *name);
extern int xml_findnextchild(int curtag);
extern int xml_getparent(int n);
extern int xml_countchildren(int n);
extern int xml_findflat(char *name,int offset);

extern bool xml_get_attribute_raw(int n,char *name,char *value,int valuesz);
extern bool xml_get_attribute_text(int n,char *name,char *value,int valuesz);
extern bool xml_get_attribute_text_default_blank(int n,char *name,char *value,int valuesz);
extern int xml_get_attribute_int(int n,char *name);
extern int xml_get_attribute_int_default(int n,char *name,int def_int);
extern int xml_get_attribute_int_array(int n,char *name,int *value,int count);
extern int xml_get_attribute_short_array(int n,char *name,short *value,int count);
extern float xml_get_attribute_float(int n,char *name);
extern float xml_get_attribute_float_default(int n,char *name,float def_float);
extern int xml_get_attribute_float_array(int n,char *name,float *value,int count);
extern bool xml_get_attribute_boolean(int n,char *name);
extern int xml_get_attribute_list(int n,char *name,char *list);
extern bool xml_get_attribute_2_coord_int(int n,char *name,int *x,int *y);
extern bool xml_get_attribute_2_coord_float(int n,char *name,float *x,float *y);
extern bool xml_get_attribute_2_coord_float_default(int n,char *name,float *x,float *y,float def_x,float def_y);
extern bool xml_get_attribute_3_coord_int(int n,char *name,int *x,int *y,int *z);
extern bool xml_get_attribute_3_coord_float(int n,char *name,float *x,float *y,float *z);
extern bool xml_get_attribute_4_coord_float(int n,char *name,float *r,float *g,float *b,float *a);
extern bool xml_get_attribute_color(int n,char *name,d3col *col);
extern bool xml_get_attribute_bit_array(int n,char *name,unsigned char *value,int count);

extern bool xml_add_tagstart(char *name);
extern bool xml_add_tagend(bool single);
extern bool xml_add_tagclose(char *name);

extern bool xml_add_attribute_text(char *name,char *value);
extern bool xml_add_attribute_int(char *name,int value);
extern bool xml_add_attribute_int_array(char *name,int *value,int count,bool removenegone);
extern bool xml_add_attribute_short_array(char *name,short *value,int count,bool removenegone);
extern bool xml_add_attribute_float(char *name,float value);
extern bool xml_add_attribute_float_array(char *name,float *value,int count);
extern bool xml_add_attribute_boolean(char *name,bool value);
extern bool xml_add_attribute_list(char *name,char *list,int value);
extern bool xml_add_attribute_2_coord_int(char *name,int x,int y);
extern bool xml_add_attribute_2_coord_float(char *name,float x,float y);
extern bool xml_add_attribute_3_coord_int(char *name,int x,int y,int z);
extern bool xml_add_attribute_3_coord_float(char *name,float x,float y,float z);
extern bool xml_add_attribute_4_coord_float(char *name,float r,float g,float b,float a);
extern bool xml_add_attribute_color(char *name,d3col *col);
extern bool xml_add_attribute_bit_array(char *name,unsigned char *value,int count);

/* =======================================================

      Bitmap
      
======================================================= */

//
// alpha modes
//

#define alpha_mode_none							0
#define alpha_mode_cut_out						1
#define alpha_mode_transparent					2

//
// anisotropic modes
//

#define anisotropic_mode_none					0
#define anisotropic_mode_low					1
#define anisotropic_mode_medium					2
#define anisotropic_mode_high					3

#define anisotropic_mode_setup_list_def			{"None","Low","Medium","High",""}

//
// mipmap modes
//

#define mipmap_mode_none						0
#define mipmap_mode_bilinear					1
#define mipmap_mode_trilinear					2

#define mipmap_mode_setup_list_def				{"None","Bilinear","Trilinear",""}

//
// bump modes
//

#define bump_mode_none							0
#define bump_mode_auto_generate					1
#define bump_mode_height_map					2
#define bump_mode_normal_map					3

#define bump_mode_xml_list_str					{"none","auto_generate","height_map","normal_map",""}

//
// texture maximums
//

#define max_texture_frame						64				// maximum number of frames in a texture
#define max_texture_frame_mask					0x3F			// mask for maximum number of textures frames

//
// shaders
//

#define max_shader_custom_vars					8

#define shader_var_type_int						0
#define shader_var_type_float					1
#define shader_var_type_vec3					2
#define shader_var_type_vec4					3

#define shader_var_type_xml_list_str			{"int","float","vec3","vec4",""}

//
// bitmap structures
//

typedef struct		{
						int						wid,high,alpha_mode;
						char					name[file_str_len];
						GLuint					gl_id;
					} bitmap_type;

//
// shader structure
//

typedef union		{
						int						i;
						float					f;
						float					vec3[3];
						float					vec4[4];
					} shader_value_type;
					
typedef struct		{
						int						var_type;
						char					name[name_str_len];
						shader_value_type		value;
					} shader_custom_var_type;
					
typedef struct		{
						char					vertex_name[file_str_len],
												fragment_name[file_str_len],
												init_error[1024];
						bool					on,global_vars_set;
						shader_custom_var_type	custom_vars[max_shader_custom_vars];
						GLhandleARB				vertex_obj,fragment_obj,program_obj;
					} shader_type;

//
// glow structure
//

typedef struct		{
						int						rate;
						float					min,max,current_color;
					} glow_type;

//
// scaling structure (only used for Editor)
//

typedef struct		{
						float					x,y;
						bool					on,lock_offset;
					} texture_scale_type;
					
//
// animation structures
//

typedef struct		{
						int						current_frame,total_wait,
												wait[max_texture_frame],
												end_tick[max_texture_frame];
						bool					on;
					} texture_animate_type;
					
//
// texture structure
//

typedef struct		{
						int						bump_mode;
						float					txt_scale_x,txt_scale_y;
						bool					additive,pixelated;
						char					material_name[name_str_len];
						d3col					col;
						bitmap_type				bitmaps[max_texture_frame],
												bumpmaps[max_texture_frame],
												specularmaps[max_texture_frame],
												glowmaps[max_texture_frame];
						texture_animate_type	animate;
						shader_type				shader;
						glow_type				glow;
						texture_scale_type		scale;
					} texture_type;

//
// functions
//

extern void bitmap_new(bitmap_type *bitmap);
extern bool bitmap_open(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,bool use_compression,bool pixelated,bool scrub_black_to_alpha);
extern bool bitmap_color(bitmap_type *bitmap,char *name,d3col *col);
extern bool bitmap_data(bitmap_type *bitmap,char *name,unsigned char *data,int wid,int high,int anisotropic_mode,int mipmap_mode,bool use_compression);
extern void bitmap_close(bitmap_type *bitmap);

extern void bitmap_texture_setup_animation(texture_type *texture,int texture_count,unsigned char *user_frames,int tick);
extern void bitmap_texture_recalc_animation(texture_type *texture);

extern void bitmap_texture_read_xml(texture_type *texture,int main_tag,bool read_scale);
extern void bitmap_texture_write_xml(texture_type *texture,int frame_count,bool write_scale);

extern bool bitmap_open_normal_from_height(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,bool use_compression,bool pixelated);
extern bool bitmap_open_normal_from_bitmap(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,bool use_compression,bool pixelated);

extern unsigned char* bitmap_load_png_data(char *path,int *p_wid,int *p_high);
extern bool bitmap_write_png_data(unsigned char *data,int wid,int high,char *path);
extern bool bitmap_check(char *path,char *err_str);
extern bool bitmap_copy(char *srce_path,char *dest_path);

extern bool bitmap_movie_start(char *path,int *wid,int *high);
extern void bitmap_movie_end(void);
extern int bitmap_movie_get_texture_id(void);
extern bool bitmap_movie_playing(void);
extern void bitmap_movie_texture_refresh(void);

extern bool shader_map_add(file_path_setup_type *file_path_setup,shader_type *shader);
extern bool shader_model_add(file_path_setup_type *file_path_setup,char *model_name,shader_type *shader);
extern void shader_delete(shader_type *shader);

/* =======================================================

      Network
      
======================================================= */

//
// additional network->host conversions
//

#define htonf(x)									(htonl((int)((x)*1000.0f)))
#define ntohf(x)									(((float)ntohl(x))/1000.0f)

//
// networking ports
//

#define net_port_host								11800
#define net_port_host_query							11801
#define net_port_host_broadcast						11802
#define net_port_host_broadcast_reply				11803

//
// low-level socket timing
//

#define socket_no_data_retry						10
#define socket_no_data_u_wait						25

//
// message sizes
//

#define net_max_msg_size							4096

#define net_max_client_msg_cache					128
#define net_max_client_msg_cache_size				((sizeof(short)*3)+net_max_msg_size)

//
// queue modes
//

#define net_queue_mode_normal						0
#define net_queue_mode_replace						1			// only one in the queue at once, new ones replace old ones

//
// common message header
//

typedef struct		{
						short						len,action,queue_mode,from_remote_uid;
					} network_header;

//
// queue structures
//

typedef struct		{
						int							idx;
						unsigned char*				ptr;
						pthread_mutex_t				lock;
					} network_message_queue;

//
// network
//

extern void network_initialize(void);
extern void network_shutdown(void);

extern void network_get_host_name(char *name);
extern void network_get_host_ip(char *ip_name,char *ip_resolve);

extern bool network_ip_local(char *ip);
extern void network_ip_local_broadcast(char *ip,char *broad_ip);

//
// sockets
//

extern d3socket network_open_socket(void);
extern d3socket network_open_socket_udp(void);
extern void network_close_socket(d3socket *sock);
extern void network_socket_blocking(d3socket sock,bool blocking);
extern bool network_connect_start(d3socket sock,char *ip,int port,char *err_str);
extern bool network_connect_check(d3socket sock);
extern void network_connect_end(d3socket sock);
extern bool network_connect_block(d3socket sock,char *ip,int port,int secs,char *err_str);
extern bool network_bind(d3socket sock,char *ip,int port,char *err_str);
extern bool network_receive_ready(d3socket sock);
extern bool network_send_ready(d3socket sock);
extern int network_receive_data(d3socket sock,unsigned char *data,int len);
extern int network_send_data(d3socket sock,unsigned char *data,int len);
extern bool network_receive_packet(d3socket sock,int *action,int *queue_mode,int *from_remote_uid,unsigned char *data,int *len);
extern bool network_send_packet(d3socket sock,int action,int queue_mode,int from_remote_uid,unsigned char *data,int len);

//
// udp sockets
//

extern d3socket network_udp_open_socket(void);
extern bool network_udp_bind_broadcast(d3socket sock,int port,char *err_str);
extern unsigned long network_udp_receive_broadcast(int sock);
extern bool network_udp_send_broadcast(char *ip,int port);

//
// queues
//

extern bool network_queue_initialize(network_message_queue *queue);
extern void network_queue_shutdown(network_message_queue *queue);
extern bool network_queue_add_message(network_message_queue *queue,int action,int queue_mode,int remote_uid,unsigned char *data,int len);
extern bool network_queue_check_message(network_message_queue *queue,int *action,int *from_remote_uid,unsigned char *data);
