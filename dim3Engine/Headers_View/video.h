/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Video Functions Header

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
	
	// init and shutdown
	
extern bool gl_in_window_mode(void);
extern bool gl_initialize(int screen_wid,int screen_high,bool lock_fps_refresh,int fsaa_mode,bool reset,char *err_str);
extern void gl_shutdown(void);

	// drawables
	
extern bool gl_is_size_widescreen(int wid,int high);
extern bool gl_is_screen_widescreen(void);
extern bool gl_screen_shot(int lft_x,int top_y,int wid,int high,bool thumbnail,char *path);

	// settings
	
extern bool gl_check_frame_buffer_ok(void);
extern bool gl_check_fsaa_ok(void);
extern bool gl_check_texture_compress_ok(void);
extern bool gl_check_texture_anisotropic_filter_ok(void);
extern bool gl_check_texture_generate_mipmaps_ok(void);
extern bool gl_check_point_sprite_ok(void);
extern bool gl_check_shader_ok(void);

	// lights

extern void gl_lights_calc_vertex(double x,double y,double z,float *cf);
extern view_light_spot_type* gl_light_find_closest_light(double x,double y,double z,int *p_dist);
extern void gl_lights_compile(int tick);
extern void gl_lights_build_from_poly(map_mesh_poly_type *poly,view_glsl_light_list_type *light_list);
extern void gl_lights_build_from_liquid(map_liquid_type *liq,view_glsl_light_list_type *light_list);
extern void gl_lights_build_from_model(model_draw *draw,view_glsl_light_list_type *light_list);

	// vbos

extern void view_create_vertex_objects(void);
extern void view_dispose_vertex_objects(void);

extern void view_init_map_vertex_object(int sz);
extern float* view_bind_map_map_vertex_object(void);
extern void view_bind_map_vertex_object(void);
extern void view_unmap_map_vertex_object(void);
extern void view_unbind_map_vertex_object(void);
extern void view_init_map_index_object(int sz);
extern unsigned int* view_bind_map_map_index_object(void);
extern void view_bind_map_index_object(void);
extern void view_unmap_map_index_object(void);
extern void view_unbind_map_index_object(void);

extern void view_init_liquid_vertex_object(int sz);
extern float* view_bind_map_liquid_vertex_object(void);
extern void view_unmap_liquid_vertex_object(void);
extern void view_unbind_liquid_vertex_object(void);
extern void view_init_liquid_index_object(int sz);
extern unsigned int* view_bind_map_liquid_index_object(void);
extern void view_unmap_liquid_index_object(void);
extern void view_unbind_liquid_index_object(void);

extern void view_init_sky_vertex_object(int sz);
extern float* view_bind_map_sky_vertex_object(void);
extern void view_bind_sky_vertex_object(void);
extern void view_unmap_sky_vertex_object(void);
extern void view_unbind_sky_vertex_object(void);

extern float* view_bind_map_next_vertex_object(int sz);
extern void view_unmap_current_vertex_object(void);
extern void view_unbind_current_vertex_object(void);
extern unsigned short* view_bind_map_next_index_object(int sz);
extern void view_unmap_current_index_object(void);
extern void view_unbind_current_index_object(void);

extern void view_draw_next_vertex_object_2D_texture_screen(int wid,int high,float gx,float gy);
extern void view_draw_next_vertex_object_2D_tint_screen(void);
extern void view_draw_next_vertex_object_2D_texture_quad(int lft,int rgt,int top,int bot);
extern void view_draw_next_vertex_object_2D_color_quad(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha);
extern void view_draw_next_vertex_object_2D_line_quad(int lft,int rgt,int top,int bot);

	// shaders
	
extern bool gl_shader_initialize(char *err_str);
extern void gl_shader_shutdown(void);
extern void gl_shader_attach_map(void);
extern void gl_shader_attach_model(model_type *mdl);
extern void gl_shader_draw_scene_initialize(void);
extern void gl_shader_draw_start(void);
extern void gl_shader_draw_end(void);
extern void gl_shader_texture_override(GLuint gl_id);
extern void gl_shader_draw_execute(texture_type *texture,int txt_idx,int frame,float dark_factor,float alpha,view_glsl_light_list_type *light_list);
extern void gl_shader_draw_hilite_execute(texture_type *texture,int txt_idx,int frame,float dark_factor,float alpha,d3pnt *pnt,d3col *col);

	// back renderers
	
extern bool gl_back_render_initialize(char *err_str);
extern void gl_back_render_shutdown(void);
extern void gl_back_render_map_start(void);
extern void gl_back_render_map_end(void);
extern void gl_back_render_frame_start(int tick);
extern bool gl_back_render_get_texture(char *node_name,GLuint *txt_id);

	// view
	
extern void gl_frame_start(d3col *col);
extern void gl_frame_end(void);
extern void gl_3D_view(void);
extern void gl_3D_rotate(d3pnt *pnt,d3ang *ang);
extern void gl_2D_view_screen(void);
extern void gl_2D_view_interface(void);
extern void gl_setup_project(void);
extern bool gl_project_in_view_z(int x,int y,int z);
extern void gl_project_point(int *x,int *y,int *z);
extern void gl_project_poly(int ptsz,int *x,int *y,int *z);
extern float gl_project_get_depth(int x,int y,int z);
extern void gl_unproject_point(float fx,float fy,float fz,int *x,int *y,int *z);
extern void gl_project_fix_rotation(int *x,int *y,int *z);

	// textures
	
extern void gl_texture_initialize(void);
extern void gl_texture_shutdown(void);
extern void gl_texture_opaque_start(void);
extern void gl_texture_opaque_end(void);
extern void gl_texture_opaque_set(GLuint txt_id);
extern void gl_texture_glow_start(void);
extern void gl_texture_glow_end(void);
extern void gl_texture_glow_set(GLuint txt_id,GLuint glow_id,float glow_color);
extern void gl_texture_decal_start(void);
extern void gl_texture_decal_end(void);
extern void gl_texture_decal_set(GLuint txt_id,float r,float g,float b,float alpha);
extern void gl_texture_transparent_start(void);
extern void gl_texture_transparent_end(void);
extern void gl_texture_transparent_set(GLuint txt_id,float alpha);
extern void gl_texture_simple_start(void);
extern void gl_texture_simple_end(void);
extern void gl_texture_simple_set(GLuint txt_id,bool clamp,float r,float g,float b,float alpha);
extern void gl_texture_movie_start(GLuint txt_id);
extern void gl_texture_movie_end(void);

	// text

extern void gl_text_initialize(void);
extern void gl_text_shutdown(void);
extern int gl_text_get_char_height(int text_size);
extern int gl_text_get_string_width(int text_size,char *str);
extern void gl_text_start(int text_size);
extern void gl_text_end(void);
extern void gl_text_draw(int x,int y,char *txt,int just,bool vcenter,d3col *col,float alpha);

	// images

extern void view_images_load(void);
extern void view_images_free(void);
extern bitmap_type* view_images_get_bitmap(int idx);
extern unsigned long view_images_get_gl_id(int idx);
