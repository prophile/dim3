/****************************** File *********************************

Module: dim3 Model Utility
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
// Model Definitions
//

#define max_model								64			// maximum models in a map

#define max_model_mesh							32			// maximum number of meshes

#define max_model_vertex						8000		// maximum number of vertexs
#define max_model_bone							128			// maximum number of bones
#define max_model_trig							8000		// maximum number of triangles in a model

#define max_model_texture						32			// maximum number of textures in model

#define max_model_pose							256			// maximum number of poses
#define max_model_animate						128			// maximum number of animations
#define max_model_animate_pose					32			// maximum number of poses per animation

#define max_model_blend_animation				4			// maximum number of blended animations
#define max_model_light							8			// maximum number of lights in a model
#define max_model_halo							8			// maximum number of halos in a model

#define max_model_animate_particle				8			// maximum number of particles for animation
#define max_model_animate_ring					8			// maximum number of rings for animation

#define max_model_hit_box						16			// maximum number of hit boxes

//
// Animator vertex select and hide
//

#define vertex_sel_hide_mask_sz					(((max_model_vertex*max_model_mesh)/8)+1)

//
// Deform Modes
//

#define deform_mode_single_rotate				0
#define deform_mode_comulative_rotate			1

#define deform_mode_xml_list_str				{"single_rotate","comulative_rotate",""}

//
// Model Tags
//

#define model_tag								unsigned long
#define model_null_tag							0x3F3F3F3F

//
// Drawing Stuctures
//

typedef struct		{
						int								parent_idx;
						bool							touch;
						d3vct							fpnt;
						d3vct							parent_dist;
						d3vct							rot;
						matrix_type						rot_mat;
					} model_draw_bone_type;

typedef struct		{
                        int								idx_1,idx_2;
                        float							factor,acceleration;
					} model_draw_pose_type;

typedef struct		{
						d3pnt							center;
						d3vct							move;
						d3ang							ang;
						d3ang							sway;
						model_draw_pose_type			poses[max_model_blend_animation];
						model_draw_bone_type			bones[max_model_bone];
                    } model_draw_setup;

//
// Model Info Structures
//		

typedef struct		{
						d3pnt							size;
						d3pnt							offset;
					} model_box_type;
					
typedef struct		{
						char							name[64];
						model_box_type					box;
					} model_hit_box_type;
                    
//
// Model Mesh Structures
//

typedef struct		{
						int								major_bone_idx,minor_bone_idx;
						float							bone_factor;
						d3pnt							pnt;
						d3vct							normal;
						d3vct							major_dist,minor_dist;
					} model_vertex_type;
					
typedef struct		{
						short							v[3];
						float							gx[3],gy[3];
					} model_trig_type;
					
typedef struct		{
						int								trig_start,trig_count;
					} model_material_type;

typedef struct		{
						float							*gl_vertex_array,*gl_color_array,
														*gl_vertex_normal_array,*gl_light_normal_array;
					} model_mesh_draw_type;

typedef struct		{
						int								nvertex,ntrig;
						char							name[name_str_len];
						bool							no_lighting,blend_add;
						unsigned char					*vertex_sel_mask,*vertex_hide_mask;
 						model_vertex_type				*vertexes;
                        model_trig_type					*trigs;
						model_material_type				*materials;
						model_mesh_draw_type			draw;
					} model_mesh_type;

//
// Model Bone Structures
//

typedef struct		{
						int								parent_idx;
						char							name[name_str_len];
						d3pnt							pnt;
						d3vct							parent_dist;
						model_tag						tag;
					} model_bone_type;

//
// Model Poses and Animation Structures
//

typedef struct		{
						int								bone_idx;
						d3pnt							offset;
					} model_bone_move_constraint_type;

typedef struct		{
						float							acceleration;
						bool							skip_blended;
						d3vct							rot;
						d3vct							mov;
						model_bone_move_constraint_type	constraint;
					} model_bone_move_type;
					
typedef struct		{
						char							name[64];
						model_bone_move_type			bone_moves[max_model_bone];
					} model_pose_type;

typedef struct		{
						int								bone_idx,buffer_idx;
						float							pitch;
						char							name[name_str_len];
					} model_pose_move_sound_type;
					
typedef struct		{
						int								particle_idx,bone_idx;
						float							motion_factor;
						char							name[name_str_len];
						bool							rotate,motion,stick;
						d3pnt							slop;
					} model_particle_type;

typedef struct		{
						int								count;
						model_particle_type				particles[max_model_animate_particle];
					} model_pose_move_particle_type;

typedef struct		{
						int								ring_idx,bone_idx;
						char							name[name_str_len];
						bool							angle;
						d3pnt							slop;
					} model_ring_type;

typedef struct		{
						int								count;
						model_ring_type					rings[max_model_animate_ring];
					} model_pose_move_ring_type;

typedef struct		{
						int								mesh_idx,fade_in_msec,fade_life_msec,fade_out_msec;
						char							name[name_str_len];
					} model_pose_move_mesh_fade_type;
					
typedef struct		{
						int								bone_idx,intensity,flash_msec,fade_msec;
						d3col							col;
					} model_pose_move_flash_type;
					
typedef struct		{
						int								distance,size,life_msec;
					} model_pose_move_shake_type;
					
typedef struct		{
						int								pose_idx,msec;
						float							acceleration;
						d3vct							mov;
						d3vct							sway;
						model_pose_move_sound_type		sound;
						model_pose_move_mesh_fade_type	mesh_fade;
						model_pose_move_flash_type		flash;
						model_pose_move_shake_type		shake;
						model_pose_move_particle_type	particle;
						model_pose_move_ring_type		ring;
 					} model_pose_move_type;
					
typedef struct		{
						int								npose_move,loop_start,loop_end;
						char							name[64];
						bool							loop;
						model_pose_move_type			pose_moves[max_model_animate_pose];
					} model_animate_type;

//
// Model Special Bones
//

typedef struct		{
						int								light_bone_idx[max_model_light],
														halo_bone_idx[max_model_halo],
														name_bone_idx;
						model_tag						light_bone_tag[max_model_light],
														halo_bone_tag[max_model_halo],
														name_bone_tag;
					} model_tags;
					
//
// Model Main Structures
//

typedef struct		{
						int								uid,reference_count,
														nmesh,nbone,npose,nanimate,nhit_box,
														deform_mode,shadow_fudge;
						char							name[name_str_len],load_base_path[1024];
						d3pnt							center;
						model_box_type					view_box,shadow_box;
						model_tags						tags;
 						model_mesh_type					meshes[max_model_mesh];
						model_bone_type					*bones;
						model_pose_type					*poses;
						model_animate_type				*animates;
						model_hit_box_type				*hit_boxes;
						texture_type					*textures;
					} model_type;

//
// Model Functions
//

extern model_tag text_to_model_tag(char *str);
extern void model_tag_to_text(model_tag tag,char *str);

extern int model_mesh_add(model_type *model);
extern int model_mesh_duplicate(model_type *model,int mesh_idx);
extern void model_mesh_copy(model_type *model,int copy_mesh_idx,int mesh_idx);
extern bool model_mesh_delete(model_type *model,int mesh_idx);

extern int model_bone_add(model_type *model,int mesh_idx);
extern void model_bone_delete(model_type *model,int bone_idx);
extern void model_set_sel_vertex_to_bone(model_type *model,int mesh_idx,int major_bone_idx,int minor_bone_idx,float factor);
extern bool model_check_bone_duplicate_tag(model_type *model,model_bone_type *bone);
extern bool model_check_bone_circular(model_type *model,model_bone_type *bone);
extern void model_bone_move(model_type *model,int bone_idx,int x,int y,int z,bool nudge_children,bool nudge_vertexes);

extern int model_pose_add(model_type *model);
extern int model_pose_duplicate(model_type *model,int pose_idx);
extern void model_pose_delete(model_type *model,int pose_idx);
extern void model_pose_clear(model_type *model,int pose_idx);
extern bool model_check_pose_in_animation(model_type *model,int pose_idx);
extern void model_pose_blend_set_all(model_type *model,int pose_idx,bool skip_blended);

extern int model_animate_add(model_type *model);
extern int model_animate_duplicate(model_type *model,int animate_idx);
extern void model_animate_delete(model_type *model,int animate_idx);
extern int model_animate_pose_insert(model_type *model,int animate_idx,int animate_pose_idx,int pose_idx);
extern void model_animate_pose_delete(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_set_loop_start(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_set_loop_end(model_type *model,int animate_idx,int animate_pose_idx);
extern int model_animate_add_particle(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_delete_particle(model_type *model,int animate_idx,int animate_pose_idx,int idx);
extern int model_animate_add_ring(model_type *model,int animate_idx,int animate_pose_idx);
extern void model_animate_delete_ring(model_type *model,int animate_idx,int animate_pose_idx,int idx);

extern int model_hit_box_add(model_type *model);
extern void model_hit_box_delete(model_type *model,int hit_box_idx);

extern int model_find_mesh(model_type *model,char *mesh_name);
extern int model_find_bone(model_type *model,model_tag tag);
extern int model_find_pose(model_type *model,char *pose_name);
extern int model_find_animate(model_type *model,char *animate_name);

extern int model_count_texture_frames(model_type *model,int txt);
extern void model_setup_animated_textures(model_type *model,unsigned char *user_frames,int tick);
extern bool model_add_texture_frame(model_type *model,int txt,char *bitmap_name,char *bumpmap_name,char *specularmap_name,char *glowmap_name);
extern bool model_delete_texture_frame(model_type *model,int txt);

extern void model_clear_draw_setup(model_type *model,model_draw_setup *draw_setup);
extern void model_calculate_parents(model_type *model);

extern void model_create_draw_bones(model_type *model,model_draw_setup *draw_setup);
extern void model_create_draw_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup);
extern void model_create_draw_2D_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup,int x_offset,int y_offset,float x_resize,float y_resize);

extern void model_resize_draw_vertex(model_type *model,int mesh_idx,float resize);
extern void model_translate_draw_vertex(model_type *model,int mesh_idx,int x,int y,int z);

extern void model_create_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup);

extern void model_get_point_position(model_draw_setup *draw_setup,int *x,int *y,int *z);
extern void model_get_draw_bone_position(model_draw_setup *draw_setup,int bone_idx,int *x,int *y,int *z);
extern void model_calc_draw_bone_position(model_type *model,model_draw_setup *draw_setup,int pose_idx,int bone_idx,int *x,int *y,int *z);
extern bool model_get_light_position(model_type *model,model_draw_setup *draw_setup,int idx,int *x,int *y,int *z);
extern bool model_get_halo_position(model_type *model,model_draw_setup *draw_setup,int idx,int *x,int *y,int *z);
extern bool model_get_name_position(model_type *model,model_draw_setup *draw_setup,int *x,int *y,int *z);

extern void model_get_size(model_type *model,int *x,int *y,int *z);
extern void model_get_vertex_extent(model_type *model,int mesh_idx,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy);
extern void model_get_vertex_extent_all(model_type *model,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy);
extern void model_get_view_complex_bounding_box(model_type *model,d3pnt *pnt,d3ang *ang,int *px,int *py,int *pz);
extern void model_recalc_boxes(model_type *model);

extern void model_recalc_normals(model_type *model,int mesh_idx);

extern void model_rescale_box(model_box_type *mbox,float x_fact,float z_fact,float y_fact);
extern void model_scale(model_type *model,int mesh_idx,float x_fact,float y_fact,float z_fact);
extern void model_scale_all(model_type *model,float x_fact,float y_fact,float z_fact);
extern void model_flip(model_type *model,int mesh_idx,bool flip_x,bool flip_y,bool flip_z);
extern void model_flip_all(model_type *model,bool flip_x,bool flip_y,bool flip_z);
extern void model_swap_xz(model_type *model,int mesh_idx);
extern void model_swap_xz_all(model_type *model);
extern void model_swap_yz(model_type *model,int mesh_idx);
extern void model_swap_yz_all(model_type *model);
extern void model_center_xz(model_type *model,int mesh_idx);
extern void model_center_xz_all(model_type *model);
extern void model_floor(model_type *model,int mesh_idx);
extern void model_floor_all(model_type *model);
extern void model_flip_uv(model_type *model,int mesh_idx,bool flip_u,bool flip_v);

extern void model_move_sel_vertexes(model_type *model,int mesh_idx,int x,int y,int z);
extern void model_scale_sel_vertexes(model_type *model,int mesh_idx,float x,float y,float z);
extern void model_rotate_sel_vertexes(model_type *model,int mesh_idx,float ang_x,float ang_y,float ang_z);
extern void model_invert_normal_sel_vertexes(model_type *model,int mesh_idx);
extern void model_delete_sel_vertex(model_type *model,int mesh_idx);

extern void model_clear_sel_mask(model_type *model,int mesh_idx);
extern void model_set_sel_mask(model_type *model,int mesh_idx,int vertex_idx,bool value);
extern void model_set_sel_mask_all(model_type *model,int mesh_idx);
extern bool model_check_sel_mask(model_type *model,int mesh_idx,int vertex_idx);
extern void model_clear_hide_mask(model_type *model,int mesh_idx);
extern void model_set_hide_mask(model_type *model,int mesh_idx,int vertex_idx,bool value);
extern bool model_check_hide_mask(model_type *model,int mesh_idx,int vertex_idx);
extern void model_hide_mask_set_sel_vertexes(model_type *model,int mesh_idx);
extern void model_hide_mask_set_non_sel_vertexes(model_type *model,int mesh_idx);
extern void model_hide_mask_show_all_vertexes(model_type *model,int mesh_idx);
extern bool model_check_hide_mask_trig(model_type *model,int mesh_idx,model_trig_type *trig);
extern void model_set_sel_mask_bone(model_type *model,int mesh_idx,int bone_idx);
extern void model_set_sel_mask_no_bone(model_type *model,int mesh_idx);
extern void model_set_sel_mask_near_bone(model_type *model,int mesh_idx,int bone_idx,float percentage);
extern void model_set_sel_mask_material(model_type *model,int mesh_idx,int material_idx);

extern void model_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int texture_quality_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression);
extern bool model_new(model_type *model,char *name);
extern bool model_open(model_type *model,char *name,bool load_bitmaps,bool setup_glowmaps,bool load_shaders);
extern bool model_save(model_type *model,bool save_bitmaps);
extern void model_close(model_type *model);

extern void model_refresh_textures(model_type *model);
